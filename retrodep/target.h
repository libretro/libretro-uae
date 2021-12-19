 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, *nix version
  *
  * Copyright 1997 Bernd Schmidt
  */

#define TARGET_NAME "libretro"

#define DEFPRTNAME "null"
#define DEFSERNAME "null"

#undef OPTIONS_IN_HOME
#define OPTIONSFILENAME "default.uae"

#if defined(WIIU)
#undef OPTIONSFILENAME
#define OPTIONSFILENAME "sd:/retroarch/saves/default.uae"
#endif
