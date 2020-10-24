/*
 * PUAE - The Un*x Amiga Emulator
 *
 * A collection of ugly and random stuff brought in from Win32
 * which desparately needs to be tidied up
 *
 * Copyright 2004 Richard Drummond
 * Copyright 2010-2013 Mustafa TUFAN
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "misc.h"
#include "cfgfile.h"
#include "memory_uae.h"
#include "custom.h"
#include "newcpu.h"
#include "events.h"
#include "uae.h"
#include "autoconf.h"
#include "traps.h"
#include "enforcer.h"
#include "picasso96.h"
#include "driveclick.h"
#include "inputdevice.h"
#include "keymap/keymap.h"
#include "keyboard.h"
#include <stdarg.h>
#include "clipboard.h"
#include "fsdb.h"
#include "debug.h"
#include "hrtimer.h"
#include "sleep.h"
#include "zfile.h"

// this is handled by the graphics drivers and set up in picasso96.c
#if defined(PICASSO96)
extern int screen_is_picasso;
#else
static int screen_is_picasso = 0;
#endif

uae_u32 redc[3 * 256], grec[3 * 256], bluc[3 * 256];

#define VBLANKTH_KILL 0
#define VBLANKTH_CALIBRATE 1
#define VBLANKTH_IDLE 2
#define VBLANKTH_ACTIVE_WAIT 3
#define VBLANKTH_ACTIVE 4
#define VBLANKTH_ACTIVE_START 5
#define VBLANKTH_ACTIVE_SKIPFRAME 6
#define VBLANKTH_ACTIVE_SKIPFRAME2 7

static volatile frame_time_t vblank_prev_time;

#ifndef __LIBRETRO__
struct winuae_currentmode {
	unsigned int flags;
	int native_width, native_height, native_depth, pitch;
	int current_width, current_height, current_depth;
	int amiga_width, amiga_height;
	int frequency;
	int initdone;
	int fullfill;
	int vsync;
};

static struct winuae_currentmode currentmodestruct;
static struct winuae_currentmode *currentmode = &currentmodestruct;
#endif

#ifndef _WIN32
typedef struct {
	WORD  dmSize;
	WORD  dmDriverExtra;
	DWORD dmFields;
	DWORD dmBitsPerPel;
	DWORD dmPelsWidth;
	DWORD dmPelsHeight;
	DWORD dmDisplayFlags;
	DWORD dmDisplayFrequency;
} DEVMODE;
#endif

/* internal prototypes */
void setmouseactivexy (int x, int y, int dir);
int get_guid_target (uae_u8 *out);
uae_u8 *save_log (int bootlog, int *len);
void refreshtitle (void);
int scan_roms (int show);
void setid (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt);
void setid_af (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt, int af);
void fetch_path (TCHAR *name, TCHAR *out, int size);
void fetch_screenshotpath (TCHAR *out, int size);
struct MultiDisplay *getdisplay (struct uae_prefs *p);
void addmode (struct MultiDisplay *md, DEVMODE *dm, int rawmode);
void updatedisplayarea (void);

/* external prototypes */
extern void setmaintitle(void);
extern int isvsync_chipset (void);
extern int isvsync_rtg (void);


void getgfxoffset (int *dxp, int *dyp, int *mxp, int *myp)
{
	*dxp = 0;
	*dyp = 0;
	*mxp = 0;
	*myp = 0;
}

