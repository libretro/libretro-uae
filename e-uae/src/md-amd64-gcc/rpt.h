 /*
  * E-UAE - The portable Amiga Emulator
  *
  * Read timestamp counter on an AMD64
  *
  * Copyright 2005 Richard Drummond
  *
  * Derived from the i386 version:
  * Copyright 1997, 1998 Bernd Schmidt
  * Copyright 2003-2005  Richard Drummond
  */

#ifndef EUAE_MACHDEP_RPT_H
#define EUAE_MACHDEP_RPT_H

STATIC_INLINE uae_s64 read_processor_time (void)
{
    uae_s64 foo1, foo2;
    uae_s64 tsc;


    /* Don't assume the assembler knows rdtsc */
    __asm__ __volatile__ (".byte 0x0f,0x31" : "=a" (foo1), "=d" (foo2) :);
    tsc = foo2 << 32 | foo1;

#ifdef __linux__
    /* Hack to synchronize syncbase and re-compute
     * vsynctime when TSC frequency changes */

/* How many times per second tsc will be synced */
#define TSC_SYNC_FREQUENCY 8
    {
	extern frame_time_t linux_get_tsc_freq (void);
	extern void         compute_vsynctime (void);
        extern frame_time_t syncbase;

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
#endif
    return tsc;
}

STATIC_INLINE frame_time_t machdep_gethrtime (void)
{
    return read_processor_time ();
}

frame_time_t machdep_gethrtimebase (void);
int          machdep_inithrtimer   (void);

#endif /* EUAE_MACHDEP_RPT_H */
