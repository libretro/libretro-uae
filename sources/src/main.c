/*
 * UAE - The Un*x Amiga Emulator
 *
 * Main program
 *
 * Copyright 1995 Ed Hanway
 * Copyright 1995, 1996, 1997 Bernd Schmidt
 * Copyright 2006-2007 Richard Drummond
 * Copyright 2010 Mustafa Tufan
 */
#include "sysconfig.h"
#include "sysdeps.h"
#include <assert.h>

#include "cfgfile.h"
#include "threaddep/thread.h"
#include "uae.h"
#include "gensound.h"
#include "audio.h"
#include "sounddep/sound.h"
#include "events.h"
#include "memory_uae.h"
#include "custom.h"
#include "serial.h"
#include "newcpu.h"
#include "disk.h"
#include "debug.h"
#include "xwin.h"
#include "inputdevice.h"
#include "keybuf.h"
#include "gui.h"
#include "autoconf.h"
#include "traps.h"
#include "osemu.h"
#include "picasso96.h"
#include "bsdsocket.h"
#include "uaeexe.h"
#include "native2amiga.h"
#include "scsidev.h"
#include "uaeserial.h"
#include "akiko.h"
#include "cdtv.h"
#include "savestate.h"
#include "filesys.h"
#include "parallel.h"
#include "a2091.h"
#include "a2065.h"
#include "ncr_scsi.h"
#include "scsi.h"
#include "sana2.h"
#include "blkdev.h"
#include "gfxfilter.h"
#include "uaeresource.h"
#include "dongle.h"
#include "sampler.h"
#include "consolehook.h"
#include "gayle.h"
#include "misc.h"
#include "keyboard.h"
#ifdef RETROPLATFORM
#include "rp.h"
#endif
#ifdef USE_SDL
#include "SDL.h"
#endif

/* internal prototypes */
uae_u32 uaerand (void);
uae_u32 uaesrand (uae_u32 seed);
uae_u32 uaerandgetseed (void);
void my_trim (TCHAR *s);
TCHAR *my_strdup_trim (const TCHAR *s);

/* internal members */
long int version = 256 * 65536L * UAEMAJOR + 65536L * UAEMINOR + UAESUBREV;

struct uae_prefs currprefs, changed_prefs;
int config_changed;

int pissoff_value = 15000 * CYCLE_UNIT;
int pause_emulation;
char start_path_data[MAX_DPATH];
bool no_gui = 0, quit_to_gui = 0;
bool cloanto_rom = 0;
bool kickstart_rom = 1;
bool console_emulation = 0;

struct gui_info gui_data;

TCHAR warning_buffer[256];

TCHAR optionsfile[256];

static unsigned long randseed;
static unsigned long oldhcounter;

#ifndef _WIN32
// Prototype in sysdeps.h
DWORD GetLastError(void)
{
	return errno;
}
#endif

#ifdef __LIBRETRO__
static int8_t real_main2_ret = 0;
#endif

static void hr (void)
{
	write_log (_T("--------------------------------------------------------------------------------\n"));
}

#ifdef __LIBRETRO__
static void show_version (void)
{
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
	write_log (_T("PUAE %d.%d.%d%s, %s %s\n"), UAEMAJOR, UAEMINOR, UAESUBREV, GIT_VERSION, __DATE__, __TIME__);
}

static void show_version_full (void)
{
	hr ();
	show_version ();
	hr ();
}
#else
static void show_version (void)
{
	write_log (_T("PUAE %d.%d.%d (%s)\n"), UAEMAJOR, UAEMINOR, UAESUBREV, PACKAGE_COMMIT);
#ifdef GIT_VERSION
	write_log (_T("Git commit:%s\n"), GIT_VERSION);
#endif
	write_log (_T("Build date: " __DATE__ " " __TIME__ "\n"));
}

static void show_version_full (void)
{
	hr ();
	show_version ();
	hr ();
	write_log (_T("Copyright 1995-2002 Bernd Schmidt\n"));
	write_log (_T("          1999-2013 Toni Wilen\n"));
	write_log (_T("          2003-2007 Richard Drummond\n"));
	write_log (_T("          2006-2013 Mustafa 'GnoStiC' Tufan\n"));
	write_log (_T("\n"));
	write_log (_T("See the source for a full list of contributors.\n"));
	write_log (_T("This is free software; see the file COPYING for copying conditions. There is NO\n"));
	write_log (_T("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"));
	hr ();
}
#endif /* __LIBRETRO__ */

uae_u32 uaesrand (uae_u32 seed)
{
	oldhcounter = -1;
	randseed = seed;
	//randseed = 0x12345678;
	//write_log (_T("seed=%08x\n"), randseed);
	return randseed;
}

uae_u32 uaerand (void)
{
	if (oldhcounter != hsync_counter) {
		srand (hsync_counter ^ randseed);
		oldhcounter = hsync_counter;
	}
	uae_u32 r = rand ();
	//write_log (_T("rand=%08x\n"), r);
	return r;
}

