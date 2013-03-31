/*
 * UAE - The Un*x Amiga Emulator
 *
 * Win32 interface
 *
 * Copyright 1997 Mathias Ortmann
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include <windows.h>
#include <sys/timeb.h>

#include "posixemu.h"
#include "filesys.h"

static DWORD lasterror;


#ifndef HAVE_GETTIMEOFDAY
/* Our Win32 implementation of this function */
void gettimeofday (struct timeval *tv, void *blah)
{
    struct timeb time;
    ftime (&time);

    tv->tv_sec  = time.time;
    tv->tv_usec = time.millitm * 1000;
}
#endif

#ifndef HAVE_TRUNCATE
int truncate (const char *name, long int len)
{
    HANDLE hFile;
    BOOL bResult = FALSE;
    int result = -1;

    if ((hFile = CreateFile (name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
	if (SetFilePointer (hFile, len, NULL, FILE_BEGIN) == (DWORD)len) {
	    if (SetEndOfFile (hFile) == TRUE)
		result = 0;
        } else {
	    write_log ("SetFilePointer() failure for %s to posn %d\n", name, len);
	}
	CloseHandle (hFile);
    } else {
	write_log( "CreateFile() failed to open %s\n", name );
    }

    if (result == -1)
	lasterror = GetLastError ();
    return result;
}
#endif

int isspecialdrive (const char *name)
{
    DWORD v;
    DWORD err;
    
    DWORD last = SetErrorMode (SEM_FAILCRITICALERRORS);

    v = GetFileAttributes (name);
    err = GetLastError ();
    
    SetErrorMode (last);
    
    if (v != INVALID_FILE_ATTRIBUTES)
	return 0;
    if (err == ERROR_NOT_READY)
	return 1;
    if (err)
	return -1;
    return 0;
}
