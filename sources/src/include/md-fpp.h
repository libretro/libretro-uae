 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68881 emulation
  * Support functions for IEEE 754-compatible host CPUs.
  * These functions use a GCC extension (type punning through unions) and
  * should only be compiled with compilers that support this.
  *
  * Copyright 1999 Sam Jordan
  * Copyright 2007 Richard Drummond
  *
  * Note: Type punning through unions is no longer a gcc extension.
  * As long as we are only compiling with strict C99/C11 this method is
  * perfectly legal *and* portable. It was made explicit with C99/TC3,
  * which added the following footnote to section 6.5.2.3:
  *   "If the member used to access the contents of a union object is not
  *    the same as the member last used to store a value in the object,
  *    the appropriate part of the object representation of the value is
  *    reinterpreted as an object representation in the new type as
  *    described in 6.2.6 (a process sometimes called "type punning").
  *    This might be a trap representation."
  * Annex J still lists it as unspecified behavior, which is a known defect
  * and has been corrected with C11, which changed
  *   "The value of a union member other than the last one stored into
  *    [is unspecified]"
  * to
  *   "The values of bytes that correspond to union members other than the
  *    one last stored into [are unspecified]"
  * It's not that big a deal as the annex is only informative, not normative.
  *  - Sven
  */

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
STATIC_INLINE double to_single (uae_u32 value)
{
    union {
	float f;
	uae_u32 u;
    } val;

    val.u = value;
    return val.f;
}

STATIC_INLINE uae_u32 from_single (double src)
{
    union {
	float f;
	uae_u32 u;
    } val;

    val.f = src;
    return val.u;
}

STATIC_INLINE double to_double (uae_u32 wrd1, uae_u32 wrd2)
{
    union {
	double d;
	uae_u32 u[2];
    } val;

#ifdef WORDS_BIGENDIAN
    val.u[0] = wrd1;
    val.u[1] = wrd2;
#else
    val.u[1] = wrd1;
    val.u[0] = wrd2;
#endif
    return val.d;
}

STATIC_INLINE void from_double (double src, uae_u32 * wrd1, uae_u32 * wrd2)
{
    union {
	double d;
	uae_u32 u[2];
    } val;

    val.d = src;
#ifdef WORDS_BIGENDIAN
    *wrd1 = val.u[0];
    *wrd2 = val.u[1];
#else
    *wrd1 = val.u[1];
    *wrd2 = val.u[0];
#endif
}

#define HAVE_from_double
#define HAVE_to_double
#define HAVE_from_single
#define HAVE_to_single
#endif // C99 and newer

/* Get the rest of the conversion functions defined.  */
#include "fpp-unknown.h"