uae_u32 uaerandgetseed (void)
{
	return randseed;
}

 void my_trim (TCHAR *s)
{
	int len;
	while (_tcscspn (s, _T("\t \r\n")) == 0)
		memmove (s, s + 1, (_tcslen (s + 1) + 1) * sizeof (TCHAR));
	len = _tcslen (s);
	while (len > 0 && _tcscspn (s + len - 1, _T("\t \r\n")) == 0)
		s[--len] = '\0';
}

TCHAR *my_strdup_trim (const TCHAR *s)
{
	TCHAR *out;
	int len;

	while (_tcscspn (s, _T("\t \r\n")) == 0)
		s++;
	len = _tcslen (s);
	while (len > 0 && _tcscspn (s + len - 1, _T("\t \r\n")) == 0)
		len--;
	out = xmalloc (TCHAR, len + 1);
	memcpy (out, s, len * sizeof (TCHAR));
	out[len] = 0;
	return out;
}

void discard_prefs (struct uae_prefs *p, int type)
{
	struct strlist **ps = &p->all_lines;
	while (*ps) {
		struct strlist *s = *ps;
		*ps = s->next;
		xfree (s->value);
		xfree (s->option);
		xfree (s);
	}
#ifdef FILESYS
	filesys_cleanup ();
#endif
}

static void fixup_prefs_dim2 (struct wh *wh)
{
	if (wh->width < 160)
		wh->width = 160;
	if (wh->height < 128)
		wh->height = 128;
	if (wh->width > MAX_UAE_WIDTH)
		wh->width = MAX_UAE_WIDTH;
	if (wh->height > MAX_UAE_HEIGHT)
		wh->height = MAX_UAE_HEIGHT;
}

void fixup_prefs_dimensions (struct uae_prefs *prefs)
{
	fixup_prefs_dim2 (&prefs->gfx_size_fs);
	fixup_prefs_dim2 (&prefs->gfx_size_win);
	if (prefs->gfx_apmode[1].gfx_vsync)
		prefs->gfx_apmode[1].gfx_vsyncmode = 1;

	for (int i = 0; i < 2; i++) {
		struct apmode *ap = &prefs->gfx_apmode[i];
		ap->gfx_vflip = 0;
		ap->gfx_strobo = false;
		if (ap->gfx_vsync) {
			if (ap->gfx_vsyncmode) {
				// low latency vsync: no flip only if no-buffer
				if (ap->gfx_backbuffers >= 1)
					ap->gfx_vflip = 1;
				if (!i && ap->gfx_backbuffers == 2)
					ap->gfx_vflip = 1;
				ap->gfx_strobo = prefs->lightboost_strobo;
			} else {
				// legacy vsync: always wait for flip
				ap->gfx_vflip = -1;
				if (prefs->gfx_api && ap->gfx_backbuffers < 1)
					ap->gfx_backbuffers = 1;
				if (ap->gfx_vflip)
					ap->gfx_strobo = prefs->lightboost_strobo;;
			}
		} else {
			// no vsync: wait if triple bufferirng
			if (ap->gfx_backbuffers >= 2)
				ap->gfx_vflip = -1;
		}
	}

	if (prefs->gfx_filter == 0 && ((prefs->gfx_filter_autoscale && !prefs->gfx_api) || (prefs->gfx_apmode[0].gfx_vsyncmode)))
		prefs->gfx_filter = 1;
	if (prefs->gfx_filter == 0 && prefs->monitoremu)
		prefs->gfx_filter = 1;
}

void fixup_cpu (struct uae_prefs *p)
{
	if (p->cpu_frequency == 1000000)
		p->cpu_frequency = 0;
	switch (p->cpu_model)
	{
	case 68000:
		p->address_space_24 = 1;
		if (p->cpu_compatible || p->cpu_cycle_exact)
			p->fpu_model = 0;
		break;
	case 68010:
		p->address_space_24 = 1;
		if (p->cpu_compatible || p->cpu_cycle_exact)
			p->fpu_model = 0;
		break;
	case 68020:
		break;
	case 68030:
		p->address_space_24 = 0;
		break;
	case 68040:
		p->address_space_24 = 0;
		if (p->fpu_model)
			p->fpu_model = 68040;
		break;
	case 68060:
		p->address_space_24 = 0;
		if (p->fpu_model)
			p->fpu_model = 68060;
		break;
	}

	if (p->cpu_model >= 68040 && p->cachesize && p->cpu_compatible)
		p->cpu_compatible = false;

	if (p->cpu_model < 68030 || p->cachesize)
		p->mmu_model = 0;

	if (p->cachesize && p->cpu_cycle_exact)
		p->cachesize = 0;

	if (p->cpu_cycle_exact && p->m68k_speed < 0)
		p->m68k_speed = 0;

	if (p->immediate_blits && p->blitter_cycle_exact)
		p->immediate_blits = false;
	if (p->immediate_blits && p->waiting_blits)
		p->waiting_blits = 0;
}



