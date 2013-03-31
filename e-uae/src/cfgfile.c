 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Config file handling
  * This still needs some thought before it's complete...
  *
  * Copyright 1998 Brian King, Bernd Schmidt
  * Copyright 2006 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <ctype.h>

#include "options.h"
#include "uae.h"
#include "filesys.h"
#include "events.h"
#include "custom.h"
#include "inputdevice.h"
#include "gfxfilter.h"
#include "gfxdep/gfx.h"
#include "sounddep/sound.h"
#include "savestate.h"
#include "mmemory.h"
#include "newcpu.h"
#include "version.h"

#define CONFIG_BLEN 2560

static int config_newfilesystem;
static struct strlist *temp_lines;

/* @@@ need to get rid of this... just cut part of the manual and print that
 * as a help text.  */
struct cfg_lines
{
    const char *config_label, *config_help;
};

/* For formatting help output - should be done dynamically */
#define MAX_OPTION_KEY_LEN  23

static const struct cfg_lines opttable[] =
{
    {"config_description", "" },
    {"config_info", "" },
    {"use_gui", "Enable the GUI?  If no, then goes straight to emulator" },
#ifdef DEBUGGER
    {"use_debugger", "Enable the debugger?" },
#endif
    {"cpu_speed", "can be max, real, or a number between 1 and 20" },
    {"cpu_type", "Can be 68000, 68010, 68020, 68020/68881" },
    {"cpu_compatible", "yes enables compatibility-mode" },
    {"cpu_24bit_addressing", "must be set to 'no' in order for Z3mem or P96mem to work" },
    {"log_illegal_mem", "print illegal memory access by Amiga software?" },
    {"fastmem_size", "Size in megabytes of fast-memory" },
    {"chipmem_size", "Size in megabytes of chip-memory" },
    {"bogomem_size", "Size in megabytes of bogo-memory at 0xC00000" },
    {"a3000mem_size", "Size in megabytes of A3000 memory" },
    {"gfxcard_size", "Size in megabytes of Picasso96 graphics-card memory" },
    {"z3mem_size", "Size in megabytes of Zorro-III expansion memory" },
    {"gfx_test_speed", "Test graphics speed?" },
    {"gfx_framerate", "Print every nth frame" },
    {"gfx_width", "Screen width" },
    {"gfx_height", "Screen height" },
    {"gfx_refreshrate", "Fullscreen refresh rate" },
    {"gfx_vsync", "Sync screen refresh to refresh rate" },
    {"gfx_lores", "Treat display as lo-res?" },
    {"gfx_linemode", "Can be none, double, or scanlines" },
    {"gfx_fullscreen_amiga", "Amiga screens are fullscreen?" },
    {"gfx_fullscreen_picasso", "Picasso screens are fullscreen?" },
    {"gfx_correct_aspect", "Correct aspect ratio?" },
    {"gfx_center_horizontal", "Center display horizontally?" },
    {"gfx_center_vertical", "Center display vertically?" },
    {"gfx_colour_mode", "" },
    {"immediate_blits", "Perform blits immediately" },
    {"hide_cursor", "Whether to hide host window manager's cursor"},
    {"show_leds", "LED display" },
    {"keyboard_leds", "Keyboard LEDs" },
    {"sound_output", "" },
    {"sound_frequency", "" },
    {"sound_bits", "" },
    {"sound_channels", "" },
    {"sound_latency", "" },
#ifdef JIT
    {"comp_trustbyte", "How to access bytes in compiler (direct/indirect/indirectKS/afterPic" },
    {"comp_trustword", "How to access words in compiler (direct/indirect/indirectKS/afterPic" },
    {"comp_trustlong", "How to access longs in compiler (direct/indirect/indirectKS/afterPic" },
    {"comp_nf", "Whether to optimize away flag generation where possible" },
    {"comp_fpu", "Whether to provide JIT FPU emulation" },
    {"compforcesettings", "Whether to force the JIT compiler settings" },
    {"cachesize", "How many MB to use to buffer translated instructions"},
#endif
    {"parallel_on_demand", "" },
    {"serial_on_demand", "" },
    {"scsi", "scsi.device emulation" },
    {"scsi_device", "SCSI device/bus to scan for SCSI devices"},
    {"joyport0", "" },
    {"joyport1", "" },
    {"kickstart_rom_file", "Kickstart ROM image, (C) Copyright Amiga, Inc." },
    {"kickstart_ext_rom_file", "Extended Kickstart ROM image, (C) Copyright Amiga, Inc." },
    {"kickstart_key_file", "Key-file for encrypted ROM images (from Cloanto's Amiga Forever)" },
    {"flash_ram_file", "Flash/battery backed RAM image file." },
#ifdef ACTION_REPLAY
    {"cart_file", "Freezer cartridge ROM image file." },
#endif
    {"floppy0", "Diskfile for drive 0" },
    {"floppy1", "Diskfile for drive 1" },
    {"floppy2", "Diskfile for drive 2" },
    {"floppy3", "Diskfile for drive 3" },
#ifdef FILESYS
    {"hardfile", "access,sectors, surfaces, reserved, blocksize, path format" },
    {"filesystem", "access,'Amiga volume-name':'host directory path' - where 'access' can be 'read-only' or 'read-write'" },
#endif
#ifdef CATWEASEL
    {"catweasel_io","Catweasel board io base address" }
#endif
};

static const char *guimode1[] = { "no", "yes", "nowait", 0 };
static const char *guimode2[] = { "false", "true", "nowait", 0 };
static const char *guimode3[] = { "0", "1", "nowait", 0 };
static const char *csmode[] = { "ocs", "ecs_agnus", "ecs_denise", "ecs", "aga", 0 };
static const char *linemode1[] = { "none", "double", "scanlines", 0 };
static const char *linemode2[] = { "n", "d", "s", 0 };
static const char *speedmode[] = { "max", "real", 0 };
static const char *cpumode[] = {
    "68000", "68000", "68010", "68010", "68ec020", "68020", "68ec020/68881", "68020/68881",
    "68040", "68040", "xxxxx", "xxxxx", "68060", "68060", 0
};
static const char *colormode1[] = { "8bit", "15bit", "16bit", "8bit_dither", "4bit_dither", "32bit", 0 };
static const char *colormode2[] = { "8", "15", "16", "8d", "4d", "32", 0 };
static const char *soundmode1[] = { "none", "interrupts", "normal", "exact", 0 };
static const char *soundmode2[] = { "none", "interrupts", "good", "best", 0 };
static const char *centermode1[] = { "none", "simple", "smart", 0 };
static const char *centermode2[] = { "false", "true", "smart", 0 };
static const char *stereomode[] = { "mono", "stereo", "4ch", "mixed", 0 };
static const char *interpolmode[] = { "none", "rh", "crux", "sinc", 0 };
static const char *collmode[] = { "none", "sprites", "playfields", "full", 0 };
static const char *compmode[] = { "direct", "indirect", "indirectKS", "afterPic", 0 };
static const char *flushmode[]   = { "soft", "hard", 0 };
static const char *kbleds[] = { "none", "POWER", "DF0", "DF1", "DF2", "DF3", "HD", "CD", 0 };
#ifdef GFXFILTER
static const char *filtermode1[] = { "no_16", "bilinear_16", "no_32", "bilinear_32", 0 };
static const char *filtermode2[] = { "0x", "1x", "2x", "3x", "4x", 0 };
#endif

static const char *obsolete[] = {
    "accuracy","gfx_opengl","gfx_32bit_blits","32bit_blits",
    "gfx_immediate_blits","gfx_ntsc","win32",
    "sound_pri_cutoff", "sound_pri_time",
    "avoid_dga", "override_dga_address", "avoid_vid", "avoid_cmov",
    "comp_midopt", "comp_lowopt",
    "fast_copper", "sound_max_buf",
    0 };

#define UNEXPANDED "$(FILE_PATH)"


/*
 * The beginnings of a less brittle, more easily maintainable way of handling
 * prefs options.
 *
 * We maintain a key/value table of options.
 *
 * TODO:
 *  Make this a hash table.
 *  Add change notification.
 *  Support other value data types.
 *  Migrate more options.
 */

typedef struct {
    const char *key;
    int         target_specific;
    const char *value;
    const char *help;
} prefs_attr_t;

static prefs_attr_t prefs_attr_table[] = {
    {"floppy_path",            1, 0, "Default directory for floppy disk images"},
    {"rom_path",               1, 0, "Default directory for ROM images"},
    {"hardfile_path",          1, 0, "Default directory for hardfiles and filesystems"},
    {"savestate_path",         1, 0, "Default directory for saved-state images"},
    {0,                        0, 0, 0}
};

static prefs_attr_t *lookup_attr (const char *key)
{
    prefs_attr_t *attr = &prefs_attr_table[0];

    while (attr->key) {
	if (0 == strcmp (key, attr->key))
	    return attr;
	attr++;
    }
    return 0;
}

static void prefs_dump_help (void)
{
    prefs_attr_t *attr = &prefs_attr_table[0];

    while (attr->key) {
	int width = -MAX_OPTION_KEY_LEN;
	if (attr->target_specific) {
	    width += strlen (TARGET_NAME) + 1;
	    write_log ("%s.", TARGET_NAME);
	}
	write_log ("%*s: %s.\n", width, attr->key, attr->help ? attr->help : "");
	attr++;
    }
}

void prefs_set_attr (const char *key, const char *value)
{
    prefs_attr_t *attr = lookup_attr (key);

    if (attr) {
	if (attr->value)
	    free ((void *)attr->value);
	attr->value = value;
    }
}

const char *prefs_get_attr (const char *key)
{
    prefs_attr_t *attr = lookup_attr (key);

    if (attr)
	return attr->value;
    else
	return 0;
}

const char* get_cpu_mode(int i) {
	return cpumode[i];	
}
const char* get_collision_mode(int i) {
	return collmode[i];
}
const char* get_sound_mode1(int i) {
	return soundmode1[i];
}
const char* get_sound_interpol_mode(int i) {
	return interpolmode[i];
}

static int match_string (const char *table[], const char *str)
{
    int i;
    for (i = 0; table[i] != 0; i++)
	if (strcasecmp (table[i], str) == 0)
	    return i;
    return -1;
}

char *cfgfile_subst_path (const char *path, const char *subst, const char *file)
{
    /* @@@ use strcasecmp for some targets.  */
    if (strlen (path) > 0 && strncmp (file, path, strlen (path)) == 0) {
	int l;
	char *p = xmalloc (strlen (file) + strlen (subst) + 2);
	strcpy (p, subst);
	l = strlen (p);
	while (l > 0 && p[l - 1] == '/')
	    p[--l] = '\0';
	l = strlen (path);
	while (file[l] == '/')
	    l++;
	strcat (p, "/"); strcat (p, file + l);
	return p;
    }
    return my_strdup (file);
}

static void subst_home (char *f, int n)
{
    const char *home = getenv ("HOME");

    if (home) {
	char *str = cfgfile_subst_path ("~", home, f);
	strncpy (f, str, n - 1);
	f[n - 1] = '\0';
	free (str);
    }
}

void cfgfile_subst_home (char *path, unsigned int maxlen)
{
    subst_home (path, maxlen);
}

void cfgfile_write (FILE *f, const char *format,...)
{
    va_list parms;
    char tmp[CONFIG_BLEN];

    va_start (parms, format);
    vsprintf (tmp, format, parms);
    fprintf (f, tmp);
    va_end (parms);
}

static void cfgfile_write_path_option (FILE *f, const char *key)
{
    const char *home = getenv ("HOME");
    const char *path = prefs_get_attr (key);
    char *out_path = 0;

    if (path)
	out_path = cfgfile_subst_path (home, "~", path);

    cfgfile_write (f, "%s.%s=%s\n", TARGET_NAME, key, out_path ? out_path : "");

    if (out_path)
	free (out_path);
}

