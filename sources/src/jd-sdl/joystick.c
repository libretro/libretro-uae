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

#define MAX_MAPPINGS 256

/* external prototypes */
extern void setid    (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt);
extern void setid_af (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt, int af);

/* internal members */
static unsigned int nr_joysticks;
static int initialized;

struct joyinfo {
    SDL_Joystick *joy;
    int axles;
    int buttons;
};

static struct joyinfo joys[MAX_INPUT_DEVICES];


static void read_joy (int nr)
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

static  int get_joystick_num (void)
{
    return nr_joysticks;
}

static  int get_joystick_widget_num (int joy)
{
    return joys[joy].axles + joys[joy].buttons;
}

static int get_joystick_widget_type (int joy, int num, TCHAR *name, uae_u32 *code)
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

static int get_joystick_widget_first (int joy, int type)
{
    switch (type) {
	case IDEV_WIDGET_BUTTON:
	    return joys[joy].axles;
	case IDEV_WIDGET_AXIS:
	    return 0;
    }
    return -1;
}

static TCHAR *get_joystick_friendlyname (int joy)
{
    return (TCHAR*)SDL_JoystickName (joy);
}

static TCHAR *get_joystick_uniquename (int joy)
{
    return (TCHAR*)SDL_JoystickName (joy);
}

static void read_joystick (void)
{
    if (get_joystick_num ()) {
		int i = 0;
		SDL_JoystickUpdate ();
		for ( ; i < get_joystick_num (); i++)
		    read_joy (i);
    }
}

