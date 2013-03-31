 /*
  * UAE - The Un*x Amiga Emulator
  *
  * OS-specific memory support functions
  *
  * Copyright 2004 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "include/memory.h"

#ifdef JIT

#include <sys/mman.h>

/*
 * Allocate executable memory for JIT cache
 */
void *cache_alloc (int size)
{
   void *cache;

   size = size < getpagesize() ? getpagesize() : size;

   if ((cache = valloc (size)))
	mprotect (cache, size, PROT_READ|PROT_WRITE|PROT_EXEC);

   return cache;
}

void cache_free (void *cache)
{
    free (cache);
}

#ifdef NATMEM_OFFSET
void init_shm (void)
{
    canbang = 1;
}
#endif

#endif