static void cfgfile_write_file_option (FILE *f, const char *option, const char *subst_key, const char *value)
{
    const char *subst_path = prefs_get_attr (subst_key);
    char *out_path = 0;

    if (subst_path)
	out_path = cfgfile_subst_path (subst_path, UNEXPANDED, value);

    cfgfile_write (f, "%s=%s\n", option, out_path ? out_path : value);

    if (out_path)
	free (out_path);
}

void save_options (FILE *f, const struct uae_prefs *p, int type)
{
    struct strlist *sl;
    int i;

    cfgfile_write (f, "config_description=%s\n", p->description);
    cfgfile_write (f, "config_hardware=%s\n", (type & CONFIG_TYPE_HARDWARE) ? "true" : "false");
    cfgfile_write (f, "config_host=%s\n", (type & CONFIG_TYPE_HOST) ? "true" : "false");
    if (p->info[0])
	cfgfile_write (f, "config_info=%s\n", p->info);
    cfgfile_write (f, "config_version=%d.%d.%d\n", UAEMAJOR, UAEMINOR, UAESUBREV);

    for (sl = p->all_lines; sl; sl = sl->next) {
	if (sl->unknown)
	    cfgfile_write (f, "%s=%s\n", sl->option, sl->value);
    }

    cfgfile_write_path_option (f, "rom_path");
    cfgfile_write_path_option (f, "floppy_path");
    cfgfile_write_path_option (f, "hardfile_path");
#ifdef SAVESTATE
    cfgfile_write_path_option (f, "savestate_path");
#endif

#ifndef _WIN32
    cfgfile_write (f, "%s.cpu_idle=%d\n", TARGET_NAME, p->cpu_idle);
#endif

    machdep_save_options (f, p);
    target_save_options (f, p);
    gfx_save_options (f, p);
    audio_save_options (f, p);

    cfgfile_write (f, "use_gui=%s\n", guimode1[p->start_gui]);
#ifdef DEBUGGER
    cfgfile_write (f, "use_debugger=%s\n", p->start_debugger ? "true" : "false");
#endif

    cfgfile_write_file_option (f, "kickstart_rom_file",     "rom_path", p->romfile);
    cfgfile_write_file_option (f, "kickstart_ext_rom_file", "rom_path", p->romextfile);
    cfgfile_write_file_option (f, "kickstart_key_file",     "rom_path", p->keyfile);
    cfgfile_write_file_option (f, "flash_file",             "rom_path", p->flashfile);
#ifdef ACTION_REPLAY
    cfgfile_write_file_option (f, "cart_file",              "rom_path", p->cartfile);
#endif

    cfgfile_write (f, "kickshifter=%s\n", p->kickshifter ? "true" : "false");

    for (i = 0; i < 4; i++) {
	char tmp_option[] = "floppy0";
	tmp_option[6] = '0' + i;
	cfgfile_write_file_option (f, tmp_option, "floppy_path", p->df[i]);
	cfgfile_write (f, "floppy%dtype=%d\n", i, p->dfxtype[i]);
#ifdef DRIVESOUND
	cfgfile_write (f, "floppy%dsound=%d\n", i, p->dfxclick[i]);
	if (p->dfxclick[i] < 0 && p->dfxclickexternal[i][0])
	    cfgfile_write (f, "floppy%dsoundext=%s\n", i, p->dfxclickexternal[i]);
#endif
    }
    for (i = 0; i < MAX_SPARE_DRIVES; i++) {
	if (p->dfxlist[i][0])
	    cfgfile_write (f, "diskimage%d=%s\n", i, p->dfxlist[i]);
    }

    cfgfile_write (f, "nr_floppies=%d\n", p->nr_floppies);
    cfgfile_write (f, "floppy_speed=%d\n", p->floppy_speed);
#ifdef DRIVESOUND
    cfgfile_write (f, "floppy_volume=%d\n", p->dfxclickvolume);
#endif
    cfgfile_write (f, "parallel_on_demand=%s\n", p->parallel_demand ? "true" : "false");
    cfgfile_write (f, "serial_on_demand=%s\n", p->serial_demand ? "true" : "false");
    cfgfile_write (f, "serial_hardware_ctsrts=%s\n", p->serial_hwctsrts ? "true" : "false");
    cfgfile_write (f, "serial_direct=%s\n", p->serial_direct ? "true" : "false");
    cfgfile_write (f, "scsi=%s\n", p->scsi ? "true" : "false");
#ifndef WIN32
    cfgfile_write (f, "scsi_device=%s\n", p->scsi_device);
#endif

    cfgfile_write (f, "sound_output=%s\n", soundmode1[p->produce_sound]);
    cfgfile_write (f, "sound_bits=%d\n", p->sound_bits);
    cfgfile_write (f, "sound_channels=%s\n", stereomode[p->sound_stereo]);
    cfgfile_write (f, "sound_stereo_separation=%d\n", p->sound_stereo_separation);
    cfgfile_write (f, "sound_stereo_mixing_delay=%d\n", p->sound_mixed_stereo >= 0 ? p->sound_mixed_stereo : 0);

    cfgfile_write (f, "sound_frequency=%d\n", p->sound_freq);
    cfgfile_write (f, "sound_interpol=%s\n", interpolmode[p->sound_interpol]);
    cfgfile_write (f, "sound_adjust=%d\n", p->sound_adjust);
    cfgfile_write (f, "sound_volume=%d\n", p->sound_volume);
    cfgfile_write (f, "sound_latency=%d\n", p->sound_latency);

#ifdef JIT
    cfgfile_write (f, "comp_trustbyte=%s\n", compmode[p->comptrustbyte]);
    cfgfile_write (f, "comp_trustword=%s\n", compmode[p->comptrustword]);
    cfgfile_write (f, "comp_trustlong=%s\n", compmode[p->comptrustlong]);
    cfgfile_write (f, "comp_trustnaddr=%s\n", compmode[p->comptrustnaddr]);
    cfgfile_write (f, "comp_nf=%s\n", p->compnf ? "true" : "false");
    cfgfile_write (f, "comp_constjump=%s\n", p->comp_constjump ? "true" : "false");
    cfgfile_write (f, "comp_oldsegv=%s\n", p->comp_oldsegv ? "true" : "false");

    cfgfile_write (f, "comp_flushmode=%s\n", flushmode[p->comp_hardflush]);
    cfgfile_write (f, "compforcesettings=%s\n", p->compforcesettings ? "true" : "false");
    cfgfile_write (f, "compfpu=%s\n", p->compfpu ? "true" : "false");
    cfgfile_write (f, "cachesize=%d\n", p->cachesize);
#endif

    for (i = 0; i < 2; i++) {
	int v = i == 0 ? p->jport0 : p->jport1;
	char tmp1[100], tmp2[50];
	if (v < JSEM_JOYS) {
	    sprintf (tmp2, "kbd%d", v + 1);
	} else if (v < JSEM_MICE) {
	    sprintf (tmp2, "joy%d", v - JSEM_JOYS);
	} else if (v < JSEM_END) {
	    strcpy (tmp2, "mouse");
	    if (v - JSEM_MICE > 0)
		sprintf (tmp2, "mouse%d", v - JSEM_MICE);
	} else
	    sprintf (tmp2, "none");
	sprintf (tmp1, "joyport%d=%s\n", i, tmp2);
	cfgfile_write (f, tmp1);
    }

    cfgfile_write (f, "bsdsocket_emu=%s\n", p->socket_emu ? "true" : "false");

    cfgfile_write (f, "synchronize_clock=%s\n", p->tod_hack ? "yes" : "no");
    cfgfile_write (f, "maprom=0x%x\n", p->maprom);

    cfgfile_write (f, "gfx_framerate=%d\n", p->gfx_framerate);
    cfgfile_write (f, "gfx_width=%d\n", p->gfx_width_win); /* compatibility with old versions */
    cfgfile_write (f, "gfx_height=%d\n", p->gfx_height_win); /* compatibility with old versions */
    cfgfile_write (f, "gfx_width_windowed=%d\n", p->gfx_width_win);
    cfgfile_write (f, "gfx_height_windowed=%d\n", p->gfx_height_win);
    cfgfile_write (f, "gfx_width_fullscreen=%d\n", p->gfx_width_fs);
    cfgfile_write (f, "gfx_height_fullscreen=%d\n", p->gfx_height_fs);
    cfgfile_write (f, "gfx_refreshrate=%d\n", p->gfx_refreshrate);
    cfgfile_write (f, "gfx_vsync=%s\n", p->gfx_vsync ? "true" : "false");
    cfgfile_write (f, "gfx_lores=%s\n", p->gfx_lores ? "true" : "false");
    cfgfile_write (f, "gfx_linemode=%s\n", linemode1[p->gfx_linedbl]);
    cfgfile_write (f, "gfx_correct_aspect=%s\n", p->gfx_correct_aspect ? "true" : "false");
    cfgfile_write (f, "gfx_fullscreen_amiga=%s\n", p->gfx_afullscreen ? "true" : "false");
    cfgfile_write (f, "gfx_fullscreen_picasso=%s\n", p->gfx_pfullscreen ? "true" : "false");
    cfgfile_write (f, "gfx_center_horizontal=%s\n", centermode1[p->gfx_xcenter]);
    cfgfile_write (f, "gfx_center_vertical=%s\n", centermode1[p->gfx_ycenter]);
    cfgfile_write (f, "gfx_colour_mode=%s\n", colormode1[p->color_mode]);

#ifdef GFXFILTER
    if (p->gfx_filter > 0) {
	int i = 0;
	struct uae_filter *uf;
	while (uaefilters[i].name) {
	    uf = &uaefilters[i];
	    if (uf->type == p->gfx_filter) {
		cfgfile_write (f, "gfx_filter=%s\n", uf->cfgname);
		if (uf->type == p->gfx_filter) {
		    if (uf->x[0]) {
			cfgfile_write (f, "gfx_filter_mode=%s\n", filtermode1[p->gfx_filter_filtermode]);
		    } else {
			int mt[4], i = 0;
			if (uf->x[1]) mt[i++] = 1;
			if (uf->x[2]) mt[i++] = 2;
			if (uf->x[3]) mt[i++] = 3;
			if (uf->x[4]) mt[i++] = 4;
			cfgfile_write (f, "gfx_filter_mode=%dx\n", mt[p->gfx_filter_filtermode]);
		    }
		}
	    }
	    i++;
	}
    } else {
	cfgfile_write (f, "gfx_filter=no\n");
    }

    cfgfile_write (f, "gfx_filter_vert_zoom=%d\n", p->gfx_filter_vert_zoom);
    cfgfile_write (f, "gfx_filter_horiz_zoom=%d\n", p->gfx_filter_horiz_zoom);
    cfgfile_write (f, "gfx_filter_vert_offset=%d\n", p->gfx_filter_vert_offset);
    cfgfile_write (f, "gfx_filter_horiz_offset=%d\n", p->gfx_filter_horiz_offset);
    cfgfile_write (f, "gfx_filter_scanlines=%d\n", p->gfx_filter_scanlines);
    cfgfile_write (f, "gfx_filter_scanlinelevel=%d\n", p->gfx_filter_scanlinelevel);
    cfgfile_write (f, "gfx_filter_scanlineratio=%d\n", p->gfx_filter_scanlineratio);
#endif

    cfgfile_write (f, "immediate_blits=%s\n", p->immediate_blits ? "true" : "false");
    cfgfile_write (f, "ntsc=%s\n", p->ntscmode ? "true" : "false");
    cfgfile_write (f, "hide_cursor=%s\n", p->hide_cursor ? "true" : "false");
    cfgfile_write (f, "show_leds=%s\n", p->leds_on_screen ? "true" : "false");
    cfgfile_write (f, "keyboard_leds=numlock:%s,capslock:%s,scrolllock:%s\n",
	kbleds[p->keyboard_leds[0]], kbleds[p->keyboard_leds[1]], kbleds[p->keyboard_leds[2]]);
    if (p->chipset_mask & CSMASK_AGA)
	cfgfile_write (f, "chipset=aga\n");
    else if ((p->chipset_mask & CSMASK_ECS_AGNUS) && (p->chipset_mask & CSMASK_ECS_DENISE))
	cfgfile_write (f, "chipset=ecs\n");
    else if (p->chipset_mask & CSMASK_ECS_AGNUS)
	cfgfile_write (f, "chipset=ecs_agnus\n");
    else if (p->chipset_mask & CSMASK_ECS_DENISE)
	cfgfile_write (f, "chipset=ecs_denise\n");
    else
	cfgfile_write (f, "chipset=ocs\n");
    cfgfile_write (f, "collision_level=%s\n", collmode[p->collision_level]);

    cfgfile_write (f, "fastmem_size=%d\n", p->fastmem_size / 0x100000);
    cfgfile_write (f, "a3000mem_size=%d\n", p->a3000mem_size / 0x100000);
    cfgfile_write (f, "z3mem_size=%d\n", p->z3fastmem_size / 0x100000);
    cfgfile_write (f, "bogomem_size=%d\n", p->bogomem_size / 0x40000);
    cfgfile_write (f, "gfxcard_size=%d\n", p->gfxmem_size / 0x100000);
    cfgfile_write (f, "chipmem_size=%d\n", (p->chipmem_size == 0x40000) ? 0 : p->chipmem_size / 0x80000);

    if (p->m68k_speed > 0)
	cfgfile_write (f, "finegrain_cpu_speed=%d\n", p->m68k_speed);
    else
	cfgfile_write (f, "cpu_speed=%s\n", p->m68k_speed == -1 ? "max" : "real");

    cfgfile_write (f, "cpu_type=%s\n", cpumode[p->cpu_level * 2 + !p->address_space_24]);
    cfgfile_write (f, "cpu_compatible=%s\n", p->cpu_compatible ? "true" : "false");
    cfgfile_write (f, "cpu_cycle_exact=%s\n", p->cpu_cycle_exact ? "true" : "false");
    cfgfile_write (f, "blitter_cycle_exact=%s\n", p->blitter_cycle_exact ? "true" : "false");

    cfgfile_write (f, "log_illegal_mem=%s\n", p->illegal_mem ? "true" : "false");
    cfgfile_write (f, "catweasel_io=0x%x\n", p->catweasel_io);

    cfgfile_write (f, "kbd_lang=%s\n", (p->keyboard_lang == KBD_LANG_DE ? "de"
				  : p->keyboard_lang == KBD_LANG_DK ? "dk"
				  : p->keyboard_lang == KBD_LANG_ES ? "es"
				  : p->keyboard_lang == KBD_LANG_US ? "us"
				  : p->keyboard_lang == KBD_LANG_SE ? "se"
				  : p->keyboard_lang == KBD_LANG_FR ? "fr"
				  : p->keyboard_lang == KBD_LANG_IT ? "it"
				  : "FOO"));

#ifdef SAVESTATE
    cfgfile_write (f, "state_replay=%s\n", p->statecapture ? "yes" : "no");
    cfgfile_write (f, "state_replay_rate=%d\n", p->statecapturerate);
    cfgfile_write (f, "state_replay_buffer=%d\n", p->statecapturebuffersize);
#endif

#ifdef FILESYS
    write_filesys_config (currprefs.mountinfo, UNEXPANDED, prefs_get_attr ("hardfile_path"), f);
    if (p->filesys_no_uaefsdb)
	cfgfile_write (f, "filesys_no_fsdb=%s\n", p->filesys_no_uaefsdb ? "true" : "false");
#endif
    write_inputdevice_config (p, f);

    /* Don't write gfxlib/gfx_test_speed options.  */
}

