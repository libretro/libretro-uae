 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Main program
  *
  * Copyright 1995 Ed Hanway
  * Copyright 1995, 1996, 1997 Bernd Schmidt
  * Copyright 2006-2007 Richard Drummond
  */

#pragma stacksize 320k

#include "sysconfig.h"
#include "sysdeps.h"
#include <assert.h>

#include "options.h"
#include "threaddep/thread.h"
#include "uae.h"
#include "audio.h"
#include "events.h"
#include "mmemory.h"
#include "custom.h"
#include "serial.h"
#include "newcpu.h"
#include "disk.h"
#include "debug.h"
#include "xwin.h"
#include "drawing.h"
#include "inputdevice.h"
#include "keybuf.h"
#include "gui.h"
#include "zfile.h"
#include "autoconf.h"
#include "traps.h"
#include "osemu.h"
#include "filesys.h"
#include "picasso96.h"
#include "bsdsocket.h"
#include "uaeexe.h"
#include "native2amiga.h"
#include "scsidev.h"
#include "akiko.h"
#include "savestate.h"
#include "hrtimer.h"
#include "sleep.h"
#include "version.h"

#ifdef USE_SDL
#include "SDL.h"
#endif

#ifdef WIN32
//FIXME: This shouldn't be necessary
#include "windows.h"
#endif

struct uae_prefs currprefs, changed_prefs;

static int restart_program;
static char restart_config[256];
static char optionsfile[256];

int cloanto_rom = 0;

int log_scsi;

struct gui_info gui_data;


/*
 * Random prefs-related junk that needs to go elsewhere.
 */

void fixup_prefs_dimensions (struct uae_prefs *prefs)
{
    if (prefs->gfx_width_fs < 320)
	prefs->gfx_width_fs = 320;
    if (prefs->gfx_height_fs < 200)
	prefs->gfx_height_fs = 200;
    if (prefs->gfx_height_fs > 1280)
	prefs->gfx_height_fs = 1280;
    prefs->gfx_width_fs += 7;
    prefs->gfx_width_fs &= ~7;
    if (prefs->gfx_width_win < 320)
	prefs->gfx_width_win = 320;
    if (prefs->gfx_height_win < 200)
	prefs->gfx_height_win = 200;
    if (prefs->gfx_height_win > 1280)
	prefs->gfx_height_win = 1280;
    prefs->gfx_width_win += 7;
    prefs->gfx_width_win &= ~7;
}

void fixup_prefs_joysticks (struct uae_prefs *prefs)
{
    int joy_count = inputdevice_get_device_total (IDTYPE_JOYSTICK);

    /* If either port is configured to use a non-existent joystick, try
     * to use a sensible alternative.
     */

    write_log("joystick count=%i \n", joy_count);
    //check port 0 
    // configured as joystick or keyboard simulation
    if (prefs->jport0 >= JSEM_JOYS && prefs->jport0 < JSEM_MICE) {
        //joystick number is greater or equal joy_count
	if (prefs->jport0 - JSEM_JOYS >= joy_count)
	    //configure port 0 as none or mouse (if mouse not configured in port 1)
	    prefs->jport0 = (prefs->jport1 != JSEM_MICE) ? JSEM_MICE : JSEM_NONE;
    }
    //check port 1
    //configured as joystick or keyboard simulation
    if (prefs->jport1 >= JSEM_JOYS && prefs->jport1 < JSEM_MICE) {
	if (prefs->jport1 - JSEM_JOYS >= joy_count)
	    prefs->jport1 = (prefs->jport0 != JSEM_KBDLAYOUT) ? JSEM_KBDLAYOUT : JSEM_NONE;
    }
}