void fixup_prefs (struct uae_prefs *p)
{
/** FIXME:
  * There must be a way to a) have a sane max_z3fastmem value
  * without NATMEM being used and b) a way to use fastmem and
  * rtg mem without crashing 64bit linux systems.
**/
#ifndef NATMEM_OFFSET
	max_z3fastmem = 0x20000000;
#endif

	int err = 0;

	built_in_chipset_prefs (p);
	fixup_cpu (p);

	if (((p->chipmem_size & (p->chipmem_size - 1)) != 0 && p->chipmem_size != 0x180000)
		|| p->chipmem_size < 0x20000
		|| p->chipmem_size > 0x800000)
	{
		write_log (_T("Unsupported chipmem size %x!\n"), p->chipmem_size);
		p->chipmem_size = 0x200000;
		err = 1;
	}
	if ((p->fastmem_size & (p->fastmem_size - 1)) != 0
		|| (p->fastmem_size != 0 && (p->fastmem_size < 0x100000 || p->fastmem_size > 0x800000)))
	{
		write_log (_T("Unsupported fastmem size %x!\n"), p->fastmem_size);
		err = 1;
	}
	if ((p->rtgmem_size & (p->rtgmem_size - 1)) != 0
		|| (p->rtgmem_size != 0 && (p->rtgmem_size < 0x100000 || p->rtgmem_size > max_z3fastmem)))
	{
		write_log (_T("Unsupported graphics card memory size %x (%x)!\n"), p->rtgmem_size, max_z3fastmem);
		if (p->rtgmem_size > max_z3fastmem)
			p->rtgmem_size = max_z3fastmem;
		else
			p->rtgmem_size = 0;
		err = 1;
	}

	if ((p->z3fastmem_size & (p->z3fastmem_size - 1)) != 0
		|| (p->z3fastmem_size != 0 && (p->z3fastmem_size < 0x100000 || p->z3fastmem_size > max_z3fastmem)))
	{
		write_log (_T("Unsupported Zorro III fastmem size %x (%x)!\n"), p->z3fastmem_size, max_z3fastmem);
		if (p->z3fastmem_size > max_z3fastmem)
			p->z3fastmem_size = max_z3fastmem;
		else
			p->z3fastmem_size = 0;
		err = 1;
	}
	if ((p->z3fastmem2_size & (p->z3fastmem2_size - 1)) != 0
		|| (p->z3fastmem2_size != 0 && (p->z3fastmem2_size < 0x100000 || p->z3fastmem2_size > max_z3fastmem)))
	{
		write_log (_T("Unsupported Zorro III fastmem size %x (%x)!\n"), p->z3fastmem2_size, max_z3fastmem);
		if (p->z3fastmem2_size > max_z3fastmem)
			p->z3fastmem2_size = max_z3fastmem;
		else
			p->z3fastmem2_size = 0;
		err = 1;
	}
	p->z3fastmem_start &= ~0xffff;
	if (p->z3fastmem_start < 0x1000000)
		p->z3fastmem_start = 0x1000000;
	if ((p->z3chipmem_size & (p->z3chipmem_size - 1)) != 0
		|| (p->z3chipmem_size != 0 && (p->z3chipmem_size < 0x100000 || p->z3chipmem_size > max_z3fastmem)))
	{
		write_log (_T("Unsupported Zorro III fake chipmem size %x (%x)!\n"), p->z3chipmem_size, max_z3fastmem);
		if (p->z3chipmem_size > max_z3fastmem)
			p->z3chipmem_size = max_z3fastmem;
		else
			p->z3chipmem_size = 0;
		err = 1;
	}

	if (p->address_space_24 && (p->z3fastmem_size != 0 || p->z3fastmem2_size != 0 || p->z3chipmem_size != 0)) {
		p->z3fastmem_size = p->z3fastmem2_size = p->z3chipmem_size = 0;
		write_log (_T("Can't use a graphics card or 32-bit memory when using a 24 bit\naddress space.\n"));
	}
	if (p->bogomem_size != 0 && p->bogomem_size != 0x80000 && p->bogomem_size != 0x100000 && p->bogomem_size != 0x180000 && p->bogomem_size != 0x1c0000) {
		p->bogomem_size = 0;
		write_log (_T("Unsupported bogomem size!\n"));
		err = 1;
	}
	if (p->bogomem_size > 0x180000 && (p->cs_fatgaryrev >= 0 || p->cs_ide || p->cs_ramseyrev >= 0)) {
		p->bogomem_size = 0x180000;
		write_log (_T("Possible Gayle bogomem conflict fixed\n"));
	}
	if (p->chipmem_size > 0x200000 && p->fastmem_size != 0) {
		write_log (_T("You can't use fastmem and more than 2MB chip at the same time!\n"));
		p->fastmem_size = 0;
		err = 1;
	}
	if (p->mbresmem_low_size > 0x04000000 || (p->mbresmem_low_size & 0xfffff)) {
		p->mbresmem_low_size = 0;
		write_log (_T("Unsupported A3000 MB RAM size\n"));
	}
	if (p->mbresmem_high_size > 0x04000000 || (p->mbresmem_high_size & 0xfffff)) {
		p->mbresmem_high_size = 0;
		write_log (_T("Unsupported Motherboard RAM size\n"));
	}

	if (p->address_space_24 && p->rtgmem_size)
		p->rtgmem_type = 0;
	if (!p->rtgmem_type && (p->chipmem_size > 2 * 1024 * 1024 || getz2size (p) > 8 * 1024 * 1024 || getz2size (p) < 0)) {
		p->rtgmem_size = 0;
		write_log (_T("Too large Z2 RTG memory size\n"));
	}


#if 0
	if (p->m68k_speed < -1 || p->m68k_speed > 20) {
		write_log (_T("Bad value for -w parameter: must be -1, 0, or within 1..20.\n"));
		p->m68k_speed = 4;
		err = 1;
	}
#endif

	if (p->produce_sound < 0 || p->produce_sound > 3) {
		write_log (_T("Bad value for -S parameter: enable value must be within 0..3\n"));
		p->produce_sound = 0;
		err = 1;
	}
#ifdef JIT
	if (p->comptrustbyte < 0 || p->comptrustbyte > 3) {
		write_log (_T("Bad value for comptrustbyte parameter: value must be within 0..2\n"));
		p->comptrustbyte = 1;
		err = 1;
	}
	if (p->comptrustword < 0 || p->comptrustword > 3) {
		write_log (_T("Bad value for comptrustword parameter: value must be within 0..2\n"));
		p->comptrustword = 1;
		err = 1;
	}
	if (p->comptrustlong < 0 || p->comptrustlong > 3) {
		write_log (_T("Bad value for comptrustlong parameter: value must be within 0..2\n"));
		p->comptrustlong = 1;
		err = 1;
	}
	if (p->comptrustnaddr < 0 || p->comptrustnaddr > 3) {
		write_log (_T("Bad value for comptrustnaddr parameter: value must be within 0..2\n"));
		p->comptrustnaddr = 1;
		err = 1;
	}
	if (p->cachesize < 0 || p->cachesize > 16384) {
		write_log (_T("Bad value for cachesize parameter: value must be within 0..16384\n"));
		p->cachesize = 0;
		err = 1;
	}
#endif
	if (p->z3fastmem_size > 0 && (p->address_space_24 || p->cpu_model < 68020)) {
		write_log (_T("Z3 fast memory can't be used with a 68000/68010 emulation. It\n"));
		write_log (_T("requires a 68020 emulation. Turning off Z3 fast memory.\n"));
		p->z3fastmem_size = 0;
		err = 1;
	}
	if (p->rtgmem_size > 0 && p->rtgmem_type && (p->cpu_model < 68020 || p->address_space_24)) {
		write_log (_T("RTG can't be used with a 68000/68010 or 68EC020 emulation. It\n"));
		write_log (_T("requires a 68020 emulation. Turning off RTG.\n"));
		p->rtgmem_size = 0;
		err = 1;
	}
#if !defined (BSDSOCKET)
	if (p->socket_emu) {
		write_log (_T("Compile-time option of BSDSOCKET_SUPPORTED was not enabled.  You can't use bsd-socket emulation.\n"));
		p->socket_emu = 0;
		err = 1;
	}
#endif

	if (p->nr_floppies < 0 || p->nr_floppies > 4) {
		write_log (_T("Invalid number of floppies.  Using 4.\n"));
		p->nr_floppies = 4;
		p->floppyslots[0].dfxtype = 0;
		p->floppyslots[1].dfxtype = 0;
		p->floppyslots[2].dfxtype = 0;
		p->floppyslots[3].dfxtype = 0;
		err = 1;
	}
	if (p->floppy_speed > 0 && p->floppy_speed < 10) {
		p->floppy_speed = 100;
	}
	if (p->input_mouse_speed < 1 || p->input_mouse_speed > 1000) {
		p->input_mouse_speed = 100;
	}
	if (p->collision_level < 0 || p->collision_level > 3) {
		write_log (_T("Invalid collision support level.  Using 1.\n"));
		p->collision_level = 1;
		err = 1;
	}
	if (p->parallel_postscript_emulation)
		p->parallel_postscript_detection = 1;
	if (p->cs_compatible == 1) {
		p->cs_fatgaryrev = p->cs_ramseyrev = p->cs_mbdmac = -1;
		p->cs_ide = 0;
		if (p->cpu_model >= 68020) {
			p->cs_fatgaryrev = 0;
			p->cs_ide = -1;
			p->cs_ramseyrev = 0x0f;
			p->cs_mbdmac = 0;
		}
	} else if (p->cs_compatible == 0) {
		if (p->cs_ide == IDE_A4000) {
			if (p->cs_fatgaryrev < 0)
				p->cs_fatgaryrev = 0;
			if (p->cs_ramseyrev < 0)
				p->cs_ramseyrev = 0x0f;
		}
	}
	/* Can't fit genlock and A2024 or Graffiti at the same time,
	 * also Graffiti uses genlock audio bit as an enable signal
	 */
	if (p->genlock && p->monitoremu)
		p->genlock = false;


	fixup_prefs_dimensions (p);

#if !defined (JIT)
	p->cachesize = 0;
#endif
#ifdef CPU_68000_ONLY
	p->cpu_model = 68000;
	p->fpu_model = 0;
#endif
#ifndef CPUEMU_0
	p->cpu_compatible = 1;
	p->address_space_24 = 1;
#endif
#if !defined (CPUEMU_11) && !defined (CPUEMU_12)
	p->cpu_compatible = 0;
	p->address_space_24 = 0;
#endif
#if !defined (CPUEMU_12)
	p->cpu_cycle_exact = p->blitter_cycle_exact = 0;
#endif
#ifndef AGA
	p->chipset_mask &= ~CSMASK_AGA;
#endif
#ifndef AUTOCONFIG
	p->z3fastmem_size = 0;
	p->fastmem_size = 0;
	p->rtgmem_size = 0;
#endif
#if !defined (BSDSOCKET)
	p->socket_emu = 0;
#endif
#if !defined (SCSIEMU)
	p->scsi = 0;
#endif
#if !defined (SANA2)
	p->sana2 = 0;
#endif
#if !defined (UAESERIAL)
	p->uaeserial = 0;
#endif
#if defined (CPUEMU_12)
	if (p->cpu_cycle_exact) {
		p->gfx_framerate = 1;
#ifdef JIT
		p->cachesize = 0;
#endif
		p->m68k_speed = 0;
	}
#endif
	if (p->maprom && !p->address_space_24)
		p->maprom = 0x0f000000;
	if ((p->maprom & 0xff000000) && p->address_space_24)
		p->maprom = 0x00e00000;
	if (p->tod_hack && p->cs_ciaatod == 0)
		p->cs_ciaatod = p->ntscmode ? 2 : 1;

	built_in_chipset_prefs (p);
#ifdef SCSIEMU
	blkdev_fix_prefs (p);
#endif
	target_fixup_options (p);
}

