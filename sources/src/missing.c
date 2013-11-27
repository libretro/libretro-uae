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
#include "uae_string.h"
#include "uae_types.h"
#include "writelog.h"

#ifndef HAVE_STRDUP

TCHAR *my_strdup (const TCHAR *s)
{
	/* The casts to char * are there to shut up the compiler on HPUX */
	TCHAR *x = (char*)xmalloc(strlen((TCHAR *)s) + 1);
	strcpy(x, (TCHAR *)s);
	return x;
}

#endif

#if 0
void *xmalloc (size_t n)
{
	void *a = malloc (n);
	if (a == NULL) {
		write_log ("xmalloc(%lu): virtual memory exhausted\n", (unsigned long)n);
		abort ();
	}
	return a;
}

void *xcalloc (size_t n, size_t size)
{
	void *a = calloc (n, size);
	if (a == NULL) {
		write_log ("xcalloc(%lu): virtual memory exhausted\n", (unsigned long)(n * size));
		abort ();
	}
	return a;
}

void xfree (const void *p)
{
	if (p)
		free ((void*)p);
}

#endif // 0
