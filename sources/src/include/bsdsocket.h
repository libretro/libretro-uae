#pragma once
#ifndef SRC_INCLUDE_BSDSOCKET_H_INCLUDED
#define SRC_INCLUDE_BSDSOCKET_H_INCLUDED 1

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * bsdsocket.library emulation
  *
  * Copyright 1997,98 Mathias Ortmann
  *
  */

#define BSD_TRACING_ENABLED 0

extern int log_bsd;

#define ISBSDTRACE (log_bsd || BSD_TRACING_ENABLED)
#define BSDTRACE(x) do { if (ISBSDTRACE) { write_log x; } } while(0)

int init_socket_layer (void);
void deinit_socket_layer (void);

/* inital size of per-process descriptor table (currently fixed) */
#define DEFAULT_DTABLE_SIZE 64

#define SCRATCHBUFSIZE 128

#define MAXPENDINGASYNC 512

#define MAXADDRLEN 256

#define SOCKET_TYPE int

/* allocated and maintained on a per-task basis */
struct socketbase {
    struct socketbase *next;
    struct socketbase *nextsig;	/* queue for tasks to signal */

    uaecptr sysbase;
    int dosignal;		/* signal flag */
    uae_u32 ownertask;		/* task that opened the library */
    int signal;			/* signal allocated for that task */
    int sb_errno, sb_herrno;	/* errno and herrno variables */
    uae_u32 errnoptr, herrnoptr;	/* pointers */
    uae_u32 errnosize, herrnosize;	/* pinter sizes */
    int dtablesize;		/* current descriptor/flag etc. table size */
    SOCKET_TYPE *dtable;	/* socket descriptor table */
    int *ftable;		/* socket flags */
    int resultval;
    uae_u32 hostent;		/* pointer to the current hostent structure (Amiga mem) */
    uae_u32 hostentsize;
    uae_u32 protoent;		/* pointer to the current protoent structure (Amiga mem) */
    uae_u32 protoentsize;
    uae_u32 servent;		/* pointer to the current servent structure (Amiga mem) */
    uae_u32 serventsize;
    uae_u32 sigstosend;
    uae_u32 eventsigs;		/* EVENT sigmask */
    uae_u32 eintrsigs;		/* EINTR sigmask */
    int eintr;			/* interrupted by eintrsigs? */
    int eventindex;		/* current socket looked at by GetSocketEvents() to prevent starvation */
	uae_u32 logstat;
	uae_u32 logptr;
	uae_u32 logmask;
	uae_u32 logfacility;
	uae_u32 fdcallback;

    /* host-specific fields below */
    uae_sem_t sem;		/* semaphore to notify the socket thread of work */
    uae_thread_id thread;	/* socket thread */
    int  sockabort[2];		/* pipe used to tell the thread to abort a select */
    int action;
    int s;			/* for accept */
    uae_u32 name;		/* For gethostbyname */
    uae_u32 a_addr;		/* gethostbyaddr, accept */
    uae_u32 a_addrlen;		/* for gethostbyaddr, accept */
    uae_u32 flags;
    void *buf;
    uae_u32 len;
    uae_u32 to, tolen, from, fromlen;
    int nfds;
    uae_u32 sets [3];
    uae_u32 timeout;
    uae_u32 sigmp;
};

#define LIBRARY_SIZEOF 36

struct UAEBSDBase {
    uae_u8 dummy[LIBRARY_SIZEOF];
    struct socketbase *sb;
    uae_u8 scratchbuf[SCRATCHBUFSIZE];
};

/* socket flags */
/* socket events to report */
#define REP_ACCEPT	 0x01	/* there is a connection to accept() */
#define REP_CONNECT	 0x02	/* connect() completed */
#define REP_OOB		 0x04	/* socket has out-of-band data */
#define REP_READ	 0x08	/* socket is readable */
#define REP_WRITE	 0x10	/* socket is writeable */
#define REP_ERROR	 0x20	/* asynchronous error on socket */
#define REP_CLOSE	 0x40	/* connection closed (graceful or not) */
#define REP_ALL      0x7f
/* socket events that occurred */
#define SET_ACCEPT	 0x0100	/* there is a connection to accept() */
#define SET_CONNECT	 0x0200	/* connect() completed */
#define SET_OOB		 0x0400	/* socket has out-of-band data */
#define SET_READ	 0x0800	/* socket is readable */
#define SET_WRITE	 0x1000	/* socket is writeable */
#define SET_ERROR	 0x2000	/* asynchronous error on socket */
#define SET_CLOSE	 0x4000	/* connection closed (graceful or not) */
#define SET_ALL      0x7f00
/* socket properties */
#define SF_BLOCKING 0x80000000
#define SF_BLOCKINGINPROGRESS 0x40000000
/* STBC_FDCALLBACK */
#define FDCB_FREE  0
#define FDCB_ALLOC 1
#define FDCB_CHECK 2

