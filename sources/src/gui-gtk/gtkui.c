/*
 * UAE - the Un*x Amiga Emulator
 *
 * Yet Another User Interface for the X11 version
 *
 * Copyright 1997, 1998 Bernd Schmidt
 * Copyright 1998 Michael Krause
 * Copyright 2003-2007 Richard Drummond
 * Copyright 2009-2010 Mustafa TUFAN
 *
 * The Tk GUI doesn't work.
 * The X Forms Library isn't available as source, and there aren't any
 * binaries compiled against glibc
 *
 * So let's try this...
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "autoconf.h"
#include "options.h"
#include "uae.h"
#include "memory_uae.h"
#include "custom.h"
#include "gui.h"
#include "newcpu.h"
#include "filesys.h"
#include "threaddep/thread.h"
#include "audio.h"
#include "savestate.h"
#include "debug.h"
#include "inputdevice.h"
#include "xwin.h"
#include "picasso96.h"
#include "gcc_warnings.h"

GCC_DIAG_OFF(strict-prototypes)
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
GCC_DIAG_ON(strict-prototypes)

#include "gui-gtk/cputypepanel.h"
#include "gui-gtk/cpuspeedpanel.h"
#include "gui-gtk/floppyfileentry.h"
#include "gui-gtk/led.h"
#include "gui-gtk/chipsettypepanel.h"
#include "gui-gtk/chipsetspeedpanel.h"
#include "gui-gtk/util.h"

//#define GUI_DEBUG
#ifdef  GUI_DEBUG
#define DEBUG_LOG write_log ( "%s: ", __func__); write_log
#else
#define DEBUG_LOG(...) { }
#endif

/* internal types */
static int gui_active;

static int gui_available;
unsigned int pause_uae = 0;

static GtkWidget *gui_window;

static GtkWidget *start_uae_widget;
static GtkWidget *stop_uae_widget;
static GtkWidget *pause_uae_widget;
static GtkWidget *reset_uae_widget;
static GtkWidget *debug_uae_widget;

static GtkWidget *chipsize_widget[5];
static GtkWidget *bogosize_widget[4];
static GtkWidget *fastsize_widget[5];
static GtkWidget *z3size_widget[10];
#ifdef PICASSO96
static GtkWidget *p96size_widget[7];
#endif
static GtkWidget *rom_text_widget;
static GtkWidget *rom_change_widget;
static GtkWidget  *sstate_text_widget, *sstate_change_widget, *sstate_load_widget, *sstate_save_widget;

static GtkWidget *floppy_widget[4];
static char *new_disk_string[4];

static gpointer power_led;

static GtkWidget *ctpanel;
static GtkWidget *ftpanel;
static GtkWidget *cspanel;
static GtkWidget *chipsettype_panel;
static GtkWidget *chipsetspeed_panel;

static GtkWidget *hdpanel;
static GtkWidget *memorypanel;

static GtkWidget *sound_widget[4], *sound_ch_widget[3], *sound_in_widget[3], *sound_fl_widget[5];
static GtkWidget *drvspeed_widget[5];
static GtkWidget *cpu_widget[6], *fpu_widget[4];

#ifdef JIT
static GtkWidget *jit_page;
static GtkWidget *compbyte_widget[4], *compword_widget[4], *complong_widget[4];
static GtkWidget *compaddr_widget[4];
static GtkWidget *compnf_widget[2];
static GtkWidget *compfpu_widget[2], *comp_hardflush_widget[2];
static GtkWidget *comp_constjump_widget[2];
static GtkAdjustment *cachesize_adj;
#endif

#ifdef XARCADE
# define JOY_WIDGET_COUNT 9
#else
# define JOY_WIDGET_COUNT 7
#endif
static GtkWidget *joy_widget[2][JOY_WIDGET_COUNT];

static unsigned int prevledstate;

static GtkWidget *hdlist_widget;
static int selected_hd_row;
static GtkWidget *hdchange_button, *hddel_button;
static GtkWidget *devname_entry, *volname_entry, *path_entry;
static GtkWidget *readonly_widget, *bootpri_widget;
static GtkWidget *leds_on_screen_widget;
static GtkWidget *dirdlg;
static GtkWidget *dirdlg_ok;
static char dirdlg_devname[256], dirdlg_volname[256], dirdlg_path[256], floppydlg_path[256];

enum hdlist_cols {
    HDLIST_DEVICE,
    HDLIST_VOLUME,
    HDLIST_PATH,
    HDLIST_READONLY,
    HDLIST_HEADS,
    HDLIST_CYLS,
    HDLIST_SECS,
    HDLIST_RSRVD,
    HDLIST_SIZE,
    HDLIST_BLKSIZE,
    HDLIST_BOOTPRI,
//    HDLIST_DONOTMOUNT,
//    HDLIST_AUTOBOOT,
//    HDLIST_FILESYSDIR,
//    HDLIST_HDC,
//    HDLIST_FLAGS,
    HDLIST_MAX_COLS
};

static const char *hdlist_col_titles[] = {
     "Device",
     "Volume",
     "File/Directory",
     "R/O",
     "Heads",
     "Cyl.",
     "Sec.",
     "Rsrvd",
     "Size",
     "Blksize",
     "Boot Pri",
//     "Donotmount",
//     "Autoboot",
//    "Filesysdir?"
//    "HDC"
//    "Flags"
     NULL
};


static smp_comm_pipe to_gui_pipe;   // For sending messages to the GUI from UAE
static smp_comm_pipe from_gui_pipe; // For sending messages from the GUI to UAE

/*
 * Messages sent to GUI from UAE via to_gui_pipe
 */
enum gui_commands {
    GUICMD_STATE_CHANGE,	// Tell GUI about change in emulator state.
    GUICMD_SHOW,			// Show yourself
    GUICMD_UPDATE,			// Refresh your state from changed preferences
    GUICMD_DISKCHANGE,		// Hey! A disk has been changed. Do something!
    GUICMD_MSGBOX,			// Display a message box for me, please
    GUICMD_NEW_ROMLIST,		// The ROM list has been updated.
    GUICMD_FLOPPYDLG,		// Open a floppy insert dialog
    GUICMD_FULLSCREEN,		// Fullscreen mode was toggled; update checkboxes
    GUICMD_PAUSE,			// We're now paused, in case you didn't notice
    GUICMD_UNPAUSE			// We're now running.
};

enum uae_commands {
    UAECMD_START,
    UAECMD_STOP,
    UAECMD_QUIT,
    UAECMD_RESET,
    UAECMD_PAUSE,
    UAECMD_RESUME,
    UAECMD_DEBUG,
    UAECMD_SAVE_CONFIG,
    UAECMD_EJECTDISK,
    UAECMD_INSERTDISK,
    UAECMD_SELECT_ROM,
	UAECMD_SAVESTATE_LOAD,
	UAECMD_SAVESTATE_SAVE
};


static uae_sem_t gui_sem;			// For mutual exclusion on various prefs settings
static uae_sem_t gui_update_sem;	// For synchronization between gui_update() and the GUI thread
static uae_sem_t gui_init_sem;		// For the GUI thread to tell UAE that it's ready.
static uae_sem_t gui_quit_sem;		// For the GUI thread to tell UAE that it's quitting.

static volatile int quit_gui = 0, quitted_gui = 0;


/* internal prototypes */
static void create_guidlg (void);

static void do_message_box (const gchar *title, const gchar *message, gboolean modal, gboolean wait);
static void handle_message_box_request (smp_comm_pipe *msg_pipe);
static GtkWidget *make_message_box (const gchar *title, const gchar *message, int modal, uae_sem_t *sem);
void on_message_box_quit (GtkWidget *w, gpointer user_data);
void on_vstat_toggle(GtkWidget *widget, gpointer statusbar);

/* external prototypes */
extern void clearallkeys (void);



static void uae_pause (void)
{
	write_comm_pipe_int (&from_gui_pipe, GUICMD_PAUSE , 1);
}

static void uae_resume (void)
{
	write_comm_pipe_int (&to_gui_pipe, GUICMD_UNPAUSE, 1);
}

static void set_mem32_widgets_state (void)
{
	int enable = changed_prefs.cpu_model >= 68020 && ! changed_prefs.address_space_24;

#ifdef AUTOCONFIG
    unsigned int i;

	for (i = 0; i < 10; i++)
		gtk_widget_set_sensitive (z3size_widget[i], enable);

# ifdef PICASSO96
	for (i = 0; i < 7; i++)
		gtk_widget_set_sensitive (p96size_widget[i], enable);
#endif
#endif
#ifdef JIT
	gtk_widget_set_sensitive (jit_page, changed_prefs.cpu_model >= 68020);
#endif
}

static void set_cpu_state (void)
{
/*
	int i;
	unsigned int lvl;
	lvl = (changed_prefs.cpu_model - 68000) / 10;

	DEBUG_LOG ("set_cpu_state: %d %d %d\n", lvl,
	changed_prefs.address_space_24, changed_prefs.m68k_speed);

    cputypepanel_set_cpulevel      (CPUTYPEPANEL (ctpanel), lvl);
    cputypepanel_set_addr24bit     (CPUTYPEPANEL (ctpanel), changed_prefs.address_space_24);

    cpuspeedpanel_set_cpuspeed     (CPUSPEEDPANEL (cspanel), changed_prefs.m68k_speed);
    cpuspeedpanel_set_cpuidle      (CPUSPEEDPANEL (cspanel), changed_prefs.cpu_idle);
    set_mem32_widgets_state ();
*/
}

static void set_fpu_state (void)
{
#ifdef FPU

#endif
}

static void set_chipset_state (void)
{
	chipsettypepanel_set_chipset_mask		(CHIPSETTYPEPANEL  (chipsettype_panel),  currprefs.chipset_mask);
	chipsettypepanel_set_ntscmode			(CHIPSETTYPEPANEL  (chipsettype_panel),  currprefs.ntscmode);
	chipsetspeedpanel_set_framerate			(CHIPSETSPEEDPANEL (chipsetspeed_panel), currprefs.gfx_framerate);
	chipsetspeedpanel_set_collision_level	(CHIPSETSPEEDPANEL (chipsetspeed_panel), currprefs.collision_level);
	chipsetspeedpanel_set_immediate_blits	(CHIPSETSPEEDPANEL (chipsetspeed_panel), currprefs.immediate_blits);
}

static void set_sound_state (void)
{
	int stereo = currprefs.sound_stereo + currprefs.sound_mixed_stereo_delay;
	unsigned int i;

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_widget[currprefs.produce_sound]), 1);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_ch_widget[stereo]), 1);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_in_widget[currprefs.sound_interpol]), 1);

	if (currprefs.sound_filter == FILTER_SOUND_OFF) {
		i = 0;
	} else {
		if (currprefs.sound_filter_type == FILTER_SOUND_TYPE_A500) i = 1;
		if (currprefs.sound_filter_type == FILTER_SOUND_TYPE_A1200) i = 3;
		if (currprefs.sound_filter == FILTER_SOUND_ON) i++;
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_fl_widget[i]), 1);
}

static void set_mem_state (void)
{
    int t, t2;

    t = 0;
    t2 = currprefs.chipmem_size;
    while (t < 4 && t2 > 0x80000)
	t++, t2 >>= 1;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chipsize_widget[t]), 1);

    t = 0;
    t2 = currprefs.bogomem_size;
    while (t < 3 && t2 >= 0x80000)
	t++, t2 >>= 1;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bogosize_widget[t]), 1);

    t = 0;
    t2 = currprefs.fastmem_size;
    while (t < 4 && t2 >= 0x100000)
	t++, t2 >>= 1;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fastsize_widget[t]), 1);

    t = 0;
    t2 = currprefs.z3fastmem_size;
    while (t < 9 && t2 >= 0x100000)
	t++, t2 >>= 1;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (z3size_widget[t]), 1);

#ifdef PICASSO96
    t = 0;
    t2 = currprefs.rtgmem_size;
    while (t < 6 && t2 >= 0x100000)
	t++, t2 >>= 1;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (p96size_widget[t]), 1);
