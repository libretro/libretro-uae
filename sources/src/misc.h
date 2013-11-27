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

void getgfxoffset (int *dxp, int *dyp, int *mxp, int *myp);
int isfullscreen (void);
void fetch_configurationpath (TCHAR *out, int size);
TCHAR* buf_out (TCHAR *buffer, int *bufsize, const TCHAR *format, ...);
TCHAR *au (const char *s);
char *ua (const TCHAR *s);
char *uutf8 (const char *s);
char *utf8u (const char *s);
int my_existsdir (const char *name);
bool show_screen_maybe (bool show);
bool render_screen (bool immediate);
void show_screen (int mode);
TCHAR *au_copy (TCHAR *dst, int maxlen, const char *src);
FILE *my_opentext (const TCHAR *name);
int my_existsfile (const char *name);
void fetch_statefilepath (TCHAR *out, int size);
void fetch_datapath (TCHAR *out, int size);
void fetch_inputfilepath (TCHAR *out, int size);
void fetch_ripperpath (TCHAR *out, int size);
uae_u32 emulib_target_getcpurate (uae_u32 v, uae_u32 *low);
void fetch_saveimagepath (TCHAR *out, int size, int dir);
char *ua_copy (char *dst, int maxlen, const char *src);
char *au_fs_copy (char *dst, int maxlen, const char *src);
char *ua_fs (const char *s, int defchar);
void close_console (void);
bool console_isch (void);
TCHAR console_getch (void);
int vsync_switchmode (int hz);
bool vsync_busywait_do (int *freetime, bool lace, bool oddeven);
void doflashscreen (void);
int scan_roms (int show);
void enumeratedisplays (void);
void sortdisplays (void);

#endif /* SRC_MISC_H_INCLUDED */