int quit_program = 0;
static int restart_program;
static TCHAR restart_config[MAX_DPATH];
static int default_config;

void uae_reset (int hardreset, int keyboardreset)
{
	if (debug_dma) {
		record_dma_reset ();
		record_dma_reset ();
	}
	currprefs.quitstatefile[0] = changed_prefs.quitstatefile[0] = 0;

	if (quit_program == 0) {
		quit_program = -UAE_RESET;
		if (keyboardreset)
			quit_program = -UAE_RESET_KEYBOARD;
		if (hardreset)
			quit_program = -UAE_RESET_HARD;
	}

}

void uae_quit (void)
{
	deactivate_debugger ();
	if (quit_program != -UAE_QUIT)
		quit_program = -UAE_QUIT;
	target_quit ();
}

/* 0 = normal, 1 = nogui, -1 = disable nogui */
void uae_restart (int opengui, const TCHAR *cfgfile)
{
	uae_quit ();
	restart_program = opengui > 0 ? 1 : (opengui == 0 ? 2 : 3);
#ifndef __LIBRETRO__
	restart_config[0] = 0;
	default_config = 0;
	if (cfgfile)
		_tcscpy (restart_config, cfgfile);
#endif
	target_restart ();
}


#ifndef DONT_PARSE_CMDLINE

