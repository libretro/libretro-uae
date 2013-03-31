 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the Tcl/Tk GUI
  *
  * Copyright 1996 Bernd Schmidt
  */

extern void gui_init (int argc, char **argv);
extern int gui_open (void);
extern int gui_update (void);
extern void gui_exit (void);
extern void gui_led (int, int);
extern void gui_handle_events (void);
extern void gui_filename (int, const char *);
extern void gui_fps (int fps, int idle);
extern void gui_hd_led (int);
extern void gui_cd_led (int);
extern unsigned int gui_ledstate;
extern void gui_display (int shortcut);
extern void gui_notify_state (int state);

struct gui_info
{
    uae_u8 drive_motor[4];          /* motor on off */
    uae_u8 drive_track[4];          /* rw-head track */
    uae_u8 drive_writing[4];        /* drive is writing */
    uae_u8 drive_disabled[4];	    /* drive is disabled */
    uae_u8 powerled;                /* state of power led */
    uae_u8 drive_side;		    /* floppy side */
    uae_u8 hd;			    /* harddrive */
    uae_u8 cd;			    /* CD */
    int fps, idle;
    char df[4][256];		    /* inserted image */
    uae_u32 crc32[4];		    /* crc32 of image */
};
#define NUM_LEDS (1 + 1 + 1 + 1 + 1 + 4)

extern struct gui_info gui_data;