static void fix_options (void)
{
    int err = 0;

    if ((currprefs.chipmem_size & (currprefs.chipmem_size - 1)) != 0
	|| currprefs.chipmem_size < 0x40000
	|| currprefs.chipmem_size > 0x800000)
    {
	currprefs.chipmem_size = 0x200000;
	write_log ("Unsupported chipmem size!\n");
	err = 1;
    }
    if (currprefs.chipmem_size > 0x80000)
	currprefs.chipset_mask |= CSMASK_ECS_AGNUS;

    if ((currprefs.fastmem_size & (currprefs.fastmem_size - 1)) != 0
	|| (currprefs.fastmem_size != 0 && (currprefs.fastmem_size < 0x100000 || currprefs.fastmem_size > 0x800000)))
    {
	currprefs.fastmem_size = 0;
	write_log ("Unsupported fastmem size!\n");
	err = 1;
    }
    if ((currprefs.gfxmem_size & (currprefs.gfxmem_size - 1)) != 0
	|| (currprefs.gfxmem_size != 0 && (currprefs.gfxmem_size < 0x100000 || currprefs.gfxmem_size > 0x2000000)))
    {
	write_log ("Unsupported graphics card memory size %lx!\n", currprefs.gfxmem_size);
	currprefs.gfxmem_size = 0;
	err = 1;
    }
    if ((currprefs.z3fastmem_size & (currprefs.z3fastmem_size - 1)) != 0
	|| (currprefs.z3fastmem_size != 0 && (currprefs.z3fastmem_size < 0x100000 || currprefs.z3fastmem_size > 0x20000000)))
    {
	currprefs.z3fastmem_size = 0;
	write_log ("Unsupported Zorro III fastmem size!\n");
	err = 1;
    }
    if (currprefs.address_space_24 && (currprefs.gfxmem_size != 0 || currprefs.z3fastmem_size != 0)) {
	currprefs.z3fastmem_size = currprefs.gfxmem_size = 0;
	write_log ("Can't use a graphics card or Zorro III fastmem when using a 24 bit\n"
		 "address space - sorry.\n");
    }
    if (currprefs.bogomem_size != 0 && currprefs.bogomem_size != 0x80000 && currprefs.bogomem_size != 0x100000 && currprefs.bogomem_size != 0x1C0000)
    {
	currprefs.bogomem_size = 0;
	write_log ("Unsupported bogomem size!\n");
	err = 1;
    }

    if (currprefs.chipmem_size > 0x200000 && currprefs.fastmem_size != 0) {
	write_log ("You can't use fastmem and more than 2MB chip at the same time!\n");
	currprefs.fastmem_size = 0;
	err = 1;
    }
#if 0
    if (currprefs.m68k_speed < -1 || currprefs.m68k_speed > 20) {
	write_log ("Bad value for -w parameter: must be -1, 0, or within 1..20.\n");
	currprefs.m68k_speed = 4;
	err = 1;
    }
#endif

    if (currprefs.produce_sound < 0 || currprefs.produce_sound > 3) {
	write_log ("Bad value for -S parameter: enable value must be within 0..3\n");
	currprefs.produce_sound = 0;
	err = 1;
    }
#ifdef JIT
    if (currprefs.comptrustbyte < 0 || currprefs.comptrustbyte > 3) {
	write_log ("Bad value for comptrustbyte parameter: value must be within 0..2\n");
	currprefs.comptrustbyte = 1;
	err = 1;
    }
    if (currprefs.comptrustword < 0 || currprefs.comptrustword > 3) {
	write_log ("Bad value for comptrustword parameter: value must be within 0..2\n");
	currprefs.comptrustword = 1;
	err = 1;
    }
    if (currprefs.comptrustlong < 0 || currprefs.comptrustlong > 3) {
	write_log ("Bad value for comptrustlong parameter: value must be within 0..2\n");
	currprefs.comptrustlong = 1;
	err = 1;
    }
    if (currprefs.comptrustnaddr < 0 || currprefs.comptrustnaddr > 3) {
	write_log ("Bad value for comptrustnaddr parameter: value must be within 0..2\n");
	currprefs.comptrustnaddr = 1;
	err = 1;
    }
    if (currprefs.compnf < 0 || currprefs.compnf > 1) {
	write_log ("Bad value for compnf parameter: value must be within 0..1\n");
	currprefs.compnf = 1;
	err = 1;
    }
    if (currprefs.comp_hardflush < 0 || currprefs.comp_hardflush > 1) {
	write_log ("Bad value for comp_hardflush parameter: value must be within 0..1\n");
	currprefs.comp_hardflush = 1;
	err = 1;
    }
    if (currprefs.comp_constjump < 0 || currprefs.comp_constjump > 1) {
	write_log ("Bad value for comp_constjump parameter: value must be within 0..1\n");
	currprefs.comp_constjump = 1;
	err = 1;
    }
    if (currprefs.comp_oldsegv < 0 || currprefs.comp_oldsegv > 1) {
	write_log ("Bad value for comp_oldsegv parameter: value must be within 0..1\n");
	currprefs.comp_oldsegv = 1;
	err = 1;
    }
    if (currprefs.cachesize < 0 || currprefs.cachesize > 16384) {
	write_log ("Bad value for cachesize parameter: value must be within 0..16384\n");
	currprefs.cachesize = 0;
	err = 1;
    }
#endif
    if (currprefs.cpu_level < 2 && currprefs.z3fastmem_size > 0) {
	write_log ("Z3 fast memory can't be used with a 68000/68010 emulation. It\n"
		 "requires a 68020 emulation. Turning off Z3 fast memory.\n");
	currprefs.z3fastmem_size = 0;
	err = 1;
    }
    if (currprefs.gfxmem_size > 0 && (currprefs.cpu_level < 2 || currprefs.address_space_24)) {
	write_log ("Picasso96 can't be used with a 68000/68010 or 68EC020 emulation. It\n"
		 "requires a 68020 emulation. Turning off Picasso96.\n");
	currprefs.gfxmem_size = 0;
	err = 1;
    }
#ifndef BSDSOCKET
    if (currprefs.socket_emu) {
	write_log ("Compile-time option of BSDSOCKET was not enabled.  You can't use bsd-socket emulation.\n");
	currprefs.socket_emu = 0;
	err = 1;
    }
#endif

    if (currprefs.nr_floppies < 0 || currprefs.nr_floppies > 4) {
	write_log ("Invalid number of floppies.  Using 4.\n");
	currprefs.nr_floppies = 4;
	currprefs.dfxtype[0] = 0;
	currprefs.dfxtype[1] = 0;
	currprefs.dfxtype[2] = 0;
	currprefs.dfxtype[3] = 0;
	err = 1;
    }

    if (currprefs.floppy_speed > 0 && currprefs.floppy_speed < 10) {
	currprefs.floppy_speed = 100;
    }
    if (currprefs.input_mouse_speed < 1 || currprefs.input_mouse_speed > 1000) {
	currprefs.input_mouse_speed = 100;
    }

    if (currprefs.collision_level < 0 || currprefs.collision_level > 3) {
	write_log ("Invalid collision support level.  Using 1.\n");
	currprefs.collision_level = 1;
	err = 1;
    }
    fixup_prefs_dimensions (&currprefs);

#ifdef CPU_68000_ONLY
    currprefs.cpu_level = 0;
#endif
#ifndef CPUEMU_0
    currprefs.cpu_compatible = 1;
    currprefs.address_space_24 = 1;
#endif
#if !defined(CPUEMU_5) && !defined (CPUEMU_6)
    currprefs.cpu_compatible = 0;
    currprefs.address_space_24 = 0;
#endif
#if !defined (CPUEMU_6)
    currprefs.cpu_cycle_exact = currprefs.blitter_cycle_exact = 0;
#endif
#ifndef AGA
    currprefs.chipset_mask &= ~CSMASK_AGA;
#endif
#ifndef AUTOCONFIG
    currprefs.z3fastmem_size = 0;
    currprefs.fastmem_size = 0;
    currprefs.gfxmem_size = 0;
#endif
#if !defined (BSDSOCKET)
    currprefs.socket_emu = 0;
#endif
#if !defined (SCSIEMU)
    currprefs.scsi = 0;
#ifdef _WIN32
    currprefs.win32_aspi = 0;
#endif
#endif

    fixup_prefs_joysticks (&currprefs);

    if (err)
	write_log ("Please use \"uae -h\" to get usage information.\n");
}


