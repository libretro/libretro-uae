 /*
  * PUAE - The portable Amiga emulator
  *
  * Standard write_log that writes to the console
  *
  * Copyright 2001 Bernd Schmidt
  * Copyright 2006 Richard Drummond
  */
#include "sysconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

#include "uae/string.h"
#include "uae/types.h"

#ifdef __LIBRETRO__
#include "libretro.h"
extern retro_log_printf_t log_cb;
void write_log (const char *fmt, ...)
{
    char text[512];
    va_list ap;

    if (fmt == NULL)
       return;

    va_start (ap, fmt);
    vsprintf (text, fmt, ap);
    va_end (ap);

    if (text[strlen(text)-1] == '\n')
       text[strlen(text)-1] = '\0';

    log_cb(RETRO_LOG_INFO, "%s\n", text);
}
#else
void write_log (const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
#ifdef HAVE_VFPRINTF
    vfprintf (stderr, fmt, ap);
#else
    /* Technique stolen from GCC.  */
    {
	int x1, x2, x3, x4, x5, x6, x7, x8;
	x1 = va_arg (ap, int);
	x2 = va_arg (ap, int);
	x3 = va_arg (ap, int);
	x4 = va_arg (ap, int);
	x5 = va_arg (ap, int);
	x6 = va_arg (ap, int);
	x7 = va_arg (ap, int);
	x8 = va_arg (ap, int);
	fprintf (stderr, fmt, x1, x2, x3, x4, x5, x6, x7, x8);
    }
#endif
}
#endif

#ifdef JIT
void jit_abort (const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
#ifdef HAVE_VFPRINTF
    vfprintf (stderr, fmt, ap);
#else
    /* Technique stolen from GCC.  */
    {
	int x1, x2, x3, x4, x5, x6, x7, x8;
	x1 = va_arg (ap, int);
	x2 = va_arg (ap, int);
	x3 = va_arg (ap, int);
	x4 = va_arg (ap, int);
	x5 = va_arg (ap, int);
	x6 = va_arg (ap, int);
	x7 = va_arg (ap, int);
	x8 = va_arg (ap, int);
	fprintf (stderr, fmt, x1, x2, x3, x4, x5, x6, x7, x8);
    }
#endif
	uae_reset(1, 0);
}
#endif

void flush_log (void)
{
	fflush (stderr);
}

static char *console_buffer;
static int console_buffer_size;

char *setconsolemode (char *buffer, int maxlen)
{
	char *ret = NULL;
	if (buffer) {
		console_buffer = buffer;
		console_buffer_size = maxlen;
	} else {
		ret = console_buffer;
		console_buffer = NULL;
	}
	return ret;
}
