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

#ifndef HAVE_SYNC
# define sync()
#endif

frame_time_t timebase;

/*
 * Get PPC timebase frequency from OF device tree on Linux
 */
#ifdef __linux__
static uae_u32 find_timebase (const char *path)
{
    const char ppc_device_node[] = "PowerPC,";
    uae_u32 tbc_freq = 0;
    int found = 0;

    DIR *dir = opendir (path);
    if (dir) {
	struct dirent *dirent;

	do {
	    dirent = readdir (dir);
	    if (dirent) {
		found = !strncmp (dirent->d_name, ppc_device_node, strlen (ppc_device_node));
		if (found)
		    break;
	    }
        }  while (dirent);

	if (found) {
	    /* We've found the PowerPC node in the OF tree, now construct
	     * a path to the timebase frequency node so we can read it */
	    const char  tbc_freq_node[] = "/timebase-frequency";
	    char       *tbc_freq_path;

	    if ((tbc_freq_path = (char *) malloc (strlen (path) + strlen (dirent->d_name) +
						  strlen (tbc_freq_node) + 1))) {
		int tbcfd;

		strcpy (tbc_freq_path, path);
		strcat (tbc_freq_path, dirent->d_name);
		strcat (tbc_freq_path, tbc_freq_node);

		tbcfd = open (tbc_freq_path, O_RDONLY);

		if (tbcfd >= 0)  {
		   read (tbcfd, &tbc_freq, sizeof (uae_u32));
		   close (tbcfd);
		}

		free (tbc_freq_path);
	     }
	}
	closedir (dir);
    }
    return tbc_freq;
}

static frame_time_t machdep_linux_gettimebase (void)
{
    uae_u32 tbc_freq = 0;

    tbc_freq     = find_timebase ("/proc/device-tree/");
    if (!tbc_freq)
	tbc_freq = find_timebase ("/proc/device-tree/cpus/");

    return tbc_freq;
}
#endif

/*
 * Get PPC timebase frequency from Mach kernel.
 */
#ifdef __MACH__
# include <mach/mach_time.h>

static frame_time_t machdep_mach_gettimebase (void)
{
    struct mach_timebase_info tbinfo;

    mach_timebase_info(&tbinfo);

    /* time (in nanosecs) = ticks * tbinfo.numer / tbinfo.numer
     *
     * Thus we get the number of ticks per second as follows:
     */
    return (frame_time_t)(1e9 * tbinfo.denom / tbinfo.numer);
}

#endif /* __MACH__ */


/*
 * Get PPC timebase frequency from AmigaOS4 timer.device on the AmigaONE.
 */
#ifdef __amigaos4__
# include <proto/timer.h>
# include <proto/expansion.h>
# include <expansion/expansion.h>

static frame_time_t machdep_amigaos4_gettimebase (void)
{
    uae_u32 result = 0;
    uint32 machine = MACHINETYPE_UNKNOWN;

    GetMachineInfoTags(GMIT_Machine, &machine, TAG_DONE);

    if (machine == MACHINETYPE_AMIGAONE) {
	struct EClockVal etime;
	result = ReadEClock (&etime);
    }
    return result;
}

#endif /* __amigaos4__ */


#ifdef __MORPHOS__
# include <proto/timer.h>

static frame_time_t machdep_morphos_gettimebase (void)
{
    UQUAD cputime;

    frame_time_t result = ReadCPUClock (&cputime);

    return result;
}
#endif /* __MORPHOS__ */


/*
 * Calibrate PPC timebase frequency the hard way...
 * This is still dumb and horribly inefficient.
 */
static frame_time_t machdep_calibrate_timebase (void)
{
    const int num_loops = 5;
    frame_time_t last_time;
    frame_time_t best_time;
    int i;

    write_log ("Calibrating timebase...\n");
    flush_log ();

    sync ();
    last_time = read_processor_time ();
    for (i = 0; i < num_loops; i++)
	uae_msleep (1000);
    best_time = read_processor_time () - last_time;

    return best_time / num_loops;
}

int machdep_inithrtimer (void)
{
    static int done = 0;

    if (!done) {
#	ifdef __MACH__
            timebase = machdep_mach_gettimebase ();
#	elif __amigaos4__
            timebase = machdep_amigaos4_gettimebase ();
#	elif __linux__
	    timebase = machdep_linux_gettimebase ();
#       elif __MORPHOS__
	    timebase = machdep_morphos_gettimebase ();
#endif

	if (!timebase)
	    timebase = machdep_calibrate_timebase ();

	write_log ("Timebase frequency: %.6f MHz\n", timebase / 1e6);

	done = 1;
    }
    return 1;
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
    cfgfile_write (f, MACHDEP_NAME ".use_tbc=%s\n", p->use_processor_clock ? "yes" : "no");
}

int machdep_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return cfgfile_yesno (option, value, "use_tbc", &p->use_processor_clock);
}

void machdep_default_options (struct uae_prefs *p)
{
    p->use_processor_clock = 1;
}
