/*
* UAE - The Un*x Amiga Emulator
*
* Not a parser, but parallel and serial emulation for Win32
*
* Copyright 1997 Mathias Ortmann
* Copyright 1998-1999 Brian King - added MIDI output support
*/

#include "sysconfig.h"

#undef SERIAL_ENET

#include "sysdeps.h"
#include "options.h"
#include "parser.h"
#include "ioport.h"
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
#include "midi.h"

#define MIN_PRTBYTES 10

int Midi_Open (void) { return 0; }
void Midi_Close(void) {}
int Midi_Parse(midi_direction_e direction, BYTE *dataptr) { return 0; }

LONG getmidibyte(void) { return -1; };
int ismidibyte(void) { return 0; }
BOOL midi_ready = false;

#define closesocket(s) close(s)
#define select(...) 0
#define accept(...) 0
#define send(...) 0
#define recv(...) 0

typedef struct fd_set_uae
{
    uae_u8  fd_count;
    SOCKET  fd_array[FD_SETSIZE];
} fd_set_uae;

#ifndef INVALID_SOCKET
#define INVALID_SOCKET 0
#endif

#define WSAStartup(x, y) 0
#define WSACleanup() 0
#define WSAGetLastError() 0

static uae_char prtbuf[PRTBUFSIZE];
static int prtbufbytes,wantwrite;
static int prtopen;
static int uartbreak;
static int parflush;

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

static int doflushprinter (void)
{
	if (prtopen == 0 && prtbufbytes < MIN_PRTBYTES) {
		if (prtbufbytes > 0)
			write_log (_T("PRINTER: %d bytes received, less than %d bytes, not printing.\n"), prtbufbytes, MIN_PRTBYTES);
		prtbufbytes = 0;
		return 0;
	}
	return 1;
}

static void flushprtbuf (void)
{
#if 0
	DWORD written = 0;

	if (!prtbufbytes)
		return;

	if (postscript_print_debugging && prtdump)
		zfile_fwrite (prtbuf, prtbufbytes, 1, prtdump);

	if (currprefs.parallel_postscript_emulation) {

		if (psmode) {
			uae_u8 *p;
			psbuffer = xrealloc (uae_u8*, psbuffer, (psbuffers + 2));
			p = xmalloc (uae_u8, prtbufbytes + 2);
			p[0] = prtbufbytes >> 8;
			p[1] = prtbufbytes;
			memcpy (p + 2, prtbuf, prtbufbytes);
			psbuffer[psbuffers++] = p;
			psbuffer[psbuffers] = NULL;
		}
		prtbufbytes = 0;
		return;

	} else if (prtbufbytes > 0) {
		int pbyt = prtbufbytes;

		if (currprefs.parallel_matrix_emulation >= PARALLEL_MATRIX_EPSON) {
			int i;
			if (!prtopen) {
				if (!doflushprinter ())
					return;
				if (epson_init (currprefs.prtname, currprefs.parallel_matrix_emulation))
					prtopen = 1;
			}
			for (i = 0; i < prtbufbytes; i++)
				epson_printchar (prtbuf[i]);
		} else {
			if (hPrt == INVALID_HANDLE_VALUE) {
				if (!doflushprinter ())
					return;
				openprinter ();
			}
			if (hPrt != INVALID_HANDLE_VALUE) {
				if (WritePrinter (hPrt, prtbuf, pbyt, &written)) {
					if (written != pbyt)
						write_log (_T("PRINTER: Only wrote %d of %d bytes!\n"), written, pbyt);
				} else {
					write_log (_T("PRINTER: Couldn't write data!\n"));
				}
			}
		}

	}
#endif
	prtbufbytes = 0;
}

void finishjob (void)
{
	flushprtbuf ();
}

void flushprinter (void)
{
	if (!doflushprinter ())
		return;
	flushprtbuf ();
	closeprinter ();
}

void closeprinter (void) {};
void doprinter (uae_u8 val) {}

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

