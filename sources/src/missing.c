/*
 * UAE - The Un*x Amiga Emulator
 *
 * Various stuff missing in some OSes.
 *
 * Copyright 1997 Bernd Schmidt
 */

#include "sysconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "uae/string.h"
#include "uae/types.h"

#ifndef HAVE_STRDUP

TCHAR *my_strdup (const TCHAR *s)
{
	/* The casts to char * are there to shut up the compiler on HPUX */
	TCHAR *x = (char*)xmalloc(strlen((TCHAR *)s) + 1);
	strcpy(x, (TCHAR *)s);
	return x;
}

#endif

