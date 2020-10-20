/*
 * E-UAE - The portable Amiga Emulator
 *
 * Generic high-resolution timer support.
 *
 * (c) 2005 Richard Drummond
 */

#ifndef EUAE_OSDEP_HRTIMER_H
#define EUAE_OSDEP_HRTIMER_H
#ifdef WIIU
#include <features_cpu.h>
#endif
#ifdef __CELLOS_LV2__
#include "ps3_headers.h"
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#ifdef VITA
#include <psp2/kernel/processmgr.h>
#define usleep sceKernelDelayThreadCB;
#endif

#include "machdep/rpt.h"


STATIC_INLINE frame_time_t osdep_gethrtime (void)
{
#ifndef _ANDROID_

#ifdef WIIU
   return cpu_features_get_time_usec();
#else
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return tv.tv_sec*1000000 + tv.tv_usec;
#endif

#else
#define osd_ticks_t uae_s64
   struct timeval    tp;
   static osd_ticks_t start_sec1 = 0;

   gettimeofday(&tp, NULL);
   if (start_sec1==0)
      start_sec1 = tp.tv_sec;
   return (tp.tv_sec - start_sec1) * (osd_ticks_t) 1000000 + tp.tv_usec;
#endif
}

STATIC_INLINE frame_time_t osdep_gethrtimebase (void)
{
    return 1000000;
}

STATIC_INLINE void osdep_inithrtimer (void)
{
}
#endif