uae_u32 addstr (uae_u32 *, const char *);
uae_u32 addmem (uae_u32 *, const char *, int len);

char *strncpyah (char *, uae_u32, int);
char *strcpyah (char *, uae_u32);
uae_u32 strcpyha (uae_u32, const char *);
uae_u32 strncpyha (uae_u32, const char *, int);

#define SB struct socketbase *sb

typedef int SOCKET;
#define INVALID_SOCKET -1

void bsdsocklib_seterrno (SB, int);
void bsdsocklib_setherrno (SB, int);

void sockmsg (unsigned int, unsigned long, unsigned long);
void sockabort (SB);

void addtosigqueue (SB, int);
void removefromsigqueue (SB);
void sigsockettasks (void);
void locksigqueue (void);
void unlocksigqueue (void);

bool checksd(SB, int sd);
void setsd(SB, int , SOCKET_TYPE);
int getsd (SB, SOCKET_TYPE);
SOCKET_TYPE getsock (SB, int);
void releasesock (SB, int);

void waitsig (TrapContext *context, SB);
void cancelsig (TrapContext *context, SB);

int host_sbinit (TrapContext*, SB);
void host_sbcleanup (SB);
void host_sbreset (void);
void host_closesocketquick (SOCKET_TYPE);

int host_dup2socket (SB, int, int);
int host_socket (SB, int, int, int);
uae_u32 host_bind (SB, uae_u32, uae_u32, uae_u32);
uae_u32 host_listen (SB, uae_u32, uae_u32);
void host_accept (TrapContext *, SB, uae_u32, uae_u32, uae_u32);
void host_sendto (TrapContext *, SB, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32);
void host_recvfrom (TrapContext *, SB, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32);
uae_u32 host_shutdown (SB, uae_u32, uae_u32);
void host_setsockopt (SB, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32);
uae_u32 host_getsockopt (SB, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32);
uae_u32 host_getsockname (SB, uae_u32, uae_u32, uae_u32);
uae_u32 host_getpeername (SB, uae_u32, uae_u32, uae_u32);
uae_u32 host_IoctlSocket (TrapContext *, SB, uae_u32, uae_u32, uae_u32);
uae_u32 host_shutdown (SB, uae_u32, uae_u32);
int host_CloseSocket (TrapContext *, SB, int);
void host_connect (TrapContext *, SB, uae_u32, uae_u32, uae_u32);
void host_WaitSelect (TrapContext *, SB, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32, uae_u32);
uae_u32 host_SetSocketSignals (void);
uae_u32 host_getdtablesize (void);
uae_u32 host_ObtainSocket (void);
uae_u32 host_ReleaseSocket (void);
uae_u32 host_ReleaseCopyOfSocket (void);
uae_u32 host_Inet_NtoA (TrapContext *context, SB, uae_u32);
uae_u32 host_inet_addr (uae_u32);
uae_u32 host_Inet_LnaOf (void);
uae_u32 host_Inet_NetOf (void);
uae_u32 host_Inet_MakeAddr (void);
uae_u32 host_inet_network (void);
void host_gethostbynameaddr (TrapContext *, SB, uae_u32, uae_u32, long);
uae_u32 host_getnetbyname (void);
uae_u32 host_getnetbyaddr (void);
void host_getservbynameport (TrapContext *, SB, uae_u32, uae_u32, uae_u32);
void host_getprotobyname (TrapContext *, SB, uae_u32);
void host_getprotobynumber (TrapContext *, SB, uae_u32);
uae_u32 host_vsyslog (void);
uae_u32 host_Dup2Socket (void);
uae_u32 host_gethostname (uae_u32, uae_u32);

uaecptr bsdlib_startup (uaecptr);
void bsdlib_install (void);
void bsdlib_reset (void);

void bsdsock_fake_int_handler(void);

#endif // SRC_INCLUDE_BSDSOCKET_H_INCLUDED
