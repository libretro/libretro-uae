/*
 * E-UAE - The portable Amiga Emulator
 *
 * AmigaOS high-resolution timer support.
 *
 * (c) 2005 Richard Drummond
 */

#ifdef __AROS__
# include "od-generic/hrtimer.h"
#else
# ifndef EUAE_OSDEP_HRTIMER_H
#  define EUAE_OSDEP_HRTIMER_H

#  include "machdep/rpt.h"

#  include <devices/timer.h>
#  include <proto/timer.h>

STATIC_INLINE frame_time_t osdep_gethrtime (void)
{
    struct EClockVal etime;

    ReadEClock (&etime);
    return etime.ev_lo;
}

extern frame_time_t osdep_gethrtimebase (void);
extern int          osdep_inithrtimer   (void);
# endif

#endif