int vsync_switchmode (int hz)
{
#ifndef __LIBRETRO__
    static struct PicassoResolution *oldmode;
    static int oldhz;
	int w = currentmode->native_width;
	int h = currentmode->native_height;
	int d = currentmode->native_depth / 8;
        struct MultiDisplay *md = getdisplay (&currprefs);
	struct PicassoResolution *found;
	int newh, i, cnt;

    newh = h * (currprefs.ntscmode ? 60 : 50) / hz;

	found = NULL;
    for (cnt = 0; cnt <= abs (newh - h) + 1 && !found; cnt++) {
            for (i = 0; md->DisplayModes[i].depth >= 0 && !found; i++) {
                    struct PicassoResolution *r = &md->DisplayModes[i];
                    if ( (r->res.width == (uae_u32)w)
					  && ( (r->res.height == (uae_u32)(newh + cnt))
						|| (r->res.height == (uae_u32)(newh - cnt)) )
					  && (r->depth == d) ) {
                            int j = 0;
                            for ( ; r->refresh[j] > 0; j++) {
                                    if (r->refresh[j] == hz || r->refresh[j] == hz * 2) {
                                            found = r;
                                            hz = r->refresh[j];
                                            break;
                                    }
                            }
                    }
            }
    }
    if (found == oldmode && hz == oldhz)
            return true;
    oldmode = found;
    oldhz = hz;
    if (!found) {
            changed_prefs.gfx_apmode[0].gfx_vsync = 0;
            if (currprefs.gfx_apmode[0].gfx_vsync != changed_prefs.gfx_apmode[0].gfx_vsync) {
                    config_changed = 1;
            }
            write_log (_T("refresh rate changed to %d but no matching screenmode found, vsync disabled\n"), hz);
            return false;
    } else {
            newh = found->res.height;
            changed_prefs.gfx_size_fs.height = newh;
            changed_prefs.gfx_apmode[0].gfx_refreshrate = hz;
            if (changed_prefs.gfx_size_fs.height != currprefs.gfx_size_fs.height ||
                    changed_prefs.gfx_apmode[0].gfx_refreshrate != currprefs.gfx_apmode[0].gfx_refreshrate) {
                    write_log (_T("refresh rate changed to %d, new screenmode %dx%d\n"), hz, w, newh);
                    config_changed = 1;
            }
            return true;
    } 
#else
    return false;
#endif
}

int extraframewait = 0;

void sleep_millis_main (int ms)
{
	uae_msleep (ms);
}

void target_restart (void)
{
}

void driveclick_fdrawcmd_close(int drive){}
void driveclick_fdrawcmd_detect(void){}
void driveclick_fdrawcmd_seek(int drive, int cyl){}
void driveclick_fdrawcmd_motor (int drive, int running){}
void driveclick_fdrawcmd_vsync(void){}

uae_u32 emulib_target_getcpurate (uae_u32 v, uae_u32 *low)
{
	return 0;
}

void setmouseactivexy (int x, int y, int dir)
{
}

void setmouseactive (int active)
{
}

// clipboard
static uaecptr clipboard_data;
static int signaling, initialized;

void amiga_clipboard_die (void)
{
	signaling = 0;
	write_log ("clipboard not initialized\n");
}

void amiga_clipboard_init (void)
{
	signaling = 0;
	write_log ("clipboard initialized\n");
	initialized = 1;
}

void amiga_clipboard_task_start (uaecptr data)
{
	clipboard_data = data;
	signaling = 1;
	write_log ("clipboard task init: %08x\n", clipboard_data);
}

uaecptr amiga_clipboard_proc_start (void)
{
	write_log ("clipboard process init: %08x\n", clipboard_data);
	signaling = 1;
	return clipboard_data;
}

void amiga_clipboard_got_data (uaecptr data, uae_u32 size, uae_u32 actual)
{
	if (!initialized) {
		write_log ("clipboard: got_data() before initialized!?\n");
		return;
	}
}

// win32
int get_guid_target (uae_u8 *out)
{
	unsigned Data1, Data2, Data3, Data4;

	srand(time(NULL));
	Data1 = rand();
	Data2 = ((rand() & 0x0fff) | 0x4000);
	Data3 = rand() % 0x3fff + 0x8000;
	Data4 = rand();

	out[0] = Data1 >> 24;
	out[1] = Data1 >> 16;
	out[2] = Data1 >>  8;
	out[3] = Data1 >>  0;
	out[4] = Data2 >>  8;
	out[5] = Data2 >>  0;
	out[6] = Data3 >>  8;
	out[7] = Data3 >>  0;
	memcpy (out + 8, (void*)(size_t)Data4, 8);
	return 1;
}

void machdep_free (void)
{
}

void target_run (void)
{
}

// dinput
int input_get_default_keyboard (int i)
{
	if (i == 0)
		return 1;
	return 0;
}


// win32gui
static int qs_override;

