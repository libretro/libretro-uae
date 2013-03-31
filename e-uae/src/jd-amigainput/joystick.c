 /*
  * E-UAE - The portable Amiga Emulator
  *
  * AmigaInput joystick driver
  *
  * Copyright 2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"

#include <amigainput/amigainput.h>
#include <proto/exec.h>
#include <proto/amigainput.h>

#define MAX_JOYSTICKS                   MAX_INPUT_DEVICES
#define MAX_AXES                        2
#define MAX_BUTTONS                     12

struct Library   *AIN_Base;
struct AIN_IFace *IAIN;

/*
 * Per-joystick data private to driver
 */
struct joystick
{
    AIN_DeviceID        id;
    const char         *name;

    AIN_DeviceHandle   *handle;
    APTR                context;

    uint32              axisCount;
    uint32              buttonCount;

    uint32              axisBufferOffset[MAX_AXES];
    int32               axisData[MAX_AXES];

    uint32              buttonBufferOffset[MAX_BUTTONS];
    int32               buttonData[MAX_BUTTONS];
};


static APTR            joystickContext;
static uint32          joystickCount;
static struct joystick joystickList [MAX_JOYSTICKS];


static BOOL openAmigaInput (void)
{
    AIN_Base = OpenLibrary ("AmigaInput.library", 51);

    if (AIN_Base) {
	IAIN = (struct AIN_IFace *) GetInterface (AIN_Base, "main", 1, NULL);

	if (!IAIN) {
	    CloseLibrary (AIN_Base);
	    AIN_Base = NULL;
	}
    } else
	write_log ("Failed to open AmigaInput.library\n");

    return AIN_Base != NULL;
}

static void closeAmigaInput (void)
{
    if (IAIN) {
	DropInterface ((struct Interface *)IAIN);
	IAIN = NULL;
    }

    if (AIN_Base) {
	CloseLibrary (AIN_Base);
	AIN_Base = NULL;
    }
}


/* A handy container to encapsulate the information we
 * need when enumerating joysticks on the system.
 */
struct enumPacket
{
     APTR             context;
     uint32          *count;
     struct joystick *joyList;
};

/*
 * Callback to enumerate joysticks
 */
static BOOL enumerateJoysticks (AIN_Device *device, void *UserData)
{
    APTR             context =  ((struct enumPacket *)UserData)->context;
    uint32          *count   =  ((struct enumPacket *)UserData)->count;
    struct joystick *joy     = &((struct enumPacket *)UserData)->joyList[*count];

    BOOL result = FALSE;

    if (*count < MAX_JOYSTICKS) {
	if (device->Type == AINDT_JOYSTICK) {

	    unsigned int i;

	    joy->context     = context;
	    joy->id          = device->DeviceID;
	    joy->name        = my_strdup (device->DeviceName);
	    joy->axisCount   = device->NumAxes;
	    joy->buttonCount = device->NumButtons;

	    if (joy->axisCount   > MAX_AXES)    joy->axisCount   = MAX_AXES;
	    if (joy->buttonCount > MAX_BUTTONS) joy->buttonCount = MAX_BUTTONS;

	    /* Query offsets in ReadDevice buffer for axes' data */
	    for (i = 0; i < joy->axisCount; i++)
		result =           AIN_Query (joy->context, joy->id, AINQ_AXIS_OFFSET, i,
					      &(joy->axisBufferOffset[i]), 4);

	    /* Query offsets in ReadDevice buffer for buttons' data */
	    for (i = 0; i < joy->buttonCount; i++)
		result = result && AIN_Query (joy->context, joy->id, AINQ_BUTTON_OFFSET, i,
					      &(joy->buttonBufferOffset[i]), 4);

	    if (result) {
		write_log ("Found joystick #%d (AI ID=%d) '%s' with %d axes, %d buttons\n",
			   *count, joy->id, joy->name, joy->axisCount, joy->buttonCount);

		(*count)++;
	    }
	}
    }

    return result;
}


static int init_joysticks (void)
{
    int i;
    int success = 0;

    if (!joystickContext) {

    	if (openAmigaInput ()) {

	    joystickContext = AIN_CreateContext (1, NULL);
	    if (joystickContext) {

		struct enumPacket packet = {
		    joystickContext, &joystickCount, &joystickList[0]
		};

		AIN_EnumDevices (joystickContext, enumerateJoysticks, &packet);

		write_log ("Found %d joysticks\n", joystickCount);
	       
	        success = 1;
            }
	}
    }

    return success;
}

static void close_joysticks (void)
{
    unsigned int i = joystickCount;

    while (i-- > 0) {
	struct joystick *joy = &joystickList[i];

	if (joy->handle) {
	    AIN_ReleaseDevice (joy->context, joy->handle);
	    joy->handle = 0;
	}
    }
    joystickCount = 0;

    AIN_DeleteContext (joystickContext);
    joystickContext = 0;
}

