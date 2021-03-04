 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Definitions for accessing cycle counters on a given machine, if possible.
  *
  * Copyright 1997, 1998 Bernd Schmidt
  * Copyright 2003-2005 Richard Drummond
  */

#ifndef EUAE_MACHDEP_RPT_H
#define EUAE_MACHDEP_RPT_H

#ifndef frame_time_t
#define frame_time_t uae_s64
#endif

extern long retro_ticks(void);

STATIC_INLINE uae_s64 read_processor_time(void)
{
   return retro_ticks();
}

STATIC_INLINE frame_time_t machdep_gethrtime (void)
{
   return read_processor_time();
}

frame_time_t machdep_gethrtimebase (void);
int          machdep_inithrtimer   (void);

#endif /* EUAE_MACHDEP_RPT_H */
