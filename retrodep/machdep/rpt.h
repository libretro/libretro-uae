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

#include <sys/time.h>

#ifndef osd_ticks_t
#define osd_ticks_t uae_s64
#endif

#ifndef frame_time_t
#define frame_time_t uae_s64
#endif

#if defined(WIIU)
#include <features_cpu.h>
#endif

#ifdef __CELLOS_LV2__
#include <sys/sys_time.h>
#endif

STATIC_INLINE uae_s64 read_processor_time(void)
{
#ifdef __CELLOS_LV2__
   unsigned long	ticks_micro;
   uint64_t secs;
   uint64_t nsecs;

   sys_time_get_current_time(&secs, &nsecs);
   ticks_micro = secs * 1000000UL + (nsecs / 1000);

   return ticks_micro;
#elif defined(WIIU)
    static osd_ticks_t start_sec = 0;
    uint64_t secs;
    uint64_t usecs;
    int64_t time = cpu_features_get_time_usec();

    secs  = time / 1000000;
    usecs = time - (secs * 1000000); 
    if (start_sec==0)
      start_sec = secs;
    return (secs - start_sec) * (osd_ticks_t) 1000000 + usecs;
#else
   struct timeval    tp;
   static osd_ticks_t start_sec = 0;

   gettimeofday(&tp, NULL);
   if (start_sec==0)
      start_sec = tp.tv_sec;
   return (tp.tv_sec - start_sec) * (osd_ticks_t) 1000000 + tp.tv_usec;
#endif
}

STATIC_INLINE frame_time_t machdep_gethrtime (void)
{
   return read_processor_time();
}

frame_time_t machdep_gethrtimebase (void);
int          machdep_inithrtimer   (void);

#endif /* EUAE_MACHDEP_RPT_H */
