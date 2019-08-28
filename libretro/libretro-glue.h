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


#define LOGI printf

#define TEX_WIDTH 400
#define TEX_HEIGHT 300

extern int retrow; 
extern int retroh;
extern int CROP_WIDTH;
extern int CROP_HEIGHT;

#define NPLGN 11
#define NLIGN 7
#define NLETT 9

#define XOFFSET 20
#define YDELTA 110

#define XBASE0 4+(XOFFSET/2)
#define YBASE0 (CROP_HEIGHT - NLIGN*YSIDE) -60
#define YBASE0A YBASE0 + YDELTA

#define XBASE3 (XOFFSET/2)
#define YBASE3 YBASE0 -4
#define YBASE3A YBASE3 + YDELTA

#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))

#ifndef _WIN32
#define TCHAR char /* from sysdeps.h */
#endif
int umain (int argc, TCHAR **argv);
#endif