#define BUFFER_OFFSET(buffer, offset)	(((int32 *)buffer)[offset])

static void read_joy (unsigned int num)
{
    struct joystick *joy = &joystickList[num];

    if (joy->handle) {
	void *buffer;

	/*
	 * Poll device for data
	 */
	if (AIN_ReadDevice (joy->context, joy->handle, &buffer)) {
	    unsigned int i;

	    /* Extract axis data from buffer and notify UAE of any changes
	     * in axis state
	     */
	    for (i = 0; i < joy->axisCount; i++) {
		int axisdata = BUFFER_OFFSET (buffer, joy->axisBufferOffset[i]);

		if (axisdata != joy->axisData[i]) {
		    setjoystickstate (num, i, axisdata, 32767);
		    joy->axisData[i] = axisdata;
		}
	    }

	    /* Extract button data from buffer and notify SDL of any changes
	     * in button state
	     *
	     * Note: We don't support analog buttons.
	     */
	    for (i = 0; i < joy->buttonCount; i++) {
		int buttondata = BUFFER_OFFSET (buffer, joy->buttonBufferOffset[i]);

		if (buttondata != joy->buttonData[i]) {
		    setjoybuttonstate (num, i, buttondata ? 1 : 0);
		    joy->buttonData[i] = buttondata;
		}
	    }

	}
    }
}

/*
 * Query number of joysticks attached to system
 */
static unsigned int get_joystick_count (void)
{
    return joystickCount;
}

/*
 * Query number of 'widgets' supported by joystick #joynum
 */
static unsigned int get_joystick_widget_num (unsigned int joynum)
{
    return joystickList[joynum].axisCount + joystickList[joynum].buttonCount;
}

/*
 * Query type of widget #widgetnum on joystick #joynum
 */
static int get_joystick_widget_type (unsigned int joynum, unsigned int widgetnum, char *name, uae_u32 *code)
{
    struct joystick *joy = &joystickList[joynum];

    if (widgetnum >= joy->axisCount && widgetnum < joy->axisCount + joy->buttonCount) {
	if (name)
	    sprintf (name, "Button %d", widgetnum + 1 - joy->axisCount);
	return IDEV_WIDGET_BUTTON;
    } else if (widgetnum < joy->axisCount) {
	if (name)
	    sprintf (name, "Axis %d", widgetnum + 1);
	return IDEV_WIDGET_AXIS;
    }
    return IDEV_WIDGET_NONE;
}

static int get_joystick_widget_first (unsigned int joynum, int type)
{
    switch (type) {
	case IDEV_WIDGET_BUTTON:
	    return joystickList[joynum].axisCount;
	case IDEV_WIDGET_AXIS:
	    return 0;
    }
    return -1;
}

static const char *get_joystick_name (unsigned int joynum)
{
    return (char *)joystickList[joynum].name;
}

static void read_joysticks (void)
{
    unsigned int i = joystickCount;

    while (i > 0)
	read_joy (--i);
}

static int acquire_joy (unsigned int joynum, int flags)
{
    struct joystick *joy = &joystickList[joynum];
    int result = 0;

    joy->handle = AIN_ObtainDevice (joy->context, joy->id);

    if (joy->handle)
        result = 1;
    else
        write_log ("Failed to acquire joy\n");
   return result;
}

static void unacquire_joy (unsigned int joynum)
{
    struct joystick *joy = &joystickList[joynum];

    if (joy->handle) {
           AIN_ReleaseDevice (joy->context, joy->handle);
           joy->handle = 0;
    }
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
 * Set default inputdevice config
 */
void input_get_default_joystick (struct uae_input_device *uid)
{
    unsigned int i, port;

    for (i = 0; i < joystickCount; i++) {
	port = i & 1;
	uid[i].eventid[ID_AXIS_OFFSET + 0][0]   = port ? INPUTEVENT_JOY2_HORIZ : INPUTEVENT_JOY1_HORIZ;
	uid[i].eventid[ID_AXIS_OFFSET + 1][0]   = port ? INPUTEVENT_JOY2_VERT  : INPUTEVENT_JOY1_VERT;
	uid[i].eventid[ID_BUTTON_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_FIRE_BUTTON : INPUTEVENT_JOY1_FIRE_BUTTON;
	uid[i].eventid[ID_BUTTON_OFFSET + 1][0] = port ? INPUTEVENT_JOY2_2ND_BUTTON  : INPUTEVENT_JOY1_2ND_BUTTON;
	uid[i].eventid[ID_BUTTON_OFFSET + 2][0] = port ? INPUTEVENT_JOY2_3RD_BUTTON  : INPUTEVENT_JOY1_3RD_BUTTON;
    }
    uid[0].enabled = 1;
}
