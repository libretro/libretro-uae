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
#include "options.h"
#include "memory.h"
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
#include "clipboard.h"
#include "fsdb.h"
#include "debug.h"
#include "sleep.h"
#include "zfile.h"
#include "xwin.h"
#include "gfxboard.h"
#include "statusline.h"
#include "devices.h"

struct uae_prefs workprefs;
struct uae_mman_data;
#include "uae/mman.h"
#include <wctype.h>

#ifdef __LIBRETRO__
#include "libretro-core.h"
#endif

#ifdef __LIBRETRO__
void to_upper (TCHAR *s, int len)
{
   s = string_to_upper(s);
}
void to_lower (TCHAR *s, int len)
{
   s = string_to_lower(s);
}
int same_aname (const TCHAR *an1, const TCHAR *an2)
{
   return string_is_equal(an1, an2);
}

void png_set_expand (void) {}
void png_destroy_read_struct (void) {}
void png_set_strip_16 (void) {}
void png_set_packing (void) {}
void png_read_end (void) {}
void png_read_image (void) {}
void png_get_rowbytes (void) {}
void png_set_add_alpha (void) {}
void png_get_IHDR (void) {}
void png_read_info (void) {}
void png_set_read_fn (void) {}
void png_create_info_struct (void) {}
void png_create_read_struct (void) {}
void png_sig_cmp (void) {}

struct zfile* png_get_io_ptr = NULL;
#endif

static struct uae_shmid_ds shmids[MAX_SHMID];
uae_u8 *natmem_reserved, *natmem_offset;

// this is handled by the graphics drivers and set up in picasso96.c
#if defined(PICASSO96)
extern int screen_is_picasso;
#else
static int screen_is_picasso = 0;
#endif

#ifndef ACTION_REPLAY
uae_u8 ar_custom[2*256];
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

static struct winuae_currentmode currentmodestruct;
static struct winuae_currentmode *currentmode = &currentmodestruct;

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

/* external prototypes */
extern void setmaintitle(void);
extern int isvsync_chipset (void);
extern int isvsync_rtg (void);

TCHAR config_filename[256] = _T("");
TCHAR start_path_data[MAX_DPATH];
int saveimageoriginalpath = 0;

int pissoff_value = 15000 * CYCLE_UNIT;

uae_u8 *save_log (int bootlog, int *len)
{
   return NULL;
}

int scan_roms (int show)
{
	return 0;
}

void getgfxoffset (int monid, float *dxp, float *dyp, float *mxp, float *myp)
{
	*dxp = 0;
	*dyp = 0;
	*mxp = 0;
	*myp = 0;
}

bool vsync_switchmode (int monid, int hz)
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
int relativepaths = 1;

void sleep_millis_main (int ms)
{
   uae_msleep(ms);
}

void sleep_millis(int ms)
{
   uae_msleep(ms);
}