void usage (void)
{
}
static void parse_cmdline_2 (int argc, TCHAR **argv)
{
	int i;

	cfgfile_addcfgparam (0);
	for (i = 1; i < argc; i++) {
		if (_tcsncmp (argv[i], _T("-cfgparam="), 10) == 0) {
			cfgfile_addcfgparam (argv[i] + 10);
		} else if (_tcscmp (argv[i], _T("-cfgparam")) == 0) {
			if (i + 1 == argc)
				write_log (_T("Missing argument for '-cfgparam' option.\n"));
			else
				cfgfile_addcfgparam (argv[++i]);
		}
	}
}

static int diskswapper_cb (struct zfile *f, void *vrsd)
{
	int *num = (int*)vrsd;
	if (*num >= MAX_SPARE_DRIVES)
		return 1;
	if (zfile_gettype (f) ==  ZFILE_DISKIMAGE) {
		_tcsncpy (currprefs.dfxlist[*num], zfile_getname (f), 255);
		(*num)++;
	}
	return 0;
}

static void parse_diskswapper (const TCHAR *s)
{
	TCHAR *tmp = my_strdup (s);
	TCHAR *delim = _T(",");
	TCHAR *p1, *p2;
	int num = 0;

	p1 = tmp;
	for (;;) {
		p2 = _tcstok (p1, delim);
		if (!p2)
			break;
		p1 = NULL;
		if (num >= MAX_SPARE_DRIVES)
			break;
		if (!zfile_zopen (p2, diskswapper_cb, &num)) {
			_tcsncpy (currprefs.dfxlist[num], p2, 255);
			num++;
		}
	}
	xfree (tmp);
}

static TCHAR *parsetext (const TCHAR *s)
{
	if (*s == '"' || *s == '\'') {
		TCHAR *d;
		TCHAR c = *s++;
		int i;
		d = my_strdup (s);
		for (i = 0; i < (int)_tcslen (d); i++) {
			if (d[i] == c) {
				d[i] = 0;
				break;
			}
		}
		return d;
	} else {
		return my_strdup (s);
	}
}
static TCHAR *parsetextpath (const TCHAR *s)
{
	TCHAR *s3 = parsetext (s);
	//TCHAR *s3 = target_expand_environment (s2);
	//xfree (s2);
	return s3;
}

