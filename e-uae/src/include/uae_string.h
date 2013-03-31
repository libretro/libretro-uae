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

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_STRDUP
# define my_strdup strdup
#else
extern char *my_strdup (const char *s);
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

#endif /* UAE_STRING_H */
