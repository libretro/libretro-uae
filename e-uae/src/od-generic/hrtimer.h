/*
 * E-UAE - The portable Amiga Emulator
 *
 * Generic high-resolution timer support.
 *
 * (c) 2005 Richard Drummond
 */

#ifndef EUAE_OSDEP_HRTIMER_H
#define EUAE_OSDEP_HRTIMER_H

#include <sys/time.h>
#include <time.h>

#include "machdep/rpt.h"

STATIC_INLINE frame_time_t osdep_gethrtime (void)
{
#if HAVE_GETTIMEOFDAY
    struct timeval t;
    gettimeofday (&t, 0);
    return t.tv_sec * 1000000 + t.tv_usec;
#else
#error "No timer support available!"
#endif
}

STATIC_INLINE frame_time_t osdep_gethrtimebase (void)
{
#if HAVE_GETTIMEOFDAY
    return 1000000;
#endif
}

STATIC_INLINE void osdep_inithrtimer (void)
{
}
#endif
