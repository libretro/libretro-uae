#ifndef LIBRETRO_GLUE_H
#define LIBRETRO_GLUE_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

extern int retrow;
extern int retroh;
extern int pix_bytes;
extern int zoomed_height;
extern bool retro_update_av_info(bool, bool, bool);
extern void reset_drawing();

extern int vkey_pressed;
extern int vkey_sticky;
extern int vkey_sticky1;
extern int vkey_sticky2;

extern int retro_thisframe_first_drawn_line;
extern int retro_thisframe_last_drawn_line;
extern int retro_min_diwstart;
extern int retro_max_diwstop;

extern char retro_system_directory[512];

#define LOGI printf

#define NPLGN 11
#define NLIGN 8
#define NLETT 9

#define RGB565(r, g, b) (((r) << (5+6)) | ((g) << 6) | (b))
#define RGB888(r, g, b) (((r * 255 / 31) << (16)) | ((g * 255 / 31) << 8) | (b * 255 / 31))

#define EMULATOR_DEF_WIDTH 720
#define EMULATOR_DEF_HEIGHT 576

#ifndef _WIN32
#define TCHAR char /* from sysdeps.h */
#endif
int umain (int argc, TCHAR **argv);
#endif
