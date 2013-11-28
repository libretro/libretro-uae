 /*
  * E-UAE - The portable Amiga emulator.
  *
  * BeOS UI - or the beginnings of one
  *
  * Copyright 2004-2007 Richard Drummond
  */

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gui.h"
#include "xwin.h"
#include "disk.h"
#include "gensound.h"
}

#include <AppKit.h>
#include <InterfaceKit.h>
#include <storage/FilePanel.h>
#include <storage/Path.h>
#include <kernel/OS.h>

/*
 * Dialog for inserting floppy images
 */
class floppyFilePanel: public BFilePanel, public BHandler {
    public:
	floppyFilePanel (int drive);
	void MessageReceived (BMessage *msg);
	void run ();
    private:
	sem_id done_sem;
	static const uint32 MSG_FLOPPY_PANEL_DRIVE0 = 'flp0';
	static const uint32 MSG_FLOPPY_PANEL_DRIVE1 = 'flp1';
	static const uint32 MSG_FLOPPY_PANEL_DRIVE2 = 'flp2';
	static const uint32 MSG_FLOPPY_PANEL_DRIVE3 = 'flp3';
};

floppyFilePanel::floppyFilePanel (int drive):
	BFilePanel (B_OPEN_PANEL, NULL, NULL, 0, false, NULL, 0, true, true)
{
    char title[80];
    BEntry dir      = BEntry (currprefs.df[drive]);
    BMessage   msg  = BMessage (MSG_FLOPPY_PANEL_DRIVE0 + drive);

    dir.GetParent (&dir);
    sprintf (title, "UAE: Select image to insert in drive DF%d:", drive);

    done_sem = create_sem (0, NULL);

    be_app->Lock ();
    be_app->AddHandler (this);
    be_app->Unlock ();

    this->SetTarget (BMessenger (this));
    this->SetMessage (&msg);
    this->SetPanelDirectory (&dir);
    this->Window ()->SetTitle (title);
}

void floppyFilePanel::MessageReceived (BMessage *msg) {
    switch (msg->what) {
	case MSG_FLOPPY_PANEL_DRIVE0:
	case MSG_FLOPPY_PANEL_DRIVE1:
	case MSG_FLOPPY_PANEL_DRIVE2:
	case MSG_FLOPPY_PANEL_DRIVE3: {
	    int drive = msg->what - MSG_FLOPPY_PANEL_DRIVE0;
	    entry_ref ref;
	    BEntry entry;
	    if (msg->FindRef ("refs", &ref) == B_NO_ERROR)
	        if (entry.SetTo (&ref) == B_NO_ERROR) {
		    BPath path;
		    entry.GetPath (&path);
//		    disk_insert (drive, path.Path ());
//		    disk_insert() doesn't work for some reason . . .
		    strcpy (changed_prefs.df[drive], path.Path ());
		    release_sem (done_sem);
	        }
		break;
	    }
	case B_CANCEL:
	    release_sem (done_sem);
	    /* fall through */
	default:
	    BHandler::MessageReceived (msg);
    }
}

void floppyFilePanel::run ()
{
    this->Window ()->Show ();
    acquire_sem (done_sem);
}


/*
 * The UAE GUI callbacks
 */

int gui_init (void)
{
}

void gui_exit (void)
{
}

int gui_update (void)
{
    return 0;
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
}

void gui_fps (int fps, int idle)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

void gui_flicker_led (int led, int unitnum, int status)
{
}

void gui_led (int led, int on)
{
}

void gui_display (int shortcut)
{
    pause_sound ();

    if (shortcut >=0 && shortcut < 4) {
	/* If we're running full-screen, we must toggle
	 * to windowed mode before opening the dialog */
	int was_fullscreen;

	if (was_fullscreen = is_fullscreen ()) {
	    toggle_fullscreen (0);
	    if (is_fullscreen ()) {
		resume_sound ();
		return;
	    }
	}

	(new floppyFilePanel (shortcut))->run ();

	if (was_fullscreen)
	    toggle_fullscreen (0);
    }
    resume_sound ();
}

void gui_message (const char *format,...)
{
    char msg[2048];
    va_list parms;
    BAlert *alert;

    va_start (parms,format);
    vsprintf (msg, format, parms);
    va_end (parms);

    write_log (msg);

    alert = new BAlert ("UAE Information", msg, "Okay", NULL, NULL,
			B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
    alert->Go();
}

static int guijoybutton[MAX_JPORTS];
static int guijoyaxis[MAX_JPORTS][4];
static bool guijoychange;

void gui_gameport_button_change (int port, int button, int onoff)
{
        //write_log ("%d %d %d\n", port, button, onoff);
#ifdef RETROPLATFORM
        int mask = 0;
        if (button == JOYBUTTON_CD32_PLAY)
                mask = RP_JOYSTICK_BUTTON5;
        if (button == JOYBUTTON_CD32_RWD)
                mask = RP_JOYSTICK_BUTTON6;
        if (button == JOYBUTTON_CD32_FFW)
                mask = RP_JOYSTICK_BUTTON7;
        if (button == JOYBUTTON_CD32_GREEN)
                mask = RP_JOYSTICK_BUTTON4;
        if (button == JOYBUTTON_3 || button == JOYBUTTON_CD32_YELLOW)
                mask = RP_JOYSTICK_BUTTON3;
        if (button == JOYBUTTON_1 || button == JOYBUTTON_CD32_RED)
                mask = RP_JOYSTICK_BUTTON1;
        if (button == JOYBUTTON_2 || button == JOYBUTTON_CD32_BLUE)
                mask = RP_JOYSTICK_BUTTON2;
        rp_update_gameport (port, mask, onoff);
#endif
        if (onoff)
                guijoybutton[port] |= 1 << button;
        else
                guijoybutton[port] &= ~(1 << button);
        guijoychange = true;
}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
        int onoff = state ? 100 : 0;
        if (axis < 0 || axis > 3)
                return;
        if (max < 0) {
                if (guijoyaxis[port][axis] == 0)
                        return;
                if (guijoyaxis[port][axis] > 0)
                        guijoyaxis[port][axis]--;
        } else {
                if (state > max)
                        state = max;
                if (state < 0)
                        state = 0;
                guijoyaxis[port][axis] = max ? state * 127 / max : onoff;
#ifdef RETROPLATFORM
                if (axis == DIR_LEFT_BIT)
                        rp_update_gameport (port, RP_JOYSTICK_LEFT, onoff);
                if (axis == DIR_RIGHT_BIT)
                        rp_update_gameport (port, DIR_RIGHT_BIT, onoff);
                if (axis == DIR_UP_BIT)
                        rp_update_gameport (port, DIR_UP_BIT, onoff);
                if (axis == DIR_DOWN_BIT)
                        rp_update_gameport (port, DIR_DOWN_BIT, onoff);
#endif
        }
        guijoychange = true;
}

