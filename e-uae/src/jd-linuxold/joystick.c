 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Joystick emulation for Linux and BSD. They share too much code to
  * split this file.
  *
  * This uses the deprecated 0.x Linux joystick API.
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 1998 Krister Walfridsson
  * Copyright 2003-2006 Richard Drummond
  * Copyright 2004 Nick Seow (Alternative Linux joystick device path)
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "memory.h"
#include "custom.h"
#include "inputdevice.h"

#define JS_MAXPATHLEN  20      // Longest device name would be "/dev/input/js"
char js_prefix[JS_MAXPATHLEN]; // Joystick device, which varies, except number

#ifdef HAVE_MACHINE_JOYSTICK_H

/* The BSD way.  */

# include <machine/joystick.h>
typedef struct joystick uae_joystick_t;

#define JS_DEVNAME_PREFIX "joy"

#else

/* The Linux way.  */

/* There are too many different versions of <linux/joystick.h>.  Rather
 * than trying to work correctly with all of them, we duplicate the
 * necessary definitions here.  */
typedef struct
{
    int buttons;
    int x;
    int y;
} uae_joystick_t;

#define JS_DEVNAME_PREFIX "js" // Try this first

#endif

/* Hard code these for the old joystick API */
#define MAX_BUTTONS  2
#define MAX_AXLES    2
#define FIRST_AXLE   0
#define FIRST_BUTTON 2

static unsigned int nr_joysticks;

static int js0, js1;

struct joy_range
{
    int minx, maxx, miny, maxy;
    int centrex, centrey;
} range0, range1;


static void read_joy (unsigned int nr)
{
    uae_joystick_t buffer;
    int len;
    int fd = nr == 0 ? js0 : js1;
    struct joy_range *r = nr == 0 ? &range0 : &range1;

    if (currprefs.input_selected_setting == 0) {
	if (jsem_isjoy (0, &currprefs) != (int)nr && jsem_isjoy (1, &currprefs) != (int)nr)
	    return;
    }

    len = read(fd, &buffer, sizeof(buffer));
    if (len != sizeof(buffer))
	return;

    /* According to old 0.x JS API, we don't know the range
     * or the centre for either axis, so we try to work these
     * out as we go along.
     *
     * Must be a better way to do this . . .
     */
    if (buffer.x < r->minx) r->minx = buffer.x;
    if (buffer.y < r->miny) r->miny = buffer.y;
    if (buffer.x > r->maxx) r->maxx = buffer.x;
    if (buffer.y > r->maxy) r->maxy = buffer.y;

    r->centrex = (r->maxx-r->minx)/2 + r->minx;
    r->centrey = (r->maxy-r->miny)/2 + r->miny;

    /* Translate these values to be centred on 0 and
     * feed 'em to the inputdevice system */
    setjoystickstate (nr, 0, buffer.x - r->centrex, r->centrex );
    setjoystickstate (nr, 1, buffer.y - r->centrey, r->centrey );

#ifdef HAVE_MACHINE_JOYSTICK_H
    setjoybuttonstate (nr, 0, buffer.b1);
    setjoybuttonstate (nr, 1, buffer.b2);
#else
    setjoybuttonstate (nr, 0, buffer.buttons & 1);
    setjoybuttonstate (nr, 1, buffer.buttons & 2);
#endif
}