int target_cfgfile_load (struct uae_prefs *p, const TCHAR *filename, int type, int isdefault)
{
	int v, i, type2;
	int ct, ct2 = 0;//, size;
	char tmp1[MAX_DPATH], tmp2[MAX_DPATH];
	char fname[MAX_DPATH];

	_tcscpy (fname, filename);
	if (!zfile_exists (fname)) {
		fetch_configurationpath (fname, sizeof (fname) / sizeof (TCHAR));
		if (_tcsncmp (fname, filename, _tcslen (fname)))
			_tcscat (fname, filename);
		else
			_tcscpy (fname, filename);
	}

	if (!isdefault)
		qs_override = 1;
	if (type < 0) {
		type = 0;
		cfgfile_get_description (fname, NULL, NULL, NULL, &type);
	}
	if (type == 0 || type == 1) {
		discard_prefs (p, 0);
	}
	type2 = type;
	if (type == 0) {
		default_prefs (p, type);
	}
		
	//regqueryint (NULL, "ConfigFile_NoAuto", &ct2);
	v = cfgfile_load (p, fname, &type2, ct2, isdefault ? 0 : 1);
	if (!v)
		return v;
	if (type > 0)
		return v;
	for (i = 1; i <= 2; i++) {
		if (type != i) {
			// size = sizeof (ct);
			ct = 0;
			//regqueryint (NULL, configreg2[i], &ct);
			if (ct && ((i == 1 && p->config_hardware_path[0] == 0) || (i == 2 && p->config_host_path[0] == 0) || ct2)) {
				// size = sizeof (tmp1) / sizeof (TCHAR);
				//regquerystr (NULL, configreg[i], tmp1, &size);
				fetch_path ("ConfigurationPath", tmp2, sizeof (tmp2) / sizeof (TCHAR));
				_tcscat (tmp2, tmp1);
				v = i;
				cfgfile_load (p, tmp2, &v, 1, 0);
			}
		}
	}
	v = 1;
	return v;
}

void stripslashes (TCHAR *p)
{
	while (_tcslen (p) > 0 && (p[_tcslen (p) - 1] == '\\' || p[_tcslen (p) - 1] == '/'))
		p[_tcslen (p) - 1] = 0;
}

void fixtrailing (TCHAR *p)
{
	if (_tcslen(p) == 0)
		return;
	if (p[_tcslen(p) - 1] == '/' || p[_tcslen(p) - 1] == '\\')
		return;
	_tcscat(p, "\\");
}

void getpathpart (TCHAR *outpath, int size, const TCHAR *inpath)
{
	_tcscpy (outpath, inpath);
	TCHAR *p = _tcsrchr (outpath, '\\');
	if (p)
		p[0] = 0;
	fixtrailing (outpath);
}

void getfilepart (TCHAR *out, int size, const TCHAR *path)
{
	out[0] = 0;
	const TCHAR *p = _tcsrchr (path, '\\');
	if (p)
		_tcscpy (out, p + 1);
	else
		_tcscpy (out, path);
}

void refreshtitle (void)
{
	if (isfullscreen () == 0)
		setmaintitle ();
}

int scan_roms (int show)
{
	return 0;
}

// writelog
TCHAR* buf_out (TCHAR *buffer, int *bufsize, const TCHAR *format, ...)
{
	/// REMOVEME: unused: int count;
	va_list parms;
	va_start (parms, format);

	if (buffer == NULL)
		return 0;
	/** REMOVEME: unused: count = **/
	vsnprintf (buffer, (*bufsize) - 1, format, parms);
	va_end (parms);
	*bufsize -= _tcslen (buffer);
	return buffer + _tcslen (buffer);
}

// dinput
void setid (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt)
{
	// wrong place!
	uid->eventid[slot][SPARE_SUB_EVENT] = uid->eventid[slot][sub];
	uid->flags[slot][SPARE_SUB_EVENT] = uid->flags[slot][sub];
	uid->port[slot][SPARE_SUB_EVENT] = MAX_JPORTS + 1;
	xfree (uid->custom[slot][SPARE_SUB_EVENT]);
	uid->custom[slot][SPARE_SUB_EVENT] = uid->custom[slot][sub];
	uid->custom[slot][sub] = NULL;

	uid[i].eventid[slot][sub] = evt;
	uid[i].port[slot][sub] = port + 1;
}

