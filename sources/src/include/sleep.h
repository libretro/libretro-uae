#ifndef UAE_SLEEP_H
#define UAE_SLEEP_H

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Wrapper for platform-specific sleep routine
  *
  * Copyright 2003-2005 Richard Drummond
  */

#ifdef __BEOS__
# include <be/kernel/OS.h>
#else
# ifdef TARGET_AMIGAOS
#  include <proto/dos.h>
#  include <clib/alib_protos.h>
# else
#  ifdef USE_SDL
#   include <SDL_timer.h>
#  endif
# endif
#endif

#ifdef __LIBRETRO__
#include "retro_timers.h"
#define uae_msleep retro_sleep
#endif

#define ONE_THOUSAND	1000
#define ONE_MILLION	(1000 * 1000)

/* The following would offer no advantage since we always call use_msleep() with
 * a constant and all arithmetic can be done at compile time. */
#if 0
/* This may seem a little odd, but using a nice, round binary factor will speed
 * up the arithmetic for only a 2% error (5% for nanosleep). Our target sleep
 * routines are not tha accurate - and even if they were, we don't need more
 * accuracy.
 */
#define ONE_THOUSAND	1024
#define ONE_MILLION	(1024 * 1024)
#endif

/*
 * void msleep (int ms)
 *
 * Sleep for ms milliseconds using an appropriate system-dependent sleep
 * functions.
 */
#ifndef __LIBRETRO__

#ifdef __BEOS__
# define uae_msleep(msecs) snooze (msecs * ONE_THOUSAND)
#elif defined(WIIU)
#define uae_msleep(msecs) //usleep (msecs * ONE_THOUSAND)
#elif defined(__PS3__)
#define uae_msleep(msec) sys_timer_usleep(msec*1000)
#elif defined TARGET_AMIGAOS
#   if defined __amigaos4__ || defined __MORPHOS__
#    define uae_msleep(msecs) TimeDelay (0, msecs / ONE_THOUSAND, (msecs % ONE_THOUSAND) * ONE_THOUSAND)
#   else
#    define uae_msleep(msecs) Delay (msecs <= 20 ? 1 : msecs/20);
# endif // __amigaos4__ || __MORPHOS__
#elif defined HAVE_NANOSLEEP
#ifndef _WIN32
#    define uae_msleep(msecs) \
	    { \
		if (msecs < 1000) { \
		    struct timespec t = { 0, (msecs) * ONE_MILLION }; \
		    nanosleep (&t, 0); \
		} else { \
		    int secs      = msecs / ONE_THOUSAND; \
		    int millisecs = msecs % ONE_THOUSAND; \
		    struct timespec t = { secs, millisecs * ONE_MILLION }; \
		    nanosleep (&t, 0); \
		} \
	    }
#else  
#define uae_msleep(msecs) \
	    { usleep (msecs * ONE_THOUSAND); }

#endif
#elif defined HAVE_USLEEP
#     define uae_msleep(msecs) usleep (msecs * ONE_THOUSAND)
#elif defined USE_SDL
#      define uae_msleep(msecs) SDL_Delay (msecs)
#     else
#      error "No system sleep function found"
#endif

#endif /* __LIBRETRO__ */
void sleep_test (void);

#endif /* UAE_SLEEP_H */
