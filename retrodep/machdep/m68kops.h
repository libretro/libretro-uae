#if defined (__x86_64__)
#include "../../sources/src/md-generic/m68kops.h"
#elif defined (__i386__) && !defined(__PIC__)
#include "../../sources/src/md-i386-gcc/m68kops.h"
#elif defined(__powerpc__)
#include "../../sources/src/md-ppc-gcc/m68kops.h"
#else
#include <retro_endianness.h>
#if RETRO_IS_BIGENDIAN
#define WORDS_BIGENDIAN
#endif
#include "../../sources/src/md-generic/m68kops.h"
#endif
