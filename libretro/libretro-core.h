#ifndef LIBRETRO_CORE_H
#define LIBRETRO_CORE_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "zfile.h"

#ifdef USE_LIBRETRO_VFS
#include <streams/file_stream_transforms.h>
#endif

#include "libretro-glue.h"
#include "libretro-dc.h"
#include "string/stdstring.h"
#include "file/file_path.h"
#include "retro_dirent.h"
#include "encodings/utf.h"

extern void reset_drawing(void);
extern void print_statusbar(void);
extern unsigned int statusbar_message_timer;
extern bool retro_message;
extern char retro_message_msg[1024];
extern void set_variable(const char* key, const char* value);
extern char* get_variable(const char *key);

extern int retro_thisframe_first_drawn_line;
extern int retro_thisframe_last_drawn_line;
extern int retro_min_diwstart;
extern int retro_max_diwstop;
extern int retro_doublescan;
extern bool video_productivity;
extern bool retro_av_info_is_lace;
extern bool libretro_frame_end;
extern bool libretro_runloop_active;
extern void libretro_do_restart(int argc, TCHAR **argv);

/* File helpers functions */
#define RETRO_PATH_MAX 512

#ifndef MAX_FLOPPY_DRIVES
#define MAX_FLOPPY_DRIVES 4
#endif

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
extern char retro_temp_directory[RETRO_PATH_MAX];
extern struct zfile *retro_deserialize_file;
extern dc_storage *dc;
extern retro_log_printf_t log_cb;
extern long retro_ticks(void);
extern int RGBc(int r, int g, int b);
extern int umain (int argc, TCHAR **argv);

/* Statusbar */
#define STATUSBAR_BOTTOM   0x01
#define STATUSBAR_TOP      0x02
#define STATUSBAR_BASIC    0x04
#define STATUSBAR_MINIMAL  0x08
#define STATUSBAR_MESSAGES 0x10

/* Autoloadfastforward */
#define AUTOLOADFASTFORWARD_FD 0x01
#define AUTOLOADFASTFORWARD_HD 0x02
#define AUTOLOADFASTFORWARD_CD 0x04

/* LED interface */
enum
{
   RETRO_LED_POWER = 0,
   RETRO_LED_DRIVES,
   RETRO_LED_HDCDMD,
   RETRO_LED_DRIVE0,
   RETRO_LED_DRIVE1,
   RETRO_LED_DRIVE2,
   RETRO_LED_DRIVE3,
   RETRO_LED_HD,
   RETRO_LED_CDMD,
   RETRO_LED_NUM
};

/* RetroPad options */
enum
{
   RETROPAD_OPTIONS_DISABLED = 0,
   RETROPAD_OPTIONS_ROTATE,
   RETROPAD_OPTIONS_JUMP,
   RETROPAD_OPTIONS_ROTATE_JUMP
};

