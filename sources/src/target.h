 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, *nix version
  *
  * Copyright 1997 Bernd Schmidt
  */

#ifndef __CELLOS_LV2__

#define TARGET_NAME "unix"

#define TARGET_ROM_PATH         "~/"
#define TARGET_FLOPPY_PATH      "~/"
#define TARGET_HARDFILE_PATH    "~/"
#define TARGET_SAVESTATE_PATH   "~/"

#ifndef OPTIONSFILENAME
# ifdef __APPLE__
#  define OPTIONSFILENAME "default.uaerc"
# else
#  define OPTIONSFILENAME ".uaerc"
# endif
#endif
#define OPTIONS_IN_HOME

#define DEFPRTNAME "lpr"
#define DEFSERNAME "/dev/ttyS1"

#else

#define TARGET_NAME "RETRO"

#define TARGET_ROM_PATH ""
#define TARGET_FLOPPY_PATH ""
#define TARGET_HARDFILE_PATH ""
#define TARGET_SAVESTATE_PATH ""

#ifndef OPTIONSFILENAME
#define OPTIONSFILENAME "/dev_hdd0/HOMEBREW/UAE/uae.cfg"
#endif

#define DEFPRTNAME "null"
#define DEFSERNAME "null"

#endif

