/*
 * E-UAE - The portable Amiga Emulator
 *
 * Processor-specific definitions
 *
 * Copyright 2005 Richard Drummond
 */

#ifndef MACHDEP_MACHDEP_H
#define MACHDEP_MACHDEP_H

#define MACHDEP_X86
#define MACHDEP_NAME    "amd64"

#define HAVE_MACHDEP_TIMER

typedef uae_s64 frame_time_t;
#define MAX_FRAME_TIME 9223372036854775807LL

#endif
