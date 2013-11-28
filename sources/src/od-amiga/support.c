 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Misc support code for AmigaOS target
  *
  * Copyright 2003-2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <proto/timer.h>

#include "sleep.h"
#include "osdep/hrtimer.h"

int truncate (const char *path, off_t length)
{
   /* FIXME */
   return 0;
}


frame_time_t timebase;

#ifndef __AROS__
int osdep_inithrtimer (void)
{
    static int done = 0;

    if (!done) {
	struct EClockVal etime;

	timebase  = ReadEClock (&etime);
	write_log ("EClock frequency:%.6f MHz\n", timebase/1e6);

	done = 1;
    }

    return 1;
}

frame_time_t osdep_gethrtimebase (void)
{
    return timebase;
}
#endif
