/*
 * UAE - The Un*x Amiga Emulator
 *
 * Linux uaenet emulation
 *
 * Copyright 2007 Toni Wilen
 *           2010 Mustafa TUFAN
 */


#include "sysconfig.h"

#include <stdio.h>
#ifdef A2065

/** REMOVEME:
  * These two are windows specific
**/
#if 0
#include "packet32.h"
#include "ntddndis.h"
#endif // 0

#include "sysdeps.h"
#include "options.h"

#include "uaenet.h"
#include "misc.h"
#include "sleep.h"

static struct netdriverdata tds[MAX_TOTAL_NET_DEVICES];
static int enumerated;

int uaenet_getdatalength (void)
{
	return sizeof (struct uaenetdata);
}

static void uaeser_initdata (struct uaenetdata *sd, struct s2devstruct *user)
{
	memset (sd, 0, sizeof (struct uaenetdata));
	sd->user = user;
	sd->fp = NULL;
}

static void *uaenet_trap_threadr (void *arg)
{
	struct uaenetdata *sd = (struct uaenetdata *)arg;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	uae_set_thread_priority (1);
	sd->threadactiver = 1;
	uae_sem_post (&sd->sync_semr);
	while (sd->threadactiver == 1) {
		int r;
		r = pcap_next_ex (sd->fp, &header, &pkt_data);
		if (r == 1) {
			uae_sem_wait (&sd->change_sem);
			sd->gotfunc (sd->user, pkt_data, header->len);
			uae_sem_post (&sd->change_sem);
		}
		if (r < 0) {
			write_log ("pcap_next_ex failed, err=%d\n", r);
			break;
		}
	}
	sd->threadactiver = 0;
	uae_sem_post (&sd->sync_semr);
	return 0;
}

static void *uaenet_trap_threadw (void *arg)
{
	struct uaenetdata *sd = (struct uaenetdata*)arg;
	uae_set_thread_priority (1);
	sd->threadactivew = 1;
	uae_sem_post (&sd->sync_semw);
	while (sd->threadactivew == 1) {
		int towrite = sd->mtu;
		uae_sem_wait (&sd->change_sem);
		if (sd->getfunc (sd->user, sd->writebuffer, &towrite)) {
			pcap_sendpacket (sd->fp, sd->writebuffer, towrite);
		}
		uae_sem_post (&sd->change_sem);
	}
	sd->threadactivew = 0;
	uae_sem_post (&sd->sync_semw);
	return 0;
}

void uaenet_trigger (struct uaenetdata *sd)
{
	if (!sd)
		return;
	/// FIXME: The current trigger was windows specific
	//SetEvent (sd->evttw);
}

int uaenet_open (struct uaenetdata *sd, struct netdriverdata *tc, struct s2devstruct *user,
				uaenet_gotfunc *gotfunc, uaenet_getfunc *getfunc, int promiscuous)
{
	char *s;

	s = ua (tc->name);
	sd->fp = pcap_open_live (s, 65536, promiscuous, 100, sd->errbuf);
	xfree (s);
	if (sd->fp == NULL) {
		TCHAR *ss = au (sd->errbuf);
		write_log ("'%s' failed to open: %s\n", tc->name, ss);
		xfree (ss);
		return 0;
	}
	sd->tc = tc;
	sd->user = user;
	sd->mtu = tc->mtu;
	sd->readbuffer = xmalloc (uae_u8, sd->mtu);
	sd->writebuffer = xmalloc (uae_u8, sd->mtu);
	sd->gotfunc = gotfunc;
	sd->getfunc = getfunc;

	uae_sem_init (&sd->change_sem, 0, 1);
	uae_sem_init (&sd->sync_semr, 0, 0);
	uae_start_thread ("uaenet_r", uaenet_trap_threadr, sd, &sd->tidr);
	uae_sem_wait (&sd->sync_semr);
	uae_sem_init (&sd->sync_semw, 0, 0);
	uae_start_thread ("uaenet_w", uaenet_trap_threadw, sd, &sd->tidw);
	uae_sem_wait (&sd->sync_semw);
	write_log ("uaenet initialized\n");
	return 1;
}

void uaenet_close (struct uaenetdata *sd)
{
	if (!sd)
		return;
	if (sd->threadactiver) {
		sd->threadactiver = -1;
	}
	if (sd->threadactivew) {
		sd->threadactivew = -1;
/// REMOVEME: win32 specific
//		SetEvent (sd->evttw);
	}
	if (sd->threadactiver) {
		while (sd->threadactiver)
			sleep_millis(10);
		write_log ("uaenet_ thread %d killed\n", sd->tidr);
		uae_kill_thread (&sd->tidr);
	}
	if (sd->threadactivew) {
		while (sd->threadactivew)
			sleep_millis(10);
/// REMOVEME: win32 specific
//		CloseHandle (sd->evttw);
		write_log ("uaenet thread %d killed\n", sd->tidw);
		uae_kill_thread (&sd->tidw);
	}
	xfree (sd->readbuffer);
	xfree (sd->writebuffer);
	if (sd->fp)
		pcap_close (sd->fp);
	uaeser_initdata (sd, sd->user);
	write_log ("uaenet_win32 closed\n");
}

