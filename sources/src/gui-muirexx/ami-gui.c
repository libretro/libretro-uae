 /*
  * UAE - The Un*x Amiga Emulator
  *
  * GUI interface (to be done).
  * Calls AREXX interface.
  *
  * Copyright 1996 Bernd Schmidt, Samuel Devulder
  * Copyright 2004-2006 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gui.h"
#include "disk.h"
#include "savestate.h"

#include <intuition/intuition.h>
#include <libraries/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <dos/dosextens.h>

/****************************************************************************/

extern int  rexx_init (void);
extern void rexx_exit (void);
extern void rexx_led (int led, int on);          /* ami-rexx.c */
extern void rexx_filename (int num, const char *name);
extern void rexx_handle_events (void);
extern void main_window_led (int led, int on);   /* ami-win.c */

/****************************************************************************/

extern struct AslIFace *IAsl;

/* File dialog types */
#define FILEDIALOG_INSERT_DF0    0
#define FILEDIALOG_INSERT_DF1    1
#define FILEDIALOG_INSERT_DF2    2
#define FILEDIALOG_INSERT_DF3    3
#define FILEDIALOG_LOAD_STATE    4
#define FILEDIALOG_SAVE_STATE    5
#define FILEDIALOG_MAX           6

#define FILEDIALOG_DRIVE(x) ((x)-FILEDIALOG_INSERT_DF0)

/* For remembering last directory used in file requesters */

static char *last_floppy_dir;
static char *last_savestate_dir;

static void free_last_floppy_dir (void)
{
	xfree (last_floppy_dir);
}

static void free_last_savestate_dir (void)
{
	xfree (last_savestate_dir);
}

static const char *get_last_floppy_dir (void)
{
    if (!last_floppy_dir) {
	static int done = 0;
	unsigned int len;

	if (!done) {
	    done = 1;
	    atexit (free_last_floppy_dir);
	}

	last_floppy_dir = my_strdup (currprefs.path_floppy.path[0]);
    }
    return last_floppy_dir;
}

static const char *get_last_savestate_dir (void)
{
    if (!last_savestate_dir) {
	static int done = 0;
	unsigned int len;

	if (!done) {
	    done = 1;
	    atexit (free_last_savestate_dir);
	}

//	last_savestate_dir = my_strdup (currprefs.path_savestate);
	last_savestate_dir = my_strdup (" ");
    }
    return last_savestate_dir;
}

static void set_last_floppy_dir (const char *path)
{
	xfree (last_floppy_dir);

    if (path) {
	unsigned int len = strlen (path);
	if (len) {
	    last_floppy_dir = malloc (len + 1);
	    if (last_floppy_dir)
		strcpy (last_floppy_dir, path);
	}
    }
}

static void set_last_savestate_dir (const char *path)
{
	xfree (last_savestate_dir);

    if (path) {
	unsigned int len = strlen (path);
	if (len) {
	    last_savestate_dir = malloc (len + 1);
	    if (last_savestate_dir)
		strcpy (last_savestate_dir, path);
	}
    }
}