int uaeser_getdatalength (void) { return sizeof (struct uaeserialdata); }
void uaeser_initdata (void *vsd, void *user) {}
int uaeser_query (void *vsd, uae_u16 *status, uae_u32 *pending) { return 0; }
int uaeser_break (void *vsd, int brklen) { return 0; }
int uaeser_setparams (void *vsd, int baud, int rbuffer, int bits, int sbits, int rtscts, int parity, uae_u32 xonxoff) { return 0; }
void uaeser_trigger (void *vsd) {}
int uaeser_write (void *vsd, uae_u8 *data, uae_u32 len) { return 0; }
int uaeser_read (void *vsd, uae_u8 *data, uae_u32 len) { return 0; }
void uaeser_clearbuffers (void *vsd) {}
int uaeser_open (void *vsd, void *user, int unit) { return 0; }
void uaeser_close (void *vsd) {}

static HANDLE hCom = (HANDLE)INVALID_HANDLE_VALUE;
static DCB dcb;
static DWORD fDtrControl = DTR_CONTROL_DISABLE, fRtsControl = RTS_CONTROL_DISABLE;
#if 0
static HANDLE writeevent, readevent;
#endif
#define SERIAL_WRITE_BUFFER 100
#define SERIAL_READ_BUFFER 100
static uae_u8 outputbuffer[SERIAL_WRITE_BUFFER];
static uae_u8 outputbufferout[SERIAL_WRITE_BUFFER];
static uae_u8 inputbuffer[SERIAL_READ_BUFFER];
static int datainoutput;
static int dataininput, dataininputcnt;
#if 0
static OVERLAPPED writeol, readol;
#endif
static int writepending;
static bool breakpending;

#if 0
static WSADATA wsadata;
#endif
static SOCKET serialsocket = INVALID_SOCKET;
static SOCKET serialconn = INVALID_SOCKET;
#if 0
static PADDRINFOW socketinfo;
#endif
static char socketaddr[sizeof(SOCKADDR_INET)];
static BOOL tcpserial;

static bool tcp_is_connected (void)
{
	socklen_t sa_len = sizeof(SOCKADDR_INET);
	if (serialsocket == INVALID_SOCKET)
		return false;
	if (serialconn == INVALID_SOCKET) {
		struct timeval tv;
		fd_set_uae fd;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		fd.fd_array[0] = serialsocket;
		fd.fd_count = 1;
		if (select (1, &fd, NULL, NULL, &tv)) {
			serialconn = accept (serialsocket, (struct sockaddr*)socketaddr, &sa_len);
			if (serialconn != INVALID_SOCKET)
				write_log (_T("SERIAL_TCP: connection accepted\n"));
		}
	}
	return serialconn != INVALID_SOCKET;
}

static void tcp_disconnect (void)
{
	if (serialconn == INVALID_SOCKET)
		return;
	closesocket (serialconn);
	serialconn = INVALID_SOCKET;
	write_log (_T("SERIAL_TCP: disconnect\n"));
}

static void closetcp (void)
{
	if (serialconn != INVALID_SOCKET)
		closesocket (serialconn);
	serialconn = INVALID_SOCKET;
	if (serialsocket != INVALID_SOCKET)
		closesocket (serialsocket);
	serialsocket = INVALID_SOCKET;
#if 0
	if (socketinfo)
		FreeAddrInfoW (socketinfo);
	socketinfo = NULL;
#endif
	WSACleanup ();
}

