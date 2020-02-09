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
extern int imagename_timer;
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

#ifndef _WIN32
#define TCHAR char /* from sysdeps.h */
#endif
#define LOGI printf

int umain (int argc, TCHAR **argv);

// VKBD
#define NPLGN 11
#define NLIGN 8
#define NLETT 9

// Colors
#define RGB565(r, g, b) (((r) << (5+6)) | ((g) << 6) | (b))
#define RGB888(r, g, b) (((r * 255 / 31) << (16)) | ((g * 255 / 31) << 8) | (b * 255 / 31))

// Amiga models
// chipmem_size 1 = 0.5MB, 2 = 1MB, 4 = 2MB
// bogomem_size 2 = 0.5MB, 4 = 1MB, 6 = 1.5MB, 7 = 1.8MB

#define A500_CONFIG "\
cpu_model=68000\n\
chipset=ocs\n\
chipset_compatible=A500\n\
chipmem_size=1\n\
bogomem_size=2\n\
"

#define A500OG_CONFIG "\
cpu_model=68000\n\
chipset=ocs\n\
chipset_compatible=A500\n\
chipmem_size=1\n\
bogomem_size=0\n\
"

#define A500PLUS_CONFIG "\
cpu_model=68000\n\
chipset=ecs\n\
chipset_compatible=A500+\n\
chipmem_size=2\n\
bogomem_size=0\n\
"

#define A600_CONFIG "\
cpu_model=68000\n\
chipset=ecs\n\
chipset_compatible=A600\n\
chipmem_size=4\n\
fastmem_size=8\n\
"

#define A1200_CONFIG "\
cpu_model=68020\n\
chipset=aga\n\
chipset_compatible=A1200\n\
chipmem_size=4\n\
fastmem_size=8\n\
"

#define A1200OG_CONFIG "\
cpu_model=68020\n\
chipset=aga\n\
chipset_compatible=A1200\n\
chipmem_size=4\n\
fastmem_size=0\n\
"

#define CD32_CONFIG "\
cpu_model=68020\n\
chipset=aga\n\
chipset_compatible=CD32\n\
chipmem_size=4\n\
fastmem_size=0\n\
floppy0type=-1\n\
"

#define CD32FR_CONFIG "\
cpu_model=68020\n\
chipset=aga\n\
chipset_compatible=CD32\n\
chipmem_size=4\n\
fastmem_size=8\n\
floppy0type=-1\n\
"

#define A4030_CONFIG "\
cpu_model=68030\n\
fpu_model=68882\n\
chipset=aga\n\
chipset_compatible=A4000\n\
chipmem_size=4\n\
fastmem_size=8\n\
"

#define A4040_CONFIG "\
cpu_model=68040\n\
fpu_model=68040\n\
chipset=aga\n\
chipset_compatible=A4000\n\
chipmem_size=4\n\
fastmem_size=8\n\
"

// Amiga kickstarts
#define A500_ROM                "kick34005.A500"
#define A500KS2_ROM             "kick37175.A500"
#define A600_ROM                "kick40063.A600"
#define A1200_ROM               "kick40068.A1200"
#define A4000_ROM               "kick40068.A4000"
#define CD32_ROM                "kick40060.CD32"
#define CD32_ROM_EXT            "kick40060.CD32.ext"

// Amiga files
#define ADF_FILE_EXT            "adf"
#define ADZ_FILE_EXT            "adz"
#define FDI_FILE_EXT            "fdi"
#define DMS_FILE_EXT            "dms"
#define IPF_FILE_EXT            "ipf"
#define HDF_FILE_EXT            "hdf"
#define HDZ_FILE_EXT            "hdz"
#define LHA_FILE_EXT            "lha"
#define CUE_FILE_EXT            "cue"
#define CCD_FILE_EXT            "ccd"
#define NRG_FILE_EXT            "nrg"
#define MDS_FILE_EXT            "mds"
#define ISO_FILE_EXT            "iso"
#define UAE_FILE_EXT            "uae"
#define M3U_FILE_EXT            "m3u"
#define LIBRETRO_PUAE_PREFIX    "puae_libretro"

// Amiga video
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

#define PUAE_VIDEO_HZ_PAL       49.9201
#define PUAE_VIDEO_HZ_NTSC      59.8251
#define PUAE_VIDEO_WIDTH        720
#define PUAE_VIDEO_HEIGHT_PAL   576
#define PUAE_VIDEO_HEIGHT_NTSC  480

// Libretro video
#define EMULATOR_DEF_WIDTH      720
#define EMULATOR_DEF_HEIGHT     576
#define EMULATOR_MAX_WIDTH      (EMULATOR_DEF_WIDTH * 2)
#define EMULATOR_MAX_HEIGHT     EMULATOR_DEF_HEIGHT

#define RETRO_BMP_SIZE          (EMULATOR_DEF_WIDTH * EMULATOR_DEF_HEIGHT * 4) // 4x is big enough for 24-bit SuperHires double line

#endif /* LIBRETRO_GLUE_H */
