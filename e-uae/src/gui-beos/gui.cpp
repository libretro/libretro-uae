 /*
  * UAE - The Un*x Amiga Emulator
  *
  * BeOS UI - or the beginnings of one
  *
  * Copyright 2004 Richard Drummond
  */

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gui.h"
#include "xwin.h"
#include "disk.h"
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

void gui_init (int argc, char **argv)
{
}

int gui_open (void)
{
    return -1;
}

void gui_notify_state (int state)
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

void gui_led (int led, int on)
{
}

void gui_hd_led (int led)
{
    static int resetcounter;

    int old = gui_data.hd;

    if (led == 0) {
	resetcounter--;
	if (resetcounter > 0)
	    return;
    }

    gui_data.hd = led;
    resetcounter = 6;
    if (old != gui_data.hd)
	gui_led (5, gui_data.hd);
}

void gui_cd_led (int led)
{
    static int resetcounter;

    int old = gui_data.cd;
    if (led == 0) {
	resetcounter--;
	if (resetcounter > 0)
	    return;
    }

    gui_data.cd = led;
    resetcounter = 6;
    if (old != gui_data.cd)
	gui_led (6, gui_data.cd);
}

void gui_display (int shortcut)
{
    if (shortcut >=0 && shortcut < 4) {
	/* If we're running full-screen, we must toggle
	 * to windowed mode before opening the dialog */
	int was_fullscreen;

	if (was_fullscreen = is_fullscreen ()) {
	    toggle_fullscreen ();
	    if (is_fullscreen ())
	        return;
	}

	(new floppyFilePanel (shortcut))->run ();

	if (was_fullscreen)
	    toggle_fullscreen ();
    }
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
