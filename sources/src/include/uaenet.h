#pragma once
#ifndef SRC_INCLUDE_UAENET_H_INCLUDED
#define SRC_INCLUDE_UAENET_H_INCLUDED 1

#define ADDR_SIZE 6

#include "threaddep/thread.h"

#ifdef A2065
#define HAVE_REMOTE
#define WPCAP
#include "pcap.h"
#endif // A2065

#define MAX_TOTAL_NET_DEVICES 10

struct s2devstruct {
	int unit, opencnt, exclusive, promiscuous;
	struct asyncreq *ar;
	struct asyncreq *s2p;
	struct mcast *mc;
	smp_comm_pipe requests;
	int thread_running;
	uae_sem_t sync_sem;
	void *sysdata;
	uae_u32 packetsreceived;
	uae_u32 packetssent;
	uae_u32 baddata;
	uae_u32 overruns;
	uae_u32 unknowntypesreceived;
	uae_u32 reconfigurations;
	uae_u32 online_micro;
	uae_u32 online_secs;
	int configured;
	int adapter;
	int online;
	struct netdriverdata *td;
	struct s2packet *readqueue;
	uae_u8 mac[ADDR_SIZE];
	int flush_timeout;
	int flush_timeout_cnt;
};

typedef void (uaenet_gotfunc)(struct s2devstruct *dev, const uae_u8 *data, int len);
typedef int (uaenet_getfunc)(struct s2devstruct *dev, uae_u8 *d, int *len);

struct netdriverdata
{
    TCHAR *name;
    TCHAR *desc;
    int mtu;
    uae_u8 mac[6];
    int active;
};

struct uaenetdata
{
	void *readdata, *writedata;

	uae_sem_t change_sem;

	volatile int threadactiver;
	uae_thread_id tidr;
	uae_sem_t sync_semr;
	volatile int threadactivew;
	uae_thread_id tidw;
	uae_sem_t sync_semw;

	struct s2devstruct *user;
	struct netdriverdata *tc;
	uae_u8 *readbuffer;
	uae_u8 *writebuffer;
	int mtu;

#ifdef A2065
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *fp;
#endif // A2065
	uaenet_gotfunc *gotfunc;
	uaenet_getfunc *getfunc;
};

struct netdriverdata *uaenet_enumerate (struct netdriverdata **out, const TCHAR *name);
void uaenet_enumerate_free (struct netdriverdata *tcp);
void uaenet_close_driver (struct netdriverdata *tc);

int uaenet_getdatalength (void);
int uaenet_getbytespending (void*);
int uaenet_open (struct uaenetdata*, struct netdriverdata*, struct s2devstruct*, uaenet_gotfunc*, uaenet_getfunc*, int);
void uaenet_close (struct uaenetdata*);
void uaenet_gotdata (struct s2devstruct *dev, const uae_u8 *data, int len);
int uaenet_getdata (struct s2devstruct *dev, uae_u8 *d, int *len);
void uaenet_trigger (struct uaenetdata*);

#endif // SRC_INCLUDE_UAENET_H_INCLUDED
