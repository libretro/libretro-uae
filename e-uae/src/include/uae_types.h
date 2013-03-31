/*
 * E-UAE - The portable Amiga Emulator
 *
 * Portable types and compiler glue.
 *
 * (c) 2003-2007 Richard Drummond
 *
 * Based on code from UAE.
 * (c) 1995-2002 Bernd Schmidt
 */

#ifndef UAE_TYPES_H
#define UAE_TYPES_H

/* If char has more then 8 bits, good night. */
typedef unsigned char  uae_u8;
typedef signed char    uae_s8;

//typedef struct { uae_u8 RGB[3]; } RGB;

#if SIZEOF_SHORT == 2
typedef unsigned short uae_u16;
typedef short          uae_s16;
#elif SIZEOF_INT == 2
typedef unsigned int   uae_u16;
typedef int            uae_s16;
#else
# error No 2 byte type, you lose.
#endif

#if SIZEOF_INT == 4
typedef unsigned int   uae_u32;
typedef int            uae_s32;
#elif SIZEOF_LONG == 4
typedef unsigned long  uae_u32;
typedef long           uae_s32;
#else
# error No 4 byte type, you lose.
#endif

typedef uae_u32        uaecptr;

#undef uae_s64
#undef uae_u64

#if SIZEOF_LONG_LONG == 8
# define uae_s64   long long
# define uae_u64   unsigned long long
# define VAL64(a)  (a ## LL)
# define UVAL64(a) (a ## uLL)
#elif SIZEOF___INT64 == 8
# define uae_s64   __int64
# define uae_u64   unsigned __int64
# define VAL64(a)  (a)
# define UVAL64(a) (a)
#elif SIZEOF_LONG == 8
# define uae_s64   long;
# define uae_u64   unsigned long;
# define VAL64(a)  (a ## l)
# define UVAL64(a) (a ## ul)
#endif

#if   SIZEOF_VOID_P == 8
typedef long int          uae_intptr;
typedef unsigned long int uae_uintptr;
#elif SIZEOF_VOID_P == 4
typedef int               uae_intptr;
typedef unsigned int      uae_uintptr;
#else
# error Unknown/unsupported pointer size
#endif


/* We can only rely on GNU C getting enums right. Mickeysoft VSC++ is known
 * to have problems, and it's likely that other compilers choke too. */
#ifdef __GNUC__
# define ENUMDECL       typedef enum
# define ENUMNAME(name) name
#else
# define ENUMDECL       enum
# define ENUMNAME(name) ; typedef int name
#endif


/* When using the selective passing of parameters in registers (on x86 and
 * 68k hosts with GCC) REGPARAM will be defined by configure to contain the
 * necessary storage modifier to be used in a function declaration to specify
 * that a function should pass parameters in registers (e.g.,
 * __attribute__((regparam(n))) for GCC).
 *
 * For historic reasons, we have a separate REGPARAM2 macro for the modifier
 * required to specify parameter passing in registers in the function's
 * corresponding definition.
 *
 * However, the distinction between modifiers for declaration and definition is
 * probably no longer necessary. We require both to be present now and with
 * GCC at least the same modifier works for both. REGPARAM2 usage will probably
 * be entirely replaced by REGPARAM eventually (unless somebody has a compiler
 * that requires different modifiers for declaration and definition).
 */
#ifndef REGPARAM
# define REGPARAM
#endif
#ifndef REGPARAM2
# define REGPARAM2 REGPARAM
#endif

#ifndef STATIC_INLINE
# if __GNUC__ - 1 > 1
#  define STATIC_INLINE static __inline__ __attribute__((always_inline))
# else
/* Keep fingers crossed for non-GCC compilers */
#  define STATIC_INLINE static inline
# endif
#endif

#ifndef NOINLINE
# if __GNUC__ - 1 > 1
#  define NOINLINE __attribute__((noinline))
# else
#  define NOINLINE
# endif
#endif


#endif /* UAE_TYPES_H */
