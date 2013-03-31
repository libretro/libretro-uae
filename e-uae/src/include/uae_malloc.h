/*
 * E-UAE - The portable Amiga Emulator
 *
 * Malloc replacement
 *
 * (c) 2003-2007 Richard Drummond
 *
 * Based on code from UAE.
 * (c) 1995-2002 Bernd Schmidt
 */

#ifndef UAE_MALLOC_H
#define UAE_MALLOC_H

#if __GNUC__ - 1 > 1
# define MALLOC __attribute__((malloc))
#else
# define MALLOC
#endif

extern void *xmalloc (size_t) MALLOC;
extern void *xcalloc (size_t, size_t) MALLOC;
extern void  xfree   (void *ptr);

#endif /* UAE_MALLOC_H */