int cfgfile_yesno (const char *option, const char *value, const char *name, int *location)
{
    if (strcmp (option, name) != 0)
	return 0;
    if (strcasecmp (value, "yes") == 0 || strcasecmp (value, "y") == 0
	|| strcasecmp (value, "true") == 0 || strcasecmp (value, "t") == 0)
	*location = 1;
    else if (strcasecmp (value, "no") == 0 || strcasecmp (value, "n") == 0
	|| strcasecmp (value, "false") == 0 || strcasecmp (value, "f") == 0)
	*location = 0;
    else {
	write_log ("Option `%s' requires a value of either `yes' or `no'.\n", option);
	return -1;
    }
    return 1;
}

int cfgfile_intval (const char *option, const char *value, const char *name, int *location, int scale)
{
    int base = 10;
    char *endptr;
    if (strcmp (option, name) != 0)
	return 0;
    /* I guess octal isn't popular enough to worry about here...  */
    if (value[0] == '0' && value[1] == 'x')
	value += 2, base = 16;
    *location = strtol (value, &endptr, base) * scale;

    if (*endptr != '\0' || *value == '\0') {
	write_log ("Option `%s' requires a numeric argument.\n", option);
	return -1;
    }
    return 1;
}

int cfgfile_strval (const char *option, const char *value, const char *name, int *location, const char *table[], int more)
{
    int val;
    if (strcmp (option, name) != 0)
	return 0;
    val = match_string (table, value);
    if (val == -1) {
	if (more)
	    return 0;

	write_log ("Unknown value for option `%s'.\n", option);
	return -1;
    }
    *location = val;
    return 1;
}

int cfgfile_string (const char *option, const char *value, const char *name, char *location, int maxsz)
{
    if (strcmp (option, name) != 0)
	return 0;
    strncpy (location, value, maxsz - 1);
    location[maxsz - 1] = '\0';
    return 1;
}

/*
 * Duplicate the path 'src'. If 'src' begins with '~/' substitue
 * the home directory.
 *
 * TODO: Clean this up.
 * TODO: Collect path handling tools in one place and cleanly
 * handle platform-specific differences.
 */
static const char *strdup_path_expand (const char *src)
{
    char *path = 0;
    unsigned int srclen, destlen;
    int need_separator = 0;
    const char *home = getenv ("HOME");

    srclen = strlen (src);

    if (srclen > 0) {
	if (src[srclen - 1] != '/' && src[srclen - 1] != '\\'
#ifdef TARGET_AMIGAOS
	    && src[srclen - 1] != ':'
#endif
	    ) {
	    need_separator = 1;
	}
    }

    destlen = srclen + need_separator;

    if (src[0] == '~' && src[1] == '/' && home) {
	destlen += srclen + strlen (home);
	src++;
	srclen--;
    } else
	home = 0;

    path = malloc (destlen + 1); path[0]=0;

    if (path) {
	if (home)
	    strcpy (path, home);

	strcat (path, src);

	if (need_separator)
	    strcat (path, "/");
    }

    return path;
}

static int cfgfile_path (const char *option, const char *value, const char *key)
{
    if (strcmp (option, key) == 0) {
	const char *path = strdup_path_expand (value);

	if (path)
	    prefs_set_attr (key, path);

	return 1;
    }
    return 0;
}

static int getintval (char **p, int *result, int delim)
{
    char *value = *p;
    int base = 10;
    char *endptr;
    char *p2 = strchr (*p, delim);

    if (p2 == 0)
	return 0;

    *p2++ = '\0';

    if (value[0] == '0' && value[1] == 'x')
	value += 2, base = 16;
    *result = strtol (value, &endptr, base);
    *p = p2;

    if (*endptr != '\0' || *value == '\0')
	return 0;

    return 1;
}

static int getintval2 (char **p, int *result, int delim)
{
    char *value = *p;
    int base = 10;
    char *endptr;
    char *p2 = strchr (*p, delim);

    if (p2 == 0) {
	p2 = strchr (*p, 0);
	if (p2 == 0) {
	    *p = 0;
	    return 0;
	}
    }
    if (*p2 != 0)
	*p2++ = '\0';

    if (value[0] == '0' && value[1] == 'x')
	value += 2, base = 16;
    *result = strtol (value, &endptr, base);
    *p = p2;

    if (*endptr != '\0' || *value == '\0') {
	*p = 0;
	return 0;
    }

    return 1;
}

static void set_chipset_mask (struct uae_prefs *p, int val)
{
    p->chipset_mask = (val == 0 ? 0
		       : val == 1 ? CSMASK_ECS_AGNUS
		       : val == 2 ? CSMASK_ECS_DENISE
		       : val == 3 ? CSMASK_ECS_DENISE | CSMASK_ECS_AGNUS
		       : CSMASK_AGA | CSMASK_ECS_DENISE | CSMASK_ECS_AGNUS);
}

