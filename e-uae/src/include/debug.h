 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Debugger
  *
  * (c) 1995 Bernd Schmidt
  *
  */

#ifdef DEBUGGER

extern int debugging;
extern int exception_debugging;
extern int debug_copper;
extern int debug_sprite_mask;

extern void debug (void);
extern void activate_debugger (void);

extern void record_copper (uaecptr addr, unsigned int hpos, unsigned int vpos);
extern void record_copper_reset (void);

#endif