#endif

    gtk_label_set_text (GTK_LABEL (rom_text_widget), changed_prefs.romfile[0]!='\0' ?
					changed_prefs.romfile : currprefs.romfile);
}

#ifdef JIT
static void set_comp_state (void)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (compbyte_widget[currprefs.comptrustbyte]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (compword_widget[currprefs.comptrustword]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (complong_widget[currprefs.comptrustlong]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (compaddr_widget[currprefs.comptrustnaddr]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (compnf_widget[currprefs.compnf]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (comp_hardflush_widget[currprefs.comp_hardflush]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (comp_constjump_widget[currprefs.comp_constjump]), 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (compfpu_widget[currprefs.compfpu]), 1);
}
#endif

static void set_misc_state (void)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (leds_on_screen_widget), currprefs.leds_on_screen);
}

/*
 * Temporary hacks for joystick widgets
 */

/*
 * widget 0 = none
 *        1 = joy 0
 *        2 = joy 1
 *        3 = mouse
 *        4 = numpad
 *        5 = cursor
 *        6 = other
 *        7 = xarcade1
 *        8 = xarcade2
 */
static int map_jsem_to_widget (int jsem)
{
    int widget = 0;

    if (jsem == JSEM_END)
		widget = 0;
    else if (jsem == JSEM_MICE)
		widget = 3;
    else if (jsem == JSEM_JOYS || jsem == JSEM_JOYS + 1 )
		widget = jsem - JSEM_JOYS + 1;
    else if (jsem >= JSEM_KBDLAYOUT)
		widget = jsem - JSEM_KBDLAYOUT + 4;

    return widget;
}

static int map_widget_to_jsem (int widget)
{
   int jsem;

   switch (widget) {
	default:
	case 0: jsem = JSEM_END;           break;
	case 1: jsem = JSEM_JOYS;          break;
	case 2: jsem = JSEM_JOYS + 1;      break;
	case 3: jsem = JSEM_MICE;          break;
	case 4: jsem = JSEM_KBDLAYOUT;     break;
	case 5: jsem = JSEM_KBDLAYOUT + 1; break;
	case 6: jsem = JSEM_KBDLAYOUT + 2; break;
	case 7: jsem = JSEM_KBDLAYOUT + 3; break;
	case 8: jsem = JSEM_KBDLAYOUT + 4; break;
   }

   return jsem;
}

static void set_joy_state (void)
{
	int j0t = map_jsem_to_widget (changed_prefs.jports[0].id);
	int j1t = map_jsem_to_widget (changed_prefs.jports[1].id);

	int joy_count = inputdevice_get_device_total (IDTYPE_JOYSTICK);
	int i;

	if (j0t != 0 && j0t == j1t) {
		/* Can't happen */
		j0t++;
		j0t %= 7;
	}

	for (i = 0; i < JOY_WIDGET_COUNT; i++) {
		if (i == 1 && joy_count == 0) continue;
		if (i == 2 && joy_count <= 1) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (joy_widget[0][i]), j0t == i);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (joy_widget[1][i]), j1t == i);
		gtk_widget_set_sensitive (joy_widget[0][i], i == 0 || j1t != i);
		gtk_widget_set_sensitive (joy_widget[1][i], i == 0 || j0t != i);
	}
}

#ifdef FILESYS
static void set_hd_state (void)
{
	char  texts[HDLIST_MAX_COLS][256];
	char *tptrs[HDLIST_MAX_COLS];
	int nr = nr_units ();
	int i;
//	UnitInfo *ui;

	DEBUG_LOG ("set_hd_state\n");
	for (i=0; i<HDLIST_MAX_COLS; i++)
		tptrs[i] = texts[i];

		gtk_clist_freeze (GTK_CLIST (hdlist_widget));
		gtk_clist_clear (GTK_CLIST (hdlist_widget));

		for (i = 0; i < nr; i++) {
			int     secspertrack=0, surfaces=0, reserved=0, blocksize=0, bootpri=0;
			uae_u64 size = 0;
			int     cylinders=0, readonly=0, flags=0;
			char   	*devname=NULL, *volname=NULL, *rootdir=NULL, *filesysdir=NULL;
			int	ret = 0;

			/* We always use currprefs.mountinfo for the GUI.  The filesystem
			   code makes a private copy which is updated every reset.  */
			struct mountedinfo mi;
			ret = get_filesys_unitconfig (&currprefs, i, &mi);
/*		failure = get_filesys_unit (i,
				    &devname, &volname, &rootdir, &readonly,
				    &secspertrack, &surfaces, &reserved,
				    &cylinders, &size, &blocksize, &bootpri,
				    &filesysdir, &flags); */

		if (ret == FILESYS_HARDDRIVE)
		    write_log ("Get Filesys Unit: HDD\n");
		if (ret == FILESYS_HARDFILE_RDB)
		    write_log ("Get Filesys Unit: HF RDB\n");
		if (ret == FILESYS_VIRTUAL)
		    write_log ("Get Filesys Unit: VIRT\n");

	//	ui = &mountinfo.ui[i]
          if (secspertrack == 0)
               strcpy (texts[HDLIST_DEVICE], "N/A" );
           else
                strncpy (texts[HDLIST_DEVICE], devname, 255);
		if (volname)
		    strncpy (texts[HDLIST_VOLUME], volname, 255);
	
	    sprintf (texts[HDLIST_HEADS],   "%d", surfaces);
	    sprintf (texts[HDLIST_CYLS],    "%d", cylinders);
	    sprintf (texts[HDLIST_SECS],    "%d", secspertrack);
	    sprintf (texts[HDLIST_RSRVD],   "%d", reserved);
	    sprintf (texts[HDLIST_SIZE],    "%lu", size);
	    sprintf (texts[HDLIST_BLKSIZE], "%d", blocksize);

		if (rootdir)
			strcpy  (texts[HDLIST_PATH],     rootdir);
		strcpy  (texts[HDLIST_READONLY], readonly ? "Y" : "N");
		sprintf (texts[HDLIST_BOOTPRI], "%d", bootpri);

		gtk_clist_append (GTK_CLIST (hdlist_widget), tptrs);
    }
    gtk_clist_thaw (GTK_CLIST (hdlist_widget));
    gtk_widget_set_sensitive (hdchange_button, false);
    gtk_widget_set_sensitive (hddel_button, false);
    DEBUG_LOG ("set_hd_state done\n");
}
#endif

static void set_floppy_state( void )
{
    unsigned int i;
    switch (currprefs.floppy_speed) {
	case 0:   i = 0;
        case 100: i = 1; 
        case 200: i = 2; 
        case 400: i = 3; 
        case 800: i = 4;
        default:  i = 1;
    }
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (drvspeed_widget[i]), 1);

    floppyfileentry_set_filename (FLOPPYFILEENTRY (floppy_widget[0]), currprefs.floppyslots[0].df);
    floppyfileentry_set_filename (FLOPPYFILEENTRY (floppy_widget[1]), currprefs.floppyslots[1].df);
    floppyfileentry_set_filename (FLOPPYFILEENTRY (floppy_widget[2]), currprefs.floppyslots[2].df);
    floppyfileentry_set_filename (FLOPPYFILEENTRY (floppy_widget[3]), currprefs.floppyslots[3].df);
}

static void update_state (void)
{
    set_cpu_state ();
#ifdef FPU
    set_fpu_state ();
#endif
    set_joy_state ();
    set_sound_state ();
#ifdef JIT
    set_comp_state ();
#endif
    set_mem_state ();
    set_floppy_state ();
#ifdef FILESYS
    set_hd_state ();
#endif
    set_chipset_state ();
    set_misc_state ();
}

static void update_buttons (void)
{
    if (gui_window) {
		int running = pause_uae ? 0 : 1;
		int paused  = pause_uae ? 1 : 0;

		gtk_widget_set_sensitive (start_uae_widget, !running && !paused);
		gtk_widget_set_sensitive (stop_uae_widget,  running || paused);
		gtk_widget_set_sensitive (pause_uae_widget, running || paused);
		gtk_widget_set_sensitive (reset_uae_widget, running);

		gtk_widget_set_sensitive (hdpanel,     !running && !paused);
		gtk_widget_set_sensitive (memorypanel, !running && !paused);
		gtk_widget_set_sensitive (rom_change_widget, !running && !paused);
//		gtk_widget_set_sensitive (sstate_change_widget, !running && !paused);

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (pause_uae_widget), paused);
    }
}


#define MY_IDLE_PERIOD        250
#define LEDS_CALLBACK_PERIOD  1000
/*
 * my_idle()
 *
 * This function is added as a callback to the GTK+ mainloop
 * and is run every 250ms. It handles messages sent from UAE.
 */
static int my_idle (void)
{
    if (quit_gui) {
		gtk_main_quit ();
		return 0;
    }

    while (comm_pipe_has_data (&to_gui_pipe)) {
	int cmd = read_comm_pipe_int_blocking (&to_gui_pipe);
	int n;
	DEBUG_LOG ("GUI got command:%d\n", cmd);
	switch (cmd) {
	 case GUICMD_STATE_CHANGE: {
	     int state = read_comm_pipe_int_blocking (&to_gui_pipe);
	     update_buttons ();
	     break;
	 }
	 case GUICMD_SHOW:
	 case GUICMD_FLOPPYDLG:
	    if (!gui_window) {
			create_guidlg ();
			update_state ();
			update_buttons (); //FIXME
	    }
	    if (cmd == GUICMD_SHOW) {
			gtk_widget_show (gui_window);
#if GTK_MAJOR_VERSION >= 2
			gtk_window_present (GTK_WINDOW (gui_window));
			gui_active = 1;
#endif
	    } else {
			n = read_comm_pipe_int_blocking (&to_gui_pipe);
			floppyfileentry_do_dialog (FLOPPYFILEENTRY (floppy_widget[n]));
	    }
	    break;
	 case GUICMD_DISKCHANGE:
	    n = read_comm_pipe_int_blocking (&to_gui_pipe);
	    if (floppy_widget[n])
			floppyfileentry_set_filename (FLOPPYFILEENTRY (floppy_widget[n]), currprefs.floppyslots[n].df);
	    break;
	 case GUICMD_UPDATE:
	    update_state ();
	    uae_sem_post (&gui_update_sem);
	    gui_active = 1;
	    DEBUG_LOG ("GUICMD_UPDATE done\n");
	    break;
	 case GUICMD_MSGBOX:
	    handle_message_box_request(&to_gui_pipe);
	    break;
#if 0
	 case GUICMD_FLOPPYDLG:
	    n = read_comm_pipe_int_blocking (&to_gui_pipe);
	    floppyfileentry_do_dialog (FLOPPYFILEENTRY (floppy_widget[n]));
	    break;
#endif
	}
    }
    gui_handle_events();
    return 1;
}

static int leds_callback (void)
{
    unsigned int leds = 0; //= gui_ledstate;
    unsigned int i = 0;

    if (!quit_gui) {
	for ( ; i < 5; i++) {
	    GtkWidget *widget = i ? floppy_widget[i-1] : power_led;
	    unsigned int mask = 1 << i;
	    unsigned int on = leds & mask;

	    if (!widget)
		continue;

	   if (on == (prevledstate & mask))
		continue;

	    if (i > 0)
		floppyfileentry_set_led (FLOPPYFILEENTRY (widget), on);
	    else {
		static GdkColor red   = {0, 0xffff, 0x0000, 0x0000};
		static GdkColor black = {0, 0x0000, 0x0000, 0x0000};
		led_set_color (LED (widget), on ? red : black);
	    }
	}
	prevledstate = leds;
    }
    return 1;
}

static int find_current_toggle (GtkWidget **widgets, int count)
{
    int i;
    for (i = 0; i < count; i++)
	if (GTK_TOGGLE_BUTTON (*widgets++)->active)
	    return i;
    DEBUG_LOG ("GTKUI: Can't happen!\n");
    return -1;
}

