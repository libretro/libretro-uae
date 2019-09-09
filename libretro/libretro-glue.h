#ifndef LIBRETRO_HATARI_H
#define LIBRETRO_HATARI_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <libco.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

extern cothread_t mainThread;
extern cothread_t emuThread;

void retro_update_geometry(bool);

#define LOGI printf

#define TEX_WIDTH 400
#define TEX_HEIGHT 300

extern int retrow; 
extern int retroh;

#define NPLGN 11
#define NLIGN 7
#define NLETT 9

#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))
#define RGB888(r, g, b)  (((r) << (16)) | ((g) << 8) | (b))

#ifndef _WIN32
#define TCHAR char /* from sysdeps.h */
#endif
int umain (int argc, TCHAR **argv);
#endif
