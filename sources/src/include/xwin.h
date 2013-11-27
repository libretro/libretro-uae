#pragma once
#ifndef SRC_INCLUDE_XWIN_H_INCLUDED
#define SRC_INCLUDE_XWIN_H_INCLUDED 1

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the graphics system (X, SVGAlib)
  *
  * Copyright 1995-1997 Bernd Schmidt
  * Copyright 2005 Richard Drummond
  */

#define MAX_UAE_WIDTH 8192
#define MAX_UAE_HEIGHT 8192

typedef uae_u32 xcolnr;

typedef int (*allocfunc_type)(int, int, int, xcolnr *);

extern xcolnr xcolors[4096];
extern xcolnr xcolors_16[4096];
extern xcolnr xcolors_32[4096];
extern uae_u32 p96_rgbx16[65536];

extern int flashscreen;
void updatedisplayarea (void);
int isvsync_chipset (void);
int isvsync_rtg (void);
int isvsync (void);

int debuggable (void);
void LED (int);
void screenshot (int,int);

int bits_in_mask (unsigned long mask);
int mask_shift (unsigned long mask);
unsigned int doMask (int p, int bits, int shift);
unsigned int doMask256 (int p, int bits, int shift);
void setup_maxcol (int);
void alloc_colors256 (int (*)(int, int, int, xcolnr *));
void alloc_colors64k (int, int, int, int, int, int, int, int, int, int);
void alloc_colors_rgb (int rw, int gw, int bw, int rs, int gs, int bs, int aw, int as, int alpha, int byte_swap,
			      uae_u32 *rc, uae_u32 *gc, uae_u32 *bc);
void alloc_colors_picasso (int rw, int gw, int bw, int rs, int gs, int bs, int rgbfmt);
void setup_greydither (int bits, allocfunc_type allocfunc);
void setup_greydither_maxcol (int maxcol, allocfunc_type allocfunc);
void setup_dither (int bits, allocfunc_type allocfunc);
void DitherLine (uae_u8 *l, uae_u16 *r4g4b4, int x, int y, uae_s16 len, int bits) ASM_SYM_FOR_FUNC("DitherLine");
double getvsyncrate (double hz, int *mult);

    /* The graphics code has a choice whether it wants to use a large buffer
     * for the whole display, or only a small buffer for a single line.
     * If you use a large buffer:
     *   - set bufmem to point at it
     *   - set linemem to 0
     *   - if memcpy within bufmem would be very slow, i.e. because bufmem is
     *     in graphics card memory, also set emergmem to point to a buffer
     *     that is large enough to hold a single line.
     *   - implement flush_line to be a no-op.
     * If you use a single line buffer:
     *   - set bufmem and emergmem to 0
     *   - set linemem to point at your buffer
     *   - implement flush_line to copy a single line to the screen
     */
struct vidbuf_description
{
	/* Function implemented by graphics driver */
	void (*flush_line)         (struct vidbuf_description *gfxinfo, int line_no);
	void (*flush_block)        (struct vidbuf_description *gfxinfo, int first_line, int end_line);
	void (*flush_screen)       (struct vidbuf_description *gfxinfo, int first_line, int end_line);
	void (*flush_clear_screen) (struct vidbuf_description *gfxinfo);
	int  (*lockscr)            (struct vidbuf_description *gfxinfo);
	void (*unlockscr)          (struct vidbuf_description *gfxinfo);
	uae_u8 *linemem;
	uae_u8 *emergmem;

	uae_u8 *bufmem, *bufmemend;
	uae_u8 *realbufmem;
	uae_u8 *bufmem_allocated;
	bool bufmem_lockable;
	int rowbytes; /* Bytes per row in the memory pointed at by bufmem. */
	int pixbytes; /* Bytes per pixel. */
	/* size of this buffer */
	int width_allocated;
	int height_allocated;
	/* size of max visible image */
	int outwidth;
	int outheight;
	/* nominal size of image for centering */
	int inwidth;
	int inheight;
	/* same but doublescan multiplier included */
	int inwidth2;
	int inheight2;
	/* use drawbuffer instead */
	bool nativepositioning;
	/* tempbuffer in use */
	bool tempbufferinuse;
	/* extra width, chipset hpos extra in right border */
	int extrawidth;

	int xoffset; /* superhires pixels from left edge */
	int yoffset; /* lines from top edge */

	int inxoffset; /* positive if sync positioning */
	int inyoffset;

	int maxblocklines; /* Set to 0 if you want calls to flush_line after each drawn line, or the number of
			* lines that flush_block wants to/can handle (it isn't really useful to use another
			* value than maxline here). */

	int gfx_resolution_reserved; // reserved space for currprefs.gfx_resolution
	int gfx_vresolution_reserved; // reserved space for currprefs.gfx_resolution
	int xchange; /* how many superhires pixels in one pixel in buffer */
	int ychange; /* how many interlaced lines in one line in buffer */
};

bool isnativevidbuf (void);

#define MAXBLOCKLINES_MAX INT_MAX;

extern struct vidbuf_description gfxvidinfo;

STATIC_INLINE void flush_line (int line_no)
{
    gfxvidinfo.flush_line (&gfxvidinfo, line_no);
}

STATIC_INLINE void flush_block (int first_line, int last_line)
{
    gfxvidinfo.flush_block (&gfxvidinfo, first_line, last_line);
}

STATIC_INLINE void flush_screen (int first_line, int last_line)
{
    gfxvidinfo.flush_screen (&gfxvidinfo, first_line, last_line);
}

STATIC_INLINE void flush_clear_screen (void)
{
    gfxvidinfo.flush_clear_screen (&gfxvidinfo);
}

STATIC_INLINE int lockscr (void)
{
    return gfxvidinfo.lockscr (&gfxvidinfo);
}

STATIC_INLINE void unlockscr (void)
{
    gfxvidinfo.unlockscr (&gfxvidinfo);
}

int  graphics_setup        (void);
void graphics_leave        (void);
int  graphics_init         (void);
int  graphics_open         (void);
void graphics_close        (void);
void graphics_notify_state (int state);
bool handle_events         (void);
int  is_fullscreen         (void);
//int  is_vsync              (void);
void toggle_fullscreen     (int mode);
void toggle_mousegrab      (void);

/* For ports using tui.c, this should be built by graphics_setup(). */
extern struct bstring *video_mode_menu;
void vidmode_menu_selected(int);

#endif // SRC_INCLUDE_XWIN_H_INCLUDED