static void joy_changed (void)
{
	if (! gui_active)
		return;
	changed_prefs.jports[0].id = map_widget_to_jsem (find_current_toggle (joy_widget[0], JOY_WIDGET_COUNT));
	changed_prefs.jports[1].id = map_widget_to_jsem (find_current_toggle (joy_widget[1], JOY_WIDGET_COUNT));

//	write_log("Joystick Port 0: %d\n", changed_prefs.jports[0].id);
//	write_log("Joystick Port 1: %d\n", changed_prefs.jports[1].id);

	if (changed_prefs.jports[0].id != currprefs.jports[0].id || changed_prefs.jports[1].id != currprefs.jports[1].id)
		inputdevice_config_change();

	set_joy_state ();
}

static void chipsize_changed (void)
{
    int t = find_current_toggle (chipsize_widget, 5);
    changed_prefs.chipmem_size = 0x80000 << t;
    for (t = 0; t < 5; t++)
	gtk_widget_set_sensitive (fastsize_widget[t], changed_prefs.chipmem_size <= 0x200000);
    if (changed_prefs.chipmem_size > 0x200000) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fastsize_widget[0]), 1);
		changed_prefs.fastmem_size = 0;
    }
}

static void bogosize_changed (void)
{
    int t = find_current_toggle (bogosize_widget, 4);
    switch (t) {
	case 0: changed_prefs.bogomem_size = 0;        break;
	case 1: changed_prefs.bogomem_size = 0x080000; break;
	case 2: changed_prefs.bogomem_size = 0x100000; break;
	case 3: changed_prefs.bogomem_size = 0x1C0000; break;
    }
}

static void fastsize_changed (void)
{
    int t = find_current_toggle (fastsize_widget, 5);
    changed_prefs.fastmem_size = (0x80000 << t) & ~0x80000;
}

static void z3size_changed (void)
{
    int t = find_current_toggle (z3size_widget, 10);
    changed_prefs.z3fastmem_size = (0x80000 << t) & ~0x80000;
}

#ifdef PICASSO96
static void p96size_changed (void)
{
    int t = find_current_toggle (p96size_widget, 7);
    changed_prefs.rtgmem_size = (0x80000 << t) & ~0x80000;
}
#endif

static void drvspeed_changed (void)
{
	switch (find_current_toggle (sound_widget, 5)) {
		case 0: changed_prefs.floppy_speed = 0; 
		case 1: changed_prefs.floppy_speed = 100; 
		case 2: changed_prefs.floppy_speed = 200; 
		case 3: changed_prefs.floppy_speed = 400; 
		case 4: changed_prefs.floppy_speed = 800; 
	}
}

static void sound_changed (void)
{
    changed_prefs.produce_sound = find_current_toggle (sound_widget, 4);
    changed_prefs.sound_stereo = find_current_toggle (sound_ch_widget, 3);
    changed_prefs.sound_mixed_stereo_delay = 0;
    if (changed_prefs.sound_stereo == 2)
		changed_prefs.sound_mixed_stereo_delay = changed_prefs.sound_stereo = 1;
    	changed_prefs.sound_interpol = find_current_toggle (sound_in_widget, 3);
    	int t = find_current_toggle (sound_fl_widget, 5);

    if (t == 0)
		changed_prefs.sound_filter = FILTER_SOUND_OFF;
    else {
		changed_prefs.sound_filter = (t & 1) ? FILTER_SOUND_EMUL : FILTER_SOUND_ON;
		changed_prefs.sound_filter_type = (t > 2 ? FILTER_SOUND_TYPE_A1200 : FILTER_SOUND_TYPE_A500);
    }
}

#ifdef JIT
static void comp_changed (void)
{
    changed_prefs.cachesize=cachesize_adj->value;
    changed_prefs.comptrustbyte = find_current_toggle (compbyte_widget, 4);
    changed_prefs.comptrustword = find_current_toggle (compword_widget, 4);
    changed_prefs.comptrustlong = find_current_toggle (complong_widget, 4);
    changed_prefs.comptrustnaddr = find_current_toggle (compaddr_widget, 4);
    changed_prefs.compnf = find_current_toggle (compnf_widget, 2);
    changed_prefs.comp_hardflush = find_current_toggle (comp_hardflush_widget, 2);
    changed_prefs.comp_constjump = find_current_toggle (comp_constjump_widget, 2);
    changed_prefs.compfpu= find_current_toggle (compfpu_widget, 2);
}
#endif

static void cpu_changed (void)
{
}

static void fpu_changed (void)
{
}

static void on_start_clicked (void)
{
    DEBUG_LOG ("Start button clicked.\n");

    write_comm_pipe_int (&from_gui_pipe, UAECMD_START, 1);
}

static void on_stop_clicked (void)
{
    DEBUG_LOG ("Stop button clicked.\n");

    write_comm_pipe_int (&from_gui_pipe, UAECMD_STOP, 1);
}

static void on_reset_clicked (void)
{
    DEBUG_LOG ("Reset button clicked.\n");

    if (!quit_gui)
	write_comm_pipe_int (&from_gui_pipe, UAECMD_RESET, 1);
}

#ifdef DEBUGGER
static void on_debug_clicked (void)
{
    DEBUG_LOG ("Called\n");

    if (!quit_gui)
	write_comm_pipe_int (&from_gui_pipe, UAECMD_DEBUG, 1);
}
#endif

static void on_quit_clicked (void)
{
    DEBUG_LOG ("Quit button clicked.\n");

    if (!quit_gui)
	write_comm_pipe_int (&from_gui_pipe, UAECMD_QUIT, 1);
}

static void on_pause_clicked (GtkWidget *widget, gpointer data)
{
    DEBUG_LOG ("Called with %d\n", GTK_TOGGLE_BUTTON (widget)->active == true );

    if (!quit_gui) {
		write_comm_pipe_int (&from_gui_pipe, GTK_TOGGLE_BUTTON (widget)->active ? UAECMD_PAUSE : UAECMD_RESUME, 1);
    }
}


static char *gui_romname;
static char *gui_sstate_name;

static void disc_changed (FloppyFileEntry *ffe, gpointer p)
{
    int num = GPOINTER_TO_INT(p);
    char *s = floppyfileentry_get_filename(ffe);
    int len;

    if (quit_gui)
	return;

    if(s == NULL || strlen(s) == 0) {
	write_comm_pipe_int (&from_gui_pipe, UAECMD_EJECTDISK, 0);
	write_comm_pipe_int (&from_gui_pipe, GPOINTER_TO_INT(p), 1);
    } else {
	/* Get the pathname, not including the filename
	 * Set floppydlg_path to this, so that when the requester
	 * dialog pops up again, we don't have to navigate to the same place. */
	len = strrchr(s, '/') - s;
	if (len > 254) len = 254;
	strncpy(floppydlg_path, s, len);
	floppydlg_path[len] = '\0';

	uae_sem_wait (&gui_sem);
	if (new_disk_string[num] != 0)
	    xfree (new_disk_string[num]);
	new_disk_string[num] = strdup (s);
	uae_sem_post (&gui_sem);
	write_comm_pipe_int (&from_gui_pipe, UAECMD_INSERTDISK, 0);
	write_comm_pipe_int (&from_gui_pipe, num, 1);
    }
}

static char fsbuffer[100];

static GtkWidget *make_file_selector (const char *title,
				      void (*insertfunc)(GtkObject *),
				      void (*closefunc)(gpointer))
{
    GtkWidget *p = gtk_file_selection_new (title);
    gtk_signal_connect (GTK_OBJECT (p), "destroy", (GtkSignalFunc) closefunc, p);

    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (p)->ok_button),
			       "clicked", (GtkSignalFunc) insertfunc,
			       GTK_OBJECT (p));
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (p)->cancel_button),
			       "clicked", (GtkSignalFunc) gtk_widget_destroy,
			       GTK_OBJECT (p));

#if 0
    gtk_window_set_title (GTK_WINDOW (p), title);
#endif

    gtk_widget_show (p);
    return p;
}

static void filesel_set_path (GtkWidget *p, const char *path)
{
    size_t len = strlen (path);
    if (len > 0 && ! access (path, R_OK)) {
	char *tmp = xmalloc (char, len + 2);
	strcpy (tmp, path);
	strcat (tmp, "/");
	gtk_file_selection_set_filename (GTK_FILE_SELECTION (p),
					 tmp);
    }
}

static GtkWidget *rom_selector;

static void did_close_rom (gpointer gdata)
{
    gtk_widget_set_sensitive (rom_change_widget, 1);
}

static void did_rom_select (GtkObject *o)
{
    const char *s = gtk_file_selection_get_filename (GTK_FILE_SELECTION (rom_selector));

    if (quit_gui)
		return;

    gtk_widget_set_sensitive (rom_change_widget, 1);

    uae_sem_wait (&gui_sem);
    gui_romname = strdup (s);
    uae_sem_post (&gui_sem);
    gtk_label_set_text (GTK_LABEL (rom_text_widget), gui_romname);
    write_comm_pipe_int (&from_gui_pipe, UAECMD_SELECT_ROM, 0);
    gtk_widget_destroy (rom_selector);
}

static void did_romchange (GtkWidget *w, gpointer data)
{
    gtk_widget_set_sensitive (rom_change_widget, 0);

    rom_selector = make_file_selector ("Select a ROM file", did_rom_select, did_close_rom);
    filesel_set_path (rom_selector, currprefs.path_rom.path[0]);
}

static void add_empty_vbox (GtkWidget *tobox)
{
    GtkWidget *thing = gtk_vbox_new (false, 0);
    gtk_widget_show (thing);
    gtk_box_pack_start (GTK_BOX (tobox), thing, true, true, 0);
}

static void add_empty_hbox (GtkWidget *tobox)
{
    GtkWidget *thing = gtk_hbox_new (false, 0);
    gtk_widget_show (thing);
    gtk_box_pack_start (GTK_BOX (tobox), thing, true, true, 0);
}

static void add_centered_to_vbox (GtkWidget *vbox, GtkWidget *w)
{
    GtkWidget *hbox = gtk_hbox_new (true, 0);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (hbox), w, true, false, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, false, true, 0);
}

static GtkWidget *make_labelled_widget (const char *str, GtkWidget *thing)
{
    GtkWidget *label = gtk_label_new (str);
    GtkWidget *hbox2 = gtk_hbox_new (false, 4);

    gtk_widget_show (label);
    gtk_widget_show (thing);

    gtk_box_pack_start (GTK_BOX (hbox2), label, false, true, 0);
    gtk_box_pack_start (GTK_BOX (hbox2), thing, false, true, 0);

    return hbox2;
}

static GtkWidget *add_labelled_widget_centered (const char *str, GtkWidget *thing, GtkWidget *vbox)
{
    GtkWidget *w = make_labelled_widget (str, thing);
    gtk_widget_show (w);
    add_centered_to_vbox (vbox, w);
    return w;
}

static int make_radio_group (const char **labels, GtkWidget *tobox,
			      GtkWidget **saveptr, gint t1, gint t2,
			      void (*sigfunc) (void), int count, GSList *group)
{
    int t = 0;

    while (*labels && (count == -1 || count-- > 0)) {
	GtkWidget *thing = gtk_radio_button_new_with_label (group, *labels++);
	group = gtk_radio_button_group (GTK_RADIO_BUTTON (thing));

	*saveptr++ = thing;
	gtk_widget_show (thing);
	gtk_box_pack_start (GTK_BOX (tobox), thing, t1, t2, 0);
	gtk_signal_connect (GTK_OBJECT (thing), "clicked", (GtkSignalFunc) sigfunc, NULL);
	t++;
    }
    return t;
}

static GtkWidget *make_radio_group_box (const char *title, const char **labels,
					GtkWidget **saveptr, int horiz,
					void (*sigfunc) (void))
{
    GtkWidget *frame, *newbox;

    frame = gtk_frame_new (title);
    newbox = (horiz ? gtk_hbox_new : gtk_vbox_new) (false, 4);
    gtk_widget_show (newbox);
    gtk_container_set_border_width (GTK_CONTAINER (newbox), 4);
    gtk_container_add (GTK_CONTAINER (frame), newbox);
    make_radio_group (labels, newbox, saveptr, horiz, !horiz, sigfunc, -1, NULL);
    return frame;
}

