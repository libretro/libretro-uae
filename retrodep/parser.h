/*
 * UAE - The Un*x Amiga Emulator
 *
 * Not a parser, but parallel and serial emulation for Win32
 *
 * Copyright 1997 Mathias Ortmann
 * Copyright 1998-1999 Brian King
 */

#define PRTBUFSIZE 65536

int setbaud(long baud );
void getserstat(int *status);
void setserstat(int mask, int onoff);
int readser(int *buffer);
int readseravail(bool*);
void flushser(void);
void writeser(int c);
void writeser_flush(void);
int openser(const TCHAR *sername);
void closeser(void);
void doserout(void);
void closeprinter(void);
void flushprinter(void);
int checkserwrite(int spaceneeded);
void serialuartbreak(int);

void hsyncstuff(void);

void shmem_serial_delete(void);
bool shmem_serial_create(void);
int shmem_serial_state(void);

#define SERIAL_INTERNAL _T("INTERNAL_SERIAL")
#define SERIAL_LOOPBACK _T("LOOPBACK_SERIAL")

#define TIOCM_CAR 1
#define TIOCM_DSR 2
#define TIOCM_RI 4
#define TIOCM_DTR 8
#define TIOCM_RTS 16
#define TIOCM_CTS 32

extern void unload_ghostscript (void);
extern int load_ghostscript (void);

#define MAX_MIDI_PORTS 100
struct midiportinfo
{
	TCHAR *name;
	unsigned int devid;
};
extern struct midiportinfo *midiinportinfo[MAX_MIDI_PORTS];
extern struct midiportinfo *midioutportinfo[MAX_MIDI_PORTS];

#define MAX_SERPAR_PORTS 100
struct serparportinfo
{
    TCHAR *dev;
    TCHAR *cfgname;
    TCHAR *name;
};
extern struct serparportinfo *comports[MAX_SERPAR_PORTS];
extern struct serparportinfo *parports[MAX_SERPAR_PORTS];

extern int enumserialports (void);
extern int enummidiports (void);
extern void sernametodev (TCHAR*);
extern void serdevtoname (TCHAR*);

extern void epson_printchar (uae_u8 c);
extern int epson_init (const TCHAR *printername, int pins);
extern void epson_close (void);

#define PARALLEL_MATRIX_TEXT 1
#define PARALLEL_MATRIX_EPSON 2
#define PARALLEL_MATRIX_EPSON9 2
#define PARALLEL_MATRIX_EPSON24 3
#define PARALLEL_MATRIX_EPSON48 4



#define SOCKET_ERROR   (-1)
#define SOCKADDR_INET  NULL
#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif

#define SO_REUSEADDR 0x0004
#define SO_LINGER 0x0080
#define SOL_SOCKET 0xffff

typedef unsigned short SOCKET;

struct linger {
	uae_u16	l_onoff;
	uae_u16	l_linger;
};

#undef socklen_t
#define socklen_t size_t

#ifndef __WIN32
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE NULL
#endif

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int CLOCK;

#define NOPARITY 0
#define CE_BREAK 0x10

typedef struct addrinfoW {
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  size_t ai_addrlen;
  char *ai_canonname;
  struct sockaddr *ai_addr;
  struct addrinfoW *ai_next;
} ADDRINFOW,*PADDRINFOW;

  typedef struct _COMSTAT {
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
  } COMSTAT,*LPCOMSTAT;

#define DTR_CONTROL_DISABLE 0x0
#define DTR_CONTROL_ENABLE 0x1
#define DTR_CONTROL_HANDSHAKE 0x2

#define RTS_CONTROL_DISABLE 0x0
#define RTS_CONTROL_ENABLE 0x1
#define RTS_CONTROL_HANDSHAKE 0x2
#define RTS_CONTROL_TOGGLE 0x3

  typedef struct _DCB {
    DWORD DCBlength;
    DWORD BaudRate;
    DWORD fBinary: 1;
    DWORD fParity: 1;
    DWORD fOutxCtsFlow:1;
    DWORD fOutxDsrFlow:1;
    DWORD fDtrControl:2;
    DWORD fDsrSensitivity:1;
    DWORD fTXContinueOnXoff: 1;
    DWORD fOutX: 1;
    DWORD fInX: 1;
    DWORD fErrorChar: 1;
    DWORD fNull: 1;
    DWORD fRtsControl:2;
    DWORD fAbortOnError:1;
    DWORD fDummy2:17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
    WORD wReserved1;
  } DCB,*LPDCB;

  typedef struct _COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
  } COMMTIMEOUTS,*LPCOMMTIMEOUTS;

#define MS_CTS_ON ((DWORD)0x10)
#define MS_DSR_ON ((DWORD)0x20)
#define MS_RING_ON ((DWORD)0x40)
#define MS_RLSD_ON ((DWORD)0x80)
#endif
