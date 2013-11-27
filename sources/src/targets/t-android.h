 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, *nix version
  *
  * Copyright 1997 Bernd Schmidt
  */

#define TARGET_NAME "android"

#define TARGET_ROM_PATH         "/mnt/sdcard/euae/"
#define TARGET_FLOPPY_PATH      "/mnt/sdcard/euae/"
#define TARGET_HARDFILE_PATH    "/mnt/sdcard/euae/hdf/"
#define TARGET_SAVESTATE_PATH   "/mnt/sdcard/euae/save/"

#ifndef OPTIONSFILENAME
# ifdef __APPLE__
#  define OPTIONSFILENAME "default.uaerc"
# else
#  define OPTIONSFILENAME "uae.cfg"
# endif
#endif
#define OPTIONS_IN_HOME

#define DEFPRTNAME "lpr"
#define DEFSERNAME "/dev/ttyS1"