/* Functions */
extern void emu_function(int function);
enum EMU_FUNCTIONS
{
   EMU_VKBD = 0,
   EMU_STATUSBAR,
   EMU_JOYMOUSE,
   EMU_RESET,
   EMU_FREEZE,
   EMU_SAVE_DISK,
   EMU_ASPECT_RATIO,
   EMU_CROP,
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
   A1000_KS11_NTSC_ROM = 0,
   A1000_KS11_PAL_ROM,
   A500_KS12_ROM,
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
   {A1000_KS11_NTSC_ROM,
         "kick31034.A1000",
         "amiga-os-110-ntsc.rom",
         "",
         "Kickstart v1.1 rev 31.34 (1985)(Commodore)(A1000)(NTSC).rom"},
   {A1000_KS11_PAL_ROM,
         "kick32034.A1000",
         "amiga-os-110-pal.rom",
         "",
         "Kickstart v1.1 rev 32.34 (1986)(Commodore)(A1000)(PAL).rom"},

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

/* Dynamic cartridge core option info */
struct puae_cart_info
{
   char *value;
   char *label;
};

/* Support files */
#define LIBRETRO_PUAE_PREFIX    "puae_libretro"

/* Video */
#define PUAE_VIDEO_PAL          0x01
#define PUAE_VIDEO_NTSC         0x02
#define PUAE_VIDEO_HIRES        0x04
#define PUAE_VIDEO_SUPERHIRES   0x08
#define PUAE_VIDEO_1x1          0x10
#define PUAE_VIDEO_DOUBLELINE   0x20
#define PUAE_VIDEO_QUADLINE     0x40

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

#define PUAE_VIDEO_HZ_PAL       49.920410f
#define PUAE_VIDEO_HZ_NTSC      59.826099f
#define PUAE_VIDEO_WIDTH        720
#define PUAE_VIDEO_HEIGHT_PAL   574
#define PUAE_VIDEO_HEIGHT_NTSC  484
#define PUAE_VIDEO_WIDTH_PROD   640
#define PUAE_VIDEO_WIDTH_S72    400
#define PUAE_VIDEO_HEIGHT_S72   300

/* Libretro video */
#define EMULATOR_DEF_WIDTH      PUAE_VIDEO_WIDTH
#define EMULATOR_DEF_HEIGHT     PUAE_VIDEO_HEIGHT_PAL
#define EMULATOR_MAX_WIDTH      EMULATOR_DEF_WIDTH * 2
#define EMULATOR_MAX_HEIGHT     EMULATOR_DEF_HEIGHT
#define RETRO_BMP_SIZE          EMULATOR_MAX_WIDTH * EMULATOR_MAX_HEIGHT * 4 /* 4x is big enough for 24-bit SuperHires double line */

extern unsigned short int retro_bmp[RETRO_BMP_SIZE];
extern uint8_t pix_bytes;
extern unsigned short int retrow;
extern unsigned short int retroh;
extern unsigned short int retrow_crop;
extern unsigned short int retroh_crop;
extern unsigned short int retrox_crop;
extern unsigned short int retroy_crop;
extern unsigned short int video_config;
extern unsigned short int video_config_geometry;

#define RESOLUTION_AUTO_NONE       0
#define RESOLUTION_AUTO_LORES      1
#define RESOLUTION_AUTO_HIRES      2
#define RESOLUTION_AUTO_SUPERHIRES 3

#define CROP_NONE            0
#define CROP_MINIMUM         1
#define CROP_SMALLER         2
#define CROP_SMALL           3
#define CROP_MEDIUM          4
#define CROP_LARGE           5
#define CROP_LARGER          6
#define CROP_MAXIMUM         7
#define CROP_AUTO            8

#define CROP_MODE_BOTH       0
#define CROP_MODE_VERTICAL   1
#define CROP_MODE_HORIZONTAL 2
#define CROP_MODE_16_9       3
#define CROP_MODE_16_10      4
#define CROP_MODE_4_3        5
#define CROP_MODE_5_4        6

#define ANALOG_STICK_SPEED_OPTIONS \
   { \
      { "0.1", "10%" }, { "0.2", "20%" }, { "0.3", "30%" }, { "0.4", "40%" }, { "0.5", "50%" },  \
      { "0.6", "60%" }, { "0.7", "70%" }, { "0.8", "80%" }, { "0.9", "90%" }, { "1.0", "100%" }, \
      { "1.1", "110%" },{ "1.2", "120%" },{ "1.3", "130%" },{ "1.4", "140%" },{ "1.5", "150%" }, \
      { "1.6", "160%" },{ "1.7", "170%" },{ "1.8", "180%" },{ "1.9", "190%" },{ "2.0", "200%" }, \
      { "2.1", "210%" },{ "2.2", "220%" },{ "2.3", "230%" },{ "2.4", "240%" },{ "2.5", "250%" }, \
      { "2.6", "260%" },{ "2.7", "270%" },{ "2.8", "280%" },{ "2.9", "290%" },{ "3.0", "300%" }, \
      { NULL, NULL }, \
   }

#endif /* LIBRETRO_CORE_H */