static int opentcp (const TCHAR *sername)
{
	int err;
	TCHAR *port, *name;
	const TCHAR *p;
	bool waitmode = false;
	const int one = 1;
	const struct linger linger_1s = { 1, 1 };

	if (WSAStartup (MAKEWORD (2, 2), &wsadata)) {
		DWORD lasterror = WSAGetLastError ();
		write_log (_T("SERIAL_TCP: can't open '%s', error %d\n"), sername, lasterror);
		return 0;
	}
	name = my_strdup (sername);
	port = NULL;
	p = _tcschr (sername, ':');
	if (p) {
		name[p - sername] = 0;
		port = my_strdup (p + 1);
		const TCHAR *p2 = _tcschr (port, '/');
		if (p2) {
			port[p2 - port] = 0;
			if (!_tcsicmp (p2 + 1, _T("wait")))
				waitmode = true;
		}
	}
	if (port && port[0] == 0) {
		xfree (port);
		port = NULL;
	}
	if (!port)
		port = 	my_strdup (_T("1234"));

#if 0
	err = GetAddrInfoW (name, port, NULL, &socketinfo);
#else
	err = -1;
#endif
	if (err < 0) {
		write_log (_T("SERIAL_TCP: GetAddrInfoW() failed, %s:%s: %d\n"), name, port, WSAGetLastError ());
		goto end;
	}
#if 0
	serialsocket = socket (socketinfo->ai_family, socketinfo->ai_socktype, socketinfo->ai_protocol);
	if (serialsocket == INVALID_SOCKET) {
		write_log(_T("SERIAL_TCP: socket() failed, %s:%s: %d\n"), name, port, WSAGetLastError ());
		goto end;
	}
	err = bind (serialsocket, socketinfo->ai_addr, socketinfo->ai_addrlen);
	if (err < 0) {
		write_log(_T("SERIAL_TCP: bind() failed, %s:%s: %d\n"), name, port, WSAGetLastError ());
		goto end;
	}
	err = listen (serialsocket, 1);
	if (err < 0) {
		write_log(_T("SERIAL_TCP: listen() failed, %s:%s: %d\n"), name, port, WSAGetLastError ());
		goto end;
	}
	err = setsockopt (serialsocket, SOL_SOCKET, SO_LINGER, (char*)&linger_1s, sizeof linger_1s);
	if (err < 0) {
		write_log(_T("SERIAL_TCP: setsockopt(SO_LINGER) failed, %s:%s: %d\n"), name, port, WSAGetLastError ());
		goto end;
	}
	err = setsockopt (serialsocket, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof one);
	if (err < 0) {
		write_log(_T("SERIAL_TCP: setsockopt(SO_REUSEADDR) failed, %s:%s: %d\n"), name, port, WSAGetLastError ());
		goto end;
	}
#endif

	if (waitmode) {
		while (tcp_is_connected () == false) {
			sleep_millis (1000);
			write_log (_T("SERIAL_TCP: waiting for connect...\n"));
		}
	}

	xfree (port);
	xfree (name);
	tcpserial = TRUE;
	return 1;
end:
	xfree (port);
	xfree (name);
	closetcp ();
	return 0;
}