static GtkWidget *make_radio_group_box_1 (const char *title, const char **labels,
					  GtkWidget **saveptr, int horiz,
					  void (*sigfunc) (void), int elts_per_column)
{
    GtkWidget *frame, *newbox;
    GtkWidget *column;
    GSList *group = 0;

    frame = gtk_frame_new (title);
    column = (horiz ? gtk_vbox_new : gtk_hbox_new) (false, 4);
    gtk_container_add (GTK_CONTAINER (frame), column);
    gtk_widget_show (column);

    while (*labels) {
	int count;
	newbox = (horiz ? gtk_hbox_new : gtk_vbox_new) (false, 4);
	gtk_widget_show (newbox);
	gtk_container_set_border_width (GTK_CONTAINER (newbox), 4);
	gtk_container_add (GTK_CONTAINER (column), newbox);
	count = make_radio_group (labels, newbox, saveptr, horiz, !horiz, sigfunc, elts_per_column, group);
	labels += count;
	saveptr += count;
	group = gtk_radio_button_group (GTK_RADIO_BUTTON (saveptr[-1]));
    }
    return frame;
}


static GtkWidget *make_file_container (const char *title, GtkWidget *vbox)
{
    GtkWidget *thing = gtk_frame_new (title);
    GtkWidget *buttonbox = gtk_hbox_new (false, 4);

    gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 4);
    gtk_container_add (GTK_CONTAINER (thing), buttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), thing, false, true, 0);
    gtk_widget_show (buttonbox);
    gtk_widget_show (thing);

    return buttonbox;
}

static GtkWidget *make_file_widget (GtkWidget *buttonbox)
{
    GtkWidget *thing, *subthing;
    GtkWidget *subframe = gtk_frame_new (NULL);

    gtk_frame_set_shadow_type (GTK_FRAME (subframe), GTK_SHADOW_ETCHED_OUT);
    gtk_box_pack_start (GTK_BOX (buttonbox), subframe, true, true, 0);
    gtk_widget_show (subframe);
    subthing = gtk_vbox_new (false, 0);
    gtk_widget_show (subthing);
    gtk_container_add (GTK_CONTAINER (subframe), subthing);
    thing = gtk_label_new ("");
    gtk_widget_show (thing);
    gtk_box_pack_start (GTK_BOX (subthing), thing, true, true, 0);

    return thing;
}

static void make_floppy_disks (GtkWidget *vbox)
{
    char buf[5];
    unsigned int i;
    static const char *drv_speed_labels[] = { "Turbo", "100% (Compatible)", "200%", "400%", "800%", NULL };

    add_empty_vbox (vbox);
    GtkWidget *frame = make_radio_group_box ("Floppy Drive Speed", drv_speed_labels, drvspeed_widget, 1, drvspeed_changed);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, false, true, 0);

    add_empty_vbox (vbox);
    for (i = 0; i < 4; i++) {
	sprintf (buf, "DF%d:", i);

	floppy_widget[i] = floppyfileentry_new ();
	if (currprefs.floppyslots[i].dfxtype == -1)
	    gtk_widget_set_sensitive(floppy_widget[i], 0);
	floppyfileentry_set_drivename (FLOPPYFILEENTRY (floppy_widget[i]), buf);
	floppyfileentry_set_label (FLOPPYFILEENTRY (floppy_widget[i]), buf);
	floppyfileentry_set_currentdir (FLOPPYFILEENTRY (floppy_widget[i]), currprefs.path_floppy.path[i]);
	gtk_box_pack_start (GTK_BOX (vbox), floppy_widget[i], false, true, 0);
	gtk_widget_show (floppy_widget[i]);
	gtk_signal_connect (GTK_OBJECT (floppy_widget[i]), "disc-changed", (GtkSignalFunc) disc_changed, GINT_TO_POINTER (i));
    }

    add_empty_vbox (vbox);
}

static GtkWidget *sstate_selector;

static void did_close_sstate (gpointer gdata)
{
    gtk_widget_set_sensitive (sstate_change_widget, 1);
}

static void did_sstate_select (GtkObject *o)
{
    const char *s = gtk_file_selection_get_filename (GTK_FILE_SELECTION (sstate_selector));

    if (quit_gui)
		return;

    gtk_widget_set_sensitive (sstate_change_widget, 1);

    uae_sem_wait (&gui_sem);
    gui_sstate_name = strdup (s);
    uae_sem_post (&gui_sem);
    gtk_label_set_text (GTK_LABEL (sstate_text_widget), gui_sstate_name);
    gtk_widget_destroy (sstate_selector);
}

static void did_sstate_change (GtkWidget *w, gpointer data)
{
    gtk_widget_set_sensitive (sstate_change_widget, 0);

    sstate_selector = make_file_selector ("Select a Savestate file", did_sstate_select, did_close_sstate);
//    filesel_set_path (sstate_selector, currprefs.path_savestate);
}

static void did_sstate_load (GtkWidget *w, gpointer data)
{
	if ((int)strlen(gui_sstate_name) == 0) {
	;//	writelog ("SaveState Load: Select a Savestate first\n");
	} else {
	    write_comm_pipe_int (&from_gui_pipe, UAECMD_SAVESTATE_LOAD, 0);
	}
}

static void did_sstate_save (GtkWidget *w, gpointer data)
{
	if ((int)strlen(gui_sstate_name) == 0) {
	;//	writelog ("SaveState Save: Select a Savestate first\n");
	} else {
	    write_comm_pipe_int (&from_gui_pipe, UAECMD_SAVESTATE_SAVE, 0);
	}
}

static void make_ss_widgets (GtkWidget *vbox)
{
	GtkWidget *hbox = gtk_hbox_new (false, 10);

	add_empty_vbox (vbox);

	GtkWidget *buttonbox = make_file_container ("Savestate file:", vbox);
	GtkWidget *thing = gtk_button_new_with_label ("Select");
	GtkWidget *thing_load = gtk_button_new_with_label ("Load");
	GtkWidget *thing_save = gtk_button_new_with_label ("Save");

	/* Current file display */
	sstate_text_widget = make_file_widget (buttonbox);

	gtk_box_pack_start (GTK_BOX (buttonbox), thing, false, true, 0);
	gtk_widget_show (thing);
	gtk_box_pack_start (GTK_BOX (buttonbox), thing_load, false, true, 0);
	gtk_widget_show (thing_load);
	gtk_box_pack_start (GTK_BOX (buttonbox), thing_save, false, true, 0);
	gtk_widget_show (thing_save);

	sstate_change_widget = thing;
	sstate_load_widget = thing_load;
	sstate_save_widget = thing_save;
	gtk_signal_connect (GTK_OBJECT (thing), "clicked", (GtkSignalFunc) did_sstate_change, 0);
	gtk_signal_connect (GTK_OBJECT (thing_load), "clicked", (GtkSignalFunc) did_sstate_load, 0);
	gtk_signal_connect (GTK_OBJECT (thing_save), "clicked", (GtkSignalFunc) did_sstate_save, 0);

    gtk_widget_show (hbox);
    add_centered_to_vbox (vbox, hbox);
}

static void leds_on_screen_changed (void)
{
    currprefs.leds_on_screen = GTK_TOGGLE_BUTTON(leds_on_screen_widget)->active;
    DEBUG_LOG("leds_on_screen = %d\n", currprefs.leds_on_screen);
}

static void make_misc_widgets (GtkWidget *vbox)
{
	GtkWidget *hbox = gtk_hbox_new (false, 10);

	add_empty_vbox (vbox);

	leds_on_screen_widget = gtk_check_button_new_with_label ("Status Line");
	gtk_signal_connect (GTK_OBJECT (leds_on_screen_widget), "clicked", (GtkSignalFunc) leds_on_screen_changed, 0);
	gtk_widget_show (leds_on_screen_widget);
	gtk_box_pack_start (GTK_BOX (hbox), leds_on_screen_widget, false, true, 0);

    gtk_widget_show (hbox);
    add_centered_to_vbox (vbox, hbox);
}
/*
 * CPU configuration page
 */
static void on_cputype_changed (void)
{
    int i;
	unsigned int mdl;

	mdl = cputypepanel_get_cpulevel (CPUTYPEPANEL (ctpanel));
    mdl = (mdl * 10) + 68000;

    DEBUG_LOG ("called\n");

    changed_prefs.cpu_model       = mdl;
    changed_prefs.cpu_cycle_exact = CPUTYPEPANEL (ctpanel)->cycleexact;

    set_mem32_widgets_state ();

    DEBUG_LOG ("cpu_model=%d address_space24=%d cpu_compatible=%d cpu_cycle_exact=%d\n",
	changed_prefs.cpu_model, changed_prefs.address_space_24,
	changed_prefs.cpu_compatible, changed_prefs.cpu_cycle_exact);
}

static void on_addr24bit_changed (void)
{
	int i;

	DEBUG_LOG ("called\n");

	changed_prefs.address_space_24 = (cputypepanel_get_addr24bit (CPUTYPEPANEL (ctpanel)) != 0);
	set_mem32_widgets_state ();

	DEBUG_LOG ("address_space_24=%d\n", changed_prefs.address_space_24);
}

static void on_cpuspeed_changed (void)
{
	DEBUG_LOG ("called\n");

	changed_prefs.m68k_speed = CPUSPEEDPANEL (cspanel)->cpuspeed;

	DEBUG_LOG ("m68k_speed=%d\n", changed_prefs.m68k_speed);
}

static void on_cpuidle_changed (void)
{
	DEBUG_LOG ("called\n");

	changed_prefs.cpu_idle       = CPUSPEEDPANEL (cspanel)->cpuidle;

	DEBUG_LOG ("cpu_idle=%d\n", changed_prefs.cpu_idle);
}