static int init_joysticks(void)
{
    char js_path[JS_MAXPATHLEN]; // temporary buffer for device name
    nr_joysticks = 0;
    js0 = -1; js1 = -1;

    snprintf (js_prefix, JS_MAXPATHLEN, "/dev/%s", JS_DEVNAME_PREFIX);

    snprintf (js_path, JS_MAXPATHLEN, "%s0", js_prefix);
    if ((js0 = open (js_path, O_RDONLY)) >= 0)
	nr_joysticks++;

    snprintf (js_path, JS_MAXPATHLEN, "%s1", js_prefix);
    if ((js1 = open (js_path, O_RDONLY)) >= 0)
	nr_joysticks++;

#ifdef __linux__
   if (nr_joysticks == 0) {
        /*
	 * If we haven't found any joysticks yet,
	 * look for /dev/input/js* nodes
	 */
	sprintf (js_prefix, "/dev/input/%s", JS_DEVNAME_PREFIX);

	snprintf (js_path, JS_MAXPATHLEN, "%s0", js_prefix);
	if ((js0 = open (js_path, O_RDONLY)) >= 0)
	    nr_joysticks++;

	snprintf (js_path, JS_MAXPATHLEN, "%s1", js_prefix);
	if ((js1 = open (js_path, O_RDONLY)) >= 0)
	    nr_joysticks++;
    }
#endif

    write_log ("Found %d joystick(s)\n", nr_joysticks);

    range0.minx = INT_MAX;
    range0.maxx = INT_MIN;
    range0.miny = INT_MAX;
    range0.maxy = INT_MIN;
    range1.minx = INT_MAX;
    range1.maxx = INT_MIN;
    range1.miny = INT_MAX;
    range1.maxy = INT_MIN;
    range0.centrex = 0;
    range1.centrey = 0;
    return 1;
}

static void close_joysticks(void)
{
    if (js0 >= 0)
	close (js0);
    if (js1 >= 0)
	close (js1);
}

static unsigned int get_joystick_num (void)
{
    return nr_joysticks;
}

static int acquire_joy (unsigned int num, int flags)
{
    return 1;
}

static void unacquire_joy (unsigned int num)
{
}

static void read_joysticks (void)
{
    unsigned int i;
    for (i = 0; i < get_joystick_num(); i++)
	read_joy (i);
}

static const char *get_joystick_name (unsigned int joy)
{
    static char name[100];
    sprintf (name, "%d: %s%d", joy + 1, js_prefix, joy);
    return name;
}

static unsigned int get_joystick_widget_num (unsigned int joy)
{
    return MAX_AXLES + MAX_BUTTONS;
}

static int get_joystick_widget_type (unsigned int joy, unsigned int num, char *name, uae_u32 *dummy)
{
    if (num >= MAX_AXLES && num < MAX_AXLES+MAX_BUTTONS) {
	if (name)
	    sprintf (name, "Button %d", num + 1 - MAX_AXLES);
	return IDEV_WIDGET_BUTTON;
    } else if (num < MAX_AXLES) {
	if (name)
	    sprintf (name, "Axis %d", num + 1);
	return IDEV_WIDGET_AXIS;
    }
    return IDEV_WIDGET_NONE;
}

static int get_joystick_widget_first (unsigned int joy, int type)
{
    switch (type) {
	case IDEV_WIDGET_BUTTON:
	    return FIRST_BUTTON;
	case IDEV_WIDGET_AXIS:
	    return FIRST_AXLE;
    }

    return -1;
}

struct inputdevice_functions inputdevicefunc_joystick = {
    init_joysticks,
    close_joysticks,
    acquire_joy,
    unacquire_joy,
    read_joysticks,
    get_joystick_num,
    get_joystick_name,
    get_joystick_widget_num,
    get_joystick_widget_type,
    get_joystick_widget_first
};

/*
 * Set default inputdevice config for joysticks
 */
void input_get_default_joystick (struct uae_input_device *uid)
{
    unsigned int i, port;

    for (i = 0; i < nr_joysticks; i++) {
        port = i & 1;
        uid[i].eventid[ID_AXIS_OFFSET + 0][0]   = port ? INPUTEVENT_JOY2_HORIZ : INPUTEVENT_JOY1_HORIZ;
        uid[i].eventid[ID_AXIS_OFFSET + 1][0]   = port ? INPUTEVENT_JOY2_VERT  : INPUTEVENT_JOY1_VERT;
        uid[i].eventid[ID_BUTTON_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_FIRE_BUTTON : INPUTEVENT_JOY1_FIRE_BUTTON;
        uid[i].eventid[ID_BUTTON_OFFSET + 1][0] = port ? INPUTEVENT_JOY2_2ND_BUTTON  : INPUTEVENT_JOY1_2ND_BUTTON;
        uid[i].eventid[ID_BUTTON_OFFSET + 2][0] = port ? INPUTEVENT_JOY2_3RD_BUTTON  : INPUTEVENT_JOY1_3RD_BUTTON;
    }
    uid[0].enabled = 1;
}
