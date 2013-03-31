 /*
  * E-UAE - The portable Amiga Emulator
  *
  * Thread and semaphore support using AmigaOS processes
  *
  * Copyright 2003-2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <exec/exec.h>
#include <exec/ports.h>
#include <exec/lists.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <clib/alib_protos.h>

#include "threaddep/thread.h"

/*
 * Handle CreateNewProc() differences between AmigaOS-like systems 
 */
#ifdef __MORPHOS__
/* CreateNewProc() on MorphOS needs to be told that code is PPC */
# define myCreateNewProcTags(...) CreateNewProcTags(NP_CodeType, CODETYPE_PPC, __VA_ARGS__)
#else
# ifdef __amigaos4__
/* On OS4, we assert that the threads we create are our children */
#  define myCreateNewProcTags(...) CreateNewProcTags(NP_Child, TRUE, __VA_ARGS__)
# else
#  define myCreateNewProcTags CreateNewProcTags
# endif
#endif

/*
 * Simple emulation of POSIX semaphores using message-passing
 * with a proxy task.
 *
 */

static struct Process *proxy_thread;
static struct MsgPort *proxy_msgport;

static int sem_count;

/*
 * Message packet which is sent to the proxy thread
 * to effect blocking on a semaphore
 */

/*
 * Message types
 */
typedef enum {
    PROXY_MSG_BLOCK,
    PROXY_MSG_UNBLOCK,
    PROXY_MSG_DESTROY,
} ProxyMsgType;

struct ProxyMsg {
    struct Message     msg;
    struct PSemaphore *sem;
    struct Task       *reply_task;
    ProxyMsgType       type;
};

/* Proxy replies to message sender with this signal */
#define PROXY_MSG_REPLY_SIG		SIGBREAKF_CTRL_F

STATIC_INLINE void init_proxy_msg (struct ProxyMsg *msg, ProxyMsgType type, struct PSemaphore *sem)
{
    memset (msg, 0, sizeof (struct ProxyMsg));

    msg->msg.mn_Length = sizeof (struct ProxyMsg);
    msg->sem           = sem;
    msg->type          = type;
    msg->reply_task    = FindTask (NULL);
}

STATIC_INLINE void put_proxy_msg (struct ProxyMsg *msg)
{
    PutMsg (proxy_msgport, (struct Message *)msg);
}

STATIC_INLINE BOOL wait_proxy_reply (void)
{
    BOOL  got_reply = FALSE;
    ULONG sigs;

    sigs = Wait (PROXY_MSG_REPLY_SIG | SIGBREAKF_CTRL_C);

    if (sigs & PROXY_MSG_REPLY_SIG)
	got_reply = TRUE;

    return got_reply;
}

/*
 * Do proxy's reply to a semaphore message
 */
STATIC_INLINE void do_proxy_reply (struct ProxyMsg *msg)
{
   ReplyMsg ((struct Message *)msg);

   Signal (msg->reply_task, PROXY_MSG_REPLY_SIG);
}



/*
 * Block current process on semaphore
 */
static int BlockMe (struct PSemaphore *psem)
{
    int result = -1;
    struct ProxyMsg msg;

    init_proxy_msg (&msg, PROXY_MSG_BLOCK, psem);

    Forbid ();

    put_proxy_msg (&msg);
    if (wait_proxy_reply ())
	result = 0;
    else {
	/* Hack - remove block request from semaphore's waiting list
	 * When the block is interrupted, the proxy won't do it */
	Remove ((struct Node *)&msg);
	msg.msg.mn_Node.ln_Succ = msg.msg.mn_Node.ln_Pred = NULL;
    }

    Permit ();

    return result;
}

/*
 * Unblock one process blocked on semaphore
 */
static void UnblockOne (struct PSemaphore *psem)
{
    struct ProxyMsg msg;

    init_proxy_msg (&msg, PROXY_MSG_UNBLOCK, psem);

    put_proxy_msg (&msg);
    wait_proxy_reply ();
}

/*
 * Destroy the semaphore block
 */
static void Destroy (struct PSemaphore *psem)
{
    struct ProxyMsg msg;

    init_proxy_msg (&msg, PROXY_MSG_DESTROY, psem);

    put_proxy_msg (&msg);
    wait_proxy_reply ();
}



