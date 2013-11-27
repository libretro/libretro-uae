 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Prototypes for main.c
  *
  * Copyright 1996 Bernd Schmidt
  * Copyright 2006-2007 Richard Drummond
  */

extern void do_start_program (void);
extern void do_leave_program (void);
extern void start_program (void);
extern void leave_program (void);
extern void real_main (int, TCHAR **);
extern void virtualdevice_init (void);
extern void usage (void);
extern void sleep_millis (int ms);
extern void sleep_millis_main (int ms);
extern void sleep_millis_busy (int ms);
extern int sleep_resolution;

#define UAE_QUIT 1
#define UAE_RESET 2
#define UAE_RESET_KEYBOARD 3
#define UAE_RESET_HARD 4

extern void uae_reset (int, int);
extern void uae_quit (void);
extern void uae_restart (int, const TCHAR*);
extern void reset_all_systems (void);
extern void target_reset (void);
extern void target_addtorecent (const TCHAR*, int);
extern void target_run (void);
extern void target_quit (void);
extern void target_restart (void);
extern bool get_plugin_path (TCHAR *out, int size, const TCHAR *path);
extern void stripslashes (TCHAR *p);
extern void fixtrailing (TCHAR *p);
extern void fullpath (TCHAR *path, int size);
extern void getpathpart (TCHAR *outpath, int size, const TCHAR *inpath);
extern void getfilepart (TCHAR *out, int size, const TCHAR *path);
extern uae_u32 getlocaltime (void);

extern int quit_program;
extern bool console_emulation;

extern TCHAR warning_buffer[256];
extern TCHAR start_path_data[];
extern TCHAR start_path_data_exe[];
extern TCHAR start_path_plugins[];

extern void setup_brkhandler (void);

#ifdef USE_SDL
int init_sdl (void);
#endif

#define UAE_STATE_STOPPED    0
#define UAE_STATE_RUNNING    1
#define UAE_STATE_PAUSED     2
#define UAE_STATE_COLD_START 3
#define UAE_STATE_WARM_START 4
#define UAE_STATE_QUITTING   5

int uae_get_state (void);
int uae_state_change_pending (void);