static int cfgfile_parse_host (struct uae_prefs *p, char *option, char *value)
{
    int i;
    char *section = 0;
    char *tmpp;
    char tmpbuf[CONFIG_BLEN];

    if (memcmp (option, "input.", 6) == 0) {
	read_inputdevice_config (p, option, value);
	return 1;
    }

    for (tmpp = option; *tmpp != '\0'; tmpp++)
	if (isupper (*tmpp))
	    *tmpp = tolower (*tmpp);
    tmpp = strchr (option, '.');
    if (tmpp) {
	section = option;
	option = tmpp + 1;
	*tmpp = '\0';
	if (strcmp (section, TARGET_NAME) == 0) {
	    /* We special case the various path options here.  */
	    if (cfgfile_path (option, value, "rom_path"))
		return 1;
	    if (cfgfile_path (option, value, "floppy_path"))
		return 1;
	    if (cfgfile_path (option, value, "hardfile_path"))
		return 1;
#ifdef SAVESTATE
            if (cfgfile_path (option, value, "savestate_path"))
		return 1;
#endif
#ifndef _WIN32
	    if (cfgfile_intval (option, value, "cpu_idle", &p->cpu_idle, 1))
		return 1;
#endif
	    if (target_parse_option (p, option, value))
		return 1;
	}
	if (strcmp (section, MACHDEP_NAME) == 0)
	    return machdep_parse_option (p, option, value);
	if (strcmp (section, GFX_NAME) == 0)
	    return gfx_parse_option (p, option, value);
	if (strcmp (section, AUDIO_NAME) == 0)
	    return audio_parse_option (p, option, value);

	return 0;
    }
    for (i = 0; i < MAX_SPARE_DRIVES; i++) {
	sprintf (tmpbuf, "diskimage%d", i);
	if (cfgfile_string (option, value, tmpbuf, p->dfxlist[i], 256)) {
#if 0
	    if (i < 4 && !p->df[i][0])
		strcpy (p->df[i], p->dfxlist[i]);
#endif
	    return 1;
	}
    }

    if (cfgfile_intval (option, value, "sound_latency", &p->sound_latency, 1)
	|| cfgfile_intval (option, value, "sound_bits", &p->sound_bits, 1)
	|| cfgfile_intval (option, value, "sound_frequency", &p->sound_freq, 1)
	|| cfgfile_intval (option, value, "sound_adjust", &p->sound_adjust, 1)
	|| cfgfile_intval (option, value, "sound_volume", &p->sound_volume, 1)
	|| cfgfile_intval (option, value, "sound_stereo_separation", &p->sound_stereo_separation, 1)
	|| cfgfile_intval (option, value, "sound_stereo_mixing_delay", &p->sound_mixed_stereo, 1)

	|| cfgfile_intval (option, value, "gfx_framerate", &p->gfx_framerate, 1)
	|| cfgfile_intval (option, value, "gfx_width_windowed", &p->gfx_width_win, 1)
	|| cfgfile_intval (option, value, "gfx_height_windowed", &p->gfx_height_win, 1)
	|| cfgfile_intval (option, value, "gfx_width_fullscreen", &p->gfx_width_fs, 1)
	|| cfgfile_intval (option, value, "gfx_height_fullscreen", &p->gfx_height_fs, 1)
	|| cfgfile_intval (option, value, "gfx_refreshrate", &p->gfx_refreshrate, 1))
	return 1;

#ifdef GFXFILTER
    if (   cfgfile_intval (option, value, "gfx_filter_vert_zoom", &p->gfx_filter_vert_zoom, 1)
	|| cfgfile_intval (option, value, "gfx_filter_horiz_zoom", &p->gfx_filter_horiz_zoom, 1)
	|| cfgfile_intval (option, value, "gfx_filter_vert_offset", &p->gfx_filter_vert_offset, 1)
	|| cfgfile_intval (option, value, "gfx_filter_horiz_offset", &p->gfx_filter_horiz_offset, 1)
	|| cfgfile_intval (option, value, "gfx_filter_scanlines", &p->gfx_filter_scanlines, 1)
	|| cfgfile_intval (option, value, "gfx_filter_scanlinelevel", &p->gfx_filter_scanlinelevel, 1)
	|| cfgfile_intval (option, value, "gfx_filter_scanlineratio", &p->gfx_filter_scanlineratio, 1))
	return 1;
#endif

#ifdef SAVESTATE
    if    (cfgfile_intval (option, value, "state_replay_rate", &p->statecapturerate, 1)
	|| cfgfile_intval (option, value, "state_replay_buffer", &p->statecapturebuffersize, 1)
	|| cfgfile_yesno  (option, value, "state_replay", &p->statecapture))
	return 1;
#endif

#ifdef DRIVESOUND
    if    (cfgfile_intval (option, value, "floppy0sound", &p->dfxclick[0], 1)
	|| cfgfile_intval (option, value, "floppy1sound", &p->dfxclick[1], 1)
	|| cfgfile_intval (option, value, "floppy2sound", &p->dfxclick[2], 1)
	|| cfgfile_intval (option, value, "floppy3sound", &p->dfxclick[3], 1)
	|| cfgfile_intval (option, value, "floppy_volume", &p->dfxclickvolume, 1)
	|| cfgfile_string (option, value, "floppy0soundext", p->dfxclickexternal[0], 256)
	|| cfgfile_string (option, value, "floppy1soundext", p->dfxclickexternal[1], 256)
	|| cfgfile_string (option, value, "floppy2soundext", p->dfxclickexternal[2], 256)
	|| cfgfile_string (option, value, "floppy3soundext", p->dfxclickexternal[3], 256))
	return 1;
#endif

    if    (cfgfile_string (option, value, "config_info", p->info, 256)
	|| cfgfile_string (option, value, "config_description", p->description, 256))
        return 1;

#ifdef DEBUGGER
    if    (cfgfile_yesno (option, value, "use_debugger", &p->start_debugger))
        return 1;
#endif

    if    (cfgfile_yesno (option, value, "log_illegal_mem", &p->illegal_mem)
	|| cfgfile_yesno (option, value, "filesys_no_fsdb", &p->filesys_no_uaefsdb)
	|| cfgfile_yesno (option, value, "gfx_vsync", &p->gfx_vsync)
	|| cfgfile_yesno (option, value, "gfx_lores", &p->gfx_lores)
	|| cfgfile_yesno (option, value, "gfx_correct_aspect", &p->gfx_correct_aspect)
	|| cfgfile_yesno (option, value, "gfx_fullscreen_amiga", &p->gfx_afullscreen)
	|| cfgfile_yesno (option, value, "gfx_fullscreen_picasso", &p->gfx_pfullscreen)
	|| cfgfile_yesno (option, value, "hide_cursor", &p->hide_cursor)
	|| cfgfile_yesno (option, value, "show_leds", &p->leds_on_screen)
	|| cfgfile_yesno (option, value, "synchronize_clock", &p->tod_hack)
	|| cfgfile_yesno (option, value, "bsdsocket_emu", &p->socket_emu))
	return 1;

    if (cfgfile_strval (option, value, "sound_output", &p->produce_sound, soundmode1, 1)
	|| cfgfile_strval (option, value, "sound_output", &p->produce_sound, soundmode2, 0)
	|| cfgfile_strval (option, value, "sound_interpol", &p->sound_interpol, interpolmode, 0)
	|| cfgfile_strval (option, value, "use_gui", &p->start_gui, guimode1, 1)
	|| cfgfile_strval (option, value, "use_gui", &p->start_gui, guimode2, 1)
	|| cfgfile_strval (option, value, "use_gui", &p->start_gui, guimode3, 0)
	|| cfgfile_strval (option, value, "gfx_linemode", &p->gfx_linedbl, linemode1, 1)
	|| cfgfile_strval (option, value, "gfx_linemode", &p->gfx_linedbl, linemode2, 0)
	|| cfgfile_strval (option, value, "gfx_center_horizontal", &p->gfx_xcenter, centermode1, 1)
	|| cfgfile_strval (option, value, "gfx_center_vertical", &p->gfx_ycenter, centermode1, 1)
	|| cfgfile_strval (option, value, "gfx_center_horizontal", &p->gfx_xcenter, centermode2, 0)
	|| cfgfile_strval (option, value, "gfx_center_vertical", &p->gfx_ycenter, centermode2, 0)
	|| cfgfile_strval (option, value, "gfx_colour_mode", &p->color_mode, colormode1, 1)
	|| cfgfile_strval (option, value, "gfx_colour_mode", &p->color_mode, colormode2, 0)
	|| cfgfile_strval (option, value, "gfx_color_mode", &p->color_mode, colormode1, 1)
	|| cfgfile_strval (option, value, "gfx_color_mode", &p->color_mode, colormode2, 0))
	return 1;


#ifdef GFXFILTER
    if (strcmp (option,"gfx_filter") == 0) {
	int i = 0;
	p->gfx_filter = 0;
	while(uaefilters[i].name) {
	    if (!strcmp (uaefilters[i].cfgname, value)) {
		p->gfx_filter = uaefilters[i].type;
		break;
	    }
	    i++;
	}
	return 1;
    }
    if (strcmp (option,"gfx_filter_mode") == 0) {
	p->gfx_filter_filtermode = 0;
	if (p->gfx_filter > 0) {
	    struct uae_filter *uf;
	    int i = 0;
	    while(uaefilters[i].name) {
		uf = &uaefilters[i];
		if (uf->type == p->gfx_filter) {
		    if (uf->x[0]) {
			cfgfile_strval (option, value, "gfx_filter_mode", &p->gfx_filter_filtermode, filtermode1, 0);
		    } else {
			int mt[4], j;
			i = 0;
			if (uf->x[1]) mt[i++] = 1;
			if (uf->x[2]) mt[i++] = 2;
			if (uf->x[3]) mt[i++] = 3;
			if (uf->x[4]) mt[i++] = 4;
			cfgfile_strval (option, value, "gfx_filter_mode", &i, filtermode2, 0);
			for (j = 0; j < i; j++) {
			    if (mt[j] == i)
				p->gfx_filter_filtermode = j;
			}
		    }
		    break;
		}
		i++;
	    }
	}
	return 1;
    }
#endif

    if (strcmp (option, "gfx_width") == 0 || strcmp (option, "gfx_height") == 0) {
	cfgfile_intval (option, value, "gfx_width", &p->gfx_width_win, 1);
	cfgfile_intval (option, value, "gfx_height", &p->gfx_height_win, 1);
	p->gfx_width_fs = p->gfx_width_win;
	p->gfx_height_fs = p->gfx_height_win;
	return 1;
    }

    if (strcmp (option, "joyport0") == 0 || strcmp (option, "joyport1") == 0) {
	int port = strcmp (option, "joyport0") == 0 ? 0 : 1;
	int start = 0;
	char *pp = 0;

	if (port)
	   p->jport1 = JSEM_NONE;
	else
	   p->jport0 = JSEM_NONE;

	if (strncmp (value, "kbd", 3) == 0) {
	    start = JSEM_KBDLAYOUT;
	    pp = value + 3;
	} else if (strncmp (value, "joy", 3) == 0) {
	    start = JSEM_JOYS;
	    pp = value + 3;
	} else if (strncmp (value, "mouse", 5) == 0) {
	    start = JSEM_MICE;
	    pp = value + 5;
	}
	if (pp) {
	    int v = atol (pp);
	    if (start >= 0) {
		if (start == JSEM_KBDLAYOUT)
		    v--;
		if (v >= 0) {
		    start += v;
		    if (port)
			p->jport1 = start;
		    else
			p->jport0 = start;
		}
	    }
	}
	return 1;
    }

#ifdef SAVESTATE
    if (cfgfile_string (option, value, "statefile", tmpbuf, sizeof (tmpbuf))) {
	savestate_state = STATE_DORESTORE;
	strcpy (savestate_fname, tmpbuf);
	return 1;
    }
#endif

    if (cfgfile_strval (option, value, "sound_channels", &p->sound_stereo, stereomode, 1)) {
	if (p->sound_stereo == 3) { /* "mixed stereo" compatibility hack */
	    p->sound_stereo = 1;
	    p->sound_mixed_stereo = 5;
	    p->sound_stereo_separation = 7;
	}
	return 1;
    }

    if (strcmp (option, "kbd_lang") == 0) {
	KbdLang l;
	if ((l = KBD_LANG_DE, strcasecmp (value, "de") == 0)
	    || (l = KBD_LANG_DK, strcasecmp (value, "dk") == 0)
	    || (l = KBD_LANG_SE, strcasecmp (value, "se") == 0)
	    || (l = KBD_LANG_US, strcasecmp (value, "us") == 0)
	    || (l = KBD_LANG_FR, strcasecmp (value, "fr") == 0)
	    || (l = KBD_LANG_IT, strcasecmp (value, "it") == 0)
	    || (l = KBD_LANG_ES, strcasecmp (value, "es") == 0))
	    p->keyboard_lang = l;
	else
	    write_log ("Unknown keyboard language\n");
	return 1;
    }

    if (cfgfile_string (option, value, "config_version", tmpbuf, sizeof (tmpbuf))) {
	char *tmpp2;
	tmpp = strchr (value, '.');
	if (tmpp) {
	    *tmpp++ = 0;
	    tmpp2 = tmpp;
	    p->config_version = atol (tmpbuf) << 16;
	    tmpp = strchr (tmpp, '.');
	    if (tmpp) {
		*tmpp++ = 0;
		p->config_version |= atol (tmpp2) << 8;
		p->config_version |= atol (tmpp);
	    }
	}
	return 1;
    }

    if (cfgfile_string (option, value, "keyboard_leds", tmpbuf, sizeof (tmpbuf))) {
	char *tmpp2 = tmpbuf;
	int i, num;
	p->keyboard_leds[0] = p->keyboard_leds[1] = p->keyboard_leds[2] = 0;
	p->keyboard_leds_in_use = 0;
	strcat (tmpbuf, ",");
	for (i = 0; i < 3; i++) {
	    tmpp = strchr (tmpp2, ':');
	    if (!tmpp)
		break;
	    *tmpp++= 0;
	    num = -1;
	    if (!strcasecmp (tmpp2, "numlock")) num = 0;
	    if (!strcasecmp (tmpp2, "capslock")) num = 1;
	    if (!strcasecmp (tmpp2, "scrolllock")) num = 2;
	    tmpp2 = tmpp;
	    tmpp = strchr (tmpp2, ',');
	    if (!tmpp)
		break;
	    *tmpp++= 0;
	    if (num >= 0) {
		p->keyboard_leds[num] = match_string (kbleds, tmpp2);
		if (p->keyboard_leds[num]) p->keyboard_leds_in_use = 1;
	    }
	    tmpp2 = tmpp;
	}
	return 1;
    }

    return 0;
}

