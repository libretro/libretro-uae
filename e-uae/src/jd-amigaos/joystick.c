 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Joystick emulation for AmigaOS using lowlevel.library
  *
  * Copyright 1996, 1997 Samuel Devulder
  * Copyright 2003-2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"

#include <libraries/lowlevel.h>
#include <proto/exec.h>
#include <proto/lowlevel.h>

struct Library *LowLevelBase;
struct LowLevelIFace *ILowLevel;

static unsigned int nr_joysticks;

#define MAX_BUTTONS  2
#define MAX_AXLES    2
#define FIRST_AXLE   0
#define FIRST_BUTTON 2


static int init_joysticks (void)
{
    LowLevelBase = (struct Library *) OpenLibrary ("lowlevel.library", 39);
#ifdef __amigaos4__
    if (LowLevelBase) {
	ILowLevel = (struct LowLevelIFace *)GetInterface (LowLevelBase, "main", 1, NULL);
	if (!ILowLevel) {
	    CloseLibrary (LowLevelBase);
	    LowLevelBase = 0;
	}
    }
#endif

    if (LowLevelBase) {
	nr_joysticks = 2;
	return 1;
    } else {
	nr_joysticks = 0;
	return 0;
    }
}

static void close_joysticks (void)
{
    if (LowLevelBase) {
	CloseLibrary (LowLevelBase);
	LowLevelBase = 0;
    }
}

static int acquire_joy (unsigned int num, int flags)
{
    return 1;
}

static void unacquire_joy (unsigned int num)
{
}

static void read_joy (unsigned int nr)
{
    if (LowLevelBase != NULL) {
	ULONG state = ReadJoyPort (nr);

	if ((state & JP_TYPE_MASK) != JP_TYPE_NOTAVAIL) {
	    int x = 0, y = 0;

	    if (state & JPF_JOY_UP)
	        y = -1;
	    else if (state & JPF_JOY_DOWN)
	        y = 1;
	    if (state & JPF_JOY_LEFT)
	        x = -1;
	    else if (state & JPF_JOY_RIGHT)
	        x = 1;

	    setjoystickstate (nr, 0, x, 1);
	    setjoystickstate (nr, 1, y, 1);

	    setjoybuttonstate (nr, 0, state & JPF_BUTTON_RED);
	    setjoybuttonstate (nr, 1, state & JPF_BUTTON_BLUE);
	}
    }
}

static void read_joysticks (void)
{
    read_joy (0);
    read_joy (1);
}

static unsigned int get_joystick_num (void)
{
    return nr_joysticks;
}

static const char *get_joystick_name (unsigned int joy)
{
    static char name[16];
    sprintf (name, "Joy port %d", joy);
    return name;
}

static unsigned int get_joystick_widget_num (unsigned int joy)
{
    return MAX_AXLES + MAX_BUTTONS;
}

static int get_joystick_widget_type (unsigned int joy, unsigned int num, char *name, uae_u32 *code)
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
 * Set default inputdevice config for Amiga joysticks
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