#ifndef DONT_PARSE_CMDLINE

void usage (void)
{
    cfgfile_show_usage ();
}

static void show_version (void)
{
#ifdef PACKAGE_VERSION
    write_log (PACKAGE_NAME " " PACKAGE_VERSION "\n");
#else
    write_log ("UAE %d.%d.%d\n", UAEMAJOR, UAEMINOR, UAESUBREV);
#endif
    write_log ("Build date: " __DATE__ " " __TIME__ "\n");
}

static void show_version_full (void)
{
    write_log ("\n");
    show_version ();
    write_log ("\nCopyright 2003-2007 Richard Drummond and contributors.\n");
    write_log ("Based on source code from:\n");
    write_log ("UAE    - copyright 1995-2002 Bernd Schmidt;\n");
    write_log ("WinUAE - copyright 1999-2007 Toni Wilen.\n");
    write_log ("See the source code for a full list of contributors.\n");

    write_log ("This is free software; see the file COPYING for copying conditions.  There is NO\n");
    write_log ("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

static void parse_cmdline (int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) {
	if (strcmp (argv[i], "-cfgparam") == 0) {
	    if (i + 1 < argc)
		i++;
	} else if (strncmp (argv[i], "-config=", 8) == 0) {
#ifdef FILESYS
	    free_mountinfo (currprefs.mountinfo);
#endif
	    if (cfgfile_load (&currprefs, argv[i] + 8, 0))
		strcpy (optionsfile, argv[i] + 8);
	}
	/* Check for new-style "-f xxx" argument, where xxx is config-file */
	else if (strcmp (argv[i], "-f") == 0) {
	    if (i + 1 == argc) {
		write_log ("Missing argument for '-f' option.\n");
	    } else {
#ifdef FILESYS
		free_mountinfo (currprefs.mountinfo);
#endif
		if (cfgfile_load (&currprefs, argv[++i], 0))
		    strcpy (optionsfile, argv[i]);
	    }
	} else if (strcmp (argv[i], "-s") == 0) {
	    if (i + 1 == argc)
		write_log ("Missing argument for '-s' option.\n");
	    else
		cfgfile_parse_line (&currprefs, argv[++i], 0);
	} else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "-help") == 0) {
	    usage ();
	    exit (0);
	} else if (strcmp (argv[i], "-version") == 0) {
	    show_version_full ();
	    exit (0);
	} else if (strcmp (argv[i], "-scsilog") == 0) {
	    log_scsi = 1;
	} else {
	    if (argv[i][0] == '-' && argv[i][1] != '\0') {
		const char *arg = argv[i] + 2;
		int extra_arg = *arg == '\0';
		if (extra_arg)
		    arg = i + 1 < argc ? argv[i + 1] : 0;
		if (parse_cmdline_option (&currprefs, argv[i][1], (char*)arg) && extra_arg)
		    i++;
	    }
	}
    }
}
#endif

