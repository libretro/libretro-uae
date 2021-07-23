#ifndef LIBRETRO_CORE_H
#define LIBRETRO_CORE_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "zfile.h"

#include "libretro-glue.h"
#include "libretro-dc.h"
#include "string/stdstring.h"
#include "file/file_path.h"
#include "encodings/utf.h"

extern int imagename_timer;
extern void reset_drawing(void);
extern void print_statusbar(void);
extern bool retro_message;
extern char retro_message_msg[1024];

extern int retro_thisframe_first_drawn_line;
extern int retro_thisframe_last_drawn_line;
extern int retro_min_diwstart;
extern int retro_max_diwstop;

/* File helpers functions */
#define RETRO_PATH_MAX 512

#ifdef WIN32
#define DIR_SEP_STR "\\"
#define DIR_SEP_CHR '\\'
#else
#define DIR_SEP_STR "/"
#define DIR_SEP_CHR '/'
#endif

/* Usual suspects */
extern char retro_system_directory[RETRO_PATH_MAX];
extern char retro_save_directory[RETRO_PATH_MAX];
extern struct zfile *retro_deserialize_file;
extern dc_storage *retro_dc;
extern retro_log_printf_t log_cb;
extern long retro_ticks(void);
extern int umain (int argc, TCHAR **argv);

/* Statusbar */
#define STATUSBAR_BOTTOM    0x01
#define STATUSBAR_TOP       0x02
#define STATUSBAR_BASIC     0x04
#define STATUSBAR_MINIMAL   0x08

/* Colors */
#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))
#define RGB888(r, g, b) (((r * 255 / 31) << (16)) | ((g * 255 / 31) << 8) | (b * 255 / 31))
#define ARGB888(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)

#define COLOR_BLACK_16           RGB565( 10,  10,  10)
#define COLOR_GRAY_16            RGB565( 96,  96,  96)
#define COLOR_WHITE_16           RGB565(255, 255, 255)
#define COLOR_BLACK_32     ARGB888(255,  10,  10,  10)
#define COLOR_GRAY_32      ARGB888(255,  96,  96,  96)
#define COLOR_WHITE_32     ARGB888(255, 255, 255, 255)

#define COLOR_10_16              RGB565( 10,  10,  10)
#define COLOR_10_32        ARGB888(255,  10,  10,  10)
#define COLOR_16_16              RGB565( 16,  16,  16)
#define COLOR_16_32        ARGB888(255,  16,  16,  16)
#define COLOR_32_16              RGB565( 32,  32,  32)
#define COLOR_32_32        ARGB888(255,  32,  32,  32)
#define COLOR_40_16              RGB565( 40,  40,  40)
#define COLOR_40_32        ARGB888(255,  40,  40,  40)
#define COLOR_64_16              RGB565( 64,  64,  64)
#define COLOR_64_32        ARGB888(255,  64,  64,  64)
#define COLOR_132_16             RGB565(132, 132, 132)
#define COLOR_132_32       ARGB888(255, 132, 132, 132)
#define COLOR_140_16             RGB565(140, 140, 140)
#define COLOR_140_32       ARGB888(255, 140, 140, 140)
#define COLOR_160_16             RGB565(160, 160, 160)
#define COLOR_160_32       ARGB888(255, 160, 160, 160)
#define COLOR_200_16             RGB565(200, 200, 200)
#define COLOR_200_32       ARGB888(255, 200, 200, 200)
#define COLOR_250_16             RGB565(250, 250, 250)
#define COLOR_250_32       ARGB888(255, 250, 250, 250)

#define COLOR_BEIGE_16           RGB565(208, 208, 202)
#define COLOR_BEIGE_32     ARGB888(255, 208, 208, 202)
#define COLOR_BEIGEDARK_16       RGB565(154, 154, 150)
#define COLOR_BEIGEDARK_32 ARGB888(255, 154, 154, 150)

/* Functions */
extern void emu_function(int function);
enum EMU_FUNCTIONS
{
   EMU_VKBD = 0,
   EMU_STATUSBAR,
   EMU_JOYMOUSE,
   EMU_RESET,
   EMU_SAVE_DISK,
   EMU_ASPECT_RATIO,
   EMU_ZOOM_MODE,
   EMU_TURBO_FIRE,
   EMU_FUNCTION_COUNT
};

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

#define PUAE_VIDEO_HZ_PAL       49.9204101562500000
#define PUAE_VIDEO_HZ_NTSC      59.8260993957519531
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
extern unsigned int video_config;
extern unsigned int video_config_geometry;

#endif /* LIBRETRO_CORE_H */