void setid_af (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt, int af)
{
	setid (uid, i, slot, sub, port, evt);
	uid[i].flags[slot][sub] &= ~(ID_FLAG_AUTOFIRE | ID_FLAG_TOGGLE);
	if (af >= JPORT_AF_NORMAL)
		uid[i].flags[slot][sub] |= ID_FLAG_AUTOFIRE;
	if (af == JPORT_AF_TOGGLE)
		uid[i].flags[slot][sub] |= ID_FLAG_TOGGLE;
}

void target_quit (void)
{
}

void target_fixup_options (struct uae_prefs *p)
{
}

TCHAR start_path_data[MAX_DPATH];

void fetch_path (TCHAR *name, TCHAR *out, int size)
{
    _tcscpy (start_path_data, "./");
    _tcscpy (out, start_path_data);
    if (!name)
        return;
    if (!_tcscmp (name, "FloppyPath"))
        _tcscat (out, "./");
    else if (!_tcscmp (name, "CDPath"))
        _tcscat (out, "./");
    if (!_tcscmp (name, "hdfPath"))
        _tcscat (out, "./");
    if (!_tcscmp (name, "KickstartPath"))
        _tcscat (out, "./");
    if (!_tcscmp (name, "ConfigurationPath"))
        _tcscat (out, "./");
}

void fetch_saveimagepath (TCHAR *out, int size, int dir)
{
    fetch_path ("SaveimagePath", out, size);
}

void fetch_configurationpath (TCHAR *out, int size)
{
	fetch_path ("ConfigurationPath", out, size);
}

void fetch_screenshotpath (TCHAR *out, int size)
{
	fetch_path ("ScreenshotPath", out, size);
}

void fetch_ripperpath (TCHAR *out, int size)
{
	fetch_path ("RipperPath", out, size);
}

void fetch_statefilepath (TCHAR *out, int size)
{
	fetch_path ("StatefilePath", out, size);
}

void fetch_inputfilepath (TCHAR *out, int size)
{
	fetch_path ("InputPath", out, size);
}

void fetch_datapath (TCHAR *out, int size)
{
	fetch_path (NULL, out, size);
}

// convert path to absolute or relative
void fullpath (TCHAR *path, int size)
{
	if (path[0] == 0 || (path[0] == '\\' && path[1] == '\\') || path[0] == ':')
		return;
        /* <drive letter>: is supposed to mean same as <drive letter>:\ */
}

char *ua (const TCHAR *s)
{
	return strdup(s);
}

char *ua_fs (const char *s, int defchar)
{
	return strdup(s);
}

char *ua_fs_copy (char *dst, int maxlen, const TCHAR *src, int defchar)
{
    dst[0] = 0;
    strncpy(dst, src, maxlen);
    return dst;
}

char *ua_copy (char *dst, int maxlen, const char *src)
{
	dst[0] = 0;
	strncpy (dst, src, maxlen);
	return dst;
}

TCHAR *au (const char *s)
{
	return strdup(s);
}

TCHAR *au_copy (TCHAR *dst, int maxlen, const char *src)
{
	dst[0] = 0;
	memcpy (dst, src, maxlen);
	return dst;
}

char *au_fs_copy (char *dst, int maxlen, const char *src)
{
	int i;

	for (i = 0; src[i] && i < maxlen - 1; i++)
		dst[i] = src[i];
	dst[i] = 0;
	return dst;
}

// writelog
int consoleopen = 0;
static int realconsole = 1;

static int debugger_type = -1;

static void openconsole (void)
{
	if (realconsole) {
		if (debugger_type == 2) {
			//open_debug_window ();
			consoleopen = 1;
		} else {
			//close_debug_window ();
			consoleopen = -1;
		}
		return;
	}
}

void close_console (void)
{
	if (realconsole)
		return;
}

bool console_isch (void)
{
	return false;
}

TCHAR console_getch (void)
{
    return 0;
}

void debugger_change (int mode)
{
	if (mode < 0)
		debugger_type = debugger_type == 2 ? 1 : 2;
	else
		debugger_type = mode;
	if (debugger_type != 1 && debugger_type != 2)
		debugger_type = 2;
//	  regsetint (NULL, "DebuggerType", debugger_type);
	openconsole ();
}

// unicode
char *uutf8 (const char *s)
{
	return strdup(s);
}

char *utf8u (const char *s)
{
	return strdup(s);
}

// debug_win32
void update_debug_info(void)
{
}

