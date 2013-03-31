 /*
  * UAE - The Un*x Amiga Emulator
  *
  * OS-specific memory support functions
  *
  * Copyright 2004 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#ifdef JIT

/*
 * Allocate executable memory for JIT cache
 */
void *cache_alloc (int size)
{
   return malloc (size);
}

void cache_free (void *cache)
{
    free (cache);
}

#endif
