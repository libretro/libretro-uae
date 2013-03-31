/*
 * E-UAE - The portable Amiga Emulator
 *
 * Version/revision info.
 *
 * (c) 2006 Richard Drummond
 */

#ifndef EUAE_VERSION_H
#define EUAE_VERSION_H

#include "sysconfig.h"

/* TODO: Version details currently are currently defined
 *       twice: once in the configure script and once here.
 *       Need to fix this.
 */

#define UAEMAJOR   0
#define UAEMINOR   8
#define UAESUBREV 29

#define UAEVERSION (256*65536L*UAEMAJOR + 65536L*UAEMINOR + UAESUBREV)

#ifdef PACKAGE_NAME
# define UAE_NAME PACKAGE_NAME
#else
# define UAE_NAME "E-UAE"
#endif

#define STRINGIZE(x) #x
#define MAKE_VERSION_STRING(x,y,z) STRINGIZE(x) "." STRINGIZE(y) "." STRINGIZE(z)

#define UAE_VERSION_STRING \
	UAE_NAME " " MAKE_VERSION_STRING (UAEMAJOR, UAEMINOR, UAESUBREV)

#endif