int openser (const TCHAR *sername)
{
	COMMTIMEOUTS CommTimeOuts;

	if (!_tcsnicmp (sername, _T("TCP://"), 6)) {
		return opentcp (sername + 6);
	}
	if (!_tcsnicmp (sername, _T("TCP:"), 4)) {
		return opentcp (sername + 4);
	}

#if 0
	if (!(readevent = CreateEvent (NULL, TRUE, FALSE, NULL))) {
		write_log (_T("SERIAL: Failed to create r event!\n"));
		return 0;
	}
	readol.hEvent = readevent;

	if (!(writeevent = CreateEvent (NULL, TRUE, FALSE, NULL))) {
		write_log (_T("SERIAL: Failed to create w event!\n"));
		return 0;
	}
	SetEvent (writeevent);
	writeol.hEvent = writeevent;
#endif

	uartbreak = 0;

#if 0
	hCom = CreateFile (sername, GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		write_log (_T("SERIAL: failed to open '%s' err=%d\n"), sername, GetLastError());
		closeser ();
		return 0;
	}

	SetCommMask (hCom, EV_RXFLAG | EV_BREAK);
	SetupComm (hCom, 65536, 128);
	PurgeComm (hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
#endif
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
#if 0
	SetCommTimeouts (hCom, &CommTimeOuts);
#endif

	dcb.DCBlength = sizeof (DCB);
#if 0
	GetCommState (hCom, &dcb);
#endif

	dcb.fBinary = TRUE;
	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.fParity = FALSE;
	dcb.StopBits = currprefs.serial_stopbits;

	dcb.fDsrSensitivity = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = fDtrControl;

	if (currprefs.serial_hwctsrts) {
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	} else {
		dcb.fRtsControl = fRtsControl;
		dcb.fOutxCtsFlow = FALSE;
	}

	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;

	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fAbortOnError = FALSE;

#if 0
	if (SetCommState (hCom, &dcb)) {
		write_log (_T("SERIAL: Using %s CTS/RTS=%d\n"), sername, currprefs.serial_hwctsrts);
		return 1;
	}

	write_log (_T("SERIAL: serial driver didn't accept new parameters\n"));
#endif
	closeser();
	return 0;
}

void closeser (void)
{
	if (tcpserial) {
		closetcp ();
		tcpserial = FALSE;
	}
	if (hCom != INVALID_HANDLE_VALUE)  {
#if 0
		CloseHandle (hCom);
#endif
		hCom = (HANDLE)INVALID_HANDLE_VALUE;
	}
	if (midi_ready) {
		extern uae_u16 serper;
		Midi_Close ();
		//need for camd Midi Stuff(it close midi and reopen it but serial.c think the baudrate
		//is the same and do not open midi), so setting serper to different value helps
		serper = 0x30;
	}
#if 0
	if(writeevent)
		CloseHandle(writeevent);
	writeevent = 0;
	if(readevent)
		CloseHandle(readevent);
	readevent = 0;
#endif
	uartbreak = 0;
}

static void outser (void)
{
	if (datainoutput <= 0)
		return;
#if 0
	DWORD v = WaitForSingleObject (writeevent, 0);
	if (v == WAIT_OBJECT_0) {
		DWORD actual;
		memcpy (outputbufferout, outputbuffer, datainoutput);
		WriteFile (hCom, outputbufferout, datainoutput, &actual, &writeol);
		datainoutput = 0;
	}
#endif
}

void writeser_flush(void)
{
	outser();
}

void writeser (int c)
{
#if 0
	write_log(_T("writeser %04X (buf=%d)\n"), c, datainoutput);
#endif
	if (tcpserial) {
		if (tcp_is_connected ()) {
			char buf[1];
			buf[0] = (char)c;
			if (send (serialconn, buf, 1, 0) != 1) {
				tcp_disconnect ();
			}
		}
	} else if (midi_ready) {
		BYTE outchar = (BYTE)c;
		Midi_Parse (midi_output, &outchar);
	} else {
		if (hCom == INVALID_HANDLE_VALUE || !currprefs.use_serial)
			return;
		if (datainoutput + 1 < sizeof (outputbuffer)) {
			outputbuffer[datainoutput++] = c;
		} else {
			write_log (_T("serial output buffer overflow, data will be lost\n"));
			datainoutput = 0;
		}
		outser ();
	}
}

int checkserwrite (int spaceneeded)
{
	if (hCom == INVALID_HANDLE_VALUE || !currprefs.use_serial)
		return 1;
	if (midi_ready) {
		return 1;
	} else {
		outser ();
		if (datainoutput + spaceneeded >= sizeof (outputbuffer))
			return 0;
	}
	return 1;
}

void flushser(void)
{
	if (!tcpserial && !midi_ready && hCom) {
#if 0
		COMSTAT ComStat;
		DWORD dwErrorFlags;
		ClearCommError(hCom, &dwErrorFlags, &ComStat);
		PurgeComm(hCom, PURGE_RXCLEAR);
#endif
	} else {
		while (readseravail(NULL)) {
			int data;
			if (readser(&data) <= 0)
				break;
		}
	}
}

int readseravail(bool *breakcond)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	if (breakcond)
		*breakcond = false;
	if (tcpserial) {
		if (tcp_is_connected ()) {
			struct timeval tv;
			fd_set_uae fd;
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			fd.fd_array[0] = serialconn;
			fd.fd_count = 1;
			int err = select (1, &fd, NULL, NULL, &tv);
			if (err == SOCKET_ERROR) {
				tcp_disconnect ();
				return 0;
			}
			if (err > 0)
				return 1;
		}
		return 0;
	} else if (midi_ready) {
		if (ismidibyte ())
			return 1;
	} else {
		if (!currprefs.use_serial)
			return 0;
		if (dataininput > dataininputcnt)
			return 1;
		if (hCom != INVALID_HANDLE_VALUE)  {
#if 0
			ClearCommError (hCom, &dwErrorFlags, &ComStat);
#endif
			if (breakcond && ((dwErrorFlags & CE_BREAK) || breakpending)) {
				*breakcond = true;
				breakpending = false;
			}
			if (ComStat.cbInQue > 0)
				return ComStat.cbInQue;
		}
	}
	return 0;
}