#ifdef __LIBRETRO__
const TCHAR *target_get_display_name (int num, bool friendlyname){return NULL;}
int target_get_display (const TCHAR *name){return -1;}
int target_checkcapslock (int scancode, int *state){return 0;}
void setmaintitle(){}
#endif

#if defined PICASSO96
///////////////////////////////////////////////////
// win32gfx.cpp
///////////////////////////////////////////////////
#define MAX_DISPLAYS 10
struct MultiDisplay Displays[MAX_DISPLAYS];

static struct MultiDisplay *getdisplay2 (struct uae_prefs *p, int index)
{
	write_log ("Multimonitor detection disabled\n");
	Displays[0].primary = 1;
	Displays[0].monitorname = "Display";

	int max;
	int display = index < 0 ? p->gfx_apmode[screen_is_picasso ? APMODE_RTG : APMODE_NATIVE].gfx_display - 1 : index;

	max = 0;
	while (Displays[max].monitorname)
		max++;
	if (max == 0) {
		gui_message (_T("no display adapters! Exiting"));
		exit (0);
	}
	if (index >= 0 && display >= max)
		return NULL;
	if (display >= max)
		display = 0;
	if (display < 0)
		display = 0;
	return &Displays[display];
}

struct MultiDisplay *getdisplay (struct uae_prefs *p)
{
	return getdisplay2 (p, -1);
}

int target_get_display (const TCHAR *name)
{
	int oldfound = -1;
	int found = -1;
	unsigned int i;
	for (i = 0; Displays[i].monitorname; i++) {
		struct MultiDisplay *md = &Displays[i];
		if (!_tcscmp (md->adapterid, name))
			found = i + 1;
		if (!_tcscmp (md->adaptername, name))
			found = i + 1;
		if (!_tcscmp (md->monitorname, name))
			found = i + 1;
		if (!_tcscmp (md->monitorid, name))
			found = i + 1;
		if (found >= 0) {
			if (oldfound != found)
				return -1;
			oldfound = found;
		}
	}
	return -1;
}
const TCHAR *target_get_display_name (int num, bool friendlyname)
{
	if (num <= 0)
		return NULL;
	struct MultiDisplay *md = getdisplay2 (NULL, num - 1);
	if (!md)
		return NULL;
	if (friendlyname)
		return md->monitorname;
	return md->monitorid;
}
#endif
static int isfullscreen_2 (struct uae_prefs *p)
{
    int idx = screen_is_picasso ? 1 : 0;
    return p->gfx_apmode[idx].gfx_fullscreen == GFX_FULLSCREEN ? 1 : (p->gfx_apmode[idx].gfx_fullscreen == GFX_FULLWINDOW ? -1 : 0);
}

int isfullscreen (void)
{
	return isfullscreen_2 (&currprefs);
}

#define SM_CXSCREEN             0
#define SM_CYSCREEN             1
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define REFRESH_RATE_RAW 1
#define REFRESH_RATE_LACE 2
#ifndef _WIN32
static int GetSystemMetrics (int nIndex) {
	switch (nIndex) {
		case SM_CXSCREEN: return 1024;
		case SM_CYSCREEN: return 768;
		case SM_CXVIRTUALSCREEN: return 1024;
		case SM_CYVIRTUALSCREEN: return 768;
	}
	return 0;
}
#endif
static int resolution_compare (const void *a, const void *b)
{
#ifndef __LIBRETRO__
	struct PicassoResolution *ma = (struct PicassoResolution *)a;
	struct PicassoResolution *mb = (struct PicassoResolution *)b;
	if (ma->res.width < mb->res.width)
		return -1;
	if (ma->res.width > mb->res.width)
		return 1;
	if (ma->res.height < mb->res.height)
		return -1;
	if (ma->res.height > mb->res.height)
		return 1;
	return ma->depth - mb->depth;
#else 
	return 0;
#endif
}