int target_sleep_nanos(int nanos)
{
   return 0;
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

int mouseactive;
bool ismouseactive (void)
{
	return mouseactive > 0;
}

void setmouseactivexy (int monid, int x, int y, int dir)
{
}

void setmouseactive (int monid, int active)
{
}

// clipboard
static uaecptr clipboard_data;
static int signaling, initialized;

void amiga_clipboard_die (TrapContext *ctx)
{
	signaling = 0;
	write_log ("clipboard not initialized\n");
}

void amiga_clipboard_init (TrapContext *ctx)
{
	signaling = 0;
	write_log ("clipboard initialized\n");
	initialized = 1;
}

void amiga_clipboard_task_start (TrapContext *ctx, uaecptr data)
{
	clipboard_data = data;
	signaling = 1;
	write_log ("clipboard task init: %08x\n", clipboard_data);
}

uaecptr amiga_clipboard_proc_start (TrapContext *ctx)
{
	write_log ("clipboard process init: %08x\n", clipboard_data);
	signaling = 1;
	return clipboard_data;
}

void amiga_clipboard_got_data (TrapContext *ctx, uaecptr data, uae_u32 size, uae_u32 actual)
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

void target_multipath_modified(struct uae_prefs *p)
{
	if (p != &workprefs)
		return;
	memcpy(&currprefs.path_hardfile, &p->path_hardfile, sizeof(struct multipath));
	memcpy(&currprefs.path_floppy, &p->path_floppy, sizeof(struct multipath));
	memcpy(&currprefs.path_cd, &p->path_cd, sizeof(struct multipath));
	memcpy(&currprefs.path_rom, &p->path_rom, sizeof(struct multipath));
}

int target_cfgfile_load (struct uae_prefs *p, const TCHAR *filename, int type, int isdefault)
{
	int v, i, type2;
	int ct, ct2, size;
	char tmp1[MAX_DPATH], tmp2[MAX_DPATH];
	TCHAR fname[MAX_DPATH], cname[MAX_DPATH];
#ifndef __LIBRETRO__
	error_log(NULL);
	_tcscpy (fname, filename);
	cname[0] = 0;
	if (!zfile_exists (fname)) {
		fetch_configurationpath (fname, sizeof (fname) / sizeof (TCHAR));
		if (_tcsncmp (fname, filename, _tcslen (fname)))
			_tcscat (fname, filename);
		else
			_tcscpy (fname, filename);
	}
#endif
	if (!isdefault)
		qs_override = 1;
	if (type < 0) {
		type = 0;
		cfgfile_get_description (NULL, fname, NULL, NULL, NULL, NULL, NULL, &type);
		if (!isdefault) {
			const TCHAR *p = _tcsrchr(fname, '\\');
			if (!p)
				p = _tcsrchr(fname, '/');
			if (p)
				_tcscpy(cname, p + 1);
		}
	}
	if (type == 0 || type == 1) {
		discard_prefs (p, 0);
	}
	type2 = type;
	if (type == 0 || type == 3) {
		default_prefs (p, true, type);
#ifndef __LIBRETRO__
		write_log(_T("config reset\n"));
#endif
	}
	ct2 = 0;
	v = cfgfile_load (p, fname, &type2, ct2, isdefault ? 0 : 1);
	if (!v)
		return v;
	if (type > 0)
		return v;
	if (cname[0])
		_tcscpy(config_filename, cname);
	for (i = 1; i <= 2; i++) {
		if (type != i) {
			size = sizeof (ct);
			ct = 0;
			if (ct && ((i == 1 && p->config_hardware_path[0] == 0) || (i == 2 && p->config_host_path[0] == 0) || ct2)) {
				size = sizeof (tmp1) / sizeof (TCHAR);
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
static void fixdriveletter(TCHAR *path)
{
	if (_istalpha(path[0]) && path[1] == ':' && path[2] == '\\' && path[3] == '.' && path[4] == 0)
		path[3] = 0;
	if (_istalpha(path[0]) && path[1] == ':' && path[2] == '\\' && path[3] == '.' && path[4] == '.' && path[5] == 0)
		path[3] = 0;
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

uae_u8 *target_load_keyfile (struct uae_prefs *p, const TCHAR *path, int *sizep, TCHAR *name)
{
#if 0
	uae_u8 *keybuf = NULL;
	HMODULE h;
	PFN_GetKey pfnGetKey;
	int size;
	const TCHAR *libname = _T("amigaforever.dll");

	h = WIN32_LoadLibrary(libname);
	if (!h) {
		TCHAR path[MAX_DPATH];
		_stprintf (path, _T("%s..\\Player\\%s"), start_path_exe, libname);
		h = WIN32_LoadLibrary(path);
		if (!h) {
			TCHAR *afr = _wgetenv (_T("AMIGAFOREVERROOT"));
			if (afr) {
				TCHAR tmp[MAX_DPATH];
				_tcscpy (tmp, afr);
				fixtrailing (tmp);
				_stprintf (path, _T("%sPlayer\\%s"), tmp, libname);
				h = WIN32_LoadLibrary(path);
			}
		}
	}
	if (!h)
		return NULL;
	GetModuleFileName (h, name, MAX_DPATH);
	//write_log (_T("keydll: %s'\n"), name);
	pfnGetKey = (PFN_GetKey)GetProcAddress (h, "GetKey");
	//write_log (_T("addr: %08x\n"), pfnGetKey);
	if (pfnGetKey) {
		size = pfnGetKey (NULL, 0);
		*sizep = size;
		//write_log (_T("size: %d\n"), size);
		if (size > 0) {
			int gotsize;
			keybuf = xmalloc (uae_u8, size);
			gotsize = pfnGetKey (keybuf, size);
			//write_log (_T("gotsize: %d\n"), gotsize);
			if (gotsize != size) {
				xfree (keybuf);
				keybuf = NULL;
			}
		}
	}
	FreeLibrary (h);
	//write_log (_T("keybuf=%08x\n"), keybuf);
	return keybuf;
#endif
}

void refreshtitle (void)
{
	if (isfullscreen () == 0)
		setmaintitle ();
}


// writelog
TCHAR* buf_out (TCHAR *buffer, int *bufsize, const TCHAR *format, ...)
{
	va_list parms;
	va_start (parms, format);

	if (buffer == NULL)
		return 0;

	vsnprintf (buffer, (*bufsize) - 1, format, parms);
	va_end (parms);
	*bufsize -= _tcslen (buffer);
	return buffer + _tcslen (buffer);
}

// dinput
void setid (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt)
{
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

void target_addtorecent (const TCHAR *name, int t)
{
}

bool get_plugin_path (TCHAR *out, int len, const TCHAR *path)
{
   return false;
}

void fetch_path (TCHAR *name, TCHAR *out, int size)
{
	_tcscpy (start_path_data, "./");
	_tcscpy (out, start_path_data);
	if (!name)
		return;
#ifdef __LIBRETRO__
	_tcscpy (out, retro_save_directory);
	_tcscat (out, DIR_SEP_STR);
#else
	if (!_tcscmp (name, "FloppyPath"))
		_tcscat (out, "./");
	else if (!_tcscmp (name, "CDPath"))
		_tcscat (out, "./");
	else if (!_tcscmp (name, "hdfPath"))
		_tcscat (out, "./");
	else if (!_tcscmp (name, "KickstartPath"))
		_tcscat (out, "./");
	else if (!_tcscmp (name, "ConfigurationPath"))
		_tcscat (out, "./");
	else if (!_tcscmp (name, "SaveimagePath"))
		_tcscat (out, "./");
#endif
}

void fetch_saveimagepath (TCHAR *out, int size, int dir)
{
	fetch_path ("SaveimagePath", out, size);
}
void fetch_configurationpath (TCHAR *out, int size)
{
	fetch_path ("ConfigurationPath", out, size);
}
void fetch_luapath (TCHAR *out, int size)
{
	fetch_path (_T("LuaPath"), out, size);
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
void fetch_rompath (TCHAR *out, int size)
{
	fetch_path (_T("KickstartPath"), out, size);
}

// convert path to absolute or relative
void fullpath_3 (TCHAR *path, int size, bool userelative)
{
	if (path[0] == 0 || (path[0] == '\\' && path[1] == '\\') || path[0] == ':')
		return;
	// has one or more environment variables? do nothing.
	if (_tcschr(path, '%'))
		return;
	if (_tcslen(path) >= 2 && path[_tcslen(path) - 1] == '.')
		return;
	/* <drive letter>: is supposed to mean same as <drive letter>:\ */
}

void fullpath(TCHAR *path, int size)
{
    fullpath_3(path, size, relativepaths);
}

bool samepath(const TCHAR *p1, const TCHAR *p2)
{
	if (!_tcsicmp(p1, p2))
		return true;
	TCHAR path1[MAX_DPATH], path2[MAX_DPATH];
	_tcscpy(path1, p1);
	_tcscpy(path2, p2);
	fixdriveletter(path1);
	fixdriveletter(path2);
	if (!_tcsicmp(path1, path2))
		return true;
	return false;
}

bool target_isrelativemode(void)
{
	return relativepaths != 0;
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

TCHAR *au_fs (const char *s)
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

void activate_console (void)
{
	if (!consoleopen)
		return;
}

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

void logging_init(void)
{
}


#define error_log

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

void updatedisplayarea (int monid) {}

#ifndef __LIBRETRO__
int vsync_busy_wait_mode;
#endif

static void vsync_sleep (bool preferbusy)
{
#ifndef __LIBRETRO__
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
#endif
}

bool show_screen_maybe (int monid, bool show)
{
#ifdef __LIBRETRO__
	show_screen (monid, 0);
#else
	struct AmigaMonitor *mon = &AMonitors[monid];
	struct amigadisplay *ad = &adisplays[monid];
	struct apmode *ap = ad->picasso_on ? &currprefs.gfx_apmode[1] : &currprefs.gfx_apmode[0];
	if (!ap->gfx_vflip || ap->gfx_vsyncmode == 0 || ap->gfx_vsync <= 0) {
		if (show)
			show_screen (monid, 0);
		return false;
	}
#endif
	return false;
}

bool render_screen (int monid, int mode, bool immediate)
{
#ifdef __LIBRETRO__
	return false;
#else
	struct AmigaMonitor *mon = &AMonitors[monid];
	struct amigadisplay *ad = &adisplays[monid];

	mon->render_ok = false;
	return mon->render_ok;
#endif
}

void show_screen (int monid, int mode)
{
#ifdef __LIBRETRO__
	return;
#else
	struct AmigaMonitor *mon = &AMonitors[monid];
	struct amigadisplay *ad = &adisplays[monid];

	mon->render_ok = false;
#endif
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
#ifndef __LIBRETRO__
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
#else
	return false;
#endif
}

uae_u32 getlocaltime (void)
{
	return 0;
}

void target_spin(int total)
{
}

int handle_msgpump (bool vblank)
{
    return 0;
}

void pausevideograb(int pause)
{
}

uae_s64 getsetpositionvideograb(uae_s64 framepos)
{
    return 0;
}






int rtg_index = -1;

// -2 = default
// -1 = prev
// 0 = chipset
// 1..4 = rtg
// 5 = next
bool toggle_rtg (int monid, int mode)
{
	struct amigadisplay *ad = &adisplays[monid];

	int old_index = rtg_index;
#ifdef PICASSO96
	if (monid > 0) {
		return true;
	}

	if (mode < -1 && rtg_index >= 0)
		return true;

	for (;;) {
		if (mode == -1) {
			rtg_index--;
		} else if (mode >= 0 && mode <= MAX_RTG_BOARDS) {
			rtg_index = mode - 1;
		} else {
			rtg_index++;
		}
		if (rtg_index >= MAX_RTG_BOARDS) {
			rtg_index = -1;
		} else if (rtg_index < -1) {
			rtg_index = MAX_RTG_BOARDS - 1;
		}
		if (rtg_index < 0) {
			if (ad->picasso_on) {
				gfxboard_rtg_disable(monid, old_index);
				ad->picasso_requested_on = false;
				statusline_add_message(STATUSTYPE_DISPLAY, _T("Chipset display"));
				set_config_changed();
				return false;
			}
			return false;
		}
		struct rtgboardconfig *r = &currprefs.rtgboards[rtg_index];
		if (r->rtgmem_size > 0 && r->monitor_id == monid) {
			if (r->rtgmem_type >= GFXBOARD_HARDWARE) {
				int idx = gfxboard_toggle(r->monitor_id, rtg_index, mode >= -1);
				if (idx >= 0) {
					rtg_index = idx;
					return true;
				}
				if (idx < -1) {
					rtg_index = -1;
					return false;
				}
			} else {
				gfxboard_toggle(r->monitor_id, -1, -1);
				if (mode < -1)
					return true;
				devices_unsafeperiod();
				gfxboard_rtg_disable(monid, old_index);
				// can always switch from RTG to custom
				if (ad->picasso_requested_on && ad->picasso_on) {
					ad->picasso_requested_on = false;
					rtg_index = -1;
					set_config_changed();
					return true;
				}
				if (ad->picasso_on)
					return false;
				// can only switch from custom to RTG if there is some mode active
				if (picasso_is_active(r->monitor_id)) {
					picasso_enablescreen(r->monitor_id, 1);
					ad->picasso_requested_on = true;
					statusline_add_message(STATUSTYPE_DISPLAY, _T("RTG %d: %s"), rtg_index + 1, _T("UAEGFX"));
					set_config_changed();
					return true;
				}
			}
		}
		if (mode >= 0 && mode <= MAX_RTG_BOARDS) {
			rtg_index = old_index;
			return false;
		}
	}
#endif
	return false;
}

void close_windows(struct AmigaMonitor *mon)
{
#if 0
	struct vidbuf_description *avidinfo = &adisplays[mon->monitor_id].gfxvidinfo;

	setDwmEnableMMCSS (FALSE);
	reset_sound ();
#if defined (GFXFILTER)
	S2X_free(mon->monitor_id);
#endif
	freevidbuffer(mon->monitor_id, &avidinfo->drawbuffer);
	freevidbuffer(mon->monitor_id, &avidinfo->tempbuffer);
	/*DirectDraw_Release();*/
	close_hwnds(mon);
#endif
}

void close_rtg(int monid, bool reset)
{
#if 0
	close_windows(&AMonitors[monid]);
#endif
}

uae_atomic atomic_and(volatile uae_atomic *p, uae_u32 v)
{
#if 0
	return _InterlockedAnd(p, v);
#else
	uae_atomic p_orig = *p;
	*p = (p_orig & v);
	return p_orig;
#endif
}
uae_atomic atomic_or(volatile uae_atomic *p, uae_u32 v)
{
#if 0
	return _InterlockedOr(p, v);
#else
	uae_atomic p_orig = *p;
	*p = (p_orig | v);
	return p_orig;
#endif
}
void atomic_set(volatile uae_atomic *p, uae_u32 v)
{
}
uae_atomic atomic_inc(volatile uae_atomic *p)
{
#if 0
	return _InterlockedIncrement(p);
#else
	return *p++;
#endif
}
uae_atomic atomic_dec(volatile uae_atomic *p)
{
#if 0
	return _InterlockedDecrement(p);
#else
	return *p--;
#endif
}
uae_u32 atomic_bit_test_and_reset(volatile uae_atomic *p, uae_u32 v)
{
#if 0
	return _interlockedbittestandreset(p, v);
#else
	uae_atomic p_orig = *p;
	uae_atomic mask = (1 << v);
	*p &= ~mask;
	return p_orig & mask;
#endif
}

#ifndef NATMEM_OFFSET
void unprotect_maprom (void)
{
#if 0
	bool protect = false;
	for (int i = 0; i < MAX_SHMID; i++) {
		struct uae_shmid_ds *shm = &shmids[i];
		if (shm->mode != PAGE_READONLY)
			continue;
		if (!shm->attached || !shm->rosize)
			continue;
		if (shm->maprom <= 0)
			continue;
		shm->maprom = -1;
		DWORD old;
		if (!VirtualProtect (shm->attached, shm->rosize, protect ? PAGE_READONLY : PAGE_READWRITE, &old)) {
			write_log (_T("unprotect_maprom VP %08lX - %08lX %x (%dk) failed %d\n"),
				(uae_u8*)shm->attached - natmem_offset, (uae_u8*)shm->attached - natmem_offset + shm->size,
				shm->size, shm->size >> 10, GetLastError ());
		}
	}
#endif
}

void protect_roms (bool protect)
{
#if 0
	if (protect) {
		// protect only if JIT enabled, always allow unprotect
		if (!currprefs.cachesize || currprefs.comptrustbyte || currprefs.comptrustword || currprefs.comptrustlong)
			return;
	}
	for (int i = 0; i < MAX_SHMID; i++) {
		struct uae_shmid_ds *shm = &shmids[i];
		if (shm->mode != PAGE_READONLY)
			continue;
		if (!shm->attached || !shm->rosize)
			continue;
		if (shm->maprom < 0 && protect)
			continue;
		DWORD old;
		if (!VirtualProtect (shm->attached, shm->rosize, protect ? PAGE_READONLY : PAGE_READWRITE, &old)) {
			write_log (_T("protect_roms VP %08lX - %08lX %x (%dk) failed %d\n"),
				(uae_u8*)shm->attached - natmem_offset, (uae_u8*)shm->attached - natmem_offset + shm->rosize,
				shm->rosize, shm->rosize >> 10, GetLastError ());
		} else {
			write_log(_T("ROM VP %08lX - %08lX %x (%dk) %s\n"),
				(uae_u8*)shm->attached - natmem_offset, (uae_u8*)shm->attached - natmem_offset + shm->rosize,
				shm->rosize, shm->rosize >> 10, protect ? _T("WPROT") : _T("UNPROT"));
		}
	}
#endif
}
#endif

uae_u8 *save_screenshot(int monid, int *len)
{
	return NULL;
}

bool is_mainthread(void)
{
#if 0
	return GetCurrentThreadId() == mainthreadid;
#else
    return true;
#endif
}

void target_cpu_speed(void)
{
#if 0
	display_vblank_thread(&AMonitors[0]);
#endif
}

#ifndef WITH_X86
void x86_rt1000_bios(struct zfile *z, struct romconfig *rc) {}
void x86_update_sound(double clk) {}
void x86_bridge_sync_change(void) {}
#endif

#ifndef PARALLEL_DIRECT
void paraport_free (void) { }
int paraport_init (void) { return 0; }
int paraport_open (TCHAR *port) { return 0; }
int parallel_direct_write_status (uae_u8 v, uae_u8 dir) { return 0; }
int parallel_direct_read_status (uae_u8 *vp) { return 0; }
int parallel_direct_write_data (uae_u8 v, uae_u8 dir) { return 0; }
int parallel_direct_read_data (uae_u8 *v) { return 0; }
#else
#include <paraport/ParaPort.h>
#endif

void clipboard_vsync (void) {}
void clipboard_reset (void) {}
void clipboard_unsafeperiod (void) {}

int amiga_clipboard_want_data(TrapContext *ctx) { return 0; }

void statusline_updated(int monid) {}
void statusline_render(int monid, uae_u8 *buf, int bpp, int pitch, int width, int height, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *alpha) {}

bool softstatusline(void)
{
	if (currprefs.gfx_api > 0)
		return false;
	return (currprefs.leds_on_screen & STATUSLINE_TARGET) == 0;
}

bool isvideograb(void) { return false; }
bool initvideograb(const TCHAR *filename) { return false; }
bool getvideograb(long **buffer, int *width, int *height) { return false; }
void uninitvideograb(void) {}
bool getpausevideograb(void) { return false; }
void isvideograb_status(void) {}
void setvolumevideograb(int volume) {}

void uaeexe_install (void) {}
void uaenative_install (void) {}

int consolehook_activate (void) { return 0; }
void consolehook_ret(TrapContext *ctx, uaecptr condev, uaecptr oldbeginio) {}
uaecptr consolehook_beginio(TrapContext *ctx, uaecptr request) {}

void filesys_addexternals (void) {}
int target_get_volume_name (struct uaedev_mount_info *mtinf, struct uaedev_config_info *ci, bool inserted, bool fullcheck, int cnt) { return 2; }
void target_getdate(int *y, int *m, int *d) {}

unsigned char def_drawer[] = {};
unsigned int def_drawer_len = 0;
unsigned char def_tool[] = {};
unsigned int def_tool_len = 0;
unsigned char def_project[] = {};
unsigned int def_project_len = 0;

/*void fp_init_softfloat(int fpu_model) {}*/
/*void fp_init_native(void) {}*/
void fpux_restore (int *v) {}

int is_tablet (void) { return 0; }
int is_touch_lightpen (void) { return 0; }

bool inprec_realtime (bool stopstart) { return false; }
void desktop_coords(int monid, int *dw, int *dh, int *ax, int *ay, int *aw, int *ah) {}
void update_disassembly(uae_u32 addr) {}
void update_memdump(uae_u32 addr) {}
int console_get (TCHAR *out, int maxlen) { return 0; }
void console_flush (void) {}