static void make_cpu_widgets (GtkWidget *vbox)
{
    GtkWidget *table;

    table = make_xtable (5, 7);
    add_table_padding (table, 0, 0);
    add_table_padding (table, 4, 4);
    add_table_padding (table, 1, 2);
    gtk_box_pack_start (GTK_BOX (vbox), table, true, true, 0);

    ctpanel = cputypepanel_new();
    gtk_table_attach (GTK_TABLE (table), ctpanel, 1, 4, 1, 2,
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_show (ctpanel);

    ftpanel = fputypepanel_new();
    gtk_table_attach (GTK_TABLE (table), ftpanel, 1, 4, 3, 4,
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_show (ftpanel);

    cspanel = cpuspeedpanel_new();
    gtk_table_attach (GTK_TABLE (table), cspanel, 1, 4, 5, 6,
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_show (cspanel);

    gtk_signal_connect (GTK_OBJECT (ctpanel), "cputype-changed",
			GTK_SIGNAL_FUNC (on_cputype_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (ctpanel), "addr24bit-changed",
			GTK_SIGNAL_FUNC (on_addr24bit_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (cspanel), "cpuspeed-changed",
			GTK_SIGNAL_FUNC (on_cpuspeed_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (cspanel), "cpuidle-changed",
			GTK_SIGNAL_FUNC (on_cpuidle_changed),
			NULL);
}

static void on_chipset_changed (void)
{
    changed_prefs.chipset_mask = CHIPSETTYPEPANEL (chipsettype_panel)->chipset_mask;
    changed_prefs.ntscmode     = CHIPSETTYPEPANEL (chipsettype_panel)->ntscmode;
}

static void on_framerate_changed (void)
{
    changed_prefs.gfx_framerate = CHIPSETSPEEDPANEL (chipsetspeed_panel)->framerate;
    DEBUG_LOG("gfx_framerate = %d\n", changed_prefs.gfx_framerate);
}

static void on_collision_level_changed (void)
{
    changed_prefs.collision_level = CHIPSETSPEEDPANEL (chipsetspeed_panel)->collision_level;
    DEBUG_LOG("collision_level = %d\n", changed_prefs.collision_level);
}

static void on_immediate_blits_changed (void)
{
    changed_prefs.immediate_blits = CHIPSETSPEEDPANEL (chipsetspeed_panel)->immediate_blits;
    DEBUG_LOG("immediate_blits = %d\n", changed_prefs.immediate_blits);
}

static void make_chipset_widgets (GtkWidget *vbox)
{
    GtkWidget *table;

    table = make_xtable (5, 7);
    add_table_padding (table, 0, 0);
    add_table_padding (table, 4, 4);
    add_table_padding (table, 1, 2);
    gtk_box_pack_start (GTK_BOX (vbox), table, true, true, 0);

    chipsettype_panel = chipsettypepanel_new ();
    gtk_table_attach (GTK_TABLE (table), chipsettype_panel, 1, 4, 1, 2,
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_show (chipsettype_panel);

    chipsetspeed_panel = chipsetspeedpanel_new ();
    gtk_table_attach (GTK_TABLE (table), chipsetspeed_panel, 1, 4, 3, 4,
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_show (chipsetspeed_panel);


    gtk_signal_connect (GTK_OBJECT (chipsettype_panel), "chipset-changed", GTK_SIGNAL_FUNC (on_chipset_changed), NULL);
    gtk_signal_connect (GTK_OBJECT (chipsetspeed_panel), "framerate-changed", GTK_SIGNAL_FUNC (on_framerate_changed), NULL);
    gtk_signal_connect (GTK_OBJECT (chipsetspeed_panel), "sprite-collisions-changed", GTK_SIGNAL_FUNC (on_collision_level_changed), NULL);
    gtk_signal_connect (GTK_OBJECT (chipsetspeed_panel), "immediate-blits-changed", GTK_SIGNAL_FUNC (on_immediate_blits_changed), NULL);
}

static void make_cpu_widgets2 (GtkWidget *vbox)
{
	static const char *cpu_labels[] = {
		"68000", "68010", "68020", "68030", "68040", "68060", NULL
	}, *fpu_labels[] = {
		"None", "68881", "68882", "CPU Internal", NULL
	};

	GtkWidget *hbox = gtk_hbox_new (false, 10);
	GtkWidget *frame, *newbox, *newbox2;
	add_empty_vbox (vbox);

	gtk_widget_show (hbox);
	add_centered_to_vbox (vbox, hbox);
	add_empty_vbox (vbox);

	//cpu emulation
	newbox = make_radio_group_box ("CPU", cpu_labels, cpu_widget, 0, cpu_changed);
	gtk_widget_show (newbox);
	gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	newbox2 = gtk_check_button_new_with_label ("24-bit addressing");
	gtk_widget_show (newbox2);
	gtk_box_pack_start (GTK_BOX (newbox), newbox2, false, true, 0);

	newbox2 = gtk_check_button_new_with_label ("More compatible");
	gtk_widget_show (newbox2);
	gtk_box_pack_start (GTK_BOX (newbox), newbox2, false, true, 0);

	newbox = gtk_check_button_new_with_label ("JIT");
	gtk_widget_show (newbox);
	gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	newbox = gtk_check_button_new_with_label ("68040 MMU");
	gtk_widget_show (newbox);
	gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	//fpu mode
	newbox = make_radio_group_box ("FPU", fpu_labels, fpu_widget, 0, fpu_changed);
	gtk_widget_show (newbox);
	gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	newbox = gtk_check_button_new_with_label ("More compatible");
	gtk_widget_show (newbox);
	gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);
}

static void make_sound_widgets (GtkWidget *vbox)
{
	static const char *snd_em_labels[] = {
	"Disabled", "Disabled, but Emulated", "Enabled", "Enabled, 100% Accurate", NULL
	}, *snd_in_labels[] = {
	"Disabled", "None", "Sinc", "RH", "Anti", NULL
	}, *snd_fl_labels[] = {
	"Disabled", "A500 (Power Led)", "A500 (Always on)", "A1200 (Power Led)", "A1200 (Always on)", NULL
	}, *snd_ch_labels[] = {
	"Mono", "Stereo", "Mixed", NULL
	};

	GtkWidget *hbox = gtk_hbox_new (false, 10);
	GtkWidget *frame, *newbox;
	add_empty_vbox (vbox);

	gtk_widget_show (hbox);
	add_centered_to_vbox (vbox, hbox);
	add_empty_vbox (vbox);

	//sound emulation
	newbox = make_radio_group_box ("Sound Emulation", snd_em_labels, sound_widget, 0, sound_changed);
	gtk_widget_set_sensitive (sound_widget[2], sound_available);
	gtk_widget_set_sensitive (sound_widget[3], sound_available);
	gtk_widget_show (newbox);
	gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	//channel mode
    newbox = make_radio_group_box ("Channels", snd_ch_labels, sound_ch_widget, 0, sound_changed);
    gtk_widget_set_sensitive (newbox, sound_available);
    gtk_widget_show (newbox);
    gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	//interpolation
    newbox = make_radio_group_box ("Interpolation", snd_in_labels, sound_in_widget, 0, sound_changed);
    gtk_widget_set_sensitive (newbox, sound_available);
    gtk_widget_show (newbox);
    gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);

	//audio filter
    newbox = make_radio_group_box ("Audio Filter", snd_fl_labels, sound_fl_widget, 0, sound_changed);
    gtk_widget_set_sensitive (newbox, sound_available);
    gtk_widget_show (newbox);
    gtk_box_pack_start (GTK_BOX (hbox), newbox, false, true, 0);
}

static void make_mem_widgets (GtkWidget *vbox)
{
    GtkWidget *hbox = gtk_hbox_new (false, 10);
    GtkWidget *label, *frame;

    static const char *chiplabels[] = {
	"512 KB", "1 MB", "2 MB", "4 MB", "8 MB", NULL
    };
    static const char *bogolabels[] = {
	"None", "512 KB", "1 MB", "1.8 MB", NULL
    };
    static const char *fastlabels[] = {
	"None", "1 MB", "2 MB", "4 MB", "8 MB", NULL
    };
    static const char *z3labels[] = {
	"None", "1 MB", "2 MB", "4 MB", "8 MB",
	"16 MB", "32 MB", "64 MB", "128 MB", "256 MB",
	NULL
    };
    static const char *p96labels[] = {
	"None", "1 MB", "2 MB", "4 MB", "8 MB", "16 MB", "32 MB", NULL
    };

    add_empty_vbox (vbox);

    {
	GtkWidget *buttonbox = make_file_container ("Kickstart ROM file:", vbox);
	GtkWidget *thing = gtk_button_new_with_label ("Change");

	/* Current file display */
	rom_text_widget = make_file_widget (buttonbox);

	gtk_box_pack_start (GTK_BOX (buttonbox), thing, false, true, 0);
	gtk_widget_show (thing);
	rom_change_widget = thing;
	gtk_signal_connect (GTK_OBJECT (thing), "clicked", (GtkSignalFunc) did_romchange, 0);
    }

    gtk_widget_show (hbox);
    add_centered_to_vbox (vbox, hbox);

    add_empty_vbox (vbox);

    frame = make_radio_group_box ("Chip Mem", chiplabels, chipsize_widget, 0, chipsize_changed);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (hbox), frame, false, true, 0);

    frame = make_radio_group_box ("Slow Mem", bogolabels, bogosize_widget, 0, bogosize_changed);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (hbox), frame, false, true, 0);

    frame = make_radio_group_box ("Fast Mem", fastlabels, fastsize_widget, 0, fastsize_changed);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (hbox), frame, false, true, 0);

    frame = make_radio_group_box_1 ("Z3 Mem", z3labels, z3size_widget, 0, z3size_changed, 5);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (hbox), frame, false, true, 0);

#ifdef PICASSO96
    frame = make_radio_group_box_1 ("P96 RAM", p96labels, p96size_widget, 0, p96size_changed, 4);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (hbox), frame, false, true, 0);
#endif

    memorypanel = hbox;
}

#ifdef JIT
static void make_comp_widgets (GtkWidget *vbox)
{
    GtkWidget *newbox;
    static const char *complabels1[] = {
	"Direct", "Indirect", "Indirect for KS", "Direct after Picasso",
	NULL
    },*complabels2[] = {
	"Direct", "Indirect", "Indirect for KS", "Direct after Picasso",
	NULL
    },*complabels3[] = {
	"Direct", "Indirect", "Indirect for KS", "Direct after Picasso",
	NULL
    },*complabels3a[] = {
	"Direct", "Indirect", "Indirect for KS", "Direct after Picasso",
	NULL
    }, *complabels4[] = {
      "Always generate", "Only generate when needed",
	NULL
    }, *complabels5[] = {
      "Disable", "Enable",
	NULL
    }, *complabels6[] = {
      "Disable", "Enable",
	NULL
    }, *complabels7[] = {
      "Disable", "Enable",
	NULL
    }, *complabels8[] = {
      "Soft", "Hard",
	NULL
    }, *complabels9[] = {
      "Disable", "Enable",
	NULL
    };
    GtkWidget *thing;

    add_empty_vbox (vbox);

    newbox = make_radio_group_box ("Byte access", complabels1, compbyte_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);
    newbox = make_radio_group_box ("Word access", complabels2, compword_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);
    newbox = make_radio_group_box ("Long access", complabels3, complong_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);
    newbox = make_radio_group_box ("Address lookup", complabels3a, compaddr_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);

    newbox = make_radio_group_box ("Flags", complabels4, compnf_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);

    newbox = make_radio_group_box ("Icache flushes", complabels8, comp_hardflush_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);

    newbox = make_radio_group_box ("Compile through uncond branch", complabels9, comp_constjump_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);

    newbox = make_radio_group_box ("JIT FPU compiler", complabels7, compfpu_widget, 1, comp_changed);
    gtk_widget_show (newbox);
    add_centered_to_vbox (vbox, newbox);

    cachesize_adj = GTK_ADJUSTMENT (gtk_adjustment_new (currprefs.cachesize, 0.0, 16385.0, 1.0, 1.0, 1.0));
    gtk_signal_connect (GTK_OBJECT (cachesize_adj), "value_changed",
			GTK_SIGNAL_FUNC (comp_changed), NULL);

    thing = gtk_hscale_new (cachesize_adj);
    gtk_range_set_update_policy (GTK_RANGE (thing), GTK_UPDATE_DELAYED);
    gtk_scale_set_digits (GTK_SCALE (thing), 0);
    gtk_scale_set_value_pos (GTK_SCALE (thing), GTK_POS_RIGHT);
    gtk_widget_set_usize (thing, 180, -1); // Hack!
    add_labelled_widget_centered ("Translation buffer(kB):", thing, vbox);

    add_empty_vbox (vbox);

    /* Kludge - remember pointer to JIT page, so that we can easily disable it */
    jit_page = vbox;
}
#endif

static void make_joy_widgets (GtkWidget *dvbox)
{
    int i;
    int joy_count = inputdevice_get_device_total (IDTYPE_JOYSTICK);
    GtkWidget *hbox = gtk_hbox_new (false, 12);

    static const char *joylabels[] = {
		"None",
		"Joystick 0",
		"Joystick 1",
		"Mouse",
		"Keyboard Layout A (NumPad, 0 & 5 = Fire)",
		"Keyboard Layout B (Cursor, RCtrl & Alt = Fire)",
		"Keyboard Layout C (WASD, LAlt = Fire)",
#ifdef XARCADE
		"X-Arcade (Left)",
		"X-Arcade (Right)",
#endif
		NULL
    };

    add_empty_vbox (dvbox);
    gtk_widget_show (hbox);
    add_centered_to_vbox (dvbox, hbox);

    for (i = 0; i < 2; i++) {
		GtkWidget *frame;
		char buffer[20];

		sprintf (buffer, "Port %d", i);
		frame = make_radio_group_box (buffer, joylabels, joy_widget[i], 0, joy_changed);
		gtk_widget_show (frame);
		gtk_box_pack_start (GTK_BOX (hbox), frame, false, true, 0);

		if (joy_count < 2)
		    gtk_widget_set_sensitive (joy_widget[i][2], 0);
		if (joy_count == 0)
		    gtk_widget_set_sensitive (joy_widget[i][1], 0);
    }

    add_empty_vbox (dvbox);
}

#ifdef FILESYS
static int hd_change_mode;

static void newdir_ok (void)
{
    int n;
    int readonly = GTK_TOGGLE_BUTTON (readonly_widget)->active;
    int bootpri  = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (bootpri_widget));
    strcpy (dirdlg_devname, gtk_entry_get_text (GTK_ENTRY (devname_entry)));
    strcpy (dirdlg_volname, gtk_entry_get_text (GTK_ENTRY (volname_entry)));
    strcpy (dirdlg_path, gtk_entry_get_text (GTK_ENTRY (path_entry)));

	unsigned int secspertrack = 0;
	unsigned int surfaces = 0;
	unsigned int reserved = 0;
	unsigned int blocksize = 0; 
	unsigned int flags = 0;
	unsigned int donotmount = 0;
	unsigned int autoboot = 129;
	unsigned int hdc = 0;

    n = strlen (dirdlg_volname);
    /* Strip colons from the end.  */
    if (n > 0) {
		if (dirdlg_volname[n - 1] == ':')
		    dirdlg_volname[n - 1] = '\0';
    }
    /* Do device name too */
    n = strlen (dirdlg_devname);
    if (n > 0) {
		if (dirdlg_devname[n - 1] == ':')
		    dirdlg_devname[n - 1] = '\0';
    }
    if (strlen (dirdlg_volname) == 0 || strlen (dirdlg_path) == 0) {
		/* Uh, no messageboxes in gtk?  */
/// FIXME: set_filesys_unit() needs uaedev_config_info* !
#if 0
    } else if (hd_change_mode) {
		set_filesys_unit (selected_hd_row, dirdlg_devname, dirdlg_volname, dirdlg_path,
				readonly, 0, secspertrack, surfaces, reserved, blocksize, bootpri,
				donotmount, autoboot, (TCHAR*)NULL, hdc, flags);
		set_hd_state ();
#endif // 0
/// FIXME: add_filesys_unit() needs uaedev_config_info* !
#if 0
    } else {
		add_filesys_unit (dirdlg_devname, dirdlg_volname, dirdlg_path, readonly,
				0, secspertrack, surfaces, reserved, blocksize, bootpri,
				donotmount, autoboot, (TCHAR*)NULL, hdc, flags);
		set_hd_state ();
#endif // 0
    }
    gtk_widget_destroy (dirdlg);
}


