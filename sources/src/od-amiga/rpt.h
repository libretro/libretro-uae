/*
  * UAE - The Un*x Amiga Emulator
  *
  * Cycle counting using AmigaOS EClock
  *
  * Copyright 2003 Richard Drummond
  */

#include <devices/timer.h>
#include <proto/timer.h>

typedef unsigned long frame_time_t;

STATIC_INLINE frame_time_t read_processor_time (void)
{
    struct EClockVal etime;

    ReadEClock (&etime);
    return etime.ev_lo;
}
