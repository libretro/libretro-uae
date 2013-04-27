 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, *nix version
  *
  * Copyright 1997 Bernd Schmidt
  */

#define TARGET_NAME "retro"

#ifndef OPTIONSFILENAME

#ifdef AND
#define OPTIONSFILENAME "/mnt/sdcard/euae/uae.cfg"
#else
	#ifdef PS3PORT
	#define OPTIONSFILENAME "/dev_hdd0/HOMEBREW/UAE/uae.cfg"
	#else
	#define OPTIONSFILENAME "../cfg/uae.cfg"
	#endif
#endif

#endif

#ifndef MULTI_OPTIONS

#ifdef AND
#define MULTI_OPTIONS "/mnt/sdcard/euae/uae.cfg", "../cfg/uae.cfg",  NULL
#else
	#ifdef PS3PORT
	#define MULTI_OPTIONS "/dev_hdd0/HOMEBREW/UAE/uae.cfg", "../cfg/uae.cfg",  NULL
	#else
	#define MULTI_OPTIONS "../cfg/uae.cfg", "./uae.cfg",  NULL
	#endif

#endif
#endif
/* #define NO_MAIN_IN_MAIN_C */

#define DEFPRTNAME "null"
#define DEFSERNAME "null"


/* #define OPTIONS_IN_HOME*/

#define TARGET_ROM_PATH ""
#define TARGET_FLOPPY_PATH ""
#define TARGET_HARDFILE_PATH ""
#define TARGET_SAVESTATE_PATH ""

#define LOG_MSG(x)  
#define LOG_MSG2(x,y)  