GtkWidget *path_selector;

static void did_dirdlg_done_select (GtkObject *o, gpointer entry )
{
    assert (GTK_IS_ENTRY (entry));

    gtk_entry_set_text (GTK_ENTRY (entry), gtk_file_selection_get_filename (GTK_FILE_SELECTION (path_selector)));
}

static void did_dirdlg_select (GtkObject *o, gpointer entry )
{
    assert( GTK_IS_ENTRY(entry) );
    path_selector = gtk_file_selection_new ("Select a folder to mount");
    gtk_file_selection_set_filename (GTK_FILE_SELECTION (path_selector), gtk_entry_get_text (GTK_ENTRY (entry)));
    gtk_window_set_modal (GTK_WINDOW (path_selector), true);

    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION(path_selector)->ok_button),
					  "clicked", GTK_SIGNAL_FUNC (did_dirdlg_done_select),
					  (gpointer) entry);
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(path_selector)->ok_button),
					  "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
					  (gpointer) path_selector);
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(path_selector)->cancel_button),
					  "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
					  (gpointer) path_selector);

    /* Gtk1.2 doesn't have a directory chooser widget, so we fake one from the
     * file dialog by hiding the widgets related to file selection */
    gtk_widget_hide ((GTK_FILE_SELECTION(path_selector)->file_list)->parent);
    gtk_widget_hide (GTK_FILE_SELECTION(path_selector)->fileop_del_file);
    gtk_widget_hide (GTK_FILE_SELECTION(path_selector)->fileop_ren_file);
    gtk_widget_hide (GTK_FILE_SELECTION(path_selector)->selection_entry);
    gtk_entry_set_text (GTK_ENTRY (GTK_FILE_SELECTION(path_selector)->selection_entry), "" );

    gtk_widget_show (path_selector);
}

static void dirdlg_on_change (GtkObject *o, gpointer data)
{
  int can_complete = (strlen (gtk_entry_get_text (GTK_ENTRY(path_entry))) !=0)
		  && (strlen (gtk_entry_get_text (GTK_ENTRY(volname_entry))) != 0)
		  && (strlen (gtk_entry_get_text (GTK_ENTRY(devname_entry))) != 0);

  gtk_widget_set_sensitive (dirdlg_ok, can_complete);
}

static void create_dirdlg (const char *title)
{
    GtkWidget *dialog_vbox, *dialog_hbox, *vbox, *frame, *table, *hbox, *thing, *label, *button;

    dirdlg = gtk_dialog_new ();

    gtk_window_set_title (GTK_WINDOW (dirdlg), title);
    gtk_window_set_position (GTK_WINDOW (dirdlg), GTK_WIN_POS_MOUSE);
    gtk_window_set_modal (GTK_WINDOW (dirdlg), true);
    gtk_widget_show (dirdlg);

    dialog_vbox = GTK_DIALOG (dirdlg)->vbox;
    gtk_widget_show (dialog_vbox);

    vbox = gtk_vbox_new (false, 0);
    gtk_widget_show (vbox);
    gtk_box_pack_start (GTK_BOX (dialog_vbox), vbox, true, false, 0);

    frame = gtk_frame_new ("Mount host folder");
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, false, false, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 8);

    hbox = gtk_hbox_new (false, 4);
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);

    label  = gtk_label_new ("Path");
    gtk_box_pack_start (GTK_BOX (hbox), label, false, false, 0);
    gtk_widget_show (label);

    thing = gtk_entry_new_with_max_length (255);
    gtk_signal_connect (GTK_OBJECT (thing), "changed", (GtkSignalFunc) dirdlg_on_change, (gpointer) NULL);
    gtk_box_pack_start (GTK_BOX (hbox), thing, true, true, 0);
    gtk_widget_show (thing);
    path_entry = thing;

    button = gtk_button_new_with_label ("Select...");
    gtk_signal_connect (GTK_OBJECT (button), "clicked", (GtkSignalFunc) did_dirdlg_select, (gpointer) path_entry);
    gtk_box_pack_start (GTK_BOX (hbox), button, false, false, 0);
    gtk_widget_show (button);

    frame = gtk_frame_new ("As Amiga disk");
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, false, false, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 8);

    table = gtk_table_new (3, 4, false);
    gtk_widget_show (table);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_container_set_border_width (GTK_CONTAINER (table), 8);
    gtk_table_set_row_spacings (GTK_TABLE (table), 4);
    gtk_table_set_col_spacings (GTK_TABLE (table), 4);

	label = gtk_label_new ("Device name");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
	gtk_widget_show (label);
	thing = gtk_entry_new_with_max_length (255);
	gtk_signal_connect (GTK_OBJECT (thing), "changed", (GtkSignalFunc) dirdlg_on_change, (gpointer) NULL);
	gtk_widget_show (thing);
	gtk_table_attach (GTK_TABLE (table), thing, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize (thing, 200, -1);
	devname_entry = thing;

	label = gtk_label_new ("Volume name");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
	gtk_widget_show (label);
	thing = gtk_entry_new_with_max_length (255);
	gtk_signal_connect (GTK_OBJECT (thing), "changed", (GtkSignalFunc) dirdlg_on_change, (gpointer) NULL);
	gtk_table_attach (GTK_TABLE (table), thing, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
	gtk_widget_show (thing);
	gtk_widget_set_usize (thing, 200, -1);
	volname_entry = thing;

	label = gtk_label_new ("Boot priority");
	gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
	gtk_widget_show (label);
	thing = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, -128, 127, 1, 5, 5)), 1, 0);
	gtk_table_attach (GTK_TABLE (table), thing, 3, 4, 0, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_widget_show (thing);
	bootpri_widget = thing;

	readonly_widget = gtk_check_button_new_with_label ("Read only");
	gtk_table_attach (GTK_TABLE (table), readonly_widget, 0, 4, 2, 3, (GtkAttachOptions) (GTK_EXPAND), (GtkAttachOptions) (0), 0, 0);
	gtk_widget_show (readonly_widget);

    dialog_hbox = GTK_DIALOG (dirdlg)->action_area;

    hbox = gtk_hbutton_box_new();
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox), GTK_BUTTONBOX_END);
    gtk_box_pack_start (GTK_BOX (dialog_hbox), hbox, true, true, 0);
    gtk_widget_show (hbox);

    button = gtk_button_new_with_label ("OK");
    gtk_widget_set_sensitive (GTK_WIDGET (button), false);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC(newdir_ok), NULL);
    gtk_box_pack_start (GTK_BOX (hbox), button, true, true, 0);
    gtk_widget_show (button);
    dirdlg_ok = button;

    button = gtk_button_new_with_label ("Cancel");
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (dirdlg));
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (hbox), button, true, true, 0);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);
}

static void did_newdir (void)
{
    hd_change_mode = 0;
    create_dirdlg ("Add a hard disk");
}
static void did_newhdf (void)
{
    hd_change_mode = 0;
}

static void did_hdchange (void)
{
    int secspertrack=0, surfaces=0, reserved=0, blocksize=0, bootpri=0;
    uae_u64 size=0;
    int cylinders=0, readonly=0, flags=0;
    char *devname=NULL, *volname=NULL, *rootdir=NULL, *filesysdir=NULL;
    const char *failure;

/*    failure = get_filesys_unit (selected_hd_row,
				&devname, &volname, &rootdir, &readonly,
				&secspertrack, &surfaces, &reserved,
				&cylinders, &size, &blocksize, &bootpri,
				&filesysdir, &flags);
*/
    hd_change_mode = 1;
    if (is_hardfile (selected_hd_row)) {
    } else {
		create_dirdlg ("Hard disk properties");
		gtk_entry_set_text (GTK_ENTRY (devname_entry), devname);
		gtk_entry_set_text (GTK_ENTRY (volname_entry), volname);
		gtk_entry_set_text (GTK_ENTRY (path_entry), rootdir);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (readonly_widget), readonly);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (bootpri_widget), bootpri);
   }
}
static void did_hddel (void)
{
    kill_filesys_unitconfig (&currprefs, selected_hd_row);
    set_hd_state ();
}

static void hdselect (GtkWidget *widget, gint row, gint column, GdkEventButton *bevent,
		      gpointer user_data)
{
    selected_hd_row = row;
    gtk_widget_set_sensitive (hdchange_button, true);
    gtk_widget_set_sensitive (hddel_button, true);
}

static void hdunselect (GtkWidget *widget, gint row, gint column, GdkEventButton *bevent,
			gpointer user_data)
{
    gtk_widget_set_sensitive (hdchange_button, false);
    gtk_widget_set_sensitive (hddel_button, false);
}
#endif // FILESYS

static GtkWidget *make_buttons (const char *label, GtkWidget *box, void (*sigfunc) (void), GtkWidget *(*create)(const char *label))
{
    GtkWidget *thing = create (label);
    gtk_widget_show (thing);
    gtk_signal_connect (GTK_OBJECT (thing), "clicked", (GtkSignalFunc) sigfunc, NULL);
    gtk_box_pack_start (GTK_BOX (box), thing, true, true, 0);

    return thing;
}
#define make_button(label, box, sigfunc) make_buttons(label, box, sigfunc, gtk_button_new_with_label)

#ifdef FILESYS
static void make_hd_widgets (GtkWidget *dvbox)
{
    GtkWidget *frame, *vbox, *scrollbox, *thing, *buttonbox, *hbox;
//    char *titles [] = {
//	"Volume", "File/Directory", "R/O", "Heads", "Cyl.", "Sec.", "Rsrvd", "Size", "Blksize"
//    };


    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (dvbox), frame, true, true, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 8);

    vbox = gtk_vbox_new (false, 0);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (frame), vbox);

    scrollbox = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrollbox);
    gtk_box_pack_start (GTK_BOX (vbox), scrollbox, true, true, 0);
    gtk_container_set_border_width (GTK_CONTAINER (scrollbox), 8);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbox), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    thing = gtk_clist_new_with_titles (HDLIST_MAX_COLS, (gchar **)hdlist_col_titles );
    gtk_clist_set_selection_mode (GTK_CLIST (thing), GTK_SELECTION_SINGLE);
    gtk_signal_connect (GTK_OBJECT (thing), "select_row", (GtkSignalFunc) hdselect, NULL);
    gtk_signal_connect (GTK_OBJECT (thing), "unselect_row", (GtkSignalFunc) hdunselect, NULL);
    hdlist_widget = thing;
    gtk_widget_set_usize (thing, -1, 200);
    gtk_widget_show (thing);
    gtk_container_add (GTK_CONTAINER (scrollbox), thing);

    /* The buttons */
    buttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (buttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), buttonbox, false, false, 0);
    gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 8);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_SPREAD);

    make_button ("Add...", buttonbox, did_newdir);
