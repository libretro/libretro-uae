 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Win32 thread support
  *
  * Copyright 1997 Mathias Ortmann
  * Copyright 2005 Richard Drummond
  *
  */

#include "thread.h"

void uae_sem_init (HANDLE *event, int manual_reset, int initial_state)
{
    if (*event) {
	if (initial_state)
	    SetEvent (*event);
	else
	    ResetEvent( *event );
    } else
	*event = CreateEvent (NULL, manual_reset, initial_state, NULL);
}

void uae_sem_wait (HANDLE *event)
{
    WaitForSingleObject (*event, INFINITE);
}

void uae_sem_post (HANDLE * event)
{
    SetEvent (*event);
}

int uae_sem_trywait (HANDLE * event)
{
    return WaitForSingleObject (*event, 0) == WAIT_OBJECT_0 ? 0 : -1;
}

void uae_sem_close (HANDLE * event)
{
    if (*event) {
	CloseHandle (*event);
	*event = NULL;
    }
}


typedef unsigned (__stdcall *BEGINTHREADEX_FUNCPTR)(void *);

int uae_start_thread (void *(*f)(void *), void *arg, DWORD *foo)
{
    HANDLE hThread;
    int result = 1;

    hThread = (HANDLE)_beginthreadex (NULL, 0, (BEGINTHREADEX_FUNCPTR)f, arg, 0, foo);
    if (hThread)
	SetThreadPriority (hThread, THREAD_PRIORITY_HIGHEST);
    else
	result = 0;
    return result;
}

void uae_set_thread_priority (int pri)
{
}
