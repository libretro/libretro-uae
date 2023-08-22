/*
 * E-UAE - The portable Amiga Emulator
 *
 * Handle the vagaries of C library string functions.
 *
 * (c) 2003-2007 Richard Drummond
 *
 * Based on code from UAE.
 * (c) 1995-2002 Bernd Schmidt
 */

#ifndef UAE_STRING_H
#define UAE_STRING_H

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifndef HAVE_STRCASECMP
# ifdef HAVE_STRCMPI
#  define strcasecmp strcmpi
# else
#  ifdef HAVE_STRICMP
#   define strcasecmp stricmp
#  endif
# endif
#endif

#ifdef __LIBRETRO__
#define _tcscspn strcspn
#else
#define _tcscspn(wcs, reject) wcscspn((const wchar_t*)(wcs), (const wchar_t*)(reject))
#endif

#endif /* UAE_STRING_H */