#ifndef __LIBRETRO__
static void sortmodes (struct MultiDisplay *md)
{
	int	i, idx = -1;
	int pw = -1, ph = -1;

	i = 0;
	while (md->DisplayModes[i].depth >= 0)
		i++;
	qsort (md->DisplayModes, i, sizeof (struct PicassoResolution), resolution_compare);
	for (i = 0; md->DisplayModes[i].depth >= 0; i++) {
		int j, k;
		for (j = 0; md->DisplayModes[i].refresh[j]; j++) {
			for (k = j + 1; md->DisplayModes[i].refresh[k]; k++) {
				if (md->DisplayModes[i].refresh[j] > md->DisplayModes[i].refresh[k]) {
					int t = md->DisplayModes[i].refresh[j];
					md->DisplayModes[i].refresh[j] = md->DisplayModes[i].refresh[k];
					md->DisplayModes[i].refresh[k] = t;
					t = md->DisplayModes[i].refreshtype[j];
					md->DisplayModes[i].refreshtype[j] = md->DisplayModes[i].refreshtype[k];
					md->DisplayModes[i].refreshtype[k] = t;
				}
			}
		}
		if ( (md->DisplayModes[i].res.height != (uae_u32)ph)
		  || (md->DisplayModes[i].res.width  != (uae_u32)pw) ) {
			ph = md->DisplayModes[i].res.height;
			pw = md->DisplayModes[i].res.width;
			idx++;
		}
		md->DisplayModes[i].residx = idx;
	}
}

static void modesList (struct MultiDisplay *md)
{
	int i, j;

	i = 0;
	while (md->DisplayModes[i].depth >= 0) {
		write_log (_T("%d: %s%s ("), i, md->DisplayModes[i].rawmode ? _T("!") : _T(""), md->DisplayModes[i].name);
		j = 0;
		while (md->DisplayModes[i].refresh[j] > 0) {
			if (j > 0)
				write_log (_T(","));
			if (md->DisplayModes[i].refreshtype[j] & REFRESH_RATE_RAW)
				write_log (_T("!"));
			write_log (_T("%d"),  md->DisplayModes[i].refresh[j]);
			if (md->DisplayModes[i].refreshtype[j] & REFRESH_RATE_LACE)
				write_log (_T("i"));
			j++;
		}
		write_log (_T(")\n"));
		i++;
	}
}

void addmode (struct MultiDisplay *md, DEVMODE *dm, int rawmode)
{
	int ct;
	int i, j;
	int w = dm->dmPelsWidth;
	int h = dm->dmPelsHeight;
	int d = dm->dmBitsPerPel;
	bool lace = false;

/*	int freq = 0;
	if (dm->dmFields & DM_DISPLAYFREQUENCY) {
		freq = dm->dmDisplayFrequency;
		if (freq < 10)
			freq = 0;
	}
	if (dm->dmFields & DM_DISPLAYFLAGS) {
		lace = (dm->dmDisplayFlags & DM_INTERLACED) != 0;
	}*/
	int freq = 75;

	ct = 0;
	if (d == 8)
		ct = RGBMASK_8BIT;
	if (d == 15)
		ct = RGBMASK_15BIT;
	if (d == 16)
		ct = RGBMASK_16BIT;
	if (d == 24)
		ct = RGBMASK_24BIT;
	if (d == 32)
		ct = RGBMASK_32BIT;
	if (ct == 0)
		return;
	d /= 8;
	i = 0;

	while (md->DisplayModes[i].depth >= 0) {
		if ( (md->DisplayModes[i].depth      == d)
		  && (md->DisplayModes[i].res.width  == (uae_u32)w)
		  && (md->DisplayModes[i].res.height == (uae_u32)h) ) {
			for (j = 0; j < MAX_REFRESH_RATES; j++) {
				if (md->DisplayModes[i].refresh[j] == 0 || md->DisplayModes[i].refresh[j] == freq)
					break;
			}
			if (j < MAX_REFRESH_RATES) {
				md->DisplayModes[i].refresh[j] = freq;
				md->DisplayModes[i].refreshtype[j] = rawmode;
				md->DisplayModes[i].refresh[j + 1] = 0;
				return;
			}
		}
		i++;
	}
	i = 0;
	while (md->DisplayModes[i].depth >= 0)
		i++;
	if (i >= MAX_PICASSO_MODES - 1)
		return;
//	md->DisplayModes[i].rawmode = rawmode;
//	md->DisplayModes[i].lace = lace;
	md->DisplayModes[i].res.width = w;
	md->DisplayModes[i].res.height = h;
	md->DisplayModes[i].depth = d;
	md->DisplayModes[i].refresh[0] = freq;
	md->DisplayModes[i].refreshtype[0] = rawmode;
	md->DisplayModes[i].refresh[1] = 0;
	md->DisplayModes[i].colormodes = ct;
	md->DisplayModes[i + 1].depth = -1;
	_stprintf (md->DisplayModes[i].name, _T("%dx%d%s, %d-bit"),
		md->DisplayModes[i].res.width, md->DisplayModes[i].res.height,
		lace ? _T("i") : _T(""),
		md->DisplayModes[i].depth * 8);

//	write_log ("Add Mode: %s\n", md->DisplayModes[i].name);
}