static void parse_cmdline_and_init_file (int argc, char **argv)
{
    char *home;
#ifdef _WIN32
    extern char *start_path;
#endif

#ifdef MULTI_OPTIONS
   char* cfg_file[] = { MULTI_OPTIONS };
   int ret;
   int i;
#endif


    strcpy (optionsfile, "");

#ifdef OPTIONS_IN_HOME
    home = getenv ("HOME");
    if (home != NULL && strlen (home) < 240)
    {
	strcpy (optionsfile, home);
	strcat (optionsfile, "/");
    }
#endif

#ifdef _WIN32
    sprintf( optionsfile, "%s\\Configurations\\", start_path );
#endif

#ifdef MULTI_OPTIONS
    {
	ret = 0;
	i=0;
	//try to load one of the config file specified in the list
	//if found then continue otherwise try the next path/config file
	while (!ret && cfg_file[i] != NULL) {
		write_log("trying to load %s \n", cfg_file[i]);
		ret = cfgfile_load (&currprefs, cfg_file[i], 0);
		if (ret) {
		    strcat (optionsfile, cfg_file[i]);
		}
		i++;
	}
	if (!ret) {
		write_log ("failed to load config file.\n");
		exit(1);
	}
	write_log("config file found! \n");
    }
#else  /* MULTI_OPTIONS */
    strcat (optionsfile, OPTIONSFILENAME);
    if (! cfgfile_load (&currprefs, optionsfile, 0)) {
	write_log ("failed to load config '%s'\n", optionsfile);
#ifdef OPTIONS_IN_HOME
	/* sam: if not found in $HOME then look in current directory */
        char *saved_path = strdup (optionsfile);
	strcpy (optionsfile, OPTIONSFILENAME);
	if (! cfgfile_load (&currprefs, optionsfile, 0) ) {
	    /* If not in current dir either, change path back to home
	     * directory - so that a GUI can save a new config file there */
	    strcpy (optionsfile, saved_path);
	}

        free (saved_path);
#endif   /* OPTIONS_IN_HOME */
    }
#endif /* MULTI_OPTIONS */

    fix_options ();

    parse_cmdline (argc, argv);

    fix_options ();
}

