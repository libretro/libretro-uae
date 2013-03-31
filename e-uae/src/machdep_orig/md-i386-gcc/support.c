 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Miscellaneous machine dependent support functions and definitions
  *
  * Copyright 1996 Bernd Schmidt
  * Copyright 2003-2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "sleep.h"
#include "machdep/rpt.h"
#include "machdep/m68k.h"

#include <signal.h>


#ifdef __linux__
/*
 * Extract x86/AMD64 timestamp counter frequency
 * from /proc/cpuinfo.
 *
 * TODO: Make this more robust.
 */
frame_time_t linux_get_tsc_freq (void)
{
    int  cpuinfo_fd;
    char buffer[1024];
    uae_s64 tsc_freq = 0;

    cpuinfo_fd = open ("/proc/cpuinfo", O_RDONLY);

    if (cpuinfo_fd >= 0) {
	char *ptr       = &buffer[0];
	int   size_read = read (cpuinfo_fd, ptr, 1024);

	while (size_read > 0) {
	   if (strncmp (ptr, "bogomips\t: ", 11) != 0) {
		while ((size_read-- > 0) && (*ptr != '\n'))
		    ptr++;
		size_read--;
		ptr++;
		continue;
	    } else {
		ptr += 11;
		tsc_freq = atoll (ptr) * 1000000 / 2;
	    }
	}
   }
   close (cpuinfo_fd);

   return tsc_freq;
}
#endif

#ifdef __BEOS__

# include <be/kernel/OS.h>

/*
 * Get timestamp counter frequency from the kernel
 */
static frame_time_t beos_get_tsc_freq (void)
{
    system_info info;

    get_system_info (&info);

    return info.cpu_clock_speed;
}
#endif

static volatile frame_time_t last_time, best_time;
static frame_time_t timebase;

static volatile int loops_to_go;
static int rpt_available;

#if defined HAVE_SETITIMER || defined HAVE_ALARM
# define USE_ALARM
# ifndef HAVE_SETITIMER
#  define TIME_UNIT 1000000
# else
#  define TIME_UNIT 100000
# endif
#else
# define TIME_DELAY 200
# define TIME_UNIT  (TIME_DELAY*1000)
#endif

#ifndef HAVE_SYNC
# define sync()
#endif

#ifdef USE_ALARM
static void set_the_alarm (void)
{
# ifndef HAVE_SETITIMER
    alarm (1);
# else
    struct itimerval t;
    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = TIME_UNIT;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = TIME_UNIT;
    setitimer (ITIMER_REAL, &t, NULL);
# endif
}

static int first_loop = 1;

#ifdef __cplusplus
static RETSIGTYPE alarmhandler(...)
#else
static RETSIGTYPE alarmhandler(int foo)
#endif
{
    frame_time_t bar;
    bar = read_processor_time ();
    if (! first_loop && bar - last_time < best_time)
	best_time = bar - last_time;
    first_loop = 0;
    if (--loops_to_go > 0) {
	signal (SIGALRM, alarmhandler);
	last_time = read_processor_time ();
	set_the_alarm ();
    } else {
	alarm (0);
	signal (SIGALRM, SIG_IGN);
    }
}
#endif /* USE_ALARM */

#include <setjmp.h>
static jmp_buf catch_test;

#ifdef __cplusplus
static RETSIGTYPE illhandler (...)
#else
static RETSIGTYPE illhandler (int foo)
#endif
{
    rpt_available = 0;
    longjmp (catch_test, 1);
}

int machdep_inithrtimer (void)
{
    static int done = 0;

    if (!done) {
	rpt_available = 1;

	write_log ("Testing the RDTSC instruction ... ");
	signal (SIGILL, illhandler);
	if (setjmp (catch_test) == 0)
	    read_processor_time ();
	signal (SIGILL, SIG_DFL);
	write_log ("done.\n");

	if (! rpt_available) {
	    write_log ("Your processor does not support the RDTSC instruction.\n");
	    return 0;
	}

	timebase = 0;

#ifdef __linux__
	timebase = linux_get_tsc_freq ();
#else
# ifdef __BEOS__
	timebase = beos_get_tsc_freq ();
# endif
#endif

	if (timebase <= 0) {

	    write_log ("Calibrating TSC frequency...");
	    flush_log ();

	    best_time = MAX_FRAME_TIME;
	    loops_to_go = 5;

#ifdef USE_ALARM
	    signal (SIGALRM, alarmhandler);
#endif

 	    /* We want exact values... */
	    sync (); sync (); sync ();

#ifdef USE_ALARM
	    last_time = read_processor_time ();
	    set_the_alarm ();

	    while (loops_to_go != 0)
		uae_msleep (10);
#else
	    int i = loops_to_go;
	    frame_time_t bar;

	    while (i-- > 0) {
		last_time = read_processor_time ();
		uae_msleep (TIME_DELAY);
		bar = read_processor_time ();
		if (i != loops_to_go && bar - last_time < best_time)
		    best_time = bar - last_time;
	    }
#endif

	    timebase = best_time * (1000000.0 / TIME_UNIT);
	}

	write_log ("TSC frequency: %f MHz\n", timebase / 1000000.0);

	done = 1;
     }
     return done;
}

frame_time_t machdep_gethrtimebase (void)
{
    return timebase;
}

void machdep_init (void)
{
}

/*
 * Handle processor-specific cfgfile options
 */
void machdep_save_options (FILE *f, const struct uae_prefs *p)
{
    cfgfile_write (f, MACHDEP_NAME ".use_tsc=%s\n", p->use_processor_clock ? "yes" : "no");
}

int machdep_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return cfgfile_yesno (option, value, "use_tsc", &p->use_processor_clock);
}

void machdep_default_options (struct uae_prefs *p)
{
    p->use_processor_clock = 1;
}
