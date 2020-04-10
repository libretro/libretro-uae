#if defined (__x86_64__)
#include "../../sources/src/md-amd64-gcc/m68k.h"
#elif defined (__i386__) && !defined(__PIC__)
#include "../../sources/src/md-i386-gcc/m68k.h"
#elif defined(__powerpc__)
#include "../../sources/src/md-ppc-gcc/m68k.h"
#else
#include "../../sources/src/md-generic/m68k.h"
#endif
