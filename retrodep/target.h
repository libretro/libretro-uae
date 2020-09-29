 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, *nix version
  *
  * Copyright 1997 Bernd Schmidt
  */

#define TARGET_NAME "libretro"

#if !defined(__CELLOS_LV2__) && !defined(WIIU)

#define TARGET_ROM_PATH         "~/"
#define TARGET_FLOPPY_PATH      "~/"
#define TARGET_HARDFILE_PATH    "~/"
#define TARGET_SAVESTATE_PATH   "~/"

#ifndef OPTIONSFILENAME
# ifdef __APPLE__
#  define OPTIONSFILENAME "default.uaerc"
# else
#  define OPTIONSFILENAME "uaerc"
# endif
#endif

#if 0
#define OPTIONS_IN_HOME
#endif

#define DEFPRTNAME "lpr"
#define DEFSERNAME "/dev/ttyS1"

#elif defined(WIIU)

#define TARGET_ROM_PATH "sd:/"
#define TARGET_FLOPPY_PATH "sd:/"
#define TARGET_HARDFILE_PATH "sd:/"
#define TARGET_SAVESTATE_PATH "sd:/"

#ifndef OPTIONSFILENAME
#define OPTIONSFILENAME "sd:/retroarch/cores/system/uae.cfg"
#endif

#define DEFPRTNAME "null"
#define DEFSERNAME "null"

#else

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
