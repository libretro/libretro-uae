 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Threading support, using SDL
  *
  * Copyright 1997, 2001 Bernd Schmidt
  */

#include <SDL.h>
#include <SDL_thread.h>

/* Sempahores. We use POSIX semaphores; if you are porting this to a machine
 * with different ones, make them look like POSIX semaphores. */
typedef SDL_sem *uae_sem_t;

STATIC_INLINE int uae_sem_init(uae_sem_t *PSEM, int DUMMY, int INIT)
{
   *PSEM = SDL_CreateSemaphore (INIT);

   return (*PSEM == 0);
}
#define uae_sem_destroy(PSEM)  SDL_DestroySemaphore (*PSEM)
#define uae_sem_post(PSEM)     SDL_SemPost (*PSEM)
#define uae_sem_wait(PSEM)     SDL_SemWait (*PSEM)
#define uae_sem_trywait(PSEM)  SDL_SemTryWait (*PSEM)
#define uae_sem_getvalue(PSEM) SDL_SemValue (*PSEM)

#include "commpipe.h"

typedef SDL_Thread *uae_thread_id;

#define BAD_THREAD 0

#define uae_set_thread_priority(pri)

STATIC_INLINE int uae_start_thread (char *name, void *(*f) (void *), void *arg, uae_thread_id *tid)
{
	int result = 1;

	*tid = SDL_CreateThread ((int (*)(void *))f, arg);
	if (*tid == NULL) {
		result = 0;
		write_log ("Thread '%s' failed to start!?\n", name ? name : "<unknown>");
	} else {
		write_log ("Thread '%s' started (%lu)\n", name, (size_t)(*tid) );
	}

	return result;
}

STATIC_INLINE int uae_wait_thread (uae_thread_id thread)
{
    SDL_WaitThread (thread, (int*)0);
    return 0;
}

STATIC_INLINE void uae_kill_thread (uae_thread_id* thread)
{
	SDL_KillThread(*thread);
	*thread = NULL;
}

/* Do nothing; thread exits if thread function returns.  */
#define UAE_THREAD_EXIT do {} while (0)
