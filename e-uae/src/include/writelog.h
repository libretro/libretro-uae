/*
 * E-UAE - The portable Amiga Emulator
 *
 * Logging functions
 *
 * (c) 2003-2007 Richard Drummond
 *
 * Based on code from UAE.
 * (c) 1995-2002 Bernd Schmidt
 */

#ifndef WRITELOG_H
#define WRITELOG_H

#if __GNUC__ - 1 > 1 || __GNUC_MINOR__ - 1 > 6
# define PRINTF_FORMAT __attribute__ ((format (printf, 1, 2)));
#else
# define PRINTF_FORMAT
#endif

extern void write_log   (const char *, ...) PRINTF_FORMAT;
extern void flush_log   (void);
extern void set_logfile (const char *logfile_name);

#endif /* WRITELOG_H */
