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


#if !defined(ANDPORT) && !defined(WIN32PORT)

#ifdef PS3PORT

#warning "GetTick PS3\n"

STATIC_INLINE uae_s64 read_processor_time (void)
{
unsigned long	ticks_micro;
uint64_t secs;
uint64_t nsecs;

sys_time_get_current_time(&secs, &nsecs);
ticks_micro = secs * 1000000UL + (nsecs / 1000);

return ticks_micro;
}
#else

STATIC_INLINE uae_s64 read_processor_time (void)
{
    uae_s64 tsc;

    /* Don't assume the assembler knows rdtsc */
    __asm__ __volatile__ (".byte 0x0f,0x31" : "=A" (tsc) :);

#ifdef __linux__ 
    /* Hack to synchronize syncbase and re-compute
     * vsynctime when TSC frequency changes */

/* How many times per second tsc will be synced */
#define TSC_SYNC_FREQUENCY 8
    {
	extern frame_time_t linux_get_tsc_freq (void);
	extern void         compute_vsynctime (void);
//	extern frame_time_t syncbaseo;
	extern int syncbase;

	static frame_time_t next_tsc_synctime;
	static frame_time_t prev_syncbase;

	if (tsc > next_tsc_synctime) {
	    uae_s64 new_tsc_freq = linux_get_tsc_freq ();

	    if (new_tsc_freq > 0) {
		syncbase = new_tsc_freq;
		next_tsc_synctime = tsc + (syncbase / TSC_SYNC_FREQUENCY);

		if (syncbase != prev_syncbase) {
		    prev_syncbase = syncbase;
		    compute_vsynctime ();
		}
	    }
	}
    }

    return tsc;
}
#endif 

#endif
#else
#include <sys/time.h>
/*
uint64_t Clock::realTime() {
	timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return (((uint64_t)now.tv_sec * 1000000000LL) + now.tv_nsec);
}
*/
#define osd_ticks_t uae_s64
STATIC_INLINE uae_s64 read_processor_time (void)
{

                struct timeval    tp;
                static osd_ticks_t start_sec = 0;
               
                gettimeofday(&tp, NULL);
                if (start_sec==0)
                        start_sec = tp.tv_sec;
                return (tp.tv_sec - start_sec) * (osd_ticks_t) 1000000 + tp.tv_usec;

/*
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (((uae_s64)now.tv_sec * 1000000000LL) + now.tv_nsec);
*/
/*
    	struct timespec now;
    	clock_gettime(CLOCK_MONOTONIC, &now);
    	return now.tv_sec*1000000 + now.tv_nsec/1000;
*/
}

#endif

#define frame_time_t uae_s64

STATIC_INLINE frame_time_t machdep_gethrtime (void)
{
    return read_processor_time ();
}

frame_time_t machdep_gethrtimebase (void);
int          machdep_inithrtimer   (void);

#endif /* EUAE_MACHDEP_RPT_H */
