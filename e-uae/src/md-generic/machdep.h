/*
 * E-UAE - The portable Amiga Emulator
 *
 * Processor-specific definitions
 */

#ifndef MACHDEP_MACHDEP_H
#define MACHDEP_MACHDEP_H

#define MACHDEP_GENERIC
#define MACHDEP_NAME    "cpu"

#undef  HAVE_MACHDEP_TIMER

typedef uae_s32 frame_time_t;
#define MAX_FRAME_TIME ((1<<31)-1)

#endif