const char* get_current_config_name() {
	if (restart_config[0] == 0) {
		return optionsfile;
	} else {
		return restart_config;
	}
}

/*
 * Save the currently loaded configuration.
 */
void uae_save_config (void)
{
    FILE *f;
    char tmp[257];

    /* Back up the old file.  */
    strcpy (tmp, optionsfile);
    strcat (tmp, "~");
    write_log ("Backing-up config file '%s' to '%s'\n", optionsfile, tmp);
    rename (optionsfile, tmp);

    write_log ("Writing new config file '%s'\n", optionsfile);
    f = fopen (optionsfile, "w");
    if (f == NULL) {
	gui_message ("Error saving configuration file.!\n"); // FIXME - better error msg.
	return;
    }

    // FIXME  - either fix this nonsense, or only allow config to be saved when emulator is stopped.
    if (uae_get_state () == UAE_STATE_STOPPED)
	save_options (f, &changed_prefs, 0);
    else
	save_options (f, &currprefs, 0);

    fclose (f);
}


/*
 * A first cut at better state management...
 */

static int uae_state;
static int uae_target_state;

int uae_get_state (void)
{
    return uae_state;
}

static void set_state (int state)
{
    uae_state = state;
    gui_notify_state (state);
    graphics_notify_state (state);
}

int uae_state_change_pending (void)
{
    return uae_state != uae_target_state;
}

void uae_start (void)
{
    uae_target_state = UAE_STATE_COLD_START;
}

void uae_pause (void)
{
    if (uae_target_state == UAE_STATE_RUNNING)
	uae_target_state = UAE_STATE_PAUSED;
}

void uae_resume (void)
{
   write_log("uae_resume: ts=%i\n", uae_target_state);
    if (uae_target_state == UAE_STATE_PAUSED)
	uae_target_state = UAE_STATE_RUNNING;
}

void uae_quit (void)
{
    if (uae_target_state != UAE_STATE_QUITTING) {
	uae_target_state = UAE_STATE_QUITTING;
    }
}

void uae_stop (void)
{
    if (uae_target_state != UAE_STATE_QUITTING && uae_target_state != UAE_STATE_STOPPED) {
	uae_target_state = UAE_STATE_STOPPED;
	restart_config[0] = 0;
    }
}