#if 0 /* later... */
    make_button ("New hardfile...", buttonbox, did_newhdf);
#endif
    hdchange_button = make_button ("Properties...", buttonbox, did_hdchange);
    hddel_button = make_button ("Remove", buttonbox, did_hddel);

    hdpanel = frame;
}
#endif

static void make_about_widgets (GtkWidget *dvbox)
{
    GtkWidget *thing;
//fixme
#if GTK_MAJOR_VERSION >= 2
    const char title[] = "<span font_desc=\"Sans 24\">2.5.0</span>";
#else
    const char title[] = "2.5.0";
#endif

    add_empty_vbox (dvbox);

#if GTK_MAJOR_VERSION >= 2
    thing = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL (thing), title);
#else
    thing = gtk_label_new (title);
    {
	GdkFont *font = gdk_font_load ("-*-helvetica-medium-r-normal--*-240-*-*-*-*-*-*");
	if (font) {
	    GtkStyle *style = gtk_style_copy (GTK_WIDGET (thing)->style);
	    gdk_font_unref (style->font);
	    gdk_font_ref (font);
	    style->font = font;
	    gtk_widget_set_style (thing, style);
	}
    }
#endif
    gtk_widget_show (thing);
    add_centered_to_vbox (dvbox, thing);

#ifdef PACKAGE_VERSION
    thing = gtk_label_new ("Version " PACKAGE_VERSION );
    gtk_widget_show (thing);
    add_centered_to_vbox (dvbox, thing);
#endif

    add_empty_vbox (dvbox);
}

static gint did_guidlg_delete (GtkWidget* window, GdkEventAny* e, gpointer data)
{
    if (!quit_gui)
	write_comm_pipe_int (&from_gui_pipe, UAECMD_QUIT, 1);
    gui_window = 0;
    return false;
}

static void on_menu_saveconfig (void)
{
    DEBUG_LOG ("Save config...\n");
    if (!quit_gui)
	write_comm_pipe_int (&from_gui_pipe, UAECMD_SAVE_CONFIG, 1);
}

void on_vstat_toggle(GtkWidget *widget, gpointer statusbar) 
{
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
    gtk_widget_show(statusbar);
  } else {
    gtk_widget_hide(statusbar);
  }
}

static void create_guidlg (void)
{
    GtkWidget *window, *notebook;
    GtkWidget *buttonbox, *vbox, *hbox;
    GtkWidget *thing;
    GtkWidget *menubar, *menuitem, *menuitem_menu;
    GtkWidget *statusbar, *vstatusbar;

    unsigned int i;
    static const struct _pages {
	const char *title;
	void (*createfunc)(GtkWidget *);
    } pages[] = {
	{ "Floppy disks", make_floppy_disks },
	{ "Memory",       make_mem_widgets },
	{ "CPU",          make_cpu_widgets2 },
	{ "Chipset",      make_chipset_widgets },
	{ "Sound",        make_sound_widgets },
#ifdef JIT
	{ "JIT",          make_comp_widgets },
#endif
	{ "Game ports",   make_joy_widgets },
#ifdef FILESYS
	{ "Hard disks",   make_hd_widgets },
#endif
#ifdef SAVESTATE
	{ "Savestate",    make_ss_widgets },
#endif
	{ "Misc",    	  make_misc_widgets },
	{ "About",        make_about_widgets }
    };

    DEBUG_LOG ("Entered\n");

    gui_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (gui_window), PACKAGE_NAME " control");
    gtk_signal_connect (GTK_OBJECT(gui_window), "delete_event", GTK_SIGNAL_FUNC(did_guidlg_delete), NULL);

    vbox = gtk_vbox_new (false, 5);
    gtk_container_add (GTK_CONTAINER (gui_window), vbox);
    gtk_container_set_border_width (GTK_CONTAINER (gui_window), 10);

    /* Quick and dirty menu bar */
    menubar = gtk_menu_bar_new();
    gtk_widget_show (menubar);
    gtk_box_pack_start (GTK_BOX (vbox), menubar, false, false, 0);

    menuitem = gtk_menu_item_new_with_mnemonic ("_File");
    gtk_widget_show (menuitem);
    gtk_container_add (GTK_CONTAINER (menubar), menuitem);

    menuitem_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menuitem_menu);

    thing = gtk_menu_item_new_with_mnemonic ("Save config");
    gtk_widget_show (thing);
    gtk_container_add (GTK_CONTAINER (menuitem_menu), thing);
    gtk_signal_connect (GTK_OBJECT(thing), "activate", GTK_SIGNAL_FUNC(on_menu_saveconfig), NULL);

    vstatusbar = gtk_check_menu_item_new_with_label("View Statusbar");
    gtk_widget_show (vstatusbar);
    gtk_container_add (GTK_CONTAINER (menuitem_menu), vstatusbar);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(vstatusbar), true);

    thing = gtk_separator_menu_item_new ();
    gtk_widget_show (thing);
    gtk_container_add (GTK_CONTAINER (menuitem_menu), thing);

    thing = gtk_menu_item_new_with_mnemonic ("Quit");
    gtk_widget_show (thing);
    gtk_container_add (GTK_CONTAINER (menuitem_menu), thing);
    gtk_signal_connect (GTK_OBJECT(thing), "activate", GTK_SIGNAL_FUNC(on_quit_clicked), NULL);

    /* First line - buttons and power LED */
    hbox = gtk_hbox_new (false, 10);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, false, true, 0);

    /* The buttons */
    buttonbox = gtk_hbox_new (true, 6);
    gtk_widget_show (buttonbox);
    gtk_box_pack_start (GTK_BOX (hbox), buttonbox, true, true, 0);
    start_uae_widget = make_button  ("Start", buttonbox, on_start_clicked);
    stop_uae_widget  = make_button  ("Stop",  buttonbox, on_stop_clicked);
    pause_uae_widget = make_buttons ("Pause", buttonbox, (void (*) (void))on_pause_clicked, gtk_toggle_button_new_with_label);
#ifdef DEBUGGER
//FIXME    debug_uae_widget = make_button  ("Debug", buttonbox, on_debug_clicked);
#endif
    reset_uae_widget = make_button  ("Reset", buttonbox, on_reset_clicked);
    make_button ("Quit", buttonbox, on_quit_clicked);

    /* The LED */
    power_led = led_new();
    gtk_widget_show(power_led);
    thing = gtk_vbox_new(false, 4);
    add_empty_hbox(thing);
    gtk_container_add(GTK_CONTAINER(thing), power_led);
    add_empty_hbox(thing);
    thing = make_labelled_widget ("Power:", thing);
    gtk_widget_show (thing);
    gtk_box_pack_start (GTK_BOX (hbox), thing, false, true, 0);

    /* More buttons */
    buttonbox = gtk_hbox_new (true, 4);
    gtk_widget_show (buttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), buttonbox, false, false, 0);

    /* Place a separator below those buttons.  */
    thing = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), thing, false, true, 0);
    gtk_widget_show (thing);

    /* Now the notebook */
    notebook = gtk_notebook_new ();
    gtk_box_pack_start (GTK_BOX (vbox), notebook, true, true, 0);
    gtk_widget_show (notebook);

    for (i = 0; i < sizeof pages / sizeof (struct _pages); i++) {
		thing = gtk_vbox_new (false, 4);
		gtk_widget_show (thing);
		gtk_container_set_border_width (GTK_CONTAINER (thing), 10);
		pages[i].createfunc (thing);
		gtk_notebook_append_page (GTK_NOTEBOOK (notebook), thing, gtk_label_new (pages[i].title));
    }

    /* Put "about" screen first.  */
    gtk_notebook_set_page (GTK_NOTEBOOK (notebook), i - 1);

    /* Statusbar for FPS etc */
    statusbar = gtk_statusbar_new();
    gtk_box_pack_end(GTK_BOX(vbox), statusbar, false, true, 1);
    gtk_widget_show(statusbar);
    g_signal_connect(G_OBJECT(vstatusbar), "activate", G_CALLBACK(on_vstat_toggle), statusbar);

    gtk_widget_show (vbox);

    gtk_timeout_add (1000, (GtkFunction)leds_callback, 0);
}

/*
 * gtk_gui_thread()
 *
 * This is launched as a separate thread to the main UAE thread
 * to create and handle the GUI. After the GUI has been set up,
 * this calls the standard GTK+ event processing loop.
 *
 */
static void *gtk_gui_thread (void *dummy)
{
    /* fake args for gtk_init() */
    int argc = 1;
    char *a[] = {"UAE"};
    char **argv = a;

    DEBUG_LOG ("Started\n");

    gui_active = 0;

    if (gtk_init_check (&argc, &argv)) {
	DEBUG_LOG ("gtk_init() successful\n");

	gtk_rc_parse ("uaegtkrc");
	gui_available = 1;

	/* Add callback to GTK+ mainloop to handle messages from UAE */
	gtk_timeout_add (250, (GtkFunction)my_idle, 0);

	/* We're ready - tell the world */
	uae_sem_post (&gui_init_sem);

	/* Enter GTK+ main loop */
	DEBUG_LOG ("Entering GTK+ main loop\n");
	gtk_main ();

	/* Main loop has exited, so the GUI will quit */
	quitted_gui = 1;
	uae_sem_post (&gui_quit_sem);
	DEBUG_LOG ("Exiting\n");
    } else {
	DEBUG_LOG ("gtk_init() failed\n");
	/* If GTK+ can't display, we still need to say we're done */
	uae_sem_post (&gui_init_sem);
    }
    return 0;
}

void gui_fps (int fps, int idle, int color)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
//        gui_led (LED_FPS, 0);
//        gui_led (LED_CPU, 0);
//        gui_led (LED_SND, gui_data.sndbuf_status > 1 || gui_data.sndbuf_status < 0);

//gchar *str;
//str = g_strdup_printf("FPS: %d",fps);
//gtk_statusbar_push(GTK_STATUSBAR(window),gtk_statusbar_get_context_id(GTK_STATUSBAR(window),str), str);
}

/*
 * gui_led()
 *
 * Called from the main UAE thread to inform the GUI
 * of disk activity so that indicator LEDs may be refreshed.
 *
 * We don't respond to this, since our LEDs are updated
 * periodically by my_idle()
 */
void gui_led (int num, int on)
{
}


/*
 * gui_filename()
 *
 * This is called from the main UAE thread to inform
 * the GUI that a floppy disk has been inserted or ejected.
 */
void gui_filename (int num, const char *name)
{
    if (!gui_available)
	    return;

	write_comm_pipe_int (&to_gui_pipe, GUICMD_DISKCHANGE, 0);
	write_comm_pipe_int (&to_gui_pipe, num, 1);
}


/*
 * gui_handle_events()
 *
 * This is called from the main UAE thread to handle the
 * processing of GUI-related events sent from the GUI thread.
 *
 * If the UAE emulation proper is not running yet or is paused,
 * this loops continuously waiting for and responding to events
 * until the emulation is started or resumed, respectively. When
 * the emulation is running, this is called periodically from
 * the main UAE event loop.
 */