static int init_joystick (void)
{
    int success = 0;

    if (!initialized) {
		if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == 0) {
		    int i = 0;

		    nr_joysticks = SDL_NumJoysticks ();
		    write_log ("Found %d joystick(s)\n", nr_joysticks);

		    if (nr_joysticks > MAX_INPUT_DEVICES)
				nr_joysticks = MAX_INPUT_DEVICES;

		    for ( ; i < get_joystick_num (); i++) {
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

static void close_joystick (void)
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

static int acquire_joystick (int num, int flags)
{
	return num < get_joystick_num ();
}

static void unacquire_joystick (int num)
{
}

static int get_joystick_flags (int num)
{
	return 0;
}

struct inputdevice_functions inputdevicefunc_joystick = {
	init_joystick,
	close_joystick,
	acquire_joystick,
	unacquire_joystick,
	read_joystick,
	get_joystick_num,
	get_joystick_friendlyname,
	get_joystick_uniquename,
	get_joystick_widget_num,
	get_joystick_widget_type,
	get_joystick_widget_first,
	get_joystick_flags
};

/*
 * Set default inputdevice config for SDL joysticks
 */
int input_get_default_joystick (struct uae_input_device *uid, int num, int port, int af, int mode, bool gp)
{
	int h,v;
//	unsigned int j;
//	struct didata *did;
	SDL_Joystick *joy;
	joy = joys[num].joy;

	if (num >= get_joystick_num ())
		return 0;

	if (mode == JSEM_MODE_MOUSE_CDTV) {
		h = INPUTEVENT_MOUSE_CDTV_HORIZ;
		v = INPUTEVENT_MOUSE_CDTV_VERT;
	} else if (port >= 2) {
		h = port == 3 ? INPUTEVENT_PAR_JOY2_HORIZ : INPUTEVENT_PAR_JOY1_HORIZ;
		v = port == 3 ? INPUTEVENT_PAR_JOY2_VERT : INPUTEVENT_PAR_JOY1_VERT;
	} else {
		h = port ? INPUTEVENT_JOY2_HORIZ : INPUTEVENT_JOY1_HORIZ;;
		v = port ? INPUTEVENT_JOY2_VERT : INPUTEVENT_JOY1_VERT;
	}
	setid (uid, num, ID_AXIS_OFFSET + 0, 0, port, h);
	setid (uid, num, ID_AXIS_OFFSET + 1, 0, port, v);

	if (port >= 2) {
		setid_af (uid, num, ID_BUTTON_OFFSET + 0, 0, port, port == 3 ? INPUTEVENT_PAR_JOY2_FIRE_BUTTON : INPUTEVENT_PAR_JOY1_FIRE_BUTTON, af);
	} else {
		setid_af (uid, num, ID_BUTTON_OFFSET + 0, 0, port, port ? INPUTEVENT_JOY2_FIRE_BUTTON : INPUTEVENT_JOY1_FIRE_BUTTON, af);
		if (SDL_JoystickNumButtons(joy) > 0)
			setid (uid, num, ID_BUTTON_OFFSET + 1, 0, port, port ? INPUTEVENT_JOY2_2ND_BUTTON : INPUTEVENT_JOY1_2ND_BUTTON);
		if (SDL_JoystickNumButtons(joy) > 1)
			setid (uid, num, ID_BUTTON_OFFSET + 2, 0, port, port ? INPUTEVENT_JOY2_3RD_BUTTON : INPUTEVENT_JOY1_3RD_BUTTON);
	}

#if 0
	for (j = 2; j < MAX_MAPPINGS - 1; j++) {
		int am = did->axismappings[j];
		if (am == DIJOFS_POV(0) || am == DIJOFS_POV(1) || am == DIJOFS_POV(2) || am == DIJOFS_POV(3)) {
			setid (uid, num, ID_AXIS_OFFSET + j + 0, 0, port, h);
			setid (uid, num, ID_AXIS_OFFSET + j + 1, 0, port, v);
			j++;
		}
	}
#endif

	if (mode == JSEM_MODE_JOYSTICK_CD32) {
		setid_af (uid, num, ID_BUTTON_OFFSET + 0, 0, port, port ? INPUTEVENT_JOY2_CD32_RED : INPUTEVENT_JOY1_CD32_RED, af);
		setid_af (uid, num, ID_BUTTON_OFFSET + 0, 1, port, port ? INPUTEVENT_JOY2_FIRE_BUTTON : INPUTEVENT_JOY1_FIRE_BUTTON, af);
		if (SDL_JoystickNumButtons(joy) > 0) {
			setid (uid, num, ID_BUTTON_OFFSET + 1, 0, port, port ? INPUTEVENT_JOY2_CD32_BLUE : INPUTEVENT_JOY1_CD32_BLUE);
			setid (uid, num, ID_BUTTON_OFFSET + 1, 1, port,  port ? INPUTEVENT_JOY2_2ND_BUTTON : INPUTEVENT_JOY1_2ND_BUTTON);
		}
		if (SDL_JoystickNumButtons(joy) > 1)
			setid (uid, num, ID_BUTTON_OFFSET + 2, 0, port, port ? INPUTEVENT_JOY2_CD32_GREEN : INPUTEVENT_JOY1_CD32_GREEN);
		if (SDL_JoystickNumButtons(joy) > 2)
			setid (uid, num, ID_BUTTON_OFFSET + 3, 0, port, port ? INPUTEVENT_JOY2_CD32_YELLOW : INPUTEVENT_JOY1_CD32_YELLOW);
		if (SDL_JoystickNumButtons(joy) > 3)
			setid (uid, num, ID_BUTTON_OFFSET + 4, 0, port, port ? INPUTEVENT_JOY2_CD32_RWD : INPUTEVENT_JOY1_CD32_RWD);
		if (SDL_JoystickNumButtons(joy) > 4)
			setid (uid, num, ID_BUTTON_OFFSET + 5, 0, port, port ? INPUTEVENT_JOY2_CD32_FFW : INPUTEVENT_JOY1_CD32_FFW);
		if (SDL_JoystickNumButtons(joy) > 5)
			setid (uid, num, ID_BUTTON_OFFSET + 6, 0, port, port ? INPUTEVENT_JOY2_CD32_PLAY :  INPUTEVENT_JOY1_CD32_PLAY);
	}
	if (num == 0)
		return 1;
	return 0;
}
