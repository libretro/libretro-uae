 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Threading support for BeOS
  *
  * Copyright 2004 Richard Drummond
  *
  * Based on Axel Doerfler's BeUAE implementation.
  */
#include <OS.h>

/* Sempahores. We use POSIX semaphores; if you are porting this to a machine
 * with different ones, make them look like POSIX semaphores. */

typedef sem_id uae_sem_t;

STATIC_INLINE int uae_sem_init (uae_sem_t *PSEM, int DUMMY, int INIT)
{
    *PSEM = create_sem (INIT, "uae");
    return *PSEM != 0;
}

#define uae_sem_destroy(PSEM)  delete_sem  (*PSEM)
#define uae_sem_post(PSEM)     release_sem (*PSEM)
#define uae_sem_wait(PSEM)     acquire_sem (*PSEM)
#define uae_sem_trywait(PSEM)  acquire_sem_etc (*PSEM, 1, B_RELATIVE_TIMEOUT, 0)

/* Possibly dodgy? Check whether this fn is even needed */
#define uae_sem_getvalue(PSEM) get_sem_count (*PSEM)

#include "commpipe.h"

typedef thread_id uae_thread_id;

#define BAD_THREAD -1


STATIC_INLINE int uae_start_thread (void *(*f) (void *), void *arg, uae_thread_id *thread)
{
    *thread = spawn_thread ((thread_func)f, "uae thread",
			    B_NORMAL_PRIORITY, arg);
    resume_thread (*thread);
    return *thread <= 0;
}

STATIC_INLINE int uae_wait_thread (uae_thread_id thread)
{
    status_t dummy_return;
    wait_for_thread (thread, &dummy_return);
    return 0;
}

/* Do nothing; thread exits if thread function returns.  */
#define UAE_THREAD_EXIT exit_thread (0)

#define uae_set_thread_priority(pri) /* todo */
