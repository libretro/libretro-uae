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
extern int RGBc(int r, int g, int b);
extern int umain (int argc, TCHAR **argv);

/* Statusbar */
#define STATUSBAR_BOTTOM    0x01
#define STATUSBAR_TOP       0x02
#define STATUSBAR_BASIC     0x04
#define STATUSBAR_MINIMAL   0x08

/* Autoloadfastforward */
#define AUTOLOADFASTFORWARD_FD 0x01
#define AUTOLOADFASTFORWARD_HD 0x02
#define AUTOLOADFASTFORWARD_CD 0x04

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
enum EMU_CONFIG
{
   EMU_CONFIG_A500 = 0,
   EMU_CONFIG_A500OG,
   EMU_CONFIG_A500PLUS,
   EMU_CONFIG_A600,
   EMU_CONFIG_A1200,
   EMU_CONFIG_A1200OG,
   EMU_CONFIG_A2000,
   EMU_CONFIG_A2000OG,
   EMU_CONFIG_A4030,
   EMU_CONFIG_A4040,
   EMU_CONFIG_CDTV,
   EMU_CONFIG_CD32,
   EMU_CONFIG_CD32FR,
   EMU_CONFIG_COUNT
};

/* Kickstarts */
enum retro_kickstart_ids
{
   A500_KS12_ROM = 0,
   A500_KS13_ROM,
   A500_KS204_ROM,
   A600_KS205_ROM,
   A600_KS31_ROM,
   A1200_KS30_ROM,
   A1200_KS31_ROM,
   A4000_KS30_ROM,
   A4000_KS31_ROM,
   CDTV_ROM,
   CD32_ROM,
   CD32_ROM_EXT
};

typedef struct
{
   unsigned id;
   char normal[20];
   char aforever[30];
   char tosec_mod[100];
   char tosec[100];
} retro_kickstarts;

static retro_kickstarts uae_kickstarts[15] =
{
   {A500_KS12_ROM,
         "kick33180.A500",
         "amiga-os-120.rom",
         "Kickstart v1.2 rev 33.180 (1986)(Commodore)(A500-A2000)[!].rom",
         "Kickstart v1.2 rev 33.180 (1986)(Commodore)(A500-A1000-A2000).rom"},
   {A500_KS13_ROM,
         "kick34005.A500",
         "amiga-os-130.rom",
         "Kickstart v1.3 rev 34.5 (1987)(Commodore)(A500-A1000-A2000-CDTV)[!].rom",
         "Kickstart v1.3 rev 34.5 (1987)(Commodore)(A500-A1000-A2000-CDTV).rom"},
   {A500_KS204_ROM,
         "kick37175.A500",
         "amiga-os-204.rom",
         "Kickstart v2.04 rev 37.175 (1991)(Commodore)(A500+)[!].rom",
         "Kickstart v2.04 rev 37.175 (1991)(Commodore)(A500+).rom"},

   {A600_KS205_ROM,
         "kick37350.A600",
         "amiga-os-205-a600.rom",
         "Kickstart v2.05 rev 37.350 (1992)(Commodore)(A600HD)[!].rom",
         ""},
   {A600_KS31_ROM,
         "kick40063.A600",
         "amiga-os-310-a600.rom",
         "Kickstart v3.1 rev 40.63 (1993)(Commodore)(A500-A600-A2000)[!].rom",
         "Kickstart v3.1 rev 40.63 (1993)(Commodore)(A500-A600-A2000).rom"},

   {A1200_KS30_ROM,
         "kick39106.A1200",
         "amiga-os-300-a1200.rom",
         "Kickstart v3.0 rev 39.106 (1992)(Commodore)(A1200)[!].rom",
         ""},
   {A1200_KS31_ROM,
         "kick40068.A1200",
         "amiga-os-310-a1200.rom",
         "Kickstart v3.1 rev 40.68 (1993)(Commodore)(A1200)[!].rom",
         "Kickstart v3.1 rev 40.68 (1993)(Commodore)(A1200).rom"},

   {A4000_KS30_ROM,
         "kick39106.A4000",
         "amiga-os-300-a4000.rom",
         "Kickstart v3.0 rev 39.106 (1992)(Commodore)(A4000)[!].rom",
         ""},
   {A4000_KS31_ROM,
         "kick40068.A4000",
         "amiga-os-310-a4000.rom",
         "",
         "Kickstart v3.1 rev 40.68 (1993)(Commodore)(A4000).rom"},

   {CDTV_ROM,
         "kick34005.CDTV",
         "amiga-os-130-cdtv-ext.rom",
         "CDTV Extended-ROM v1.0 (1991)(Commodore)(CDTV)[!].rom",
         "CDTV Extended-ROM v1.0 (1992)(Commodore)(CDTV).rom"},
   {CD32_ROM,
         "kick40060.CD32",
         "amiga-os-310-cd32.rom",
         "",
         "Kickstart v3.1 rev 40.60 (1993)(Commodore)(CD32).rom"},
   {CD32_ROM_EXT,
         "kick40060.CD32.ext",
         "amiga-os-310-cd32-ext.rom",
         "",
         "CD32 Extended-ROM rev 40.60 (1993)(Commodore)(CD32).rom"},
};

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