int readser (int *buffer)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	DWORD actual;

	if (tcpserial) {
		if (tcp_is_connected ()) {
			char buf[1];
			buf[0] = 0;
			int err = recv (serialconn, buf, 1, 0);
			if (err == 1) {
				*buffer = buf[0];
				//write_log(_T(" %02X "), buf[0]);
				return 1;
			} else {
				tcp_disconnect ();
			}
		}
		return 0;
	} else if (midi_ready) {
		*buffer = getmidibyte ();
		if (*buffer < 0)
			return 0;
		return 1;
	} else {
		if (!currprefs.use_serial)
			return 0;
		if (dataininput > dataininputcnt) {
			*buffer = inputbuffer[dataininputcnt++];
			return 1;
		}
		dataininput = 0;
		dataininputcnt = 0;
		if (hCom != INVALID_HANDLE_VALUE)  {
			/* only try to read number of bytes in queue */
#if 0
			ClearCommError (hCom, &dwErrorFlags, &ComStat);
#endif
			if (dwErrorFlags & CE_BREAK)
				breakpending = true;
			if (ComStat.cbInQue)  {
				int len = ComStat.cbInQue;
				if (len > sizeof (inputbuffer))
					len = sizeof (inputbuffer);
#if 0
				if (!ReadFile (hCom, inputbuffer, len, &actual, &readol))  {
					if (GetLastError() == ERROR_IO_PENDING)
						WaitForSingleObject (&readol, INFINITE);
					else
						return 0;
				}
#endif
				dataininput = actual;
				dataininputcnt = 0;
				if (actual == 0)
					return 0;
				return readser (buffer);
			}
		}
	}
	return 0;
}

void serialuartbreak (int v)
{
	if (hCom == INVALID_HANDLE_VALUE || !currprefs.use_serial)
		return;

#if 0
	if (v)
		EscapeCommFunction (hCom, SETBREAK);
	else
		EscapeCommFunction (hCom, CLRBREAK);
#endif
}

void getserstat (int *pstatus)
{
	DWORD stat;
	int status = 0;

	*pstatus = 0;
	if (hCom == INVALID_HANDLE_VALUE || !currprefs.use_serial)
		return;

#if 0
	GetCommModemStatus (hCom, &stat);
#endif
	if (stat & MS_CTS_ON)
		status |= TIOCM_CTS;
	if (stat & MS_RLSD_ON)
		status |= TIOCM_CAR;
	if (stat & MS_DSR_ON)
		status |= TIOCM_DSR;
	if (stat & MS_RING_ON)
		status |= TIOCM_RI;
	*pstatus = status;
}

void setserstat (int mask, int onoff)
{
	if (mask & TIOCM_DTR) {
#if 0
		if (currprefs.use_serial && hCom != INVALID_HANDLE_VALUE) {
			EscapeCommFunction(hCom, onoff ? SETDTR : CLRDTR);
		}
#endif
		fDtrControl = onoff ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;
	}
	if (!currprefs.serial_hwctsrts) {
		if (mask & TIOCM_RTS) {
#if 0
			if (currprefs.use_serial && hCom != INVALID_HANDLE_VALUE) {
				EscapeCommFunction(hCom, onoff ? SETRTS : CLRRTS);
			}
#endif
			fRtsControl = onoff ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
		}
	}
}

int setbaud (long baud)
{
	if(baud == 31400 && currprefs.win32_midioutdev >= -1) {
		/* MIDI baud-rate */
		if (!midi_ready) {
			if (Midi_Open())
				write_log (_T("Midi enabled\n"));
		}
		return 1;
	} else {
		if (midi_ready) {
			Midi_Close();
		}
		if (!currprefs.use_serial)
			return 1;
		if (hCom != INVALID_HANDLE_VALUE)  {
			dcb.BaudRate = baud;
			if (!currprefs.serial_hwctsrts) {
				dcb.fRtsControl = fRtsControl;
			} else {
				dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			}
			dcb.fDtrControl = fDtrControl;
			write_log(_T("SERIAL: baud rate %d. DTR=%d RTS=%d\n"), baud, dcb.fDtrControl, dcb.fRtsControl);
#if 0
			if (!SetCommState (hCom, &dcb)) {
				write_log (_T("SERIAL: Error setting baud rate %d!\n"), baud);
				return 0;
			}
#endif
		}
	}
	return 1;
}

void initparallel (void)
{
#if 0
	if (uae_boot_rom_type) {
		uaecptr a = here (); //this install the ahisound
		org (rtarea_base + 0xFFC0);
		calltrap (deftrapres (ahi_demux, 0, _T("ahi_winuae")));
		dw (RTS);
		org (a);
		init_ahi_v2 ();
	}
#endif
}