void uae_reset (int hard_reset)
{
    switch (uae_target_state) {
	case UAE_STATE_QUITTING:
	case UAE_STATE_STOPPED:
	case UAE_STATE_COLD_START:
	case UAE_STATE_WARM_START:
	    /* Do nothing */
	    break;
	default:
	    uae_target_state = hard_reset ? UAE_STATE_COLD_START : UAE_STATE_WARM_START;
    }
}


/* This needs to be rethought */
void uae_restart (int opengui, char *cfgfile)
{
    uae_stop ();
    restart_program = opengui > 0 ? 1 : (opengui == 0 ? 2 : 3);
    restart_config[0] = 0;
    if (cfgfile)
	strcpy (restart_config, cfgfile);
}


/*
 * Early initialization of emulator, parsing of command-line options,
 * and loading of config files, etc.
 *
 * TODO: Need better cohesion! Break this sucker up!
 */
static int do_preinit_machine (int argc, char **argv)
{
    if (! graphics_setup ()) {
	exit (1);
    }
    if (restart_config[0]) {
#ifdef FILESYS
	free_mountinfo (currprefs.mountinfo);
#endif
	default_prefs (&currprefs, 0);
	fix_options ();
    }

#ifdef NATMEM_OFFSET
    init_shm ();
#endif

#ifdef FILESYS
    rtarea_init ();
    hardfile_install ();
#endif

    if (restart_config[0])
        parse_cmdline_and_init_file (argc, argv);
    else
	currprefs = changed_prefs;

    uae_inithrtimer ();

    machdep_init ();

    if (! audio_setup ()) {
	write_log ("Sound driver unavailable: Sound output disabled\n");
	currprefs.produce_sound = 0;
    }
    inputdevice_init ();

    return 1;
}

/*
 * Initialization of emulator proper
 */
static int do_init_machine (void)
{
#ifdef JIT
    if (!(( currprefs.cpu_level >= 2 ) && ( currprefs.address_space_24 == 0 ) && ( currprefs.cachesize )))
	canbang = 0;
#endif

#ifdef _WIN32
    logging_init(); /* Yes, we call this twice - the first case handles when the user has loaded
		       a config using the cmd-line.  This case handles loads through the GUI. */
#endif

#ifdef SAVESTATE
    savestate_init ();
#endif
#ifdef SCSIEMU
    scsidev_install ();
#endif
#ifdef AUTOCONFIG
    /* Install resident module to get 8MB chipmem, if requested */
    rtarea_setup ();
#endif

    keybuf_init (); /* Must come after init_joystick */

#ifdef AUTOCONFIG
    expansion_init ();
#endif
    memory_init ();
    memory_reset ();

#ifdef FILESYS
    filesys_install ();
#endif
#ifdef AUTOCONFIG
    bsdlib_install ();
    emulib_install ();
    uaeexe_install ();
    native2amiga_install ();
#endif

    if (custom_init ()) { /* Must come after memory_init */
#ifdef SERIAL_PORT
	serial_init ();
#endif
	DISK_init ();

	reset_frame_rate_hack ();
	init_m68k(); /* must come after reset_frame_rate_hack (); */

	gui_update ();

	if (graphics_init ()) {

#ifdef DEBUGGER
	    setup_brkhandler ();

	    if (currprefs.start_debugger && debuggable ())
		activate_debugger ();
#endif

#ifdef WIN32
#ifdef FILESYS
	    filesys_init (); /* New function, to do 'add_filesys_unit()' calls at start-up */
#endif
#endif
	    if (sound_available && currprefs.produce_sound > 1 && ! audio_init ()) {
		write_log ("Sound driver unavailable: Sound output disabled\n");
		currprefs.produce_sound = 0;
	    }

	    return 1;
	}
    }
    return 0;
}

/*
 * Helper for reset method
 */