static int cfgfile_parse_hardware (struct uae_prefs *p, char *option, char *value)
{
    int tmpval, dummy, i;
    char tmpbuf[CONFIG_BLEN];

    if (cfgfile_yesno (option, value, "immediate_blits", &p->immediate_blits)
	|| cfgfile_yesno (option, value, "kickshifter", &p->kickshifter)
	|| cfgfile_yesno (option, value, "ntsc", &p->ntscmode)
	|| cfgfile_yesno (option, value, "cpu_compatible", &p->cpu_compatible)
	|| cfgfile_yesno (option, value, "cpu_cycle_exact", &p->cpu_cycle_exact)
	|| cfgfile_yesno (option, value, "blitter_cycle_exact", &p->blitter_cycle_exact)
	|| cfgfile_yesno (option, value, "cpu_24bit_addressing", &p->address_space_24)
	|| cfgfile_yesno (option, value, "parallel_on_demand", &p->parallel_demand)
	|| cfgfile_yesno (option, value, "serial_on_demand", &p->serial_demand)
	|| cfgfile_yesno (option, value, "serial_hardware_ctsrts", &p->serial_hwctsrts)
	|| cfgfile_yesno (option, value, "serial_direct", &p->serial_direct)
#ifdef JIT
	|| cfgfile_yesno (option, value, "comp_nf", &p->compnf)
	|| cfgfile_yesno (option, value, "comp_constjump", &p->comp_constjump)
	|| cfgfile_yesno (option, value, "comp_oldsegv", &p->comp_oldsegv)
	|| cfgfile_yesno (option, value, "compforcesettings", &p->compforcesettings)
	|| cfgfile_yesno (option, value, "compfpu", &p->compfpu)
#endif
	|| cfgfile_yesno (option, value, "scsi", &p->scsi))
	return 1;

    if (cfgfile_intval (option, value, "fastmem_size", (int *)&p->fastmem_size, 1)) {
	write_log(" * fastmem_size=%i \0", p->fastmem_size);
	p->fastmem_size *= 0x100000;	
	return 1;
    }	
    if (cfgfile_intval (option, value, "bogomem_size", (int *)&p->bogomem_size, 1)) {
	write_log(" * bogomem_size=%i \0", p->bogomem_size);
	p->bogomem_size *= 0x40000;
	return 1;
    }

    if (
	
	cfgfile_intval (option, value, "a3000mem_size", (int *)&p->a3000mem_size, 0x100000)
	|| cfgfile_intval (option, value, "z3mem_size", (int *)&p->z3fastmem_size, 0x100000)
	|| cfgfile_intval (option, value, "gfxcard_size", (int *)&p->gfxmem_size, 0x100000)
	|| cfgfile_intval (option, value, "floppy_speed", &p->floppy_speed, 1)
	|| cfgfile_intval (option, value, "nr_floppies", &p->nr_floppies, 1)
	|| cfgfile_intval (option, value, "floppy0type", &p->dfxtype[0], 1)
	|| cfgfile_intval (option, value, "floppy1type", &p->dfxtype[1], 1)
	|| cfgfile_intval (option, value, "floppy2type", &p->dfxtype[2], 1)
	|| cfgfile_intval (option, value, "floppy3type", &p->dfxtype[3], 1)
	|| cfgfile_intval (option, value, "maprom", (int *)&p->maprom, 1)
	|| cfgfile_intval (option, value, "catweasel_io", &p->catweasel_io, 1))
	return 1;
#ifdef JIT
    if (cfgfile_intval (option, value, "cachesize", &p->cachesize, 1)
# ifdef NATMEM_OFFSET
	|| cfgfile_strval (option, value, "comp_trustbyte",  &p->comptrustbyte,  compmode, 0)
	|| cfgfile_strval (option, value, "comp_trustword",  &p->comptrustword,  compmode, 0)
	|| cfgfile_strval (option, value, "comp_trustlong",  &p->comptrustlong,  compmode, 0)
	|| cfgfile_strval (option, value, "comp_trustnaddr", &p->comptrustnaddr, compmode, 0)
# else
	|| cfgfile_strval (option, value, "comp_trustbyte",  &p->comptrustbyte,  compmode, 1)
	|| cfgfile_strval (option, value, "comp_trustword",  &p->comptrustword,  compmode, 1)
	|| cfgfile_strval (option, value, "comp_trustlong",  &p->comptrustlong,  compmode, 1)
	|| cfgfile_strval (option, value, "comp_trustnaddr", &p->comptrustnaddr, compmode, 1)
# endif
	|| cfgfile_strval (option, value, "comp_flushmode", &p->comp_hardflush, flushmode, 0))
	return 1;
#endif
    if (cfgfile_strval (option, value, "collision_level", &p->collision_level, collmode, 0))
        return 1;
    if (cfgfile_string (option, value, "kickstart_rom_file", p->romfile, 256)
	|| cfgfile_string (option, value, "kickstart_ext_rom_file", p->romextfile, 256)
	|| cfgfile_string (option, value, "kickstart_key_file", p->keyfile, 256)
	|| cfgfile_string (option, value, "flash_file", p->flashfile, 256)
#ifdef ACTION_REPLAY
	|| cfgfile_string (option, value, "cart_file", p->cartfile, 256)
#endif
#ifndef WIN32
	|| cfgfile_string (option, value, "scsi_device", p->scsi_device, 256)
#endif
	)
	return 1;

    for (i = 0; i < 4; i++) {
	sprintf (tmpbuf, "floppy%d", i);
	if (cfgfile_string (option, value, tmpbuf, p->df[i], 256)) {
#if 0
	    strcpy (p->dfxlist[i], p->df[i]);
#endif
	    return 1;
	}
    }

    if (cfgfile_intval (option, value, "chipmem_size", &dummy, 1)) {
	write_log(" * chipmem_size=%i \0", dummy);
	if (!dummy)
	    p->chipmem_size = 0x40000;
	else
	    p->chipmem_size = dummy * 0x80000;
	return 1;
    }

    if (cfgfile_strval (option, value, "chipset", &tmpval, csmode, 0)) {
	set_chipset_mask (p, tmpval);
	return 1;
    }

    if (cfgfile_strval (option, value, "cpu_type", &p->cpu_level, cpumode, 0)) {
	p->address_space_24 = p->cpu_level < 8 && !(p->cpu_level & 1);
	p->cpu_level >>= 1;
	return 1;
    }
    if (p->config_version < (21 << 16)) {
	if (cfgfile_strval (option, value, "cpu_speed", &p->m68k_speed, speedmode, 1)
	    /* Broken earlier versions used to write this out as a string.  */
	    || cfgfile_strval (option, value, "finegraincpu_speed", &p->m68k_speed, speedmode, 1))
	{
	    p->m68k_speed--;
	    return 1;
	}
    }

    if (cfgfile_intval (option, value, "cpu_speed", &p->m68k_speed, 1)) {
	p->m68k_speed *= CYCLE_UNIT;
	return 1;
    }

    if (cfgfile_intval (option, value, "finegrain_cpu_speed", &p->m68k_speed, 1)) {
	if (OFFICIAL_CYCLE_UNIT > CYCLE_UNIT) {
	    int factor = OFFICIAL_CYCLE_UNIT / CYCLE_UNIT;
	    p->m68k_speed = (p->m68k_speed + factor - 1) / factor;
	}
	if (strcasecmp (value, "max") == 0)
	    p->m68k_speed = -1;
	return 1;
    }

    if (strcmp (option, "filesystem") == 0
	|| strcmp (option, "hardfile") == 0)
    {
	int secs, heads, reserved, bs, ro;
	char *aname, *root;
	char *tmpp = strchr (value, ',');

	if (config_newfilesystem)
	    return 1;

	if (tmpp == 0)
	    goto invalid_fs;

	*tmpp++ = '\0';
	if (strcmp (value, "1") == 0 || strcasecmp (value, "ro") == 0
	    || strcasecmp (value, "readonly") == 0
	    || strcasecmp (value, "read-only") == 0)
	    ro = 1;
	else if (strcmp (value, "0") == 0 || strcasecmp (value, "rw") == 0
		 || strcasecmp (value, "readwrite") == 0
		 || strcasecmp (value, "read-write") == 0)
	    ro = 0;
	else
	    goto invalid_fs;
	secs = 0; heads = 0; reserved = 0; bs = 0;

	value = tmpp;
	if (strcmp (option, "filesystem") == 0) {
	    tmpp = strchr (value, ':');
	    if (tmpp == 0)
		goto invalid_fs;
	    *tmpp++ = '\0';
	    aname = value;
	    root = tmpp;
	} else {
	    if (! getintval (&value, &secs, ',')
		|| ! getintval (&value, &heads, ',')
		|| ! getintval (&value, &reserved, ',')
		|| ! getintval (&value, &bs, ','))
		goto invalid_fs;
	    root = value;
	    aname = 0;
	}
#ifdef FILESYS
	{
	    const char *err_msg;
	    char *str;

	    str = cfgfile_subst_path (UNEXPANDED, prefs_get_attr ("hardfile_path"), root);
	    err_msg = add_filesys_unit (currprefs.mountinfo, 0, aname, str, ro, secs,
					heads, reserved, bs, 0, 0, 0);

	    if (err_msg)
		write_log ("Error: %s\n", err_msg);

	    free (str);
        }
#endif
	return 1;
    }

    if (strcmp (option, "filesystem2") == 0
	|| strcmp (option, "hardfile2") == 0)
    {
	int secs, heads, reserved, bs, ro, bp;
	char *dname, *aname, *root, *fs;
	char *tmpp = strchr (value, ',');

	config_newfilesystem = 1;
	if (tmpp == 0)
	    goto invalid_fs;

	*tmpp++ = '\0';
	if (strcasecmp (value, "ro") == 0)
	    ro = 1;
	else if (strcasecmp (value, "rw") == 0)
	    ro = 0;
	else
	    goto invalid_fs;
	secs = 0; heads = 0; reserved = 0; bs = 0; bp = 0;
	fs = 0;

	value = tmpp;
	if (strcmp (option, "filesystem2") == 0) {
	    tmpp = strchr (value, ':');
	    if (tmpp == 0)
		goto invalid_fs;
	    *tmpp++ = 0;
	    dname = value;
	    aname = tmpp;
	    tmpp = strchr (tmpp, ':');
	    if (tmpp == 0)
		goto invalid_fs;
	    *tmpp++ = 0;
	    root = tmpp;
	    tmpp = strchr (tmpp, ',');
	    if (tmpp == 0)
		goto invalid_fs;
	    *tmpp++ = 0;
	    if (! getintval (&tmpp, &bp, 0))
		goto invalid_fs;
	} else {
	    tmpp = strchr (value, ':');
	    if (tmpp == 0)
		goto invalid_fs;
	    *tmpp++ = '\0';
	    dname = value;
	    root = tmpp;
	    tmpp = strchr (tmpp, ',');
	    if (tmpp == 0)
		goto invalid_fs;
	    *tmpp++ = 0;
	    aname = 0;
	    if (! getintval (&tmpp, &secs, ',')
		|| ! getintval (&tmpp, &heads, ',')
		|| ! getintval (&tmpp, &reserved, ',')
		|| ! getintval (&tmpp, &bs, ','))
		goto invalid_fs;
	    if (getintval2 (&tmpp, &bp, ',')) {
		fs = tmpp;
		tmpp = strchr (tmpp, ',');
		if (tmpp != 0)
		    *tmpp = 0;
	    }
	}
#ifdef FILESYS
	{
	    const char *err_msg;
	    char *str;

	    str = cfgfile_subst_path (UNEXPANDED, prefs_get_attr ("hardfile_path"), root);
	    err_msg = add_filesys_unit (currprefs.mountinfo, dname, aname, str, ro, secs,
					heads, reserved, bs, bp, fs, 0);

	    if (err_msg)
		write_log ("Error: %s\n", err_msg);

	    free (str);
        }
#endif
	return 1;

      invalid_fs:
	write_log ("Invalid filesystem/hardfile specification.\n");
	return 1;
    }

    return 0;
}

