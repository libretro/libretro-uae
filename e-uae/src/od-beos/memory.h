 /*
  * UAE - The Un*x Amiga Emulator
  *
  * See if this OS has mmap or equivalent
  *
  * Copyright 1996 Bernd Schmidt
  */

#undef USE_MAPPED_MEMORY
#undef CAN_MAP_MEMORY

/* Don't need to set execute permission on the
 * compiler cache for BeOS. */
#define COMPILER_CACHE_NEEDS_NO_MPROTECT
