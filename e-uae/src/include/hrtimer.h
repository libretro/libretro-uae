/*
 * E-UAE - The portable Amiga Emulator
 *
 * High-resolution timer support.
 *
 * (c) 2005 Richard Drummond
 */

#ifndef EUAE_HRTIMER_H
#define EUAE_HRTIMER_H

#include "machdep/rpt.h"
#include "osdep/hrtimer.h"

STATIC_INLINE frame_time_t uae_gethrtime (void)
{
#ifdef HAVE_MACHDEP_TIMER
    if (currprefs.use_processor_clock)
	return machdep_gethrtime ();
    else
#endif
	return osdep_gethrtime ();
}

STATIC_INLINE frame_time_t uae_gethrtimebase (void)
{
#ifdef HAVE_MACHDEP_TIMER
   if (currprefs.use_processor_clock)
       return machdep_gethrtimebase ();
   else
#endif
       return osdep_gethrtimebase ();
}

STATIC_INLINE void uae_inithrtimer (void)
{
#ifdef HAVE_MACHDEP_TIMER
   if (currprefs.use_processor_clock && machdep_inithrtimer ())
       return;
   else
#endif
       osdep_inithrtimer ();
}

#endif