int cfgfile_parse_option (struct uae_prefs *p, char *option, char *value, int type)
{
    if (!strcmp (option, "config_hardware"))
	return 1;
    if (!strcmp (option, "config_host"))
	return 1;
    if (type == 0 || (type & CONFIG_TYPE_HARDWARE)) {
	if (cfgfile_parse_hardware (p, option, value))
	    return 1;
    }
    if (type == 0 || (type & CONFIG_TYPE_HOST)) {
	if (cfgfile_parse_host (p, option, value))
	    return 1;
    }
    if (type > 0)
	return 1;
    return 0;
}

static int cfgfile_separate_line (char *line, char *line1b, char *line2b)
{
    char *line1, *line2;
    int i;

    line1 = line;
    line2 = strchr (line, '=');
    if (! line2) {
	write_log ("CFGFILE: line was incomplete with only %s\n", line1);
	return 0;
    }
    *line2++ = '\0';
    strcpy (line1b, line1);
    strcpy (line2b, line2);

    /* Get rid of whitespace.  */
    i = strlen (line2);
    while (i > 0 && (line2[i - 1] == '\t' || line2[i - 1] == ' '
		     || line2[i - 1] == '\r' || line2[i - 1] == '\n'))
	line2[--i] = '\0';
    line2 += strspn (line2, "\t \r\n");
    strcpy (line2b, line2);
    i = strlen (line);
    while (i > 0 && (line[i - 1] == '\t' || line[i - 1] == ' '
		     || line[i - 1] == '\r' || line[i - 1] == '\n'))
	line[--i] = '\0';
    line += strspn (line, "\t \r\n");
    strcpy (line1b, line);
    return 1;
}

static int isobsolete (char *s)
{
    int i = 0;
    while (obsolete[i]) {
	if (!strcasecmp (s, obsolete[i])) {
	    write_log ("obsolete config entry '%s'\n", s);
	    return 1;
	}
	i++;
    }
    if (strlen (s) >= 10 && !memcmp (s, "gfx_opengl", 10)) {
	write_log ("obsolete config entry '%s\n", s);
	return 1;
    }
    if (strlen (s) >= 6 && !memcmp (s, "gfx_3d", 6)) {
	write_log ("obsolete config entry '%s\n", s);
	return 1;
    }
    return 0;
}

static void cfgfile_parse_separated_line (struct uae_prefs *p, char *line1b, char *line2b, int type)
{
    char line3b[CONFIG_BLEN], line4b[CONFIG_BLEN];
    struct strlist *sl;
    int ret;

    strcpy (line3b, line1b);
    strcpy (line4b, line2b);
    ret = cfgfile_parse_option (p, line1b, line2b, type);
    if (!isobsolete (line3b)) {
	for (sl = p->all_lines; sl; sl = sl->next) {
	    if (sl->option && !strcasecmp (line1b, sl->option)) break;
	}
	if (!sl) {
	    struct strlist *u = xcalloc (sizeof (struct strlist), 1);
	    u->option = my_strdup(line3b);
	    u->value = my_strdup(line4b);
	    u->next = p->all_lines;
	    p->all_lines = u;
	    if (!ret) {
		u->unknown = 1;
		write_log ("unknown config entry: '%s=%s'\n", u->option, u->value);
	    }
	}
    }
}

void cfgfile_parse_line (struct uae_prefs *p, char *line, int type)
{
    char line1b[CONFIG_BLEN], line2b[CONFIG_BLEN];

    if (!cfgfile_separate_line (line, line1b, line2b))
	return;
    cfgfile_parse_separated_line (p, line1b, line2b, type);
    return;
}

static void subst (const char *p, char *f, int n)
{
    char *str = cfgfile_subst_path (UNEXPANDED, p, f);
    strncpy (f, str, n - 1);
    f[n - 1] = '\0';
    free (str);
}

static char *cfg_fgets (char *line, int max, FILE *fh)
{
#ifdef SINGLEFILE
    extern char singlefile_config[];
    static char *sfile_ptr;
    char *p;
#endif

    if (fh)
	return fgets (line, max, fh);
#ifdef SINGLEFILE
    if (sfile_ptr == 0) {
	sfile_ptr = singlefile_config;
	if (*sfile_ptr) {
	    write_log ("singlefile config found\n");
	    while (*sfile_ptr++);
	}
    }
    if (*sfile_ptr == 0) {
	sfile_ptr = singlefile_config;
	return 0;
    }
    p = sfile_ptr;
    while (*p != 13 && *p != 10 && *p != 0) p++;
    memset (line, 0, max);
    memcpy (line, sfile_ptr, p - sfile_ptr);
    sfile_ptr = p + 1;
    if (*sfile_ptr == 13)
	sfile_ptr++;
    if (*sfile_ptr == 10)
	sfile_ptr++;
    return line;
#endif
    return 0;
}

static int cfgfile_load_2 (struct uae_prefs *p, const char *filename, int real, int *type)
{
    int i;
    FILE *fh;
    char line[CONFIG_BLEN], line1b[CONFIG_BLEN], line2b[CONFIG_BLEN];
    struct strlist *sl;
    int type1 = 0, type2 = 0, askedtype = 0;

    if (type) {
	askedtype = *type;
	*type = 0;
    }
    if (real) {
	p->config_version = 0;
	config_newfilesystem = 0;
	reset_inputdevice_config (p);
    }

    write_log ("Opening cfgfile '%s'...", filename);
    fh = fopen (filename, "r");
#ifndef SINGLEFILE
    if (! fh) {
        write_log ("failed\n");
	return 0;
    }
#endif
    write_log ("okay.\n");

    while (cfg_fgets (line, sizeof (line), fh) != 0) {
	int len = strlen (line);
	/* Delete trailing whitespace.  */
	while (len > 0 && strcspn (line + len - 1, "\t \r\n") == 0)
	    line[--len] = '\0';
	if (strlen (line) > 0) {
	    if (line[0] == '#' || line[0] == ';')
		continue;
	    if (!cfgfile_separate_line (line, line1b, line2b))
		continue;
	    type1 = type2 = 0;
	    if (cfgfile_yesno (line1b, line2b, "config_hardware", &type1) ||
		cfgfile_yesno (line1b, line2b, "config_host", &type2)) {
		if (type1 && type)
		    *type |= CONFIG_TYPE_HARDWARE;
		if (type2 && type)
		    *type |= CONFIG_TYPE_HOST;
		continue;
	    }
	    if (real) {
		cfgfile_parse_separated_line (p, line1b, line2b, askedtype);
	    } else {
		cfgfile_string (line1b, line2b, "config_description", p->description, 128);
	    }
	}
    }

    if (type && *type == 0)
	*type = CONFIG_TYPE_HARDWARE | CONFIG_TYPE_HOST;
    if (fh)
	fclose (fh);

    if (!real)
	return 1;

    for (sl = temp_lines; sl; sl = sl->next) {
	sprintf (line, "%s=%s", sl->option, sl->value);
	cfgfile_parse_line (p, line, 0);
    }

    for (i = 0; i < 4; i++)
	subst (prefs_get_attr("floppy_path"), p->df[i], sizeof p->df[i]);

    subst (prefs_get_attr("rom_path"), p->romfile, sizeof p->romfile);
    subst (prefs_get_attr("rom_path"), p->romextfile, sizeof p->romextfile);
    subst (prefs_get_attr("rom_path"), p->keyfile, sizeof p->keyfile);

    return 1;
}

int cfgfile_load (struct uae_prefs *p, const char *filename, int *type)
{
    return cfgfile_load_2 (p, filename, 1, type);
}

int cfgfile_save (const struct uae_prefs *p, const char *filename, int type)
{
    FILE *fh = fopen (filename, "w");
    write_log ("save config '%s'\n", filename);
    if (! fh)
	return 0;

    if (!type)
	type = CONFIG_TYPE_HARDWARE | CONFIG_TYPE_HOST;
    save_options (fh, p, type);
    fclose (fh);
    return 1;
}

int cfgfile_get_description (const char *filename, char *description, int *type)
{
    int result = 0;
    struct uae_prefs *p = xmalloc (sizeof (struct uae_prefs));
    p->description[0] = 0;
    if (cfgfile_load_2 (p, filename, 0, type)) {
	result = 1;
	if (description)
	    strcpy (description, p->description);
    }
    free (p);
    return result;
}

void cfgfile_show_usage (void)
{
    unsigned int i;
    write_log ("UAE Configuration Help:\n" \
	       "=======================\n");
    for (i = 0; i < sizeof opttable / sizeof *opttable; i++)
	write_log ("%*s: %s\n", -MAX_OPTION_KEY_LEN, opttable[i].config_label, opttable[i].config_help);
    prefs_dump_help ();
}

/* This implements the old commandline option parsing.  I've re-added this
   because the new way of doing things is painful for me (it requires me
   to type a couple hundred characters when invoking UAE).  The following
   is far less annoying to use.  */
static void parse_gfx_specs (struct uae_prefs *p, char *spec)
{
    char *x0 = my_strdup (spec);
    char *x1, *x2;

    x1 = strchr (x0, ':');
    if (x1 == 0)
	goto argh;
    x2 = strchr (x1+1, ':');
    if (x2 == 0)
	goto argh;
    *x1++ = 0; *x2++ = 0;

    p->gfx_width_win = p->gfx_width_fs = atoi (x0);
    p->gfx_height_win = p->gfx_height_fs = atoi (x1);
    p->gfx_lores = strchr (x2, 'l') != 0;
    p->gfx_xcenter = strchr (x2, 'x') != 0 ? 1 : strchr (x2, 'X') != 0 ? 2 : 0;
    p->gfx_ycenter = strchr (x2, 'y') != 0 ? 1 : strchr (x2, 'Y') != 0 ? 2 : 0;
    p->gfx_linedbl = strchr (x2, 'd') != 0;
    p->gfx_linedbl += 2 * (strchr (x2, 'D') != 0);
    p->gfx_afullscreen = strchr (x2, 'a') != 0;
    p->gfx_pfullscreen = strchr (x2, 'p') != 0;
    p->gfx_correct_aspect = strchr (x2, 'c') != 0;

    if (p->gfx_linedbl == 3) {
	write_log ("You can't use both 'd' and 'D' modifiers in the display mode specification.\n");
    }

    free (x0);
    return;

    argh:
    write_log ("Bad display mode specification.\n");
    write_log ("The format to use is: \"width:height:modifiers\"\n");
    write_log ("Type \"uae -h\" for detailed help.\n");
    free (x0);
}

static void parse_sound_spec (struct uae_prefs *p, char *spec)
{
    char *x0 = my_strdup (spec);
    char *x1, *x2 = NULL, *x3 = NULL, *x4 = NULL, *x5 = NULL;

    x1 = strchr (x0, ':');
    if (x1 != NULL) {
	*x1++ = '\0';
	x2 = strchr (x1 + 1, ':');
	if (x2 != NULL) {
	    *x2++ = '\0';
	    x3 = strchr (x2 + 1, ':');
	    if (x3 != NULL) {
		*x3++ = '\0';
		x4 = strchr (x3 + 1, ':');
		if (x4 != NULL) {
		    *x4++ = '\0';
		    x5 = strchr (x4 + 1, ':');
		}
	    }
	}
    }
    p->produce_sound = atoi (x0);
    if (x1) {
	p->sound_stereo_separation = 16;
	if (*x1 == 'S') {
	    p->sound_stereo = 1;
	    p->sound_stereo_separation = 10;
	} else if (*x1 == 's')
	    p->sound_stereo = 1;
	else
	    p->sound_stereo = 0;
    }
    if (x2)
	p->sound_bits = atoi (x2);
    if (x3)
	p->sound_freq = atoi (x3);
//    if (x4)
//	p->sound_maxbsiz = atoi (x4);
    free (x0);
    return;
}


