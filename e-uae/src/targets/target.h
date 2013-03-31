 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, *nix version
  *
  * Copyright 1997 Bernd Schmidt
  */

#define TARGET_NAME "retro"

#ifndef OPTIONSFILENAME
#define OPTIONSFILENAME "/dev_usb000/uae/uae.cfg"
#endif

#ifndef MULTI_OPTIONS
#define MULTI_OPTIONS "/dev_usb000/uae/uae.cfg", "/dev_hdd0/game/EUAE00825/USRDIR/UAE.CFG",  NULL
#endif

/* #define NO_MAIN_IN_MAIN_C */

#define DEFPRTNAME "null"
#define DEFSERNAME "null"


/* #define OPTIONS_IN_HOME*/

#define TARGET_ROM_PATH ""
#define TARGET_FLOPPY_PATH ""
#define TARGET_HARDFILE_PATH ""
#define TARGET_SAVESTATE_PATH ""