/**
 ** Proxy thread
 **/

static void handle_startup_msg (void)
{
    struct Process *self = (struct Process*) FindTask (NULL);
    WaitPort (&self->pr_MsgPort);
    ReplyMsg (GetMsg (&self->pr_MsgPort));
}

static void proxy_thread_main (void)
{
    ULONG sigmask;
    int dont_exit = 1;
    struct ProxyMsg *msg = NULL;

    proxy_msgport = CreateMsgPort ();
    sigmask = SIGBREAKF_CTRL_C | (1 << proxy_msgport->mp_SigBit);

    /* Wait for and reply to startup msg */
    handle_startup_msg ();

    while (dont_exit) {
	ULONG sigs = Wait (sigmask);

	/*
	 * Handle any semaphore message sent us
	 */
	while ((msg = (struct ProxyMsg *) GetMsg (proxy_msgport))) {
	    struct PSemaphore *psem = msg->sem;

	    switch (msg->type) {
		case PROXY_MSG_BLOCK:
		    /* Block requesting task on semaphore.
		     *
		     * We effect the block by not replying to this message now. Just
		     * put the request in the waiting list and reply to it when we
		     * get an unblock request for this semaphore. The process requesting
		     * the block will sleep until we do (unless it is interrupted). */
		    AddTail ((struct List*)&psem->wait_queue, (struct Node *)msg);
		    break;

		case PROXY_MSG_UNBLOCK:
		    /* A request to unblock a task on semaphore. */
		    {
			struct ProxyMsg *wait_msg;

			Forbid ();

			/* If there's a task blocking on the semaphore, remove
			 * it from the waiting list */
			wait_msg = (struct ProxyMsg *) RemHead ((struct List *)&psem->wait_queue);

			/* Reply to the unblock request */
			do_proxy_reply (msg);

			/* Reply to the block request - this will wake up the
			 * blocked task. */
			if (wait_msg)
			    do_proxy_reply (wait_msg);

			Permit ();
		    }
		    break;

		case PROXY_MSG_DESTROY:
		    Forbid ();
		    {
			struct ProxyMsg *wait_msg;

			while ((wait_msg = (struct ProxyMsg *) RemHead ((struct List *)&psem->wait_queue))) {
			    /* Break the block for all tasks blocked on the semaphore */
			    Signal (wait_msg->reply_task, SIGBREAKF_CTRL_C);
			}
		    }
		    do_proxy_reply (msg);
		    Permit ();
		    break;
	    } /* switch (msg->type) */
	} /* while */

	/*
	 * Check for break signal and exit if received
	 */
	if ((sigs & SIGBREAKF_CTRL_C) != 0)
	    dont_exit = FALSE;
    } /* while (dont_exit) */

   /*
    * Clean up and exit
    */
   Forbid ();
   {
	struct ProxyMsg *rem_msg;

	/* Reply to any unanswered requests */
	while ((rem_msg = (struct ProxyMsg *) GetMsg (proxy_msgport)) != NULL)
	    do_proxy_reply (rem_msg);

	/* reply to the quit msg */
	if (msg != NULL)
	    do_proxy_reply (msg);

	DeleteMsgPort (proxy_msgport);
	proxy_msgport = 0;
	proxy_thread  = 0;
   }
   Permit ();
}

static void stop_proxy_thread (void)
{
    if (proxy_thread)
	Signal ((struct Task *)proxy_thread, SIGBREAKF_CTRL_C);
}

/*
 * Start proxy thread
 */
static int start_proxy_thread (void)
{
    int result = -1;
    struct MsgPort *replyport = CreateMsgPort();
    struct Process *p;

    if (replyport) {
	p = myCreateNewProcTags (NP_Name,	(ULONG) "E-UAE semaphore proxy",
				 NP_Priority,		10,
				 NP_StackSize,		2048,
				 NP_Entry,	(ULONG) proxy_thread_main,
				 TAG_DONE);
	if (p) {
	    /* Send startup message */
	    struct Message msg;
	    msg.mn_ReplyPort = replyport;
	    msg.mn_Length    = sizeof msg;
	    PutMsg (&p->pr_MsgPort, (struct Message*)&msg);
	    WaitPort (replyport);

	    proxy_thread = p;

	    atexit (stop_proxy_thread);

	    result = 0;
	}
	DeleteMsgPort (replyport);
    }
    return result;
}