void sortdisplays (void)
{
	struct MultiDisplay *md;
	int i, idx;

	int w = GetSystemMetrics (SM_CXSCREEN);
	int h = GetSystemMetrics (SM_CYSCREEN);
	int b = 0;
//	HDC hdc = GetDC (NULL);
//	if (hdc) {
//		b = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
//		ReleaseDC (NULL, hdc);
//	}
	write_log (_T("Desktop: W=%d H=%d B=%d. CXVS=%d CYVS=%d\n"), w, h, b,
		GetSystemMetrics (SM_CXVIRTUALSCREEN), GetSystemMetrics (SM_CYVIRTUALSCREEN));

	md = Displays;
	while (md->monitorname) {
		md->DisplayModes = xmalloc (struct PicassoResolution, MAX_PICASSO_MODES);
		md->DisplayModes[0].depth = -1;

		write_log (_T("%s '%s' [%s]\n"), md->adaptername, md->adapterid, md->adapterkey);
		write_log (_T("-: %s [%s]\n"), md->fullname, md->monitorid);
		for (int mode = 0; mode < 2; mode++) {
			DEVMODE dm;
			dm.dmSize = sizeof dm;
			dm.dmDriverExtra = 0;
			idx = 0;

//let's hope for the best
dm.dmPelsWidth = 1280;
dm.dmPelsHeight = 1024;
dm.dmBitsPerPel = 32;
dm.dmDisplayFrequency = 50;
//dm.dmDisplayFlags =
//dm.dmPosition = 
//dm.dmDisplayOrientation = 
//			while (EnumDisplaySettingsEx (md->adapterid, idx, &dm, mode ? EDS_RAWMODE : 0)) {
				int found = 0;
				int idx2 = 0;
				while (md->DisplayModes[idx2].depth >= 0 && !found) {
					struct PicassoResolution *pr = &md->DisplayModes[idx2];
					if ( (pr->res.width  == (uae_u32)dm.dmPelsWidth)
					  && (pr->res.height == (uae_u32)dm.dmPelsHeight)
					  && (pr->depth      == (int)(dm.dmBitsPerPel / 8)) ) {
						for (i = 0; pr->refresh[i]; i++) {
							if (pr->refresh[i] == (int)dm.dmDisplayFrequency) {
								found = 1;
								break;
							}
						}
					}
					idx2++;
				}
				if (!found && dm.dmBitsPerPel > 8) {
					/// REMOVEME: unused: int freq = 0;
//					if ((dm.dmFields & DM_PELSWIDTH) && (dm.dmFields & DM_PELSHEIGHT) && (dm.dmFields & DM_BITSPERPEL)) {
						addmode (md, &dm, mode);
//					}
				}
				idx++;
//			}
		}
		sortmodes (md);
		modesList (md);
		i = 0;
		while (md->DisplayModes[i].depth > 0)
			i++;
		write_log (_T("%d display modes.\n"), i);
		md++;
	}
}

void enumeratedisplays (void) {
	struct MultiDisplay *md = Displays;

	md->adaptername = strdup ("DeviceString");
	md->adapterid = strdup ("DeviceName");
	md->adapterkey = strdup ("DeviceID");
	md->monitorname = strdup ("DeviceString");
	md->monitorid = strdup ("DeviceKey");
	md->fullname = strdup ("DeviceName");
	md->primary = true;
}
#endif
void updatedisplayarea (void)
{
}

static bool render_ok;

int vsync_busy_wait_mode;

