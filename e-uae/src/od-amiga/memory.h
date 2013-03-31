 /*
  * UAE - The Un*x Amiga Emulator
  *
  * See if this OS has mmap or equivalent
  *
  * Copyright 1996 Bernd Schmidt
  */

#undef USE_MAPPED_MEMORY
#undef CAN_MAP_MEMORY

#define OS_WITHOUT_MEMORY_MANAGEMENT

#ifndef SAVE_MEMORY
#define SAVE_MEMORY
#endif

/* sam: fmode defined in custom.c conflicts with the done defined */
/* in sc:include/stdio.h */
#ifdef __SASC
#define fmode my_fmode
#endif