void uaenet_enumerate_free (struct netdriverdata *tcp)
{
	int i;

	if (!tcp)
		return;
	for (i = 0; i < MAX_TOTAL_NET_DEVICES; i++) {
		xfree (tcp[i].name);
		xfree (tcp[i].desc);
		tcp[i].name = NULL;
		tcp[i].desc = NULL;
		tcp[i].active = 0;
	}
}

static struct netdriverdata *enumit (const TCHAR *name)
{
	int cnt;
	for (cnt = 0; cnt < MAX_TOTAL_NET_DEVICES; cnt++) {
		TCHAR mac[20];
		struct netdriverdata *tc = tds + cnt;
		_stprintf (mac, "%02X:%02X:%02X:%02X:%02X:%02X",
			tc->mac[0], tc->mac[1], tc->mac[2], tc->mac[3], tc->mac[4], tc->mac[5]);
		if (tc->active && name && (!_tcsicmp (name, tc->name) || !_tcsicmp (name, mac)))
			return tc;
	}
	return NULL;
}

struct netdriverdata *uaenet_enumerate (struct netdriverdata **out, const TCHAR *name)
{
	static int done;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldevs, *d;
	int cnt;
/// REMOVEME: Win32 specific
#if 0
	HMODULE hm;
	LPADAPTER lpAdapter = 0;
	PPACKET_OID_DATA OidData;
	struct netdriverdata *tc, *tcp;
#endif // 0
	pcap_t *fp;
	int val;
	TCHAR *ss;

	if (enumerated) {
		if (out)
			*out = tds;
		return enumit (name);
	}
/// REMOVEME: win32 specific
#if 0
	tcp = tds;
	hm = LoadLibrary ("wpcap.dll");
	if (hm == NULL) {
		write_log ("uaenet: winpcap not installed (wpcap.dll)\n");
		return NULL;
	}
	FreeLibrary (hm);
	hm = LoadLibrary ("packet.dll");
	if (hm == NULL) {
		write_log ("uaenet: winpcap not installed (packet.dll)\n");
		return NULL;
	}
	FreeLibrary (hm);
	if (!isdllversion ("wpcap.dll", 4, 0, 0, 0)) {
		write_log ("uaenet: too old winpcap, v4 or newer required\n");
		return NULL;
	}

	ss = au (pcap_lib_version ());
	if (!done)
		write_log ("uaenet: %s\n", ss);
	xfree (ss);
#endif // 0

	if (pcap_findalldevs (&alldevs, errbuf) == -1) {
		ss = au (errbuf);
		write_log ("uaenet: failed to get interfaces: %s\n", ss);
		xfree (ss);
		return NULL;
	}

	if (!done)
		write_log ("uaenet: detecting interfaces\n");
	for(cnt = 0, d = alldevs; d != NULL; d = d->next) {
		char *n2;
		TCHAR *ss2;
/// REMOVEME: unused
//		tc = tcp + cnt;
		if (cnt >= MAX_TOTAL_NET_DEVICES) {
			write_log ("buffer overflow\n");
			break;
		}
		ss = au (d->name);
		ss2 = d->description ? au (d->description) : "(no description)";
		write_log ("%s\n- %s\n", ss, ss2);
		xfree (ss2);
		xfree (ss);
		n2 = d->name;
		if (!strlen (n2)) {
			write_log ("- corrupt name\n");
			continue;
		}
		fp = pcap_open_live (d->name, 65536, 0, 0, errbuf);
		if (!fp) {
			ss = au (errbuf);
			write_log ("- pcap_open() failed: %s\n", ss);
			xfree (ss);
			continue;
		}
		val = pcap_datalink (fp);
		pcap_close (fp);
		if (val != DLT_EN10MB) {
			if (!done)
				write_log ("- not an ethernet adapter (%d)\n", val);
			continue;
		}

/// REMOVEME: win32 specific
#if 0
		lpAdapter = PacketOpenAdapter (n2 + strlen (PCAP_SRC_IF_STRING));
		if (lpAdapter == NULL) {
			if (!done)
				write_log ("- PacketOpenAdapter() failed\n");
			continue;
		}
		OidData = (PPACKET_OID_DATA)xcalloc (uae_u8, 6 + sizeof(PACKET_OID_DATA));
		if (OidData) {
			OidData->Length = 6;
			OidData->Oid = OID_802_3_CURRENT_ADDRESS;
			if (PacketRequest (lpAdapter, false, OidData)) {
				memcpy (tc->mac, OidData->Data, 6);
				if (!done)
					write_log ("- MAC %02X:%02X:%02X:%02X:%02X:%02X (%d)\n",
					tc->mac[0], tc->mac[1], tc->mac[2],
					tc->mac[3], tc->mac[4], tc->mac[5], cnt++);
				tc->active = 1;
				tc->mtu = 1522;
				tc->name = au (d->name);
				tc->desc = au (d->description);
			} else {
				write_log (" - failed to get MAC\n");
			}
			xfree (OidData);
		}
		PacketCloseAdapter (lpAdapter);
#endif // 0
	}
	if (!done)
		write_log ("uaenet: end of detection\n");
	done = 1;
	pcap_freealldevs (alldevs);
	enumerated = 1;
	if (out)
		*out = tds;
	return enumit (name);
}

void uaenet_close_driver (struct netdriverdata *tc)
{
	int i;

	if (!tc)
		return;
	for (i = 0; i < MAX_TOTAL_NET_DEVICES; i++) {
		tc[i].active = 0;
	}
}

#endif