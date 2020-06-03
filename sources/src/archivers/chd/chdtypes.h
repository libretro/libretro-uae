#ifndef UAE_CHD
#define UAE_CHD

#ifndef USE_ZFILE
#include "sysconfig.h"
#include "sysdeps.h"
#include "zfile.h"
#endif

#define NO_MEM_TRACKING

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#define DECL_NORETURN			__declspec(noreturn)
#else
#define DECL_NORETURN
#endif

#ifndef INLINE
#define INLINE __inline
#endif

#ifndef MIN
#define MIN(x,y)			((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)			((x) > (y) ? (x) : (y))
#endif

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x)		(sizeof(x) / sizeof(x[0]))
#endif

#ifndef CLIB_DECL
#define CLIB_DECL __cdecl
#endif

#ifndef FLAC_API_EXPORTS
#define FLAC_API_EXPORTS
#endif

/* Macros for normalizing data into big or little endian formats */
#define FLIPENDIAN_INT16(x)	(((((UINT16) (x)) >> 8) | ((x) << 8)) & 0xffff)
#define FLIPENDIAN_INT32(x)	((((UINT32) (x)) << 24) | (((UINT32) (x)) >> 24) | \
	(( ((UINT32) (x)) & 0x0000ff00) << 8) | (( ((UINT32) (x)) & 0x00ff0000) >> 8))
#define FLIPENDIAN_INT64(x)	\
	(												\
		(((((UINT64) (x)) >> 56) & ((UINT64) 0xFF)) <<  0)	|	\
		(((((UINT64) (x)) >> 48) & ((UINT64) 0xFF)) <<  8)	|	\
		(((((UINT64) (x)) >> 40) & ((UINT64) 0xFF)) << 16)	|	\
		(((((UINT64) (x)) >> 32) & ((UINT64) 0xFF)) << 24)	|	\
		(((((UINT64) (x)) >> 24) & ((UINT64) 0xFF)) << 32)	|	\
		(((((UINT64) (x)) >> 16) & ((UINT64) 0xFF)) << 40)	|	\
		(((((UINT64) (x)) >>  8) & ((UINT64) 0xFF)) << 48)	|	\
		(((((UINT64) (x)) >>  0) & ((UINT64) 0xFF)) << 56)		\
	)
#endif
