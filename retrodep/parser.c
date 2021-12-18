/*
 * UAE - The Un*x Amiga Emulator
 *
 * Not a parser, but parallel and serial emulation for Linux
 *
 * Copyright 2010 Mustafa TUFAN
 */

#include "sysconfig.h"

#undef SERIAL_ENET

#include "sysdeps.h"
#include "options.h"
#include "parser.h"
#include "ioport.h"
#if 0
#include "gensound.h"
#include "events.h"
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "autoconf.h"
#include "newcpu.h"
#include "traps.h"
#include "threaddep/thread.h"
#include "serial.h"
#include "parallel.h"
#include "savestate.h"
#include "xwin.h"
#include "drawing.h"
#else
#include "threaddep/thread.h"
#endif

#define MIN_PRTBYTES 10

static uae_char prtbuf[PRTBUFSIZE];
static int prtbufbytes,wantwrite;
static int prtopen;

struct uaeserialdata
{
	long hCom;
	long evtr, evtw, evtt, evtwce;
	long olr, olw, olwce;
	int writeactive;
	void *readdata, *writedata;
	volatile int threadactive;
	uae_sem_t change_sem, sync_sem;
	void *user;
};

int uaeser_getdatalength (void)
{
	return sizeof (struct uaeserialdata);
}

void uaeser_initdata (void *vsd, void *user)
{
}

int uaeser_query (void *vsd, uae_u16 *status, uae_u32 *pending)
{
	return 0;
}

int uaeser_break (void *vsd, int brklen)
{
	return 0;
}

int uaeser_setparams (void *vsd, int baud, int rbuffer, int bits, int sbits, int rtscts, int parity, uae_u32 xonxoff)
{
	return 0;
}

void uaeser_trigger (void *vsd)
{
}

int uaeser_write (void *vsd, uae_u8 *data, uae_u32 len)
{
	return 0;
}

int uaeser_read (void *vsd, uae_u8 *data, uae_u32 len)
{
	return 0;
}

void uaeser_clearbuffers (void *vsd)
{
}

int uaeser_open (void *vsd, void *user, int unit)
{
	return 0;
}

void uaeser_close (void *vsd)
{
}

#if 0
#define SERIAL_WRITE_BUFFER 100
#define SERIAL_READ_BUFFER 100
static uae_u8 outputbuffer[SERIAL_WRITE_BUFFER];
static uae_u8 outputbufferout[SERIAL_WRITE_BUFFER];
static uae_u8 inputbuffer[SERIAL_READ_BUFFER];
static int datainoutput;
static int dataininput, dataininputcnt;
static int writepending;
#endif

int isprinter (void)
{
	if (!currprefs.prtname[0])
		return 0;
	if (!_tcsncmp (currprefs.prtname, _T("LPT"), 3)) {
		/*paraport_open (currprefs.prtname);*/
		return -1;
	}
	return 1;
}

int isprinteropen (void)
{
	if (prtopen || prtbufbytes > 0)
		return 1;
	return 0;
}

void doprinter (uae_u8 val) {}
void initparallel (void) {}

int seriallog = 0, log_sercon = 0;
void serial_rbf_clear (void) {}
void serial_check_irq (void) {}
void serial_uartbreak (int v) {}
void serial_hsynchandler (void) {}
