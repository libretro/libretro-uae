 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Win32 thread support
  *
  * Copyright 1997 Mathias Ortmann
  * Copyright 2005 Richard Drummond
  *
  */

#include <windows.h>

typedef HANDLE uae_sem_t;

void uae_sem_init    (uae_sem_t *event, int manual_reset, int initial_state);
void uae_sem_wait    (uae_sem_t *event);
void uae_sem_post    (uae_sem_t *event);
int  uae_sem_trywait (uae_sem_t *event);
void uae_sem_close   (uae_sem_t *event);



typedef int uae_thread_id;

int  uae_start_thread        (void *(*f)(void *), void *arg, DWORD * foo);
void uae_set_thread_priority (int pri);
int  uae_wait_thread         (uae_thread_id thread);



#include "commpipe.h"
