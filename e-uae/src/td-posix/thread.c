 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Threading support, using pthreads
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 2004 Richard Drummond
  *
  * This handles initialization when using named semaphores.
  * Idea stolen from SDL.
  */

#include <pthread.h>
#include <semaphore.h>

#include "sysconfig.h"
#include "sysdeps.h"

#include "threaddep/thread.h"

#ifdef USE_NAMED_SEMAPHORES

int uae_sem_init (uae_sem_t *sem, int pshared, unsigned int value)
{
    char name[32];
    static int semno = 0;
    int result = 0;

    sprintf (name, "/uaesem-%d-%d", getpid (), semno++);

    if ((sem->sem = sem_open (name, O_CREAT, 0600, value)) != (sem_t *)SEM_FAILED)
	sem_unlink (name);
    else {
	sem->sem = 0;
	result = -1;
    }
    return result;
}

#else

/* Satisfy those linkers that complain about empty linker libs */
void thread_dummy (void);

void thread_dummy (void)
{
}

#endif
