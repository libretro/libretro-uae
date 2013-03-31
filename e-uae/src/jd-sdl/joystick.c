 /*
  * UAE - The Un*x Amiga Emulator
  *
  * SDL Joystick code
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 1998 Krister Walfridsson
  * Copyright 2003-2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include <SDL.h>

static unsigned int nr_joysticks;
static int initialized;

struct joyinfo {
    SDL_Joystick *joy;
    unsigned int axles;
    unsigned int buttons;
};

static struct joyinfo joys[MAX_INPUT_DEVICES];


static void read_joy (unsigned int nr)
{
    unsigned int num, i, axes, axis;
    SDL_Joystick *joy;

    if (currprefs.input_selected_setting == 0) {
	if (jsem_isjoy (0, &currprefs) != (int)nr && jsem_isjoy (1, &currprefs) != (int)nr)
	    return;
    }
    joy = joys[nr].joy;
    axes = SDL_JoystickNumAxes (joy);
    for (i = 0; i < axes; i++) {
	axis = SDL_JoystickGetAxis (joy, i);
	setjoystickstate (nr, i, axis, 32767);
    }

    num = SDL_JoystickNumButtons (joy);
    for (i = 0; i < num; i++) {
	int bs = SDL_JoystickGetButton (joy, i) ? 1 : 0;
	setjoybuttonstate (nr, i, bs);
    }
}

static unsigned int get_joystick_num (void)
{
    return nr_joysticks;
}

static unsigned int get_joystick_widget_num (unsigned int joy)
{
    return joys[joy].axles + joys[joy].buttons;
}

static int get_joystick_widget_type (unsigned int joy, unsigned int num, char *name, uae_u32 *code)
{
    if (num >= joys[joy].axles && num < joys[joy].axles + joys[joy].buttons) {
	if (name)
	    sprintf (name, "Button %d", num + 1 - joys[joy].axles);
	return IDEV_WIDGET_BUTTON;
    } else if (num < joys[joy].axles) {
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
	    return joys[joy].axles;
	case IDEV_WIDGET_AXIS:
	    return 0;
    }
    return -1;
}

static const char *get_joystick_name (unsigned int joy)
{
    return SDL_JoystickName (joy);
}

static void read_joysticks (void)
{
    if (get_joystick_num ()) {
	unsigned int i;
	SDL_JoystickUpdate ();
	for (i = 0; i < get_joystick_num (); i++)
	    read_joy (i);
    }
}

static int init_joysticks (void)
{
    int success = 0;

    if (!initialized) {
	if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == 0) {
	    unsigned int i;

	    nr_joysticks = SDL_NumJoysticks ();
	    write_log ("Found %d joystick(s)\n", nr_joysticks);

	    if (nr_joysticks > MAX_INPUT_DEVICES)
		nr_joysticks = MAX_INPUT_DEVICES;

	    for (i = 0; i < get_joystick_num (); i++) {
		joys[i].joy     = SDL_JoystickOpen (i);
		joys[i].axles   = SDL_JoystickNumAxes (joys[i].joy);
		joys[i].buttons = SDL_JoystickNumButtons (joys[i].joy);
	    }
	    success = initialized = 1;
	} else
	    write_log ("Failed to initialize joysticks\n");
    }

    return success;
}

static void close_joysticks (void)
{
    unsigned int i;
    for (i = 0; i < nr_joysticks; i++) {
	SDL_JoystickClose (joys[i].joy);
	joys[i].joy = 0;
    }
    nr_joysticks = 0;

    if (initialized) {
	SDL_QuitSubSystem (SDL_INIT_JOYSTICK);
	initialized = 0;
    }
}

static int acquire_joy (unsigned int num, int flags)
{
    return num < get_joystick_num ();
}

static void unacquire_joy (unsigned int num)
{
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
 * Set default inputdevice config for SDL joysticks
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