int flashscreen;

void doflashscreen (void)
{
#if 0
	flashscreen = 10;
	init_colors(0);
	picasso_refresh(0);
	reset_drawing ();
	//flush_screen (gfxvidinfo.outbuffer, 0, 0);
#endif
}

void hsyncstuff (void)
	//only generate Interrupts when
	//writebuffer is complete flushed
	//check state of lwin rwin
{
	static int keycheck = 0;

#ifdef AHI
	{ //begin ahi_sound
		static int count;
		if (ahi_on) {
			count++;
			//15625/count freebuffer check
			if(count > ahi_pollrate) {
				ahi_updatesound (1);
				count = 0;
			}
		}
	} //end ahi_sound
#endif
#ifdef PARALLEL_PORT
	keycheck++;
	if(keycheck >= 1000)
	{
#if 0
		if (prtopen)
			flushprtbuf ();
		{
			if (flashscreen > 0) {
				flashscreen--;
				if (flashscreen == 0) {
					init_colors(0);
					reset_drawing ();
					picasso_refresh(0);
					//flush_screen (gfxvidinfo.outbuffer, 0, 0);
				}
			}
		}
#endif
		keycheck = 0;
	}
	if (currprefs.parallel_autoflush_time && !currprefs.parallel_postscript_detection) {
		parflush++;
		if (parflush / ((currprefs.ntscmode ? MAXVPOS_NTSC : MAXVPOS_PAL) * MAXHPOS_PAL / maxhpos) >= currprefs.parallel_autoflush_time * 50) {
			flushprinter ();
			parflush = 0;
		}
	}
#endif
}

#if 0
const static GUID GUID_DEVINTERFACE_PARALLEL = {0x97F76EF0,0xF883,0x11D0,
{0xAF,0x1F,0x00,0x00,0xF8,0x00,0x84,0x5C}};

static const GUID serportsguids[] =
{
	GUID_DEVINTERFACE_COMPORT,
	// GUID_DEVINTERFACE_MODEM
	{ 0x2C7089AA, 0x2E0E, 0x11D1, { 0xB1, 0x14, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE4} }
};
static const GUID parportsguids[] =
{
	GUID_DEVINTERFACE_PARALLEL
};
#endif