void gui_handle_events (void)
{
    if (!gui_available)
			return;

    while (comm_pipe_has_data (&from_gui_pipe)) {
		int cmd = read_comm_pipe_int_blocking (&from_gui_pipe);

	switch (cmd) {
	    case UAECMD_EJECTDISK: {
			int n = read_comm_pipe_int_blocking (&from_gui_pipe);
			uae_sem_wait (&gui_sem);
			changed_prefs.floppyslots[n].df[0] = '\0';
			uae_sem_post (&gui_sem);
			if (pause_uae) {
			    /* When UAE is running it will notify the GUI when a disk has been inserted
			     * or removed itself. When UAE is paused, however, we need to do this ourselves
			     * or the change won't be realized in the GUI until UAE is resumed */
			    write_comm_pipe_int (&to_gui_pipe, GUICMD_DISKCHANGE, 0);
			    write_comm_pipe_int (&to_gui_pipe, n, 1);
			}
			break;
		    }
	    case UAECMD_INSERTDISK: {
			int n = read_comm_pipe_int_blocking (&from_gui_pipe);
			uae_sem_wait (&gui_sem);
			strncpy (changed_prefs.floppyslots[n].df, new_disk_string[n], 255);
			xfree (new_disk_string[n]);
			new_disk_string[n] = 0;
			changed_prefs.floppyslots[n].df[255] = '\0';
			uae_sem_post (&gui_sem);
			if (pause_uae) {
			    /* When UAE is running it will notify the GUI when a disk has been inserted
			     * or removed itself. When UAE is paused, however, we need to do this ourselves
			     * or the change won't be realized in the GUI until UAE is resumed */
			    write_comm_pipe_int (&to_gui_pipe, GUICMD_DISKCHANGE, 0);
			    write_comm_pipe_int (&to_gui_pipe, n, 1);
			}
			break;
		    }
	    case UAECMD_RESET:
			uae_reset (0, 0);
			break;
#ifdef DEBUGGER
	    case UAECMD_DEBUG:
			activate_debugger ();
			break;
#endif
	    case UAECMD_QUIT:
			uae_quit ();
			break;
	    case UAECMD_PAUSE:
			pause_uae = 1;
			uae_pause ();
			break;
	    case UAECMD_RESUME:
			pause_uae = 0;
			uae_resume ();
			break;
	    case UAECMD_SAVE_CONFIG:
			uae_sem_wait (&gui_sem);
			//uae_save_config ();
			uae_sem_post (&gui_sem);
			break;
	    case UAECMD_SELECT_ROM:
			uae_sem_wait (&gui_sem);
			strncpy (changed_prefs.romfile, gui_romname, 255);
			changed_prefs.romfile[255] = '\0';
			xfree (gui_romname);
			uae_sem_post (&gui_sem);
			break;
#ifdef SAVESTATE
	    case UAECMD_SAVESTATE_LOAD:
			uae_sem_wait (&gui_sem);
			savestate_initsave (gui_sstate_name, 0, 0, 0);
			savestate_state = STATE_DORESTORE;
			write_log ("Restoring state from '%s'...\n", gui_sstate_name);
			uae_sem_post (&gui_sem);
			break;
	    case UAECMD_SAVESTATE_SAVE:
			uae_sem_wait (&gui_sem);
			savestate_initsave (gui_sstate_name, 0, 0, 0);
			save_state (gui_sstate_name, "puae");
			write_log ("Saved state to '%s'...\n", gui_sstate_name);
			uae_sem_post (&gui_sem);
			break;
#endif
/*	    case UAECMD_START:
			uae_start ();
			break;
		case UAECMD_STOP:
			uae_stop ();
			break;*/
	}
    }
}

/*
 * gui_update()
 *
 * This is called from the main UAE thread to tell the GUI to update itself
 * using the current state of currprefs. This function will block
 * until it receives a message from the GUI telling it that the update
 * is complete.
 */
int gui_update (void)
{
    DEBUG_LOG( "Entered\n" );
    return 0;
    if (gui_available) {
		write_comm_pipe_int (&to_gui_pipe, GUICMD_UPDATE, 1);
		uae_sem_wait (&gui_update_sem);
    }
    return 0;
}


/*
 * gui_exit()
 *
 * This called from the main UAE thread to tell the GUI to gracefully
 * quit. We don't need to do anything here for now. Our main() takes
 * care of putting the GUI to bed.
 */
void gui_exit (void)
{
}

/*
 * gui_shutdown()
 *
 * Tell the GUI thread it's time to say goodnight...
 */
static void gui_shutdown (void)
{
    DEBUG_LOG( "Entered\n" );

    if (gui_available) {
	if (!quit_gui) {
	    quit_gui = 1;
	    DEBUG_LOG( "Waiting for GUI thread to quit.\n" );
	    uae_sem_wait (&gui_quit_sem);
	}
    }
}

static void gui_flicker_led2 (int led, int unitnum, int status)
{
        static int resetcounter[LED_MAX];
        uae_s8 old;
        uae_s8 *p;

        if (led == LED_HD)
                p = &gui_data.hd;
        else if (led == LED_CD)
                p = &gui_data.cd;
        else if (led == LED_MD)
                p = &gui_data.md;
        else
                return;
        old = *p;
        if (status == 0) {
                resetcounter[led]--;
                if (resetcounter[led] > 0)
                        return;
        }
#ifdef RETROPLATFORM
        if (led == LED_HD)
                rp_hd_activity (unitnum, status ? 1 : 0, status == 2 ? 1 : 0);
        else if (led == LED_CD)
                rp_cd_activity (unitnum, status);
#endif
        *p = status;
        resetcounter[led] = 6;
        if (old != *p)
                gui_led (led, *p);
}

void gui_flicker_led (int led, int unitnum, int status)
{
        if (led < 0) {
                gui_flicker_led2 (LED_HD, 0, 0);
                gui_flicker_led2 (LED_CD, 0, 0);
                gui_flicker_led2 (LED_MD, 0, 0);
        } else {
                gui_flicker_led2 (led, unitnum, status);
        }
}

void gui_display (int shortcut)
{
    DEBUG_LOG ("called with shortcut=%d\n", shortcut);

    if (gui_available) {
	/* If running fullscreen, then we must try to switched to windowed
	 * mode before activating the GUI */
	if (is_fullscreen ()) {
	    toggle_fullscreen (0);
	    if (is_fullscreen ()) {
		write_log ("Cannot activate GUI in full-screen mode\n");
		return;
	    }
	}

	if (shortcut == -1)
	    write_comm_pipe_int (&to_gui_pipe, GUICMD_SHOW, 1);

	if (shortcut >=0 && shortcut <4) {
	    /* In this case, shortcut is the drive number to display
	     * the insert requester for */
	    write_comm_pipe_int (&to_gui_pipe, GUICMD_FLOPPYDLG, 0);
	    write_comm_pipe_int (&to_gui_pipe, shortcut, 1);
	}
    }

	inputdevice_copyconfig (&changed_prefs, &currprefs);
	inputdevice_config_change_test ();
	clearallkeys ();
	inputdevice_acquire (true);
	setmouseactive (1);
}

void gui_message (const char *format,...)
{
    char msg[2048];
    va_list parms;

    va_start (parms,format);
    vsprintf ( msg, format, parms);
    va_end (parms);

    if (gui_available)
		do_message_box (NULL, msg, true, true);

    write_log ("%s", msg);
}

/*
 * do_message_box()
 *
 * This makes up for GTK's lack of a function for creating simple message dialogs.
 * It can be called from any context. gui_init() must have been called at some point
 * previously.
 *
 * title   - will be displayed in the dialog's titlebar (or NULL for default)
 * message - the message itself
 * modal   - should the dialog block input to the rest of the GUI
 * wait    - should the dialog wait until the user has acknowledged it
 */
static void do_message_box (const gchar *title, const gchar *message, gboolean modal, gboolean wait )
{
    uae_sem_t msg_quit_sem = {0};

    // If we a need reply, then this semaphore which will be used
    // to signal us when the dialog has been exited.
    uae_sem_init (&msg_quit_sem, 0, 0);

    write_comm_pipe_int   (&to_gui_pipe, GUICMD_MSGBOX, 0);
    write_comm_pipe_pvoid (&to_gui_pipe, (void *) title, 0);
    write_comm_pipe_pvoid (&to_gui_pipe, (void *) message, 0);
    write_comm_pipe_int   (&to_gui_pipe, (int) modal, 0);
    write_comm_pipe_pvoid (&to_gui_pipe, wait?&msg_quit_sem:NULL, 1);

    if (wait)
	uae_sem_wait (&msg_quit_sem);

    DEBUG_LOG ("do_message_box() done");
    return;
}

/*
 * handle_message_box_request()
 *
 * This is called from the GUI's context in repsonse to do_message_box()
 * to actually create the dialog box
 */
static void handle_message_box_request (smp_comm_pipe *msg_pipe)
{
    const gchar *title      = (const gchar *)  read_comm_pipe_pvoid_blocking (msg_pipe);
    const gchar *msg        = (const gchar *)  read_comm_pipe_pvoid_blocking (msg_pipe);
    int modal               =                  read_comm_pipe_int_blocking   (msg_pipe);
    uae_sem_t *msg_quit_sem = (uae_sem_t *)    read_comm_pipe_pvoid_blocking (msg_pipe);

    GtkWidget *dialog = make_message_box (title, msg, modal, msg_quit_sem);
}

/*
 * on_message_box_quit()
 *
 * Handler called when message box is exited. Signals anybody that cares
 * via the semaphore it is supplied.
 */
void on_message_box_quit (GtkWidget *w, gpointer user_data)
{
    uae_sem_post ((uae_sem_t *)user_data);
}

/*
 * make_message_box()
 *
 * This does the actual work of constructing the message dialog.
 *
 * title   - displayed in the dialog's titlebar
 * message - the message itself
 * modal   - whether the dialog should block input to the rest of the GUI
 * sem     - semaphore used for signalling that the dialog's finished
 *
 * TODO: Make that semaphore go away. We shouldn't need to know about it here.
 */
static GtkWidget *make_message_box (const gchar *title, const gchar *message, int modal, uae_sem_t *sem )
{
    GtkWidget *dialog;
    GtkWidget *vbox;
    GtkWidget *label;
    GtkWidget *hseparator;
    GtkWidget *hbuttonbox;
    GtkWidget *button;
    guint      key;
    GtkAccelGroup *accel_group;

    accel_group = gtk_accel_group_new ();

    dialog = gtk_window_new ( GTK_WINDOW_TOPLEVEL /*GTK_WINDOW_DIALOG*/);
    gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);
    if (title==NULL || (title!=NULL && strlen(title)==0))
	title = PACKAGE_NAME " information";
    gtk_window_set_title (GTK_WINDOW (dialog), title);
    gtk_window_set_modal (GTK_WINDOW (dialog), modal);
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);

    vbox = gtk_vbox_new (false, 0);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (dialog), vbox);

    label = gtk_label_new (message);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (vbox), label, true, true, 0);
    gtk_label_set_line_wrap (GTK_LABEL (label), true);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_box_pack_start (GTK_BOX (vbox), hseparator, false, false, 8);

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, false, false, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox), 4);

    button = make_labelled_button ("_Okay", accel_group);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

    if (sem)
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (on_message_box_quit), sem);
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
						   GTK_SIGNAL_FUNC (gtk_widget_destroy),
						   GTK_OBJECT (dialog));

    gtk_widget_grab_default (button);
    gtk_window_add_accel_group (GTK_WINDOW (dialog), accel_group);
    gtk_widget_show( dialog );

    return dialog;
}

int gui_init (void)
{
    uae_thread_id tid;

    /* Check whether we are running with SUID bit set */
    if (getuid() == geteuid()) {
	/*
	 * Only try to start Gtk+ GUI if SUID bit is not set
	 */

	init_comm_pipe (&to_gui_pipe, 20, 1);
	init_comm_pipe (&from_gui_pipe, 20, 1);
	uae_sem_init (&gui_sem, 0, 1);          // Unlock mutex on prefs $
	uae_sem_init (&gui_update_sem, 0, 0);
	uae_sem_init (&gui_init_sem, 0, 0);
	uae_sem_init (&gui_quit_sem, 0, 0);

	/* Start GUI thread to construct GUI */
	uae_start_thread ("GTK-GUI", gtk_gui_thread, NULL, &tid);

	/* Wait until GUI thread is ready */
	DEBUG_LOG ("Waiting for GUI thread\n");
	uae_sem_wait (&gui_init_sem);
	DEBUG_LOG ("Okay\n");
    }
	return 1;
}

void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected) {}
void gui_lock (void) {}
void gui_unlock (void) {}
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