static void vsync_sleep (bool preferbusy)
{
	struct apmode *ap = picasso_on ? &currprefs.gfx_apmode[1] : &currprefs.gfx_apmode[0];
	bool dowait;

	if (vsync_busy_wait_mode == 0) {
		dowait = ap->gfx_vflip || !preferbusy;
		//dowait = !preferbusy;
	} else if (vsync_busy_wait_mode < 0) {
		dowait = true;
	} else {
		dowait = false;
	}
	if (dowait && (currprefs.m68k_speed >= 0 || currprefs.m68k_speed_throttle < 0))
		sleep_millis_main (1);
}

bool show_screen_maybe (bool show)
{
	struct apmode *ap = picasso_on ? &currprefs.gfx_apmode[1] : &currprefs.gfx_apmode[0];
	if (!ap->gfx_vflip || ap->gfx_vsyncmode == 0 || !ap->gfx_vsync) {
		if (show)
			show_screen (0);
		return false;
	}
	return false;
}

bool render_screen (bool immediate)
{
	render_ok = false;
	return render_ok;
}

void show_screen (int mode)
{
	render_ok = false;
}

static int maxscanline, minscanline, prevvblankpos;

static bool getvblankpos (int *vp)
{
	int sl = 0;
	*vp = -2;
	prevvblankpos = sl;
	if (sl > maxscanline)
		maxscanline = sl;
	if ( (sl > 0) && ( (sl < minscanline) || (minscanline < 0) ) ) {
			minscanline = sl;
	}
	*vp = sl;
	return true;
}

static bool getvblankpos2 (int *vp, int *flags)
{
	if (!getvblankpos (vp))
		return false;
	if (*vp > 100 && flags) {
		if ((*vp) & 1)
			*flags |= 2;
		else
			*flags |= 1;
	}
	return true;
}

static bool waitvblankstate (bool state, int *maxvpos, int *flags)
{
	int vp;
	if (flags)
		*flags = 0;
	for (;;) {
		int omax = maxscanline;
		if (!getvblankpos2 (&vp, flags))
			return false;
		while (omax != maxscanline) {
			omax = maxscanline;
			if (!getvblankpos2 (&vp, flags))
				return false;
		}
		if (maxvpos)
			*maxvpos = maxscanline;
		if (vp < 0) {
			if (state)
				return true;
		} else {
			if (!state)
				return true;
		}
	}
}

static int vblank_wait (void)
{
	int vp;

	for (;;) {
		int opos = prevvblankpos;
		if (!getvblankpos (&vp))
			return -2;
		if (opos > (maxscanline + minscanline) / 2 && vp < (maxscanline + minscanline) / 3)
			return vp;
		if (vp <= 0)
			return vp;
		vsync_sleep (true);
	}
}

static bool isthreadedvsync (void)
{
	return isvsync_chipset () <= -2 || isvsync_rtg () < 0;
}

bool vsync_busywait_do (int *freetime, bool lace, bool oddeven)
{
	bool v;
	static bool framelost;
	int ti;
	frame_time_t t;
	frame_time_t prevtime = vblank_prev_time;
	struct apmode *ap = picasso_on ? &currprefs.gfx_apmode[1] : &currprefs.gfx_apmode[0];

	t = read_processor_time ();
	ti = t - prevtime;
	if (ti) {
		waitvblankstate (false, NULL, NULL);
		vblank_prev_time = t;

		return true;
	}

	if (freetime)
		*freetime = 0;
	v = 0;

	if (isthreadedvsync ()) {

		framelost = false;
		v = 1;

	} else {
		int vp;

		if (currprefs.turbo_emulation) {
			show_screen (0);
			vblank_prev_time = read_processor_time ();
			framelost = true;
			v = -1;

		} else {
			while (!framelost && read_processor_time () - prevtime < 0) {
				vsync_sleep (false);
			}
			vp = vblank_wait ();
			if (vp >= -1) {
				vblank_prev_time = read_processor_time ();
				if (ap->gfx_vflip == 0) {
					show_screen (0);
				}
				for (;;) {
					if (!getvblankpos (&vp))
						break;
					if (vp > 0)
						break;
					sleep_millis (1);
				}
				if (ap->gfx_vflip != 0) {
					show_screen (0);
				}
				v = framelost ? -1 : 1;
			}

			framelost = false;
		}
		getvblankpos (&vp);
	}

	return v;
}

unsigned int flashscreen;   

void doflashscreen (void)
{
}

uae_u32 getlocaltime (void)
{
	return 0;
}