#if 0
static int enumports_2 (struct serparportinfo **pi, int cnt, bool parport)
{
	// Create a device information set that will be the container for
	// the device interfaces.
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;
	SP_DEVICE_INTERFACE_DATA ifcData;
	DWORD dwDetDataSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA) + 256 * sizeof (TCHAR);
	const GUID *guids = parport ? parportsguids : serportsguids;
	int guidcnt = parport ? sizeof(parportsguids)/sizeof(parportsguids[0]) : sizeof(serportsguids)/sizeof(serportsguids[0]);

	for (int guididx = 0; guididx < guidcnt; guididx++) {
		hDevInfo = SetupDiGetClassDevs (&guids[guididx], NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if(hDevInfo == INVALID_HANDLE_VALUE)
			continue;
		// Enumerate the serial ports
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)xmalloc (uae_u8, dwDetDataSize);
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
		BOOL bOk = TRUE;
		for (int ii = 0; bOk; ii++) {
			bOk = SetupDiEnumDeviceInterfaces (hDevInfo, NULL, &guids[guididx], ii, &ifcData);
			if (bOk) {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = { sizeof (SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail (hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk) {
					// Got a path to the device. Try to get some more info.
					TCHAR fname[256];
					TCHAR desc[256];
					BOOL bSuccess = SetupDiGetDeviceRegistryProperty (
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(PBYTE)fname, sizeof (fname), NULL);
					bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty (
						hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
						(PBYTE)desc, sizeof (desc), NULL);
					if (bSuccess && cnt < MAX_SERPAR_PORTS) {
						TCHAR *p;
						pi[cnt] = xcalloc (struct serparportinfo, 1);
						pi[cnt]->dev = my_strdup (pDetData->DevicePath);
						pi[cnt]->name = my_strdup (fname);
						p = _tcsstr (fname, parport ? _T("(LPT") : _T("(COM"));
						if (p && (p[5] == ')' || p[6] == ')')) {
							pi[cnt]->cfgname = xmalloc (TCHAR, 100);
							if (isdigit(p[5]))
								_stprintf (pi[cnt]->cfgname, parport ? _T("LPT%c%c") : _T("COM%c%c"), p[4], p[5]);
							else
								_stprintf (pi[cnt]->cfgname, parport ? _T("LPT%c") : _T("COM%c"), p[4]);
						} else {
							pi[cnt]->cfgname = my_strdup (pDetData->DevicePath);
						}
						write_log (_T("%s: '%s' = '%s' = '%s'\n"), parport ? _T("PARPORT") : _T("SERPORT"), pi[cnt]->name, pi[cnt]->cfgname, pi[cnt]->dev);
						cnt++;
					}
				} else {
					write_log (_T("SetupDiGetDeviceInterfaceDetail failed, err=%d"), GetLastError ());
					break;
				}
			} else {
				DWORD err = GetLastError ();
				if (err != ERROR_NO_MORE_ITEMS) {
					write_log (_T("SetupDiEnumDeviceInterfaces failed, err=%d"), err);
					break;
				}
			}
		}
		xfree(pDetData);
		if (hDevInfo != INVALID_HANDLE_VALUE)
			SetupDiDestroyDeviceInfoList (hDevInfo);
	}
	return cnt;
}

static struct serparportinfo *parports[MAX_SERPAR_PORTS];

int enumserialports (void)
{
	int cnt, i, j;
	TCHAR name[256];
	DWORD size = sizeof (COMMCONFIG);
	TCHAR devname[1000];

	write_log (_T("Serial port enumeration..\n"));
	cnt = 0;

#ifdef SERIAL_ENET
	comports[cnt].dev = my_strdup (_T("ENET:H"));
	comports[cnt].cfgname = my_strdup (comports[0].dev);
	comports[cnt].name = my_strdup (_T("NET (host)"));
	cnt++;
	comports[cnt].dev = my_strdup (_T("ENET:L"));
	comports[cnt].cfgname = my_strdup (comports[1].dev);
	comports[cnt].name = my_strdup (_T("NET (client)"));
	cnt++;
#endif

	cnt = enumports_2 (comports, cnt, false);
	j = 0;
	for (i = 0; i < 10; i++) {
		_stprintf (name, _T("COM%d"), i);
		if (!QueryDosDevice (name, devname, sizeof devname / sizeof (TCHAR)))
			continue;
		for(j = 0; j < cnt; j++) {
			if (!_tcscmp (comports[j]->cfgname, name))
				break;
		}
		if (j == cnt) {
			if (cnt >= MAX_SERPAR_PORTS)
				break;
			comports[j] = xcalloc(struct serparportinfo, 1);
			comports[j]->dev = xmalloc (TCHAR, 100);
			_stprintf (comports[cnt]->dev, _T("\\.\\\\%s"), name);
			comports[j]->cfgname = my_strdup (name);
			comports[j]->name = my_strdup (name);
			write_log (_T("SERPORT: %d:'%s' = '%s' (%s)\n"), cnt, comports[j]->name, comports[j]->dev, devname);
			cnt++;
			j++;
		}
	}

	for (i = 0; i < cnt; i++) {
		for (j = i + 1; j < cnt; j++) {
			if (_tcsicmp (comports[i]->name, comports[j]->name) > 0) {
				struct serparportinfo *spi;
				spi = comports[i];
				comports[i] = comports[j];
				comports[j] = spi;
			}
		}
	}


	if (cnt < MAX_SERPAR_PORTS) {
		comports[cnt] = xcalloc(struct serparportinfo, 1);
		comports[cnt]->dev = my_strdup (SERIAL_INTERNAL);
		comports[cnt]->cfgname = my_strdup (comports[cnt]->dev);
		comports[cnt]->name = my_strdup (_T("WinUAE inter-process serial port"));
		cnt++;
	}

	if (cnt < MAX_SERPAR_PORTS) {
		comports[cnt] = xcalloc(struct serparportinfo, 1);
		comports[cnt]->dev = my_strdup (_T("TCP://0.0.0.0:1234"));
		comports[cnt]->cfgname = my_strdup (comports[cnt]->dev);
		comports[cnt]->name = my_strdup (comports[cnt]->dev);
		cnt++;
	}
	if (cnt < MAX_SERPAR_PORTS) {
		comports[cnt] = xcalloc(struct serparportinfo, 1);
		comports[cnt]->dev = my_strdup (_T("TCP://0.0.0.0:1234/wait"));
		comports[cnt]->cfgname = my_strdup (comports[cnt]->dev);
		comports[cnt]->name = my_strdup (comports[cnt]->dev);
		cnt++;
	}

	write_log (_T("Parallel port enumeration..\n"));
	enumports_2 (parports, 0, true);
	write_log (_T("Port enumeration end\n"));

	return cnt;
}
#endif

#if 0
int enummidiports (void)
{
	MIDIOUTCAPS midiOutCaps;
	MIDIINCAPS midiInCaps;
	int i, j, num, total;
	int innum, outnum;
	
	outnum = midiOutGetNumDevs();
	innum = midiInGetNumDevs();
	write_log (_T("MIDI port enumeration.. IN=%d OUT=%d\n"), innum, outnum);

	num = outnum;
	for (i = 0; i < num + 1 && i < MAX_MIDI_PORTS - 1; i++) {
		MMRESULT r = midiOutGetDevCaps ((UINT)(i - 1), &midiOutCaps, sizeof (midiOutCaps));
		if (r != MMSYSERR_NOERROR) {
			num = i;
			break;
		}
		midioutportinfo[i] = xcalloc (struct midiportinfo, 1);
		midioutportinfo[i]->name = my_strdup (midiOutCaps.szPname);
		midioutportinfo[i]->devid = i - 1;
		write_log (_T("MIDI OUT: %d:'%s' (%d/%d)\n"), midioutportinfo[i]->devid, midioutportinfo[i]->name, midiOutCaps.wMid, midiOutCaps.wPid);
	}
	total = num + 1;
	for (i = 1; i < num + 1; i++) {
		for (j = i + 1; j < num + 1; j++) {
			if (_tcsicmp (midioutportinfo[i]->name, midioutportinfo[j]->name) > 0) {
				struct midiportinfo *mi;
				mi = midioutportinfo[i];
				midioutportinfo[i] = midioutportinfo[j];
				midioutportinfo[j] = mi;
			}
		}
	}
	num = innum;
	for (i = 0; i < num && i < MAX_MIDI_PORTS - 1; i++) {
		if (midiInGetDevCaps (i, &midiInCaps, sizeof (midiInCaps)) != MMSYSERR_NOERROR) {
			num = i;
			break;
		}
		midiinportinfo[i] = xcalloc (struct midiportinfo, 1);
		midiinportinfo[i]->name = my_strdup (midiInCaps.szPname);
		midiinportinfo[i]->devid = i;
		write_log (_T("MIDI IN: %d:'%s' (%d/%d)\n"), midiinportinfo[i]->devid, midiinportinfo[i]->name, midiInCaps.wMid, midiInCaps.wPid);
	}
	total += num;
	for (i = 0; i < num; i++) {
		for (j = i + 1; j < num; j++) {
			if (_tcsicmp (midiinportinfo[i]->name, midiinportinfo[j]->name) > 0) {
				struct midiportinfo *mi;
				mi = midiinportinfo[i];
				midiinportinfo[i] = midiinportinfo[j];
				midiinportinfo[j] = mi;
			}
		}
	}

	write_log (_T("MIDI port enumeration end\n"));

	return total;
}
#endif

void sernametodev (TCHAR *sername)
{
	int i;

	for (i = 0; i < MAX_SERPAR_PORTS && comports[i]; i++) {
		if (!_tcscmp (sername, comports[i]->cfgname)) {
			_tcscpy (sername, comports[i]->dev);
			return;
		}
	}
	if (!_tcsncmp (sername, _T("TCP:"), 4))
		return;
	sername[0] = 0;
}

void serdevtoname (TCHAR *sername)
{
	int i;
	if (!_tcsncmp (sername, _T("TCP:"), 4))
		return;
	for (i = 0; i < MAX_SERPAR_PORTS && comports[i]; i++) {
		if (!_tcscmp (sername, comports[i]->dev)) {
			_tcscpy (sername, comports[i]->cfgname);
			return;
		}
	}
	sername[0] = 0;
}
