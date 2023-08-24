#if defined (__x86_64__)
#include "../../sources/src/md-generic/m68kops.h"
#elif defined (__i386__)
#include "../../sources/src/md-i386-gcc/m68kops.h"
#elif defined(__powerpc__) && !defined(WIIU) && !defined(__PS3__)
#include "../../sources/src/md-ppc-gcc/m68kops.h"
#else
#include <retro_endianness.h>
#if RETRO_IS_BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif
#include "../../sources/src/md-generic/m68kops.h"
#endif