static void parse_cmdline (int argc, TCHAR **argv)
{
	int i;

	for (i = 1; i < argc; i++) {
		if (!_tcsncmp (argv[i], _T("-diskswapper="), 13)) {
			TCHAR *txt = parsetextpath (argv[i] + 13);
			parse_diskswapper (txt);
			xfree (txt);
		} else if (_tcsncmp (argv[i], _T("-cfgparam="), 10) == 0) {
			;
		} else if (_tcscmp (argv[i], _T("-cfgparam")) == 0) {
			if (i + 1 < argc)
				i++;
		} else if (_tcsncmp (argv[i], _T("-config="), 8) == 0) {
			TCHAR *txt = parsetextpath (argv[i] + 8);
			currprefs.mountitems = 0;
			target_cfgfile_load (&currprefs, txt, -1, 0);
			xfree (txt);
		} else if (_tcsncmp (argv[i], _T("-statefile="), 11) == 0) {
#ifdef SAVESTATE
			TCHAR *txt = parsetextpath (argv[i] + 11);
			savestate_state = STATE_DORESTORE;
			_tcscpy (savestate_fname, txt);
			xfree (txt);
#else
			write_log (_T("Option -statefile ignored:\n"));
			write_log (_T("-> puae has been configured with --disable-save-state\n"));
#endif // SAVESTATE
		} else if (_tcscmp (argv[i], _T("-f")) == 0) {
			/* Check for new-style "-f xxx" argument, where xxx is config-file */
			if (i + 1 == argc) {
				write_log (_T("Missing argument for '-f' option.\n"));
			} else {
				TCHAR *txt = parsetextpath (argv[++i]);
				currprefs.mountitems = 0;
				target_cfgfile_load (&currprefs, txt, -1, 0);
				xfree (txt);
			}
		} else if (_tcscmp (argv[i], _T("-s")) == 0) {
			if (i + 1 == argc)
				write_log (_T("Missing argument for '-s' option.\n"));
			else
				cfgfile_parse_line (&currprefs, argv[++i], 0);
		} else if (_tcscmp (argv[i], _T("-h")) == 0 || _tcscmp (argv[i], _T("-help")) == 0) {
			usage ();
			exit (0);
		} else if (_tcsncmp (argv[i], _T("-cdimage="), 9) == 0) {
			TCHAR *txt = parsetextpath (argv[i] + 9);
			TCHAR *txt2 = xmalloc(TCHAR, _tcslen(txt) + 2);
			_tcscpy(txt2, txt);
			if (_tcsrchr(txt2, ',') != NULL)
				_tcscat(txt2, _T(","));
			cfgfile_parse_option (&currprefs, _T("cdimage0"), txt2, 0);
			xfree(txt2);
			xfree (txt);
		} else {
			if (argv[i][0] == '-' && argv[i][1] != '\0') {
				const TCHAR *arg = argv[i] + 2;
				int extra_arg = *arg == '\0';
				if (extra_arg)
					arg = i + 1 < argc ? argv[i + 1] : 0;
				if (parse_cmdline_option (&currprefs, argv[i][1], arg) && extra_arg)
					i++;
			}
		}
	}
}
#endif

static void parse_cmdline_and_init_file (int argc, TCHAR **argv)
{
	_tcscpy (optionsfile, _T(""));

#ifdef OPTIONS_IN_HOME
	{
		TCHAR *home = getenv ("HOME");
		if (home != NULL && strlen (home) < 240)
		{
			_tcscpy (optionsfile, home);
			_tcscat (optionsfile, _T("/"));
		}
	}
#endif

#ifdef __LIBRETRO__
	_tcscpy (optionsfile, ".");
	_tcscat (optionsfile, _T("/"));
	target_cfgfile_load (&currprefs, "", 0, default_config);
#else
	parse_cmdline_2 (argc, argv);
	_tcscat (optionsfile, restart_config);

	if (argc > 1 && ! target_cfgfile_load (&currprefs, argv[1], 0, default_config)) {
		write_log (_T("failed to load config '%s'\n"), optionsfile);
#ifdef OPTIONS_IN_HOME
		/* sam: if not found in $HOME then look in current directory */
		_tcscpy (optionsfile, restart_config);
		target_cfgfile_load (&currprefs, optionsfile, 0, default_config);
#endif
	}
#endif
	fixup_prefs (&currprefs);

	parse_cmdline (argc, argv);
//	fixup_prefs (&currprefs); //fixup after cmdline
}

void reset_all_systems (void)
{
	init_eventtab ();

#ifdef PICASSO96
	picasso_reset ();
#endif
#ifdef SCSIEMU
	scsi_reset ();
	scsidev_reset ();
	scsidev_start_threads ();
#endif
#ifdef A2065
	a2065_reset ();
#endif
#ifdef SANA2
	netdev_reset ();
	netdev_start_threads ();
#endif
#ifdef FILESYS
	filesys_prepare_reset ();
	filesys_reset ();
#endif
#ifdef NATMEM_OFFSET
	init_shm ();
#endif
	memory_reset ();
#if defined (BSDSOCKET)
	bsdlib_reset ();
#endif
#ifdef FILESYS
	filesys_start_threads ();
	hardfile_reset ();
#endif
#ifdef UAESERIAL
	uaeserialdev_reset ();
	uaeserialdev_start_threads ();
#endif
#if defined (PARALLEL_PORT)
	initparallel ();
#endif
	native2amiga_reset ();
	dongle_reset ();
#ifdef SAMPLER
	sampler_init ();
#endif
    device_func_reset();
}