static void do_file_dialog (unsigned int type)
{
    struct FileRequester *FileRequest;
    struct Window *win;

    char buf[80];
    char path[512];

    const char *req_prompt;
    const char *req_pattern = 0;
    const char *req_lastdir;
    int         req_do_save = FALSE;

#ifdef __amigaos4__
    int release_asl = 0;
#endif

    if (type >= FILEDIALOG_MAX)
	return;

    if (!AslBase) {
	AslBase = OpenLibrary ("asl.library", 36);
	if (!AslBase) {
	    write_log ("Can't open asl.library v36.\n");
	    return;
	} else {
#ifdef __amigaos4__
	    IAsl = (struct AslIFace *) GetInterface ((struct Library *)AslBase, "main", 1, NULL);
	    if (!IAsl) {
		CloseLibrary (AslBase);
		AslBase = 0;
		write_log ("Can't get asl.library interface\n");
	    }
#endif
	}
#ifdef __amigaos4__
    } else {
	IAsl->Obtain ();
	release_asl = 1;
#endif
    }

    FileRequest = AllocAslRequest (ASL_FileRequest, NULL);
    if (!FileRequest) {
	write_log ("Unable to allocate file requester.\n");
	return;
    }

    /* Find this task's default window */
    win = ((struct Process *) FindTask (NULL))->pr_WindowPtr;
    if (win == (struct Window *)-1)
	win = 0;

    /*
     * Prepare requester.
     */
    switch (type) {

	default: /* to stop GCC complaining */
	case FILEDIALOG_INSERT_DF0:
	case FILEDIALOG_INSERT_DF1:
	case FILEDIALOG_INSERT_DF2:
	case FILEDIALOG_INSERT_DF3:
	    sprintf (buf, "Select image to insert in drive DF%d:", FILEDIALOG_DRIVE(type));
	    req_prompt = buf;
	    req_pattern = "(#?.(ad(f|z)|dms|ipf|zip)#?|df?|?)";
	    req_lastdir = get_last_floppy_dir ();
	    break;

	case FILEDIALOG_SAVE_STATE:
	    req_prompt = "Select file to save emulator state to";
	    req_pattern = "#?.uss";
	    req_lastdir = get_last_savestate_dir ();
	    req_do_save = TRUE;
	    break;

	case FILEDIALOG_LOAD_STATE:
	    req_prompt = "Select saved state file to load";
	    req_pattern = "#?.uss";
	    req_lastdir = get_last_savestate_dir ();
	    break;
    }

    /*
     * Do the file request.
     */
    if (AslRequestTags (FileRequest,
			ASLFR_TitleText,      req_prompt,
			ASLFR_InitialDrawer,  req_lastdir,
			ASLFR_InitialPattern, req_pattern,
			ASLFR_DoPatterns,     req_pattern != 0,
			ASLFR_DoSaveMode,     req_do_save,
			ASLFR_RejectIcons,    TRUE,
			ASLFR_Window,         win,
			TAG_DONE)) {

	/*
	 * User selected a file.
	 *
	 * Construct file path to selected image.
	 */
	strcpy (path, FileRequest->fr_Drawer);
	if (strlen (path) && !(path[strlen (path) - 1] == ':' || path[strlen (path) - 1] == '/'))
	    strcat (path, "/");
	strcat (path, FileRequest->fr_File);

	/*
	 * Process selected file.
	 */
	switch (type) {

	    default: /* to stop GCC complaining */
	    case FILEDIALOG_INSERT_DF0:
	    case FILEDIALOG_INSERT_DF1:
	    case FILEDIALOG_INSERT_DF2:
	    case FILEDIALOG_INSERT_DF3:
		set_last_savestate_dir (FileRequest->fr_Drawer);
		strcpy (changed_prefs.df[FILEDIALOG_DRIVE(type)], path);
		break;
#ifdef SAVESTATE
	    case FILEDIALOG_SAVE_STATE:
		set_last_savestate_dir (FileRequest->fr_Drawer);
		savestate_initsave (path, 1, 0);
		save_state (path, "Description");
		break;

	    case FILEDIALOG_LOAD_STATE:
		set_last_savestate_dir (FileRequest->fr_Drawer);
		savestate_initsave (path, 1, 0);
		savestate_state = STATE_DORESTORE;
		write_log ("Restoring state from '%s'...\n", path);
		break;
#endif // SAVESTATE
	}
    }

    FreeAslRequest (FileRequest);

#ifdef __amigaos4__
    if (release_asl)
	IAsl->Release ();
#endif

    return;
}

/****************************************************************************/

int gui_init (void)
{
}

/****************************************************************************/

static int have_rexx = 0;

/****************************************************************************/

void gui_exit (void)
{
}

/****************************************************************************/

int gui_update (void)
{
    return 0;
}

/****************************************************************************/

void gui_flicker_led (int led, int unitnum, int status)
{
}

/****************************************************************************/

void gui_led (int led, int on)
{
    if (have_rexx)
	rexx_led (led, on);
}

/****************************************************************************/

void gui_filename (int num, const char *name)
{
    if (have_rexx)
	rexx_filename (num, name);
}

/****************************************************************************/

void gui_handle_events (void)
{
    if (have_rexx)
	rexx_handle_events();
}

/****************************************************************************/

void gui_fps (int fps, int idle)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

/****************************************************************************/

void gui_display (int shortcut)
{
    switch (shortcut) {

	case 0:
	case 1:
	case 2:
	case 3:
	    do_file_dialog (FILEDIALOG_INSERT_DF0 + shortcut);
	    break;
	case 4:
	    do_file_dialog (FILEDIALOG_LOAD_STATE);
	    break;
	case 5:
	    do_file_dialog (FILEDIALOG_SAVE_STATE);
	    break;
	default:
	    ;
    }
}

/****************************************************************************/

void gui_message (const char *format,...)
{
    char msg[2048];
    va_list parms;
    struct EasyStruct req;
    struct Window *win;

    va_start (parms,format);
    vsprintf ( msg, format, parms);
    va_end (parms);

    /* Find this task's default window */
    win = ((struct Process *) FindTask (NULL))->pr_WindowPtr;
    if (win == (struct Window *)-1)
	win = 0;

    req.es_StructSize   = sizeof req;
    req.es_Flags        = 0;
    req.es_Title        = (char *) PACKAGE_NAME " Information";
    req.es_TextFormat   = (char *) msg;
    req.es_GadgetFormat = (char *) "Okay";
    EasyRequest (win, &req, NULL, NULL);

    write_log (msg);
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