static void parse_joy_spec (struct uae_prefs *p, char *spec)
{
    int v0 = 2, v1 = 0;
    if (strlen(spec) != 2)
	goto bad;

    switch (spec[0]) {
     case '0': v0 = JSEM_JOYS; break;
     case '1': v0 = JSEM_JOYS + 1; break;
     case 'M': case 'm': v0 = JSEM_MICE; break;
     case 'A': case 'a': v0 = JSEM_KBDLAYOUT; break;
     case 'B': case 'b': v0 = JSEM_KBDLAYOUT + 1; break;
     case 'C': case 'c': v0 = JSEM_KBDLAYOUT + 2; break;
     default: goto bad;
    }

    switch (spec[1]) {
     case '0': v1 = JSEM_JOYS; break;
     case '1': v1 = JSEM_JOYS + 1; break;
     case 'M': case 'm': v1 = JSEM_MICE; break;
     case 'A': case 'a': v1 = JSEM_KBDLAYOUT; break;
     case 'B': case 'b': v1 = JSEM_KBDLAYOUT + 1; break;
     case 'C': case 'c': v1 = JSEM_KBDLAYOUT + 2; break;
     default: goto bad;
    }
    if (v0 == v1)
	goto bad;
    /* Let's scare Pascal programmers */
    if (0)
bad:
    write_log ("Bad joystick mode specification. Use -J xy, where x and y\n"
	     "can be 0 for joystick 0, 1 for joystick 1, M for mouse, and\n"
	     "a, b or c for different keyboard settings.\n");

    p->jport0 = v0;
    p->jport1 = v1;
}

static void parse_filesys_spec (int readonly, const char *spec)
{
    char buf[256];
    char *s2;

    strncpy (buf, spec, 255); buf[255] = 0;
    s2 = strchr (buf, ':');
    if (s2) {
	*s2++ = '\0';
#ifdef __DOS__
	{
	    char *tmp;

	    while ((tmp = strchr (s2, '\\')))
		*tmp = '/';
	}
#endif
	s2 = 0;
#ifdef FILESYS
	{
	    const char *err;

	    err = add_filesys_unit (currprefs.mountinfo, 0, buf, s2, readonly, 0, 0, 0, 0, 0, 0, 0);

	    if (err)
		write_log ("%s\n", s2);
	}
#endif
    } else {
	write_log ("Usage: [-m | -M] VOLNAME:mount_point\n");
    }
}

static void parse_hardfile_spec (char *spec)
{
    char *x0 = my_strdup (spec);
    char *x1, *x2, *x3, *x4;

    x1 = strchr (x0, ':');
    if (x1 == NULL)
	goto argh;
    *x1++ = '\0';
    x2 = strchr (x1 + 1, ':');
    if (x2 == NULL)
	goto argh;
    *x2++ = '\0';
    x3 = strchr (x2 + 1, ':');
    if (x3 == NULL)
	goto argh;
    *x3++ = '\0';
    x4 = strchr (x3 + 1, ':');
    if (x4 == NULL)
	goto argh;
    *x4++ = '\0';
#ifdef FILESYS
    {
       const char *err_msg;
       err_msg = add_filesys_unit (currprefs.mountinfo, 0, 0, x4, 0, atoi (x0), atoi (x1), atoi (x2), atoi (x3), 0, 0, 0);

       if (err_msg)
           write_log ("%s\n", err_msg);
    }
#endif

    free (x0);
    return;

 argh:
    free (x0);
    write_log ("Bad hardfile parameter specified - type \"uae -h\" for help.\n");
    return;
}

static void parse_cpu_specs (struct uae_prefs *p, char *spec)
{
    if (*spec < '0' || *spec > '4') {
	write_log ("CPU parameter string must begin with '0', '1', '2', '3' or '4'.\n");
	return;
    }

    p->cpu_level = *spec++ - '0';
    p->address_space_24 = p->cpu_level < 2;
    p->cpu_compatible = 0;
    while (*spec != '\0') {
	switch (*spec) {
	 case 'a':
	    if (p->cpu_level < 2)
		write_log ("In 68000/68010 emulation, the address space is always 24 bit.\n");
	    else if (p->cpu_level >= 4)
		write_log ("In 68040/060 emulation, the address space is always 32 bit.\n");
	    else
		p->address_space_24 = 1;
	    break;
	 case 'c':
	    if (p->cpu_level != 0)
		write_log  ("The more compatible CPU emulation is only available for 68000\n"
			 "emulation, not for 68010 upwards.\n");
	    else
		p->cpu_compatible = 1;
	    break;
	 default:
	    write_log  ("Bad CPU parameter specified - type \"uae -h\" for help.\n");
	    break;
	}
	spec++;
    }
}

/* Returns the number of args used up (0 or 1).  */
int parse_cmdline_option (struct uae_prefs *p, char c, char *arg)
{
    struct strlist *u = xcalloc (sizeof (struct strlist), 1);
    const char arg_required[] = "0123rKpImWSAJwNCZUFcblOdHRv";

    if (strchr (arg_required, c) && ! arg) {
	write_log ("Missing argument for option `-%c'!\n", c);
	return 0;
    }

    u->option = malloc (2);
    u->option[0] = c;
    u->option[1] = 0;
    u->value = arg ? my_strdup (arg) : NULL;
    u->next = p->all_lines;
    p->all_lines = u;

    switch (c) {
    case 'h': usage (); exit (0);

    case '0': strncpy (p->df[0], arg, 255); p->df[0][255] = 0; break;
    case '1': strncpy (p->df[1], arg, 255); p->df[1][255] = 0; break;
    case '2': strncpy (p->df[2], arg, 255); p->df[2][255] = 0; break;
    case '3': strncpy (p->df[3], arg, 255); p->df[3][255] = 0; break;
    case 'r': strncpy (p->romfile, arg, 255); p->romfile[255] = 0; break;
    case 'K': strncpy (p->keyfile, arg, 255); p->keyfile[255] = 0; break;
    case 'p': strncpy (p->prtname, arg, 255); p->prtname[255] = 0; break;
	/*     case 'I': strncpy (p->sername, arg, 255); p->sername[255] = 0; currprefs.use_serial = 1; break; */
    case 'm': case 'M': parse_filesys_spec (c == 'M', arg); break;
    case 'W': parse_hardfile_spec (arg); break;
    case 'S': parse_sound_spec (p, arg); break;
    case 'R': p->gfx_framerate = atoi (arg); break;
    case 'i': p->illegal_mem = 1; break;
    case 'J': parse_joy_spec (p, arg); break;
    case 't': p->test_drawing_speed = 1; break;
#if defined USE_X11_GFX
    case 'L': p->x11_use_low_bandwidth = 1; break;
    case 'T': p->x11_use_mitshm = 1; break;
#elif defined USE_AMIGA_GFX
    case 'T': p->amiga_use_grey = 1; break;
    case 'x': p->amiga_use_dither = 0; break;
#elif defined USE_CURSES_GFX
    case 'x': p->curses_reverse_video = 1; break;
#endif
    case 'w': p->m68k_speed = atoi (arg); break;

	/* case 'g': p->use_gfxlib = 1; break; */
    case 'G': p->start_gui = 0; break;
#ifdef DEBUGGER
    case 'D': p->start_debugger = 1; break;
#endif

    case 'n':
	if (strchr (arg, 'i') != 0)
	    p->immediate_blits = 1;
	break;

    case 'v':
	set_chipset_mask (p, atoi (arg));
	break;

    case 'C':
	parse_cpu_specs (p, arg);
	break;

    case 'Z':
	p->z3fastmem_size = atoi (arg) * 0x100000;
	break;

    case 'U':
	p->gfxmem_size = atoi (arg) * 0x100000;
	break;

    case 'F':
	p->fastmem_size = atoi (arg) * 0x100000;
	break;

    case 'b':
	p->bogomem_size = atoi (arg) * 0x40000;
	break;

    case 'c':
	p->chipmem_size = atoi (arg) * 0x80000;
	break;

    case 'l':
	if (0 == strcasecmp(arg, "de"))
	    p->keyboard_lang = KBD_LANG_DE;
	else if (0 == strcasecmp(arg, "dk"))
	    p->keyboard_lang = KBD_LANG_DK;
	else if (0 == strcasecmp(arg, "us"))
	    p->keyboard_lang = KBD_LANG_US;
	else if (0 == strcasecmp(arg, "se"))
	    p->keyboard_lang = KBD_LANG_SE;
	else if (0 == strcasecmp(arg, "fr"))
	    p->keyboard_lang = KBD_LANG_FR;
	else if (0 == strcasecmp(arg, "it"))
	    p->keyboard_lang = KBD_LANG_IT;
	else if (0 == strcasecmp(arg, "es"))
	    p->keyboard_lang = KBD_LANG_ES;
	break;

    case 'O': parse_gfx_specs (p, arg); break;
    case 'd':
	if (strchr (arg, 'S') != NULL || strchr (arg, 's')) {
	    write_log ("  Serial on demand.\n");
	    p->serial_demand = 1;
	}
	if (strchr (arg, 'P') != NULL || strchr (arg, 'p')) {
	    write_log ("  Parallel on demand.\n");
	    p->parallel_demand = 1;
	}

	break;

    case 'H':
#ifndef USE_AMIGA_GFX
	p->color_mode = atoi (arg);
	if (p->color_mode < 0) {
	    write_log ("Bad color mode selected. Using default.\n");
	    p->color_mode = 0;
	}
#else
        p->amiga_screen_type = atoi (arg);
        if (p->amiga_screen_type < 0 || p->amiga_screen_type > 2) {
	    write_log ("Bad screen-type selected. Defaulting to public screen.\n");
	    p->amiga_screen_type = 2;
	}
#endif
	break;
    default:
	write_log ("Unknown option `-%c'!\n", c);
	break;
    }
    return !! strchr (arg_required, c);
}

void cfgfile_addcfgparam (char *line)
{
    struct strlist *u;
    char line1b[CONFIG_BLEN], line2b[CONFIG_BLEN];

    if (!line) {
	struct strlist **ps = &temp_lines;
	while (*ps) {
	    struct strlist *s = *ps;
	    *ps = s->next;
	    free (s->value);
	    free (s->option);
	    free (s);
	}
	temp_lines = 0;
	return;
    }
    if (!cfgfile_separate_line (line, line1b, line2b))
	return;
    u = xcalloc (sizeof (struct strlist), 1);
    u->option = my_strdup(line1b);
    u->value = my_strdup(line2b);
    u->next = temp_lines;
    temp_lines = u;
}

unsigned int cmdlineparser (const char *s, char *outp[], unsigned int max)
{
    int j;
    unsigned int cnt = 0;
    int slash = 0;
    int quote = 0;
    char tmp1[MAX_DPATH];
    const char *prev;
    int doout;

    doout = 0;
    prev = s;
    j = 0;
    while (cnt < max) {
	char c = *s++;
	if (!c)
	    break;
	if (c < 32)
	    continue;
	if (c == '\\')
	    slash = 1;
	if (!slash && c == '"') {
	    if (quote) {
		quote = 0;
		doout = 1;
	    } else {
		quote = 1;
		j = -1;
	    }
	}
	if (!quote && c == ' ')
	    doout = 1;
	if (!doout) {
	    if (j >= 0) {
		tmp1[j] = c;
		tmp1[j + 1] = 0;
	    }
	    j++;
	}
	if (doout) {
	    outp[cnt++] = my_strdup (tmp1);
	    tmp1[0] = 0;
	    doout = 0;
	    j = 0;
	}
	slash = 0;
    }
    if (j > 0 && cnt < max)
	outp[cnt++] = my_strdup (tmp1);

    return cnt;
}

#define UAELIB_MAX_PARSE 100

