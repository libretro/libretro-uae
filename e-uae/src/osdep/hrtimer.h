/*
 * E-UAE - The portable Amiga Emulator
 *
 * Generic high-resolution timer support.
 *
 * (c) 2005 Richard Drummond
 */

#ifndef EUAE_OSDEP_HRTIMER_H
#define EUAE_OSDEP_HRTIMER_H

#ifdef PS3PORT
#include "sys/sys_time.h"
#include "sys/timer.h"
#define usleep  sys_timer_usleep

STATIC_INLINE void gettimeofday (struct timeval *tv, void *blah)
{
    int64_t time = sys_time_get_system_time();

    tv->tv_sec  = time / 1000000;
    tv->tv_usec = time - (tv->tv_sec * 1000000);  // implicit rounding will take care of this for us
}

#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

#include "machdep/rpt.h"


STATIC_INLINE frame_time_t osdep_gethrtime (void)
{
    
#ifndef _ANDROID_

   	struct timeval tv;
   	gettimeofday (&tv, NULL);
   	return tv.tv_sec*1000000 + tv.tv_usec;

#else

    	struct timespec now;
    	clock_gettime(CLOCK_MONOTONIC, &now);
    	return now.tv_sec*1000000 + now.tv_nsec/1000;
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
