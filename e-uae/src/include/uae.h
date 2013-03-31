 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Prototypes for main.c
  *
  * Copyright 1996 Bernd Schmidt
  * Copyright 2006-2007 Richard Drummond
  */

extern void real_main (int, char **);
extern void usage (void);

extern void sleep_millis (int ms);
extern void sleep_millis_busy (int ms);

extern void uae_start (void);
extern void uae_pause (void);
extern void uae_resume (void);
extern void uae_reset (int);
extern void uae_quit (void);
extern void uae_stop (void);
extern void uae_restart (int, char*);
extern void uae_save_config (void);

extern void setup_brkhandler (void);

#ifdef WIN32
extern char warning_buffer[256];
extern char *start_path;

void logging_init (void);
void filesys_init (void);
#endif

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
