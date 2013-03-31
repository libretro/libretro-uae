/*
 * E-UAE - The portable Amiga Emulator
 *
 * Endian swapping
 *
 * (c) 2003-2007 Richard Drummond
 *
 * Based on code from UAE.
 * (c) 1995-2002 Bernd Schmidt
 */

#ifndef UAE_ENDIAN_H
#define UAE_ENDIAN_H

/* Try to use system bswap_16/bswap_32 functions. */
#if defined HAVE_BSWAP_16 && defined HAVE_BSWAP_32
# ifdef HAVE_BYTESWAP_H
#  include <byteswap.h>
# endif
#else
/* Else, if using SDL, try SDL's endian functions. */
# ifdef USE_SDL
#  include <SDL_endian.h>
#  define bswap_16(x) SDL_Swap16(x)
#  define bswap_32(x) SDL_Swap32(x)
# else
/* Otherwise, we'll roll our own. */
#  define bswap_16(x) (((x) >> 8) | (((x) & 0xFF) << 8))
#  define bswap_32(x) (((x) << 24) | (((x) << 8) & 0x00FF0000) | (((x) >> 8) & 0x0000FF00) | ((x) >> 24))
# endif
#endif

#endif /* UAE_ENDIAN_H */
