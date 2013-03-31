 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Threading support, using pthreads
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 2004 Richard Drummond
  */

#include <pthread.h>
#include <semaphore.h>


#ifndef USE_NAMED_SEMAPHORES

typedef struct {
    sem_t  sem;
} uae_sem_t;

STATIC_INLINE int uae_sem_init (uae_sem_t *sem, int pshared, unsigned int value)
{
   return sem_init (&sem->sem, pshared, value);
}

STATIC_INLINE int uae_sem_destroy (uae_sem_t *sem)
{
    return sem_destroy (&sem->sem);
}

STATIC_INLINE int uae_sem_post (uae_sem_t *sem)
{
    return sem_post (&sem->sem);
}

STATIC_INLINE int uae_sem_wait (uae_sem_t *sem)
{
    return sem_wait (&sem->sem);
}

STATIC_INLINE int uae_sem_trywait (uae_sem_t *sem)
{
    return sem_trywait (&sem->sem);
}

STATIC_INLINE int uae_sem_getvalue (uae_sem_t *sem, int *sval)
{
    return sem_getvalue (&sem->sem, sval);
}

#else

typedef struct {
    sem_t *sem;
} uae_sem_t;

extern int uae_sem_init (uae_sem_t *sem, int pshared, unsigned int value);

STATIC_INLINE int uae_sem_destroy (uae_sem_t *sem)
{
    return sem->sem == 0 ? -1 : sem_close (sem->sem);
}

STATIC_INLINE int uae_sem_post (uae_sem_t *sem)
{
    return sem->sem == 0 ? -1 : sem_post (sem->sem);
}

STATIC_INLINE int uae_sem_wait (uae_sem_t *sem)
{
    return sem->sem == 0 ? -1 : sem_wait (sem->sem);
}

STATIC_INLINE int uae_sem_trywait (uae_sem_t *sem) {
    return sem->sem == 0 ? -1 : sem_trywait (sem->sem);
}

STATIC_INLINE int uae_sem_getvalue (uae_sem_t *sem, int *sval)
{
    return sem->sem == 0 ? -1 : sem_getvalue (sem->sem, sval);
}

#endif

#include "commpipe.h"

typedef pthread_t uae_thread_id;
#define BAD_THREAD -1

STATIC_INLINE int uae_start_thread (void *(*f) (void *), void *arg, uae_thread_id *foo)
{
    return pthread_create (foo, 0, f, arg);
}

STATIC_INLINE int uae_wait_thread (uae_thread_id thread)
{
    return pthread_join (thread, (void**)0);
}

#define UAE_THREAD_EXIT pthread_exit(0)

#define uae_set_thread_priority(pri)