uae_u32 cfgfile_modify (uae_u32 index, char *parms, uae_u32 size, char *out, uae_u32 outsize)
{
    char *p;
    char *argc[UAELIB_MAX_PARSE];
    unsigned int argv, i;
    uae_u32 err;
    uae_u8 zero = 0;
    static FILE *configstore = 0;
    static char *configsearch;
    static int configsearchfound;

    err = 0;
    argv = 0;
    p = 0;
    if (index != 0xffffffff) {
	if (!configstore) {
	    err = 20;
	    goto end;
	}
	if (configsearch) {
	    char tmp[CONFIG_BLEN];
	    unsigned int j = 0;
	    char *in = configsearch;
	    unsigned int inlen = strlen (configsearch);
	    int joker = 0;

	    if (in[inlen - 1] == '*') {
		joker = 1;
		inlen--;
	    }

	    for (;;) {
		uae_u8 b = 0;

		if (fread (&b, 1, 1, configstore) != 1) {
		    err = 10;
		    if (configsearch)
			err = 5;
		    if (configsearchfound)
			err = 0;
		    goto end;
		}
		if (j >= sizeof (tmp) - 1)
		    j = sizeof (tmp) - 1;
		if (b == 0) {
		    err = 10;
		    if (configsearch)
			err = 5;
		    if (configsearchfound)
			err = 0;
		    goto end;
		}
		if (b == '\n') {
		    if (configsearch && !strncmp (tmp, in, inlen) &&
			((inlen > 0 && strlen (tmp) > inlen && tmp[inlen] == '=') || (joker))) {
			char *p;
			if (joker)
			    p = tmp - 1;
			else
			    p = strchr (tmp, '=');
			if (p) {
			    for (i = 0; i < outsize - 1; i++) {
				uae_u8 b = *++p;
				out[i] = b;
				out[i + 1] = 0;
				if (!b)
				    break;
			    }
			}
			err = 0xffffffff;
			configsearchfound++;
			goto end;
		    }
		    index--;
		    j = 0;
		} else {
		    tmp[j++] = b;
		    tmp[j] = 0;
		}
	    }
	}
	err = 0xffffffff;
	for (i = 0; i < outsize - 1; i++) {
	    uae_u8 b = 0;
	    if (fread (&b, 1, 1, configstore) != 1)
		err = 0;
	    if (b == 0)
		err = 0;
	    if (b == '\n')
		b = 0;
	    out[i] = b;
	    out[i + 1] = 0;
	    if (!b)
		break;
	}
	goto end;
    }

    if (size > 10000)
	return 10;
    argv = cmdlineparser (parms, argc, UAELIB_MAX_PARSE);

    if (argv <= 1 && index == 0xffffffff) {
	if (configstore) {
	    fclose (configstore);
	    configstore = 0;
	}
	free (configsearch);

	configstore = fopen ("configstore", "w+");
	configsearch = NULL;
	if (argv > 0 && strlen (argc[0]) > 0)
	    configsearch = my_strdup (argc[0]);
	if (!configstore) {
	    err = 20;
	    goto end;
	}
	fseek (configstore, 0, SEEK_SET);
	save_options (configstore, &currprefs, 0);
	fwrite (&zero, 1, 1, configstore);
	fseek (configstore, 0, SEEK_SET);
	err = 0xffffffff;
	configsearchfound = 0;
	goto end;
    }

    for (i = 0; i < argv; i++) {
	if (i + 2 <= argv) {
	    if (!inputdevice_uaelib (argc[i], argc[i + 1])) {
		if (!cfgfile_parse_option (&changed_prefs, argc[i], argc[i + 1], 0)) {
		    err = 5;
		    break;
		}
	    }
	    set_special (&regs, SPCFLAG_BRK);
	    i++;
	}
    }
end:
    for (i = 0; i < argv; i++)
	free (argc[i]);
    free (p);
    return err;
}

uae_u32 cfgfile_uaelib_modify (uae_u32 index, uae_u32 parms, uae_u32 size, uae_u32 out, uae_u32 outsize)
{
    char *p, *parms_p = NULL, *out_p = NULL;
    unsigned int i;
    int ret;

    put_byte (out, 0);
    parms_p = xmalloc (size + 1);
    if (!parms_p) {
	ret = 10;
	goto end;
    }
    out_p = xmalloc (outsize + 1);
    if (!out_p) {
	ret = 10;
	goto end;
    }
    p = parms_p;
    for (i = 0; i < size; i++) {
	p[i] = get_byte (parms + i);
	if (p[i] == 10 || p[i] == 13 || p[i] == 0)
	    break;
    }
    p[i] = 0;
    out_p[0] = 0;
    ret = cfgfile_modify (index, parms_p, size, out_p, outsize);
    p = out_p;
    for (i = 0; i < outsize - 1; i++) {
	uae_u8 b = *p++;
	put_byte (out + i, b);
	put_byte (out + i + 1, 0);
	if (!b)
	    break;
    }
end:
    free (out_p);
    free (parms_p);
    return ret;
}

uae_u32 cfgfile_uaelib (int mode, uae_u32 name, uae_u32 dst, uae_u32 maxlen)
{
    char tmp[CONFIG_BLEN];
    unsigned int i;
    struct strlist *sl;

    if (mode)
	return 0;

    for (i = 0; i < sizeof(tmp); i++) {
	tmp[i] = get_byte (name + i);
	if (tmp[i] == 0)
	    break;
    }
    tmp[sizeof(tmp) - 1] = 0;
    if (tmp[0] == 0)
	return 0;
    for (sl = currprefs.all_lines; sl; sl = sl->next) {
	if (!strcasecmp (sl->option, tmp))
	    break;
    }

    if (sl) {
	for (i = 0; i < maxlen; i++) {
	    put_byte (dst + i, sl->value[i]);
	    if (sl->value[i] == 0)
		break;
	}
	return dst;
    }
    return 0;
}

static void default_prefs_mini (struct uae_prefs *p, int type)
{
    strcpy (p->description, "UAE default A500 configuration");

    p->nr_floppies = 1;
    p->dfxtype[0] = 0;
    p->dfxtype[1] = -1;
    p->cpu_level = 0;
    p->address_space_24 = 1;
    p->chipmem_size = 0x00080000;
    p->bogomem_size = 0x00080000;
}

void default_prefs (struct uae_prefs *p, int type)
{
    memset (p, 0, sizeof (*p));
    strcpy (p->description, "UAE default configuration");

    p->start_gui = 1;
#ifdef DEBUGGER
    p->start_debugger = 0;
#endif

    p->all_lines = 0;
    /* Note to porters: please don't change any of these options! UAE is supposed
     * to behave identically on all platforms if possible.
     * (TW says: maybe it is time to update default config..) */
    p->illegal_mem = 0;
    p->use_serial = 0;
    p->serial_demand = 0;
    p->serial_hwctsrts = 1;
    p->parallel_demand = 0;

    p->jport0 = JSEM_MICE;
    p->jport1 = JSEM_KBDLAYOUT;
    p->keyboard_lang = KBD_LANG_US;

    p->produce_sound = 3;
    p->sound_stereo = 1;
    p->sound_stereo_separation = 7;
    p->sound_mixed_stereo = 0;
    p->sound_bits = DEFAULT_SOUND_BITS;
    p->sound_freq = DEFAULT_SOUND_FREQ;
    p->sound_latency = DEFAULT_SOUND_LATENCY;
    p->sound_interpol = 0;

#ifdef JIT
# ifdef NATMEM_OFFSET
    p->comptrustbyte = 0;
    p->comptrustword = 0;
    p->comptrustlong = 0;
    p->comptrustnaddr= 0;
# else
    p->comptrustbyte = 1;
    p->comptrustword = 1;
    p->comptrustlong = 1;
    p->comptrustnaddr= 1;
# endif
    p->compnf = 1;
    p->comp_hardflush = 0;
    p->comp_constjump = 1;
    p->comp_oldsegv = 0;
    p->compfpu = 1;
    p->compforcesettings = 0;
    p->cachesize = 0;
    {
	int i;
	for (i = 0;i < 10; i++)
	    p->optcount[i] = -1;
	p->optcount[0] = 4; /* How often a block has to be executed before it
			     is translated */
	p->optcount[1] = 0; /* How often to use the naive translation */
	p->optcount[2] = 0;
	p->optcount[3] = 0;
	p->optcount[4] = 0;
	p->optcount[5] = 0;
    }
#endif
    p->gfx_framerate = 1;
    p->gfx_width_fs = 800;
    p->gfx_height_fs = 600;
    p->gfx_width_win = 720;
    p->gfx_height_win = 568;
    p->gfx_lores = 0;
    p->gfx_linedbl = 1;
    p->gfx_afullscreen = 0;
    p->gfx_pfullscreen = 0;
    p->gfx_correct_aspect = 0;
    p->gfx_xcenter = 0;
    p->gfx_ycenter = 0;
    p->color_mode = 0;

    machdep_default_options (p);
    target_default_options (p);
    gfx_default_options (p);
    audio_default_options (p);

    p->immediate_blits = 0;
    p->collision_level = 2;
    p->leds_on_screen = 0;
    p->hide_cursor = 1;
    p->keyboard_leds_in_use = 0;
    p->keyboard_leds[0] = p->keyboard_leds[1] = p->keyboard_leds[2] = 0;
    p->scsi = 0;
    p->cpu_idle = 0;
    p->catweasel_io = 0;
    p->tod_hack = 0;
    p->maprom = 0;

#ifdef GFXFILTER
    p->gfx_filter = 0;
    p->gfx_filter_filtermode = 1;
    p->gfx_filter_scanlineratio = (1 << 4) | 1;
#endif

    p->df[0][0] = '\0';
    p->df[1][0] = '\0';
    p->df[2][0] = '\0';
    p->df[3][0] = '\0';

    strcpy (p->romfile, "kick.rom");
    strcpy (p->keyfile, "");
    strcpy (p->romextfile, "");
    strcpy (p->flashfile, "");
#ifdef ACTION_REPLAY
    strcpy (p->cartfile, "");
#endif

    prefs_set_attr ("rom_path",       strdup_path_expand (TARGET_ROM_PATH));
    prefs_set_attr ("floppy_path",    strdup_path_expand (TARGET_FLOPPY_PATH));
    prefs_set_attr ("hardfile_path",  strdup_path_expand (TARGET_HARDFILE_PATH));
#ifdef SAVESTATE
    prefs_set_attr ("savestate_path", strdup_path_expand (TARGET_SAVESTATE_PATH));
#endif

    strcpy (p->prtname, DEFPRTNAME);
    strcpy (p->sername, DEFSERNAME);

    p->cpu_level = 0;
    p->m68k_speed = 0;
    p->cpu_compatible = 1;
    p->address_space_24 = 1;
    p->cpu_cycle_exact = 0;
    p->blitter_cycle_exact = 0;
    p->chipset_mask = CSMASK_ECS_AGNUS;

    p->fastmem_size = 0x00000000;
    p->a3000mem_size = 0x00000000;
    p->z3fastmem_size = 0x00000000;
    p->chipmem_size = 0x00080000;
    p->bogomem_size = 0x00000000;
    p->gfxmem_size = 0x00000000;

    p->nr_floppies = 2;
    p->dfxtype[0] = 0;
    p->dfxtype[1] = 0;
    p->dfxtype[2] = -1;
    p->dfxtype[3] = -1;
    p->floppy_speed = 100;
#ifdef DRIVESOUND
    p->dfxclickvolume = 33;
#endif

#ifdef SAVESTATE
    p->statecapturebuffersize = 20 * 1024 * 1024;
    p->statecapturerate = 5 * 50;
    p->statecapture = 0;
#endif

#ifdef FILESYS
    p->mountinfo = &options_mountinfo;
#endif

#ifdef UAE_MINI
    default_prefs_mini (p, 0);
#endif

    inputdevice_default_prefs (p);
}