/* Okay, this stuff looks strange, but it is here to encourage people who
* port UAE to re-use as much of this code as possible. Functions that you
* should be using are do_start_program () and do_leave_program (), as well
* as real_main (). Some OSes don't call main () (which is braindamaged IMHO,
* but unfortunately very common), so you need to call real_main () from
* whatever entry point you have. You may want to write your own versions
* of start_program () and leave_program () if you need to do anything special.
* Add #ifdefs around these as appropriate.
*/
extern unsigned int pause_uae;
#ifndef __LIBRETRO__
void do_start_program (void)
{
	if (quit_program == -UAE_QUIT)
		return;
#ifdef JIT
	if (!canbang && candirect < 0)
		candirect = 0;
	if (canbang && candirect < 0)
		candirect = 1;
#endif
	/* Do a reset on startup. Whether this is elegant is debatable. */
	inputdevice_updateconfig (&changed_prefs, &currprefs);
	if (quit_program >= 0)
		quit_program = UAE_RESET;

	{
		m68k_go (1);
	}
}
#endif

void do_leave_program (void)
{
#ifdef SAMPLER
	sampler_free ();
#endif
	graphics_leave ();
	inputdevice_close ();
	DISK_free ();
	close_sound ();
	dump_counts ();
#ifdef SERIAL_PORT
	serial_exit ();
#endif
#ifdef CDTV
	cdtv_free ();
#endif
#ifdef A2091
	a2091_free ();
#endif
#ifdef NCR
	ncr_free ();
#endif
#ifdef CD32
	akiko_free ();
#endif
	if (! no_gui)
		gui_exit ();
#ifdef USE_SDL
	SDL_Quit ();
#endif
#ifdef AUTOCONFIG
	expansion_cleanup ();
#endif
#ifdef FILESYS
	filesys_cleanup ();
#endif
#ifdef BSDSOCKET
	bsdlib_reset ();
#endif
#ifdef SCSIEMU
#ifdef GAYLE
	gayle_free ();
#endif
	device_func_free ();
#endif
	savestate_free ();
	memory_cleanup ();
#ifdef NATMEM_OFFSET
	free_shm ();
#endif
	cfgfile_addcfgparam (0);
	machdep_free ();
}

#ifndef __LIBRETRO__
void start_program (void)
{
	gui_display (-1);
	do_start_program ();
}
#endif

void leave_program (void)
{
	do_leave_program ();
#ifdef __LIBRETRO__
	quit_program = 0;
#if 0
	zfile_exit ();
#endif
#endif
}


void virtualdevice_init (void)
{
#ifdef AUTOCONFIG
	/* Install resident module to get 8MB chipmem, if requested */
	rtarea_setup ();
#endif
#ifdef FILESYS
	rtarea_init ();
	uaeres_install ();
	hardfile_install ();
#endif
#ifdef SCSIEMU
	scsi_reset ();
	scsidev_install ();
#endif
#ifdef SANA2
	netdev_install ();
#endif
#ifdef UAESERIAL
	uaeserialdev_install ();
#endif
#ifdef AUTOCONFIG
	expansion_init ();
#endif
#ifndef __LIBRETRO__
	emulib_install ();
	uaeexe_install ();
#endif
#ifdef FILESYS
	filesys_install ();
#endif
#if defined (BSDSOCKET)
	bsdlib_install ();
#endif
}

static int real_main2 (int argc, TCHAR **argv)
{
#ifdef USE_SDL
	int result = (SDL_Init (SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) == 0);
	if (result)
		atexit (SDL_Quit);
#endif
	config_changed = 1;
	if (restart_config[0]) {
		default_prefs (&currprefs, 0);
		fixup_prefs (&currprefs);
	}

	if (! graphics_setup ()) {
		write_log (_T("Graphics Setup Failed\n"));
		exit (1);
	}

	if (restart_config[0])
		parse_cmdline_and_init_file (argc, argv);
	else
		currprefs = changed_prefs;

//	uae_inithrtimer ();

	if (!machdep_init ()) {
		write_log (_T("Machine Init Failed.\n"));
		restart_program = 0;
		return -1;
	}

#ifndef __LIBRETRO__
	if (console_emulation) {
		consolehook_config (&currprefs);
		fixup_prefs (&currprefs);
	}
#endif

	if (! setup_sound ()) {
		write_log (_T("Sound driver unavailable: Sound output disabled\n"));
		currprefs.produce_sound = 0;
	}
	inputdevice_init ();

	changed_prefs = currprefs;
	no_gui = ! currprefs.start_gui;

	if (restart_program == 2)
		no_gui = 1;
	else if (restart_program == 3)
		no_gui = 0;

	restart_program = 0;
	if (! no_gui && currprefs.start_gui) {
		int err = gui_init ();
		currprefs = changed_prefs;
		config_changed = 1;

		if (err == -1) {
			write_log (_T("Failed to initialize the GUI\n"));
			return -1;
		} else if (err == -2) {
			return 1;
		}
	}

	memset (&gui_data, 0, sizeof gui_data);
	gui_data.cd = -1;
	gui_data.hd = -1;
	gui_data.net = -1;
	gui_data.md = (currprefs.cs_cd32nvram || currprefs.cs_cdtvram) ? 0 : -1;

#ifdef NATMEM_OFFSET
	init_shm ();
#endif

#ifdef PICASSO96
	picasso_reset ();
#endif

	fixup_prefs (&currprefs);

#ifdef RETROPLATFORM
	rp_fixup_options (&currprefs);
#endif

	changed_prefs = currprefs;
	target_run ();
	/* force sound settings change */
	currprefs.produce_sound = 0;

	savestate_init ();
	keybuf_init (); /* Must come after init_joystick */

	memory_hardreset (2);
	memory_reset ();

#ifdef AUTOCONFIG
	native2amiga_install ();
#endif

	custom_init (); /* Must come after memory_init */

#ifdef SERIAL_PORT
	serial_init ();
#endif

	DISK_init ();

	reset_frame_rate_hack ();
	init_m68k (); /* must come after reset_frame_rate_hack (); */

	gui_update ();

	if (graphics_init ()) {

#ifdef DEBUGGER
		setup_brkhandler ();
		if (currprefs.start_debugger && debuggable ())
			activate_debugger ();
#endif

		if (!init_audio ()) {
			if (sound_available && currprefs.produce_sound > 1) {
				write_log (_T("Sound driver unavailable: Sound output disabled\n"));
			}
			currprefs.produce_sound = 0;
		}
#ifdef __LIBRETRO__
		gui_display (-1);

		if (quit_program == -UAE_QUIT)
			return 0;
#ifdef JIT
		if (!canbang && candirect < 0)
			candirect = 0;
		if (canbang && candirect < 0)
			candirect = 1;
#endif
		/* Do a reset on startup. Whether this is elegant is debatable. */
		inputdevice_updateconfig (&changed_prefs, &currprefs);

		if (quit_program >= 0)
			quit_program = UAE_RESET;

#else
		start_program ();
#endif
	}

	return 0;
}