/**
 ** Emulation of POSIX semaphore API
 ** (or, at least, UAE's version thereof).
 **/

int uae_sem_init (uae_sem_t *sem, int pshared, unsigned int value)
{
    int result = 0;

    InitSemaphore (&sem->mutex);
    NewList ((struct List *)&sem->wait_queue);
    sem->value = value;
    sem->live  = 1;

    if (sem_count == 0)
	result = start_proxy_thread ();

    if (result != -1)
	sem_count++;

    return result;
}

int uae_sem_destroy (uae_sem_t *sem)
{
    ObtainSemaphore (&sem->mutex);

    sem->live = 0;
    Destroy (sem);

    /* If this is the last semaphore to die,
     * kill the proxy thread */
    if (--sem_count == 0)
	stop_proxy_thread ();

    ReleaseSemaphore (&sem->mutex);

    return 0;
}

int uae_sem_wait (uae_sem_t *sem)
{
    int result = -1;

    ObtainSemaphore (&sem->mutex);

    if (sem->live) {
	if (sem->value > 0) {
	    --sem->value;
	    result = 0;
        } else {
	    ReleaseSemaphore (&sem->mutex);

	    /* Block on this semaphore by waiting for
	     * the proxy thread to reply to our lock request
	     */
	    result = BlockMe (sem);

	    ObtainSemaphore (&sem->mutex);

	    if (result != -1)
		--sem->value;
	}
    }
    ReleaseSemaphore (&sem->mutex);
    return result;
}

int uae_sem_trywait (uae_sem_t *sem)
{
    int result = -1;

    ObtainSemaphore (&sem->mutex);

    if (sem->live) {
	if (sem->value > 0) {
	    --sem->value;
	    result = 0;
        }
    }
    ReleaseSemaphore (&sem->mutex);
    return result;
}

int uae_sem_get_value (uae_sem_t *sem)
{
    int value;

    ObtainSemaphore (&sem->mutex);
    value = sem->value;
    ReleaseSemaphore (&sem->mutex);

    return value;
}

int uae_sem_post (uae_sem_t *sem)
{
    int result = -1;

    ObtainSemaphore (&sem->mutex);

    if (sem->live) {
	sem->value++;

	if (sem->value == 1) {
	    /* Ask the proxy to wake up a task blocked
	     * on this semaphore */
	    UnblockOne (sem);
	}
	result = 0;
    }
    ReleaseSemaphore (&sem->mutex);

    return result;
}


/**
 ** Thread support
 **/

struct startupmsg
{
    struct Message msg;
    void           *(*func) (void *);
    void           *arg;
};

static void do_thread (void)
{
   struct Process *pr = (struct Process *) FindTask (NULL);
   struct startupmsg *msg;
   void *(*func) (void *);
   void *arg;

   WaitPort (&pr->pr_MsgPort);
   msg = (struct startupmsg *) GetMsg(&pr->pr_MsgPort);
   func = msg->func;
   arg  = msg->arg;
   ReplyMsg ((struct Message*)msg);

   func (arg);
}

int uae_start_thread (void *(*f) (void *), void *arg, uae_thread_id *foo)
{
    struct MsgPort *replyport = CreateMsgPort();

    if (replyport) {
	*foo = (struct Task *)myCreateNewProcTags (NP_Output,		   Output (),
						   NP_Input,		   Input (),
						   NP_Name,	   (ULONG) "UAE thread",
						   NP_CloseOutput,	   FALSE,
						   NP_CloseInput,	   FALSE,
						   NP_StackSize,	   16384,
						   NP_Entry,	   (ULONG) do_thread,
						   TAG_DONE);

	if(*foo) {
	    struct startupmsg msg;

	    msg.msg.mn_ReplyPort = replyport;
	    msg.msg.mn_Length    = sizeof msg;
	    msg.func             = f;
	    msg.arg              = arg;
	    PutMsg (&((struct Process*)*foo)->pr_MsgPort, (struct Message*)&msg);
	    WaitPort (replyport);
	}
	DeleteMsgPort (replyport);
    }

    return *foo!=0;
}

extern void uae_set_thread_priority (int pri)
{
    SetTaskPri (FindTask (NULL), pri);
}
