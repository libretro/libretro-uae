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

// This is really no longer needed. - Sven
#if 0

#if __GNUC__ - 1 > 1
# define MALLOC __attribute__((malloc))
#else
# define MALLOC
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef xmalloc
extern void *xmalloc (size_t) MALLOC;
#endif

#ifndef xcalloc
extern void *xcalloc (size_t, size_t) MALLOC;
#endif

#ifndef xfree
extern void  xfree   (const void *p);
#endif

#if defined(__cplusplus)
}
#endif

#endif // 0

#endif /* UAE_MALLOC_H */