void real_main (int argc, TCHAR **argv)
{
	show_version_full ();
	restart_program = 1;

#ifndef __LIBRETRO__
	fetch_configurationpath (restart_config, sizeof (restart_config) / sizeof (TCHAR));
#endif

	if(argc>1) {
		_tcscat (restart_config, argv[1]);
	} else  _tcscat (restart_config, OPTIONSFILENAME);
	default_config = 1;

#ifdef NATMEM_OFFSET
	preinit_shm ();
#endif

#ifndef __LIBRETRO__
	write_log (_T("Enumerating display devices.. \n"));
	enumeratedisplays ();
	write_log (_T("Sorting devices and modes..\n"));
	sortdisplays ();

	write_log (_T("Display buffer mode = %d\n"), ddforceram);
	enumerate_sound_devices ();
	write_log (_T("done\n"));
#endif

	keyboard_settrans ();
#ifdef CATWEASEL
	catweasel_init ();
#endif
#ifdef PARALLEL_PORT
	paraport_mask = paraport_init ();
#endif

#ifdef __LIBRETRO__
	if (restart_program)
	{
		changed_prefs = currprefs;
		real_main2_ret = real_main2 (argc, argv);
	}
#else
	while (restart_program) {
		int ret;
		changed_prefs = currprefs;
		ret = real_main2 (argc, argv);
		if (ret == 0 && quit_to_gui)
			restart_program = 1;
		leave_program ();
		quit_program = 0;
	}
	zfile_exit ();
#endif
}

#ifndef NO_MAIN_IN_MAIN_C
#ifdef __LIBRETRO__
int umain (int argc, TCHAR **argv)
#else
int main (int argc, TCHAR **argv)
#endif
{
	real_main (argc, argv);
	return 0;
}
#endif

#ifdef SINGLEFILE
uae_u8 singlefile_config[50000] = { "_CONFIG_STARTS_HERE" };
uae_u8 singlefile_data[1500000] = { "_DATA_STARTS_HERE" };
#endif

#ifdef __LIBRETRO__
static int real_main2 (int argc, TCHAR **argv);

extern void libretro_do_restart (int argc, TCHAR **argv)
{
	// This is used inside libretro.c to recreate the
	// behaviour of the standard real_main() function
	// - i.e. normally it would do this:
	//    while (restart_program) {
	//      int ret;
	//      changed_prefs = currprefs;
	//      ret = real_main2 (argc, argv);
	//      if (ret == 0 && quit_to_gui)
	//         restart_program = 1;
	//      leave_program ();
	//      quit_program = 0;
	//      }
	// ...whereas the libretro implementation does this:
	//    if (restart_program)
	//    {
	//       changed_prefs = currprefs;
	//       real_main2_ret = real_main2 (argc, argv);
	//    }
	// Consequently, whenever the m68k_go() function returns
	// without detecting a libretro 'frame end' (this basically
	// only happens after calling uae_restart()), we have to
	// 'manually insert' the extra steps (execution order wraps
	// around from real_main2()).
	// Note that after calling libretro_do_restart(), we must
	// emulate a 'first pass' frame - i.e. set firstpass to 1
	// inside libretro.c.
	if (real_main2_ret == 0 && quit_to_gui) // This will never be true, but include for consistency
		restart_program = 1;

	do_leave_program ();
	quit_program = 0;

	if (restart_program)
	{
		changed_prefs = currprefs;
		real_main2_ret = real_main2 (argc, argv);
	}
}
#endif
