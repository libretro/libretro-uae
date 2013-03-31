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
#include <sys/systime.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

#include "machdep/rpt.h"


STATIC_INLINE frame_time_t osdep_gethrtime (void)
{
    
#ifndef _ANDROID_

#ifdef PS3PORT

#warning "GetTick PS3\n"

	unsigned long	ticks_micro;
	uint64_t secs;
	uint64_t nsecs;

	sys_time_get_current_time(&secs, &nsecs);
	ticks_micro =  secs * 1000000UL + (nsecs / 1000);

	return ticks_micro;
#else
   	struct timeval tv;
   	gettimeofday (&tv, NULL);
   	return tv.tv_sec*1000000 + tv.tv_usec;
#endif

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
