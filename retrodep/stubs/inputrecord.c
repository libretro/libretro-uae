/*
* UAE - The Un*x Amiga Emulator
*
* Input record/playback
*
* Copyright 2010 Toni Wilen
*
*/

#define INPUTRECORD_DEBUG 0
#define ENABLE_DEBUGGER 0

#define HEADERSIZE 12

#include "sysconfig.h"
#include "sysdeps.h"

#if 0
#include "options.h"
#include "inputrecord.h"
#include "zfile.h"
#include "custom.h"
#include "savestate.h"
#include "cia.h"
#include "events.h"
#include "uae.h"
#include "disk.h"
#include "misc.h"
#include "fsdb.h"

#if INPUTRECORD_DEBUG > 0
#include "memory_uae.h"
#include "newcpu.h"
#endif
#endif

int inputrecord_debug = 0;
#if 0
extern int inputdevice_logging;

#define INPREC_BUFFER_SIZE 10000

static uae_u8 *inprec_buffer, *inprec_p;
static struct zfile *inprec_zf;
static int inprec_size;
#endif
int input_record = 0;
int input_play = 0;
#if 0
static uae_u8 *inprec_plast, *inprec_plastptr;
static int header_end, header_end2;
static int replaypos;
static int lasthsync, endhsync;
static TCHAR inprec_path[MAX_DPATH];
static uae_u32 seed;
static uae_u32 lastcycle;
static uae_u32 cycleoffset;

static uae_u32 pcs[16];
static uae_u32 pcs2[16];
extern void activate_debugger (void);
static int warned;

/* external prototypes */
extern void refreshtitle (void);
extern uae_u32 uaesrand (uae_u32 seed);
extern uae_u32 uaerandgetseed (void);



static void setlasthsync (void)
{
}

static void flush (void)
{
}

void inprec_ru8 (uae_u8 v)
{
}

void inprec_ru16 (uae_u16 v)
{
}

void inprec_ru32 (uae_u32 v)
{
}

static void inprec_rstr (const TCHAR *src)
{
}

static bool inprec_rstart (uae_u8 type)
{
	return false;
}

static void inprec_rend (void)
{
}

static bool inprec_realtime (bool stopstart)
{
	return true;
}

static int inprec_pstart (uae_u8 type)
{
	return 0;
}

static void inprec_pend (void)
{
}

static uae_u8 inprec_pu8 (void)
{
	return 0;
}
static uae_u16 inprec_pu16 (void)
{
	return 0;
}
static uae_s16 inprec_ps16 (void)
{
	return 0;
}
static uae_u32 inprec_pu32 (void)
{
	return 0;
}
static int inprec_pstr (TCHAR *dst)
{
	return 0;
}

static void findlast (void)
{
}
#endif

int inprec_open (const TCHAR *fname, const TCHAR *statefilename)
{
	return 0;
}

void inprec_startup (void)
{
}

bool inprec_prepare_record (const TCHAR *statefilename)
{
	return true;
}


void inprec_close (bool clear)
{
}

#if 0
static void setwriteprotect (const TCHAR *fname, bool readonly)
{
}
#endif

void inprec_playdiskchange (void)
{
}

bool inprec_playevent (int *nr, int *state, int *max, int *autofire)
{
	return false;
}

void inprec_recorddebug_cia (uae_u32 v1, uae_u32 v2, uae_u32 v3)
{
}
void inprec_playdebug_cia (uae_u32 v1, uae_u32 v2, uae_u32 v3)
{
}

void inprec_recorddebug_cpu (int mode)
{
}
void inprec_playdebug_cpu (int mode)
{
}

void inprec_recorddebug (uae_u32 val)
{
}
void inprec_playdebug (uae_u32 val)
{
}


void inprec_recordevent (int nr, int state, int max, int autofire)
{
}

void inprec_recorddiskchange (int nr, const TCHAR *fname, bool writeprotected)
{
}

int inprec_getposition (void)
{
	return -1;
}

// normal play to re-record
void inprec_playtorecord (void)
{
}

void inprec_setposition (int offset, int replaycounter)
{
}
#if 0
static void savelog (const TCHAR *path, const TCHAR *file)
{
}

static int savedisk (const TCHAR *path, const TCHAR *file, uae_u8 *data, uae_u8 *outdata)
{
	return 0;
}
#endif
void inprec_save (const TCHAR *filename, const TCHAR *statefilename)
{
}

bool inprec_realtimev (void)
{
	return false;
}

void inprec_getstatus (TCHAR *title)
{
}
