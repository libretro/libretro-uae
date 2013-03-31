/*
 *  E-UAE - The portable Amiga Emulator
 *
 *  BeOS joystick driver
 *
 *  (c) Richard Drummond 2005
 */

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "memory.h"
#include "custom.h"
#include "inputdevice.h"
}

#include <device/Joystick.h>
#include <support/String.h>

//#define DEBUG
#ifdef DEBUG
#define DEBUG_LOG write_log
#else
#define DEBUG_LOG(x...) { do ; while(0); }
#endif

extern "C" {
static int           init_joysticks            (void);
static void          close_joysticks           (void);
static int           acquire_joy               (unsigned int nr, int flags);
static void          unacquire_joy             (unsigned int nr);
static void          read_joysticks            (void);
static unsigned int  get_joystick_count        (void);
static const char   *get_joystick_name         (unsigned int nr);
static unsigned int  get_joystick_widget_num   (unsigned int nr);
static int           get_joystick_widget_type  (unsigned int nr, unsigned int num, char *name);
static int           get_joystick_widget_first (unsigned int nr, int type);
};


/*
 * The BJoystick class can't make up its mind whether it represents a joystick
 * port or the device attached to a port.
 *
 * We choose to believe both and thus that there's at most one joystick attached
 * to each port. Since USB ain't supported, I don't have any hardware which
 * disproves that belief...
 */

class UAEJoystick :public BJoystick
{
    public:
	UAEJoystick (unsigned int nr, const char *port_name);

    private:
	unsigned int nr;		/* Device number that UAE assigns to a joystick */
	BString      port_name;		/* Name used to open the joystick port */
	BString      name;		/* Full name used to describe this joystick to the user */

    public:
	const char  *getName ()		{ return name.String(); }

	int	     acquire ();
	void         unacquire ();
	void         read ();
};

UAEJoystick :: UAEJoystick (unsigned int nr, const char *port_name)
{
    /* Create joystick name using both port and joystick name */
    BString stick_name;

    this->Open(port_name);
    this->GetControllerName (&stick_name);
    this->Close ();

    this->name  = port_name;
    this->name += ":";
    this->name += stick_name;

    this->port_name = port_name;
    this->nr = nr;
}

int UAEJoystick :: acquire ()
{
    return this->Open (this->port_name.String());
}

void UAEJoystick :: unacquire ()
{
    this->Close ();
}

void UAEJoystick :: read ()
{
    DEBUG_LOG ("read: polling joy:%d\n", this->nr);

    if (this->Update () != B_ERROR) {

	/* Read axis values */
	{
	    unsigned int nr_axes = this->CountAxes ();
	    int16        values[nr_axes];
	    unsigned int axis;

	    this->GetAxisValues (values);

	    for (axis = 0; axis < nr_axes; axis++)
		setjoystickstate (this->nr, axis, values[axis], 32767);
	}

	/* Read button values */
	{
	    unsigned int nr_buttons = this->CountButtons ();
	    int32        values;
	    unsigned int button;

	    values = this->ButtonValues ();

	    for (button = 0; button < nr_buttons; button++) {
		setjoybuttonstate (this->nr, button, values & 1);
		values >>= 1;
	    }
	}
   }
}


/*
 * Inputdevice API
 */

#define MAX_JOYSTICKS	MAX_INPUT_DEVICES

static unsigned int  nr_joysticks;
static UAEJoystick  *joysticks [MAX_JOYSTICKS];


static int init_joysticks (void)
{
    BJoystick joy;
    unsigned int nr_ports;
    unsigned int i;

    nr_joysticks = 0;

    nr_ports = joy.CountDevices ();
    if (nr_ports > MAX_JOYSTICKS)
	nr_ports = MAX_JOYSTICKS;

    /*
     * Enumerate joysticks
     */

    for (i = 0; i < nr_ports; i++) {

	char port_name[B_OS_NAME_LENGTH];

	joy.GetDeviceName (i, port_name);

	if (joy.Open (port_name)) {
	    BString stick_name;

	    joy.Close ();

	    joysticks[nr_joysticks] = new UAEJoystick (nr_joysticks, port_name);

	    write_log ("BJoystick: device %d = %s\n", nr_joysticks,
						      joysticks[nr_joysticks]->getName ());

	    nr_joysticks++;
	} else
	    DEBUG_LOG ("Failed to open port='%s'\n", port_name);

    }

    write_log ("BJoystick: Found %d joystick(s)\n", nr_joysticks);

    return 1;
}

static void close_joysticks (void)
{
    unsigned int i;

    for (i = 0; i < nr_joysticks; i++)
	delete joysticks[i];

    nr_joysticks = 0;
}

static int acquire_joy (unsigned int nr, int flags)
{
    int result = 0;

    DEBUG_LOG ("acquire_joy (%d)...\n", nr);

    if (nr < nr_joysticks)
	result = joysticks[nr]->acquire ();

    DEBUG_LOG ("%s\n", result ? "okay" : "failed");

    return result;
}

static void unacquire_joy (unsigned int nr)
{
    DEBUG_LOG ("unacquire_joy (%d)\n", nr);

    if (nr < nr_joysticks)
	joysticks[nr]->unacquire ();
}

static void read_joysticks (void)
{
    unsigned int i;

    for (i = 0; i < get_joystick_count (); i++) {
    	/* In compatibility mode, don't read joystick unless it's selected in the prefs */
	if (currprefs.input_selected_setting == 0) {
	    if (jsem_isjoy (0, &currprefs) != (int)i && jsem_isjoy (1, &currprefs) != (int)i)
		continue;
	}
	joysticks[i]->read ();
    }
}

static unsigned int get_joystick_count (void)
{
    return nr_joysticks;
}

static const char *get_joystick_name (unsigned int nr)
{
    return joysticks[nr]->getName ();
}

static unsigned int get_joystick_widget_num (unsigned int nr)
{
    return joysticks[nr]->CountAxes () + joysticks[nr]->CountButtons ();
}

static int get_joystick_widget_type (unsigned int nr, unsigned int widget_num, char *name, uae_u32 *what)
{
    unsigned int nr_axes    = joysticks[nr]->CountAxes ();
    unsigned int nr_buttons = joysticks[nr]->CountButtons ();

    if (widget_num >= nr_axes && widget_num < nr_axes + nr_buttons) {
	if (name)
	    sprintf (name, "Button %d", widget_num + 1 - nr_axes);
	return IDEV_WIDGET_BUTTON;
    } else if (widget_num < nr_axes) {
	if (name)
	    sprintf (name, "Axis %d", widget_num + 1);
	return IDEV_WIDGET_AXIS;
    }
    return IDEV_WIDGET_NONE;
}

static int get_joystick_widget_first (unsigned int nr, int type)
{
    switch (type) {
	case IDEV_WIDGET_BUTTON:
	    return joysticks[nr]->CountAxes ();
	case IDEV_WIDGET_AXIS:
	    return 0;
    }

    return -1;
}


struct inputdevice_functions inputdevicefunc_joystick = {
    init_joysticks,
    close_joysticks,
    acquire_joy,
    unacquire_joy,
    read_joysticks,
    get_joystick_count,
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