static void reset_all_systems (void)
{
    init_eventtab ();

    memory_reset ();
#ifdef BSDSOCKET
    bsdlib_reset ();
#endif
#ifdef FILESYS
    filesys_reset ();
    filesys_start_threads ();
    hardfile_reset ();
#endif
#ifdef SCSIEMU
    scsidev_reset ();
    scsidev_start_threads ();
#endif
}

/*
 * Reset emulator
 */
static void do_reset_machine (int hardreset)
{
#ifdef SAVESTATE
    if (savestate_state == STATE_RESTORE)
	restore_state (savestate_fname);
    else if (savestate_state == STATE_REWIND)
	savestate_rewind ();
#endif
    /* following three lines must not be reordered or
     * fastram state restore breaks
     */
    reset_all_systems ();
    customreset ();
    m68k_reset ();
    if (hardreset) {
	memset (chipmemory, 0, allocated_chipmem);
	write_log ("chipmem cleared\n");
    }
#ifdef SAVESTATE
    /* We may have been restoring state, but we're done now.  */
    if (savestate_state == STATE_RESTORE || savestate_state == STATE_REWIND)
    {
	map_overlay (1);
	fill_prefetch_slow (&regs); /* compatibility with old state saves */
    }
    savestate_restore_finish ();
#endif

    fill_prefetch_slow (&regs);
    if (currprefs.produce_sound == 0)
	eventtab[ev_audio].active = 0;
    handle_active_events ();

    inputdevice_updateconfig (&currprefs);
}

//RETRO HACK
void retro_uae_reset (int hard_reset)
{
	do_reset_machine (hard_reset);
}

/*
 * Run emulator
 */
static void do_run_machine (void)
{
#if defined (NATMEM_OFFSET) && defined( _WIN32 ) && !defined( NO_WIN32_EXCEPTION_HANDLER )
    extern int EvalException ( LPEXCEPTION_POINTERS blah, int n_except );
    __try
#endif
    {
//RETRO HACK
	//m68k_go (1);
	testloop();

    }
#if defined (NATMEM_OFFSET) && defined( _WIN32 ) && !defined( NO_WIN32_EXCEPTION_HANDLER )
    __except( EvalException( GetExceptionInformation(), GetExceptionCode() ) )
    {
	// EvalException does the good stuff...
    }
#endif
}

/*
 * Exit emulator
 */
static void do_exit_machine (void)
{
    graphics_leave ();
    inputdevice_close ();

#ifdef SCSIEMU
    scsidev_exit ();
#endif
    DISK_free ();
    audio_close ();
    dump_counts ();
#ifdef SERIAL_PORT
    serial_exit ();
#endif
#ifdef CD32
    akiko_free ();
#endif
    gui_exit ();

#ifdef AUTOCONFIG
    expansion_cleanup ();
#endif
#ifdef FILESYS
    filesys_cleanup ();
#endif
#ifdef SAVESTATE
    savestate_free ();
#endif
    memory_cleanup ();
    cfgfile_addcfgparam (0);
}


/*
 * Here's where all the action takes place!
 */
