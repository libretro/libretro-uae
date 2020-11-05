#ifndef LIBRETRO_CORE_H
#define LIBRETRO_CORE_H

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

#ifdef VITA
#include <psp2/types.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/threadmgr.h>
#define rmdir(name) sceIoRmdir(name)
#endif

#include "zfile.h"

#include "libretro-glue.h"
#include "libretro-dc.h"
#include "string/stdstring.h"
#include "file/file_path.h"

extern int imagename_timer;
extern void reset_drawing(void);
extern void print_statusbar(void);
extern bool retro_message;
extern char retro_message_msg[1024];

extern int retro_thisframe_first_drawn_line;
extern int retro_thisframe_last_drawn_line;
extern int retro_min_diwstart;
extern int retro_max_diwstop;

extern char retro_system_directory[512];
extern struct zfile *retro_deserialize_file;
extern dc_storage *retro_dc;
extern retro_log_printf_t log_cb;

#ifndef _WIN32
#define TCHAR char /* from sysdeps.h */
#endif

int umain (int argc, TCHAR **argv);

/* File helpers functions */
#define RETRO_PATH_MAX 512

#ifdef _WIN32
#define RETRO_PATH_SEPARATOR        "\\"
/* Windows also support the unix path separator */
#define RETRO_PATH_SEPARATOR_ALT    "/"
#else
#define RETRO_PATH_SEPARATOR        "/"
#endif

#ifdef WIN32
#define DIR_SEP_STR "\\"
#else
#define DIR_SEP_STR "/"
#endif

/* VKBD */
#define VKBDX 11
#define VKBDY 8
#if 0
#define POINTER_DEBUG
#endif
#ifdef POINTER_DEBUG
extern int pointer_x;
extern int pointer_y;
#endif

extern int vkey_pos_x;
extern int vkey_pos_y;
extern int vkey_pressed;
extern int vkey_sticky;
extern int vkey_sticky1;
extern int vkey_sticky2;

extern int vkbd_x_min;
extern int vkbd_x_max;
extern int vkbd_y_min;
extern int vkbd_y_max;

/* Statusbar */
#define STATUSBAR_BOTTOM    0x01
#define STATUSBAR_TOP       0x02
#define STATUSBAR_BASIC     0x04
#define STATUSBAR_MINIMAL   0x08

/* Colors */
#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))
#define RGB888(r, g, b) (((r * 255 / 31) << (16)) | ((g * 255 / 31) << 8) | (b * 255 / 31))
#define ARGB888(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)

/* Configs */
enum EMU_CONFIG {
   EMU_CONFIG_A500 = 0,
   EMU_CONFIG_A500OG,
   EMU_CONFIG_A500PLUS,
   EMU_CONFIG_A600,
   EMU_CONFIG_A1200,
   EMU_CONFIG_A1200OG,
   EMU_CONFIG_A4030,
   EMU_CONFIG_A4040,
   EMU_CONFIG_CDTV,
   EMU_CONFIG_CD32,
   EMU_CONFIG_CD32FR,
   EMU_CONFIG_COUNT
};

/* Kickstarts */
#define A500_ROM                "kick34005.A500"
#define A500KS2_ROM             "kick37175.A500"
#define A600_ROM                "kick40063.A600"
#define A1200_ROM               "kick40068.A1200"
#define A4000_ROM               "kick40068.A4000"
#define CDTV_ROM                "kick34005.CDTV"
#define CD32_ROM                "kick40060.CD32"
#define CD32_ROM_EXT            "kick40060.CD32.ext"

/* Support files */
#define LIBRETRO_PUAE_PREFIX    "puae_libretro"

/* Video */
#define PUAE_VIDEO_PAL          0x01
#define PUAE_VIDEO_NTSC         0x02
#define PUAE_VIDEO_HIRES        0x04
#define PUAE_VIDEO_SUPERHIRES   0x08
#define PUAE_VIDEO_DOUBLELINE   0x10

#define PUAE_VIDEO_PAL_LO       PUAE_VIDEO_PAL
#define PUAE_VIDEO_PAL_HI       PUAE_VIDEO_PAL|PUAE_VIDEO_HIRES
#define PUAE_VIDEO_PAL_HI_DL    PUAE_VIDEO_PAL|PUAE_VIDEO_HIRES|PUAE_VIDEO_DOUBLELINE
#define PUAE_VIDEO_PAL_SUHI     PUAE_VIDEO_PAL|PUAE_VIDEO_SUPERHIRES
#define PUAE_VIDEO_PAL_SUHI_DL  PUAE_VIDEO_PAL|PUAE_VIDEO_SUPERHIRES|PUAE_VIDEO_DOUBLELINE

#define PUAE_VIDEO_NTSC_LO      PUAE_VIDEO_NTSC
#define PUAE_VIDEO_NTSC_HI      PUAE_VIDEO_NTSC|PUAE_VIDEO_HIRES
#define PUAE_VIDEO_NTSC_HI_DL   PUAE_VIDEO_NTSC|PUAE_VIDEO_HIRES|PUAE_VIDEO_DOUBLELINE
#define PUAE_VIDEO_NTSC_SUHI    PUAE_VIDEO_NTSC|PUAE_VIDEO_SUPERHIRES
#define PUAE_VIDEO_NTSC_SUHI_DL PUAE_VIDEO_NTSC|PUAE_VIDEO_SUPERHIRES|PUAE_VIDEO_DOUBLELINE

#define PUAE_VIDEO_HZ_PAL       49.9201277955271580
#define PUAE_VIDEO_HZ_NTSC      59.8250950570342180
#define PUAE_VIDEO_WIDTH        720
#define PUAE_VIDEO_HEIGHT_PAL   576
#define PUAE_VIDEO_HEIGHT_NTSC  480

/* Libretro video */
#define EMULATOR_DEF_WIDTH      720
#define EMULATOR_DEF_HEIGHT     576
#define EMULATOR_MAX_WIDTH      (EMULATOR_DEF_WIDTH * 2)
#define EMULATOR_MAX_HEIGHT     EMULATOR_DEF_HEIGHT
#define RETRO_BMP_SIZE          (EMULATOR_DEF_WIDTH * EMULATOR_DEF_HEIGHT * 4) /* 4x is big enough for 24-bit SuperHires double line */

extern unsigned short int retro_bmp[RETRO_BMP_SIZE];
extern unsigned int pix_bytes;
extern int retrow;
extern int retroh;
extern int zoomed_width;
extern int zoomed_height;

#endif /* LIBRETRO_CORE_H */
