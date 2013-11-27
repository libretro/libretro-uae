 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the Tcl/Tk GUI
  *
  * Copyright 1996 Bernd Schmidt
  */

extern int gui_init (void);
extern int gui_update (void);
extern void gui_exit (void);
extern void gui_led (int, int);
extern void gui_handle_events (void);
extern void gui_filename (int, const TCHAR *);
extern void gui_fps (int fps, int idle, int color);
extern void gui_changesettings (void);
extern void gui_lock (void);
extern void gui_unlock (void);
extern void gui_flicker_led (int, int, int);
extern void gui_disk_image_change (int, const TCHAR *, bool writeprotected);
extern unsigned int gui_ledstate;
extern void gui_display (int shortcut);

extern void gui_gameport_button_change (int port, int button, int onoff);
extern void gui_gameport_axis_change (int port, int axis, int state, int max);

extern bool no_gui, quit_to_gui;

#define LED_CD_ACTIVE 1
#define LED_CD_ACTIVE2 2
#define LED_CD_AUDIO 4

#define LED_POWER 0
#define LED_DF0 1
#define LED_DF1 2
#define LED_DF2 3
#define LED_DF3 4
#define LED_HD 5
#define LED_CD 6
#define LED_FPS 7
#define LED_CPU 8
#define LED_SND 9
#define LED_MD 10
#define LED_MAX 11

struct gui_info
{
	bool drive_motor[4];		/* motor on off */
	uae_u8 drive_track[4];		/* rw-head track */
	bool drive_writing[4];		/* drive is writing */
	bool drive_disabled[4];		/* drive is disabled */
	bool powerled;			/* state of power led */
	uae_u8 powerled_brightness;	/* 0 to 255 */
	uae_s8 drive_side;		/* floppy side */
	uae_s8 hd;			/* harddrive */
	uae_s8 cd;			/* CD */
	uae_s8 md;			/* CD32 or CDTV internal storage */
	bool cpu_halted;
	int fps, idle;
	int fps_color;
	int sndbuf, sndbuf_status;
	TCHAR df[4][256];		/* inserted image */
	uae_u32 crc32[4];		/* crc32 of image */
};
#define NUM_LEDS (LED_MAX)
#define VISIBLE_LEDS (LED_MAX - 1)

extern struct gui_info gui_data;

/* Functions to be called when prefs are changed by non-gui code.  */
extern void gui_update_gfx (void);

#define notify_user gui_message
#define NUMSG_NEEDEXT2      "The software uses a non-standard floppy disk format. You may need to use a custom floppy disk image file instead of a standard one. This message will not appear again."
#define NUMSG_NOROMKEY      "Could not find system ROM key file."
#define NUMSG_KSROMCRCERROR "System ROM checksum incorrect. The system ROM image file may be corrupt."
#define NUMSG_KSROMREADERROR "Error while reading system ROM."
#define NUMSG_NOEXTROM      "No extended ROM found."
#define NUMSG_MODRIP_NOTFOUND "No music modules or packed data found."
#define NUMSG_MODRIP_FINISHED "Scan finished.\n"
#define NUMSG_MODRIP_SAVE   "Module/packed data found\n%s\nStart address %08.8X, Size %d bytes\nWould you like to save it?"
#define NUMSG_KS68020       "The selected system ROM requires a 68020 with 32-bit addressing or 68030 or higher CPU."
#define NUMSG_ROMNEED       "One of the following system ROMs is required:\n\n%s\n\nCheck the System ROM path in the Paths panel and click Rescan ROMs."
#define NUMSG_STATEHD       "WARNING: Current configuration is not fully compatible with state saves.\nThis message will not appear again."
#define NUMSG_NOCAPS        "Selected disk image needs the SPS plugin\nwhich is available from\nhttp//www.softpres.org/"
#define NUMSG_OLDCAPS       "You need an updated SPS plugin\nwhich is available from\nhttp//www.softpres.org/"
#define NUMSG_KS68EC020     "The selected system ROM requires a 68020 with 24-bit addressing or higher CPU."
#define NUMSG_KICKREP       "You need to have a floppy disk (image file) in DF0: to use the system ROM replacement."
#define NUMSG_KICKREPNO     "The floppy disk (image file) in DF0: is not compatible with the system ROM replacement functionality."
#define NUMSG_NOROM         "Could not load system ROM, trying system ROM replacement.\n"
#define NUMSG_EXPROMNEED    "One of the following expansion boot ROMs is required:\n\n%s\n\nCheck the System ROM path in the Paths panel and click Rescan ROMs."
#define NUMSG_KS68030       "The selected system ROM requires a 68030 CPU."
#define NUMSG_KS68030PLUS   "The selected system ROM requires a 68030 or higher CPU."