void real_main (int argc, char **argv)
{
    show_version ();

    currprefs.mountinfo = changed_prefs.mountinfo = &options_mountinfo;
    restart_program = 1;
#ifdef _WIN32
    sprintf (restart_config, "%sConfigurations\\", start_path);
#endif
    strcat (restart_config, OPTIONSFILENAME);

    /* Initial state is stopped */
    uae_target_state = UAE_STATE_STOPPED;

    while (uae_target_state != UAE_STATE_QUITTING) {
        int want_gui;

        set_state (uae_target_state);

        do_preinit_machine (argc, argv);

        /* Should we open the GUI? TODO: This mess needs to go away */
        want_gui = currprefs.start_gui;
        if (restart_program == 2)
        	want_gui = 0;
        else if (restart_program == 3)
        	want_gui = 1;

        changed_prefs = currprefs;


        if (want_gui) {
        	/* Handle GUI at start-up */
        	int err = gui_open ();

        	if (err >= 0) {
#ifdef __PS3__
//just a precaution - don't want to get stuck in this loop forewer
//The ps3 gui handles all events in its own loop, so this one is useless.
//Also the leave condition is ambiguous from the porting perspective....
//I'd expect something like: 
//   do{
//	    int gui_event = gui_handle_events ();
//      uae_msleep(10);
//   } while (gui_event != GUI_CLOSED)	
#else
        		do {
        			gui_handle_events ();

        			uae_msleep (10);
        		} while (!uae_state_change_pending ());
#endif
        	} else if (err == - 1) {
        		if (restart_program == 3) {
        			restart_program = 0;
        			uae_quit ();
        		}
        	} else {
        		uae_quit ();
        	}

        	currprefs = changed_prefs;
        	fix_options ();
        	inputdevice_init ();
        } //end of want_gui

        restart_program = 0;
        write_log("main: 1: uae_target_state=%i \n", uae_target_state );
        if (uae_target_state == UAE_STATE_QUITTING) {
        	write_log ("uae: exiting the gui/machine loop\n");
        	break;
        }

        uae_target_state = UAE_STATE_COLD_START;

        /* Start emulator proper. */
        if (!do_init_machine ())
        	break;

        while (uae_target_state != UAE_STATE_QUITTING && uae_target_state != UAE_STATE_STOPPED) {
        	/* Reset */
        	set_state (uae_target_state);
        	do_reset_machine (uae_state == UAE_STATE_COLD_START);

        	/* Running */
        	uae_target_state = UAE_STATE_RUNNING;

        	/*
        	 * Main Loop
        	 */
        	do {
        		set_state (uae_target_state);

//RETRO HACK
        		/* Run emulator. */
        		do_run_machine ();
return 0;

        		if (uae_target_state == UAE_STATE_PAUSED) {
        			/* Paused */
        			set_state (uae_target_state);

        			audio_pause ();

        			/* While UAE is paused we have to handle
        			 * input events, etc. ourselves.
        			 */
        			do {
        				gui_handle_events ();
        				handle_events ();

        				/* Manually pump input device */
        				inputdevicefunc_keyboard.read ();
        				inputdevicefunc_mouse.read ();
        				inputdevicefunc_joystick.read ();
        				inputdevice_handle_inputcode ();

        				/* Don't busy wait. */
        				uae_msleep (10);

        			} while (!uae_state_change_pending ());

        			audio_resume ();
        		}

        	} while (uae_target_state == UAE_STATE_RUNNING);

        	/*
        	 * End of Main Loop
        	 *
        	 * We're no longer running or paused.
        	 */

        	set_inhibit_frame (IHF_QUIT_PROGRAM);

#ifdef FILESYS
        	/* Ensure any cached changes to virtual filesystem are flushed before
        	 * resetting or exitting. */
        	filesys_prepare_reset ();
#endif

        } /* while (!QUITTING && !STOPPED) */

        do_exit_machine ();

        /* TODO: This stuff is a hack. What we need to do is
         * check whether a config GUI is available. If not,
         * then quit.
         */
        restart_program = 3;
    }
	write_log ("uae: zfile_exit\n");
    zfile_exit ();
	write_log ("uae: real_main quit\n");

}

#ifdef USE_SDL
int init_sdl (void)
{
    int result = (SDL_Init (SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE /*| SDL_INIT_AUDIO*/) == 0);
    if (result)
        atexit (SDL_Quit);

    return result;
}
#else
#define init_sdl()
#endif

#ifndef NO_MAIN_IN_MAIN_C
int umain (int argc, char **argv)
{
    init_sdl ();
    gui_init (argc, argv);
    real_main (argc, argv);
    return 0;
}
#endif

#ifdef SINGLEFILE
uae_u8 singlefile_config[50000] = { "_CONFIG_STARTS_HERE" };
uae_u8 singlefile_data[1500000] = { "_DATA_STARTS_HERE" };
#endif
