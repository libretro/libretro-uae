#pragma once
#ifndef SRC_MISC_H_INCLUDED
#define SRC_MISC_H_INCLUDED 1

/*
 * PUAE - The Un*x Amiga Emulator
 *
 * Misc
 *
 * Copyright 2010-2013 Mustafa TUFAN
 */

#include <stdint.h>

#ifndef WIN32

#ifndef UINT
#define UINT uint32_t
#endif

#ifndef LONG
#define LONG int64_t
#endif

#ifndef HWND
#define HWND uintptr_t
#endif

#ifndef RECT
typedef struct tagRECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT, *NPRECT, *LPRECT;
#endif

#ifndef HBRUSH
typedef void* HBRUSH;
#endif

#ifndef GUID
typedef struct _GUID {
  unsigned long  Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char  Data4[8];
} GUID;
#endif

#ifndef LUID
typedef struct _LUID {
  uint32_t LowPart;
  int64_t  HighPart;
} LUID, *PLUID;
#endif

#ifndef HMONITOR
typedef void* HMONITOR;
#endif

#endif /* WIN32 */

#define MAX_SHMID 256

#define MAX_DISPLAYS 1
#define MAX_AMIGAMONITORS 4
#define MAX_REFRESH_RATES 100

struct winuae_currentmode {
	unsigned int flags;
	int native_width, native_height, native_depth, pitch;
	int current_width, current_height, current_depth;
	int amiga_width, amiga_height;
	int initdone;
	int fullfill;
	int vsync;
	int freq;
};

struct AmigaMonitor {
	int monitor_id;
	HWND hAmigaWnd;
	HWND hMainWnd;
	struct MultiDisplay *md;

	RECT amigawin_rect, mainwin_rect;
	RECT amigawinclip_rect;
	int window_extra_width, window_extra_height;
	int window_extra_height_bar;
	int win_x_diff, win_y_diff;
	int setcursoroffset_x, setcursoroffset_y;
	int mouseposx, mouseposy;
	int windowmouse_max_w;
	int windowmouse_max_h;
	int ratio_width, ratio_height;
	int ratio_adjust_x, ratio_adjust_y;
	bool ratio_sizing;
	bool render_ok, wait_render;
	int dpi;

	int in_sizemove;
	int manual_painting_needed;
	int minimized;
	int screen_is_picasso;
	int screen_is_initialized;
	int scalepicasso;
	bool rtg_locked;
	int p96_double_buffer_firstx, p96_double_buffer_lastx;
	int p96_double_buffer_first, p96_double_buffer_last;
	int p96_double_buffer_needs_flushing;

	HWND hStatusWnd;
	HBRUSH hStatusBkgB;

	struct winuae_currentmode currentmode;
	struct uae_filter *usedfilter;
};
extern struct AmigaMonitor *amon;
extern struct AmigaMonitor AMonitors[MAX_AMIGAMONITORS];

#ifdef PICASSO96
struct PicassoResolution
{
	struct ScreenResolution res;
	int depth;   /* depth in bytes-per-pixel */
	int residx;
	int refresh[MAX_REFRESH_RATES]; /* refresh-rates in Hz */
	int refreshtype[MAX_REFRESH_RATES]; /* 0=normal,1=raw,2=lace */
	TCHAR name[25];
	/* Bit mask of RGBFF_xxx values.  */
	uae_u32 colormodes;
	int rawmode;
	bool lace; // all modes lace
};
#endif

struct MultiDisplay {
	bool primary;
	GUID ddguid;
	HMONITOR monitor;
	TCHAR *adaptername, *adapterid, *adapterkey;
	TCHAR *monitorname, *monitorid;
	TCHAR *fullname;
	struct PicassoResolution *DisplayModes;
	RECT rect;
	RECT workrect;
	LUID AdapterLuid;
	UINT VidPnSourceId;
	UINT AdapterHandle;
	bool HasAdapterData;
};

extern struct MultiDisplay Displays[MAX_DISPLAYS + 1];
extern int amigamonid;

void getgfxoffset(int monid, float *dxp, float *dyp, float *mxp, float *myp);
int isfullscreen (void);
void fetch_configurationpath (TCHAR *out, int size);
TCHAR* buf_out (TCHAR *buffer, int *bufsize, const TCHAR *format, ...);
int my_existsdir (const char *name);
bool show_screen_maybe (int monid, bool show);
bool render_screen (int monid, int mode, bool immediate);
void show_screen (int monid, int mode);
FILE *my_opentext (const TCHAR *name);
int my_existsfile (const char *name);
void fetch_statefilepath (TCHAR *out, int size);
void fetch_datapath (TCHAR *out, int size);
void fetch_inputfilepath (TCHAR *out, int size);
void fetch_ripperpath (TCHAR *out, int size);
uae_u32 emulib_target_getcpurate (uae_u32 v, uae_u32 *low);
void fetch_saveimagepath (TCHAR *out, int size, int dir);
TCHAR *au (const char *s);
char *ua (const TCHAR *s);
char *uutf8 (const char *s);
char *utf8u (const char *s);
char *ua_copy (char *dst, int maxlen, const char *src);
TCHAR *au_copy (TCHAR *dst, int maxlen, const char *src);
TCHAR *au_fs (const char*);
char *au_fs_copy (char *dst, int maxlen, const char *src);
char *ua_fs (const char *s, int defchar);
char *ua_fs_copy (char *dst, int maxlen, const TCHAR *src, int defchar);
void close_console (void);
bool console_isch (void);
TCHAR console_getch (void);
bool vsync_switchmode (int monid, int hz);
bool vsync_busywait_do (int *freetime, bool lace, bool oddeven);
void doflashscreen (void);
int scan_roms (int show);
void enumeratedisplays (void);
void sortdisplays (void);
extern TCHAR start_path_data[MAX_DPATH];
extern void clipboard_reset (void);

#endif /* SRC_MISC_H_INCLUDED */
