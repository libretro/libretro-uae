#include "sysconfig.h"
#include "sysdeps.h"

#include <ctype.h>
#include <assert.h>

#include "options.h"
#include "uae.h"
#include "xwin.h"
#include "gui.h"
#include "custom.h"
#include "drawing.h"
#include "inputdevice.h"
#include "statusline.h"

#define STATUSLINE_MS 3000

#ifdef __LIBRETRO__
#include "libretro-core.h"
extern bool retro_statusbar;
extern int opt_statusbar;
extern int opt_statusbar_position;
static int num_multip = 1;
#endif

/*
* Some code to put status information on the screen.
*/

static bool td_custom;

void statusline_getpos(int monid, int *x, int *y, int width, int height)
{
	int mx = td_custom ? 1 : statusline_get_multiplier(monid) / 100;
	int total_height = TD_TOTAL_HEIGHT * mx;
#ifdef __LIBRETRO__
	total_height = TD_TOTAL_HEIGHT * ((video_config_geometry & PUAE_VIDEO_QUADLINE) ? 2 : 1);
	currprefs.osd_pos.x = 0;
	currprefs.osd_pos.y = (opt_statusbar_position == -1) ? 30000 : opt_statusbar_position; /* Have to fake -1 to get -0 as top position */
#endif
	if (currprefs.osd_pos.x >= 20000) {
		if (currprefs.osd_pos.x >= 30000)
			*y = width * (currprefs.osd_pos.x - 30000) / 1000;
		else
			*y = width - (width * (30000 - currprefs.osd_pos.y) / 1000);
	} else {
		if (currprefs.osd_pos.x >= 0)
			*x = currprefs.osd_pos.x;
		else
			*x = -currprefs.osd_pos.x + 1;
	}
	if (currprefs.osd_pos.y >= 20000) {
		if (currprefs.osd_pos.y >= 30000)
			*y = (height - total_height) * (currprefs.osd_pos.y - 30000) / 1000;
		else
			*y = (height - total_height) - ((height - total_height) * (30000 - currprefs.osd_pos.y) / 1000);
	} else {
		if (currprefs.osd_pos.y >= 0)
			*y = height - total_height - currprefs.osd_pos.y;
		else
			*y = -currprefs.osd_pos.y + 1;
	}
}

int td_numbers_pos = TD_RIGHT | TD_BOTTOM;
int td_numbers_width = TD_DEFAULT_NUM_WIDTH;
int td_numbers_height = TD_DEFAULT_NUM_HEIGHT;
int td_numbers_padx = TD_DEFAULT_PADX;
int td_numbers_pady = TD_DEFAULT_PADY;
const TCHAR *td_characters = _T("0123456789CHD%+-PNKV");
int td_led_width = TD_DEFAULT_LED_WIDTH;
static int td_led_height = TD_DEFAULT_LED_HEIGHT;
int td_width = TD_DEFAULT_WIDTH;

#ifdef __LIBRETRO__
static const char *numbers = { /* ugly  0123456789CHD%+-PNKV */
	"+++++++--++++-+++++++++++++++++-++++++++++++++++++++++++++++++++++++++-+++++++++-+++++++++-+++++++--------------++++++++++-++++++-++++++-+++"
	"+xxxxx+--+xx+-+xxxxx++xxxxx++x+-+x++xxxxx++xxxxx++xxxxx++xxxxx++xxxxx+++xxxx++x+-+x++xxxx+++xx++x+---+++--------+xxxxx++x+++x++x+++x++x+-+x+"
	"+x+++x+--++x+-+++++x++++++x++x+++x++x++++++x++++++++++x++x+++x++x+++x++x++++++x+++x++x+++x++xx+x++--++x++---+++-+x---x++xx++x++x++x+++x+++x+"
	"+x+-+x+---+x+-+xxxxx++xxxxx++xxxxx++xxxxx++xxxxx+--++x+-+xxxxx++xxxxx++x+----+xxxxx++x+-+x++++x+++--+xxx+--+xxx++xxxxx++x+x+x++xxx++-++x+x++"
	"+x+++x+---+x+-+x++++++++++x++++++x++++++x++x+++x+--+x+--+x+++x++++++x++x++++++x+++x++x+++x+++x+xx+--++x++---+++-+x++++++x++xx++x++x++-+x+x+-"
	"+xxxxx+---+x+-+xxxxx++xxxxx+----+x++xxxxx++xxxxx+--+x+--+xxxxx++xxxxx+++xxxx++x+-+x++xxxx+++x++xx+---+++--------+x+----+x+++x++x+++x+-++x++-"
	"+++++++---+++-++++++++++++++----+++++++++++++++++--+++--++++++++++++++-+++++++++-+++++++++-+++++++--------------+++----+++-++++++-+++--+++--"
};
#else
static const char *numbers_default = { /* ugly  0123456789CHD%+-PNKV */
	"+++++++--++++-+++++++++++++++++-++++++++++++++++++++++++++++++++++++++++++++-++++++-++++----++---+--------------++++++++++-++++++++++++  +++"
	"+xxxxx+--+xx+-+xxxxx++xxxxx++x+-+x++xxxxx++xxxxx++xxxxx++xxxxx++xxxxx++xxxx+-+x++x+-+xxx++-+xx+-+x---+----------+xxxxx++x+-+x++x++x++x+  +x+"
	"+x+++x+--++x+-+++++x++++++x++x+++x++x++++++x++++++++++x++x+++x++x+++x++x++++-+x++x+-+x++x+--+x++x+--+x+----+++--+x---x++xx++x++x+x+++x+  +x+"
	"+x+-+x+---+x+-+xxxxx++xxxxx++xxxxx++xxxxx++xxxxx+--++x+-+xxxxx++xxxxx++x+----+xxxx+-+x++x+----+x+--+xxx+--+xxx+-+xxxxx++x+x+x++xx+   +x++x+ "
	"+x+++x+---+x+-+x++++++++++x++++++x++++++x++x+++x+--+x+--+x+++x++++++x++x++++-+x++x+-+x++x+---+x+x+--+x+----+++--+x++++++x+x+x++x+x++  +xx+  "
	"+xxxxx+---+x+-+xxxxx++xxxxx+----+x++xxxxx++xxxxx+--+x+--+xxxxx++xxxxx++xxxx+-+x++x+-+xxx+---+x++xx--------------+x+----+x++xx++x++x+  +xx+  "
	"+++++++---+++-++++++++++++++----+++++++++++++++++--+++--++++++++++++++++++++-++++++-++++------------------------+++----+++++++++++++  ++++  "
//   x      x      x      x      x      x      x      x      x      x      x      x      x      x      x      x      x      x      x      x      x  
};

static const char *numbers = numbers_default;
#endif


STATIC_INLINE uae_u32 ledcolor(uae_u32 c, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *a)
{
	uae_u32 v = rc[(c >> 16) & 0xff] | gc[(c >> 8) & 0xff] | bc[(c >> 0) & 0xff];
	if (a)
		v |= a[255 - ((c >> 24) & 0xff)];
	return v;
}

static void write_tdnumber(uae_u8 *buf, int bpp, int x, int y, int num, uae_u32 c1, uae_u32 c2, int mult)
{
	int j;
	const char *numptr;

#ifdef __LIBRETRO__
	numptr = numbers + num * td_numbers_width + NUMBERS_NUM * td_numbers_width * y;
	for (j = 0; j < (td_numbers_width * num_multip); j++) {
		if (*numptr == 'x')
		{
			putpixel (buf, NULL, bpp, x + j, c1);
            switch (num_multip)
            {
                case 2:
                    putpixel (buf, NULL, bpp, x + 1 + j, c1);
                    break;
                case 4:
                    putpixel (buf, NULL, bpp, x + 1 + j, c1);
                    putpixel (buf, NULL, bpp, x + 2 + j, c1);
                    putpixel (buf, NULL, bpp, x + 3 + j, c1);
                    break;
            }
        }
		else if (*numptr == '+')
		{
		    putpixel (buf, NULL, bpp, x + j, c2);
		    switch (num_multip)
		    {
		        case 2:
		            putpixel (buf, NULL, bpp, x + 1 + j, c2);
		            break;
		        case 4:
		            putpixel (buf, NULL, bpp, x + 1 + j, c2);
		            putpixel (buf, NULL, bpp, x + 2 + j, c2);
		            putpixel (buf, NULL, bpp, x + 3 + j, c2);
		            break;
            }
        }
        switch (num_multip)
        {
            case 2:
                j++;
                break;
            case 4:
                j++;
                j++;
                j++;
                break;
        }
		numptr++;
	}
#else
	numptr = numbers + num * td_numbers_width + NUMBERS_NUM * td_numbers_width * y;
	for (j = 0; j < td_numbers_width; j++) {
		for (int k = 0; k < mult; k++) {
			if (*numptr == 'x')
				putpixel(buf, NULL, bpp, x + j * mult + k, c1);
			else if (*numptr == '+')
				putpixel(buf, NULL, bpp, x + j * mult + k, c2);
		}
		numptr++;
	}
#endif
}

static uae_u32 rgbmuldiv(uae_u32 rgb, int mul, int div)
{
	uae_u32 out = 0;
	for (int i = 0; i < 3; i++) {
		int v = (rgb >> (i * 8)) & 0xff;
		v *= mul;
		v /= div;
		out |= v << (i * 8);
	}
	out |= rgb & 0xff000000;
	return out;
}

static int statusline_mult[2];

void statusline_set_font(const char *newnumbers, int width, int height)
{
#ifndef __LIBRETRO__
	td_numbers_width = TD_DEFAULT_NUM_WIDTH;
	td_numbers_height = TD_DEFAULT_NUM_HEIGHT;
	td_numbers_padx = TD_DEFAULT_PADX;
	td_numbers_pady = TD_DEFAULT_PADY;
	td_led_width = TD_DEFAULT_LED_WIDTH;
	td_led_height = TD_DEFAULT_LED_HEIGHT;
	td_width = TD_DEFAULT_WIDTH;
	td_custom = false;
	numbers = numbers_default;
	if (!newnumbers)
		return;
	numbers = newnumbers;
	td_numbers_width = width;
	td_numbers_height = height;
	td_led_width = td_numbers_width * 3 + td_numbers_width / 2;
	td_width = td_led_width + 6;
	td_custom = true;
#endif
}

int statusline_set_multiplier(int monid, int width, int height)
{
	struct amigadisplay *ad = &adisplays[monid];
	int idx = ad->picasso_on ? 1 : 0;
	int mult = currprefs.leds_on_screen_multiplier[idx];
	if (mult < 1 * 100) {
		mult = 1 * 100;
	}
	if (mult > 4 * 100) {
		mult = 4 * 100;
	}
	statusline_mult[idx] = mult;
	return mult;
}

int statusline_get_multiplier(int monid)
{
	struct amigadisplay *ad = &adisplays[monid];
	int idx = ad->picasso_on ? 1 : 0;
	if (statusline_mult[idx] < 1 * 100)
		return 1 * 100;
	return statusline_mult[idx];
}

#ifdef __LIBRETRO__
#define BLACK           0x000000
#define YELLOW_DISABLED 0x222200
#define YELLOW_DARK     0x555500
#define YELLOW_DIM      0x888800
#define YELLOW_BRIGHT   0xCCCC00
#define GREEN_DISABLED  0x002200
#define GREEN_DARK      0x005500
#define GREEN_DIM       0x008800
#define GREEN_BRIGHT    0x00CC00
#define RED_DARK        0x550000
#define RED_DIM         0x880000
#define RED_BRIGHT      0xCC0000

void draw_status_line_single(int monid, uae_u8 *buf, int bpp, int y, int totalwidth, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *alpha)
{
	struct amigadisplay *ad = &adisplays[monid];
	int x_start, j, led, border;
	uae_u32 c1, c2, cb;
	int mult = (video_config & PUAE_VIDEO_QUADLINE) ? 2 : 1;

	if (!mult)
		return;

	y /= mult;

    if (!retro_statusbar)
        return;

    totalwidth = retrow_crop;
    num_multip = 1;

    if (doublescan || (retrow == PUAE_VIDEO_WIDTH_S72 || retrow == PUAE_VIDEO_WIDTH_S72 * 2))
        num_multip = (retrow == PUAE_VIDEO_WIDTH_S72 * 2 && retroh == PUAE_VIDEO_HEIGHT_S72) ? 2 : 1;
    else if (currprefs.gfx_resolution == RES_HIRES && currprefs.gfx_vresolution == VRES_NONDOUBLE)
        num_multip = 2;
    else if (currprefs.gfx_resolution == RES_SUPERHIRES)
    {
        if (currprefs.gfx_vresolution == VRES_DOUBLE)
            num_multip = 2;
        else
            num_multip = 4;
    }

    int LED_WIDTH = 16;
    int TD_WIDTH = (LED_WIDTH * num_multip);
    td_led_width = TD_WIDTH;

    c1 = ledcolor (0x00ffffff, rc, gc, bc, alpha);
    c2 = ledcolor (0x00000000, rc, gc, bc, alpha);
    cb = ledcolor (TD_BORDER, rc, gc, bc, alpha);

    if (td_numbers_pos & TD_RIGHT)
        x_start = totalwidth - td_numbers_padx - VISIBLE_LEDS * TD_WIDTH;
    else
        x_start = td_numbers_padx;

    x_start += retrox_crop;

    int floppies = 1;
    if (gui_data.hd >= 0 || gui_data.cd >= 0 || gui_data.md >= 0)
    {
        floppies = !(opt_statusbar & STATUSBAR_BASIC) ? 1 : 0;
        if (gui_data.drives[0].df[0])
            floppies = 1;
    }

    for (led = 0; led < LED_MAX; led++) {
        int side, pos, num1 = -1, num2 = -1, num3 = -1, num4 = -1;
        int x, c, on = 0, am = 2;
        xcolnr on_rgb = 0, on_rgb2 = 0, off_rgb = 0, pen_rgb = 0;
        int half = 0;

        if (!(currprefs.leds_on_screen_mask[ad->picasso_on ? 1 : 0] & (1 << led)))
            continue;

        pen_rgb = c1;
        if (led >= LED_DF0 && led <= LED_DF3 && floppies) {
            int pled = led - LED_DF0;
			struct floppyslot *fs = &currprefs.floppyslots[pled];
			struct gui_info_drive *gid = &gui_data.drives[pled];
			int track = gid->drive_track;
            pos = 6 + pled;
            if ((gui_data.hd >= 0 || gui_data.cd >= 0 || gui_data.md >= 0) && !gui_data.drives[0].df[0] && !gui_data.drives[1].df[0])
            {
                on_rgb  = BLACK;
                on_rgb2 = BLACK;
                off_rgb = BLACK;
            }
            else
            {
                if (currprefs.chipset_mask & CSMASK_MASK)
                {
                    on_rgb  = YELLOW_BRIGHT;
                    on_rgb2 = YELLOW_DIM;
                    off_rgb = YELLOW_DARK;
                }
                else
                {
                    on_rgb  = GREEN_BRIGHT;
                    on_rgb2 = GREEN_DIM;
                    off_rgb = GREEN_DARK;
                }

                num1 = -1;
                num2 = track / 10;
                num3 = track % 10;
                on = gid->drive_motor;
                if (gid->drive_writing) {
                    on_rgb  = RED_BRIGHT;
                    on_rgb2 = RED_DIM;
                }
                half = gui_data.drive_side ? 1 : -1;
                if (!gui_data.drives[pled].df[0])
                {
                    if (currprefs.chipset_mask & CSMASK_MASK)
                    {
                        pen_rgb = ledcolor (YELLOW_DIM, rc, gc, bc, alpha);
                        off_rgb = YELLOW_DISABLED;
                    }
                    else
                    {
                        pen_rgb = ledcolor (GREEN_DIM, rc, gc, bc, alpha);
                        off_rgb = GREEN_DISABLED;
                    }
                    num2 = 12;
                    num3 = pled;
                }
            }
            side = gui_data.drive_side;

            if (opt_statusbar & STATUSBAR_MINIMAL)
                num1 = num2 = num3 = -1;
#if 0
        } else if (led == LED_POWER) {
            pos = 3;
            on_rgb = ((gui_data.powerled_brightness * 10 / 16) + 0x33) << 16;
            on = 1;
            off_rgb = 0x330000;
#endif
        } else if (led == LED_CD && gui_data.cd >= 0) {
            pos = (gui_data.hd >= 0) ? 8 : 9;
            if (gui_data.cd >= 0) {
                on = gui_data.cd & (LED_CD_AUDIO | LED_CD_ACTIVE);
                on_rgb = (on & LED_CD_AUDIO) ? YELLOW_DIM : YELLOW_BRIGHT;
                off_rgb = YELLOW_DARK;
#if 0
                if ((gui_data.cd & LED_CD_ACTIVE2) && !(gui_data.cd & LED_CD_AUDIO)) {
                    on_rgb &= 0xfefefe;
                    on_rgb >>= 1;
                }
#endif
                if (!currprefs.cdslots[0].name[0])
                {
                    pen_rgb = ledcolor (YELLOW_DIM, rc, gc, bc, alpha);
                    off_rgb = YELLOW_DISABLED;
                }
                num1 = -1;
                num2 = 10;
                num3 = 12;
            }
            if (opt_statusbar & STATUSBAR_MINIMAL)
                num1 = num2 = num3 = -1;
        } else if (led == LED_HD && gui_data.hd >= 0) {
            pos = 9;
            if (gui_data.hd >= 0) {
                on = gui_data.hd;
                on_rgb  = on == 2 ? RED_BRIGHT : YELLOW_BRIGHT;
                off_rgb = YELLOW_DARK;
                num1 = -1;
                num2 = 11;
                num3 = 12;
            }
            if (opt_statusbar & STATUSBAR_MINIMAL)
                num1 = num2 = num3 = -1;
        } else if (led == LED_FPS) {
            pos = 10;
            if (pause_emulation) {
                num1 = -1;
                num2 = -1;
                num3 = 16;
                on_rgb  = 0xcccccc;
                off_rgb = 0x000000;
                am = 2;
            } else {
                int fps = (gui_data.fps + 5) / 10;
                on_rgb  = 0x000000;
                off_rgb = gui_data.fps_color ? 0xcccc00 : 0x000000;
                am = 2;
                if (fps > 999) {
                    num1 = -1;
                    num2 = fps / 1000;
                    num3 = 18;
                } else if (fps > 99) {
                    num1 = -1;
                    num2 = fps / 100;
                    num3 = 10;
                } else {
                    num1 = fps / 100;
                    num2 = (fps - num1 * 100) / 10;
                    num3 = fps % 10;
                    if (num1 == 0)
                        am = 2;
                }
            }
            if (currprefs.chipset_mask & CSMASK_MASK)
            {
                on_rgb  = GREEN_BRIGHT;
                off_rgb = GREEN_DARK;
            }
            else
            {
                on_rgb  = RED_BRIGHT;
                off_rgb = RED_DARK;
            }
            on = (gui_data.powerled_brightness > 0) ? 1 : 0;
            if (opt_statusbar & STATUSBAR_MINIMAL)
                num1 = num2 = num3 = -1;
#if 0
        } else if (led == LED_CPU) {
            int idle = (gui_data.idle + 5) / 10;
            pos = 1;
            on_rgb = 0xcc0000;
            off_rgb = 0x000000;
            if (gui_data.cpu_halted) {
                on_rgb = 0xcccc00;
                idle = 0;
                on = 1;
                if (gui_data.cpu_halted < 0) {
                    on_rgb = 0x000000;
                    num1 = 16; // PPC
                    num2 = 16;
                    num3 = 10;
                    am = 3;
                } else {
                    on_rgb = 0xcccc00;
                    num1 = gui_data.cpu_halted >= 10 ? 11 : -1;
                    num2 = gui_data.cpu_halted >= 10 ? gui_data.cpu_halted / 10 : 11;
                    num3 = gui_data.cpu_halted % 10;
                    am = 2;
                }
            } else {
                num1 = idle / 100;
                num2 = (idle - num1 * 100) / 10;
                num3 = idle % 10;
                num4 = num1 == 0 ? 13 : -1;
                am = 3;
            }
        } else if (led == LED_SND) {
            int snd = abs(gui_data.sndbuf + 5) / 10;
            if (snd > 99)
                snd = 99;
            pos = 0;
            on = gui_data.sndbuf_status;
            if (on < 3) {
                num1 = gui_data.sndbuf < 0 ? 15 : 14;
                num2 = snd / 10;
                num3 = snd % 10;
            }
            on_rgb = 0x000000;
            if (on < 0)
                on_rgb = 0xcccc00; // underflow
            else if (on == 2)
                on_rgb = 0xcc0000; // really big overflow
            else if (on == 1)
                on_rgb = 0x0000cc; // "normal" overflow
            off_rgb = 0x000000;
            am = 3;
#endif
        } else if (led == LED_MD && gui_data.drives[3].drive_disabled && gui_data.md >= 1) {
            // DF3 reused as internal non-volatile ram led (cd32/cdtv)
            pos = 8;
            if (gui_data.md >= 1) {
                on = gui_data.md;
                on_rgb = on == 2 ? RED_BRIGHT : GREEN_BRIGHT;
                off_rgb = BLACK;
            }
            num1 = -1;
            num2 = -1;
            num3 = -1;

            if (on)
            {
                num2 = 17;
                num3 = 19;
            }
            if (opt_statusbar & STATUSBAR_MINIMAL)
                num1 = num2 = num3 = -1;
#if 0
        } else if (led == LED_NET) {
            pos = 6;
            if (gui_data.net >= 0) {
                on = gui_data.net;
                on_rgb = 0;
                if (on & 1)
                    on_rgb |= 0x00cc00;
                if (on & 2)
                    on_rgb |= 0xcc0000;
                off_rgb = 0x000000;
                num1 = -1;
                num2 = -1;
                num3 = 17;
                am = 1;
            }
#endif
        } else {
            continue;
        }
        on_rgb |= 0x33000000;
        off_rgb |= 0x33000000;
        if (half > 0) {
            c = ledcolor (on ? (y >= TD_TOTAL_HEIGHT / 2 ? on_rgb2 : on_rgb) : off_rgb, rc, gc, bc, alpha);
        } else if (half < 0) {
            c = ledcolor (on ? (y < TD_TOTAL_HEIGHT / 2 ? on_rgb2 : on_rgb) : off_rgb, rc, gc, bc, alpha);
        } else {
            c = ledcolor (on ? on_rgb : off_rgb, rc, gc, bc, alpha);
        }
        border = 0;

        if (y == 0 || y == TD_TOTAL_HEIGHT - 1) {
            c = ledcolor (TD_BORDER, rc, gc, bc, alpha);
            //border = 1;// Disable rounded borders
        }

        x = x_start - num_multip + 1 + pos * TD_WIDTH;
        if (!border)
        {
            putpixel (buf, NULL, bpp, x, cb);
            switch (num_multip)
            {
                case 2:
                    putpixel (buf, NULL, bpp, x + 1, cb);
                    break;
                case 4:
                    putpixel (buf, NULL, bpp, x + 1, cb);
                    putpixel (buf, NULL, bpp, x + 2, cb);
                    putpixel (buf, NULL, bpp, x + 3, cb);
                    break;
            }
        }
        x = x + (num_multip);
        for (j = 0; j < td_led_width - num_multip; j++)
            putpixel (buf, NULL, bpp, x + j, c);
        if (!border)
        {
            putpixel (buf, NULL, bpp, x + j, cb);
            switch (num_multip)
            {
                case 2:
                    putpixel (buf, NULL, bpp, x + 1 + j, cb);
                    break;
                case 4:
                    putpixel (buf, NULL, bpp, x + 1 + j, cb);
                    putpixel (buf, NULL, bpp, x + 2 + j, cb);
                    putpixel (buf, NULL, bpp, x + 3 + j, cb);
                    break;
            }
        }

        if (y >= td_numbers_pady && y - td_numbers_pady < td_numbers_height) {
            if (num3 >= 0) {
                x += (td_led_width - am * td_numbers_width * num_multip) / 2;
                if (num1 > 0) {
                    write_tdnumber (buf, bpp, x, y - td_numbers_pady, num1, pen_rgb, c2, mult);
                    x += td_numbers_width * num_multip;
                }
                if (num2 >= 0) {
                    write_tdnumber (buf, bpp, x, y - td_numbers_pady, num2, pen_rgb, c2, mult);
                    x += td_numbers_width * num_multip;
                }
                x = x - (num_multip);
                write_tdnumber (buf, bpp, x, y - td_numbers_pady, num3, pen_rgb, c2, mult);
                x += td_numbers_width * num_multip;
                if (num4 > -1)
                    write_tdnumber (buf, bpp, x, y - td_numbers_pady, num4, pen_rgb, c2, mult);
            }
        }
    }
}
#else
void draw_status_line_single(int monid, uae_u8 *buf, int bpp, int y, int totalwidth, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *alpha)
{
	struct amigadisplay *ad = &adisplays[monid];
	int x_start, j, led, border;
	uae_u32 c1, c2, cb;
	int mult = td_custom ? 1 : statusline_mult[ad->picasso_on ? 1 : 0] / 100;

	if (!mult)
		return;

	y /= mult;

	c1 = ledcolor (0x00ffffff, rc, gc, bc, alpha);
	c2 = ledcolor (0x00111111, rc, gc, bc, alpha);

	if (td_numbers_pos & TD_RIGHT)
		x_start = totalwidth - (td_numbers_padx + VISIBLE_LEDS * td_width) * mult;
	else
		x_start = td_numbers_padx * mult;

	for (led = 0; led < LED_MAX; led++) {
		int pos, num1 = -1, num2 = -1, num3 = -1, num4 = -1;
		int x, c, on = 0, am = 2;
		xcolnr on_rgb = 0, on_rgb2 = 0, off_rgb = 0, pen_rgb = 0;
		int half = 0, extraborder = 0;

		cb = ledcolor(TD_BORDER, rc, gc, bc, alpha);

		if (!(currprefs.leds_on_screen_mask[ad->picasso_on ? 1 : 0] & (1 << led)))
			continue;

		pen_rgb = c1;
		if (led >= LED_DF0 && led <= LED_DF3) {
			int pled = led - LED_DF0;
			struct floppyslot *fs = &currprefs.floppyslots[pled];
			struct gui_info_drive *gid = &gui_data.drives[pled];
			int track = gid->drive_track;
			pos = 7 + pled;
			on_rgb = 0x00cc00;
			if (!gid->drive_disabled) {
				num1 = -1;
				num2 = track / 10;
				num3 = track % 10;
				on = gid->drive_motor;
				if (gid->drive_writing) {
					on_rgb = 0xcc0000;
				}
				half = gui_data.drive_side ? 1 : -1;
				if (!gid->floppy_inserted) {
					pen_rgb = ledcolor(0x00aaaaaa, rc, gc, bc, alpha);
				} else if (gid->floppy_protected) {
					cb = ledcolor(0x00cc00, rc, gc, bc, alpha);
					extraborder = 1;
				}
			}
			on_rgb &= 0xffffff;
			off_rgb = rgbmuldiv(on_rgb, 2, 4);
			on_rgb2 = rgbmuldiv(on_rgb, 2, 3);
		} else if (led == LED_POWER) {
			pos = 3;
			on_rgb = ((gui_data.powerled_brightness * 10 / 16) + 0x33) << 16;
			on = 1;
			off_rgb = 0x330000;
		} else if (led == LED_CD) {
			pos = 5;
			if (gui_data.cd >= 0) {
				on = gui_data.cd & (LED_CD_AUDIO | LED_CD_ACTIVE);
				on_rgb = (on & LED_CD_AUDIO) ? 0x00cc00 : 0x0000cc;
				if ((gui_data.cd & LED_CD_ACTIVE2) && !(gui_data.cd & LED_CD_AUDIO)) {
					on_rgb &= 0xfefefe;
					on_rgb >>= 1;
				}
				off_rgb = 0x000033;
				num1 = -1;
				num2 = 10;
				num3 = 12;
			}
		} else if (led == LED_HD) {
			pos = 4;
			if (gui_data.hd >= 0) {
				on = gui_data.hd;
				on_rgb = on == 2 ? 0xcc0000 : 0x0000cc;
				off_rgb = 0x000033;
				num1 = -1;
				num2 = 11;
				num3 = 12;
			}
		} else if (led == LED_FPS) {
			pos = 2;
			if (pause_emulation) {
				num1 = -1;
				num2 = -1;
				num3 = 16;
				on_rgb = 0xcccccc;
				off_rgb = 0x111111;
				am = 2;
			} else {
				int fps = (gui_data.fps + 5) / 10;
				on_rgb = 0x111111;
				off_rgb = gui_data.fps_color == 1 ? 0xcccc00 : (gui_data.fps_color == 2 ? 0x0000cc : 0x111111);
				am = 3;
				if (gui_data.fps_color >= 2) {
					num1 = -1;
					num2 = 15;
					num3 = 15;
					am = 2;
				} else {
					if (fps > 999) {
						fps += 50;
						fps /= 10;
						if (fps > 999)
							fps = 999;
						num1 = fps / 100;
						num1 %= 10;
						num2 = 18;
						num3 = (fps - num1 * 100) / 10;
					} else {
						num1 = fps / 100;
						num2 = (fps - num1 * 100) / 10;
						num3 = fps % 10;
						num1 %= 10;
						num2 %= 10;
						if (num1 == 0)
							am = 2;
					}
				}
			}
		} else if (led == LED_CPU) {
			int idle = (gui_data.idle + 5) / 10;
			pos = 1;
			on_rgb = 0xcc0000;
			off_rgb = 0x111111;
			if (gui_data.cpu_halted) {
				idle = 0;
				on = 1;
				if (gui_data.cpu_halted < 0) {
					on_rgb = 0x111111;
					num1 = 16; // PPC
					num2 = 16;
					num3 = 10;
					am = 3;
				} else {
					on_rgb = 0xcccc00;
					num1 = gui_data.cpu_halted >= 10 ? 11 : -1;
					num2 = gui_data.cpu_halted >= 10 ? (gui_data.cpu_halted / 10) % 10 : 11;
					num3 = gui_data.cpu_halted % 10;
					am = 2;
				}
			} else {
				num1 = idle / 100;
				num2 = (idle - num1 * 100) / 10;
				num3 = idle % 10;
				num1 %= 10;
				num2 %= 10;
				num4 = num1 == 0 ? 13 : -1;
				if (!num1 && !num2) {
					num2 = -2;
				}
				am = 3;
			}
		} else if (led == LED_SND && gui_data.sndbuf_avail) {
			int snd = abs(gui_data.sndbuf + 5) / 10;
			if (snd > 99)
				snd = 99;
			pos = 0;
			on = gui_data.sndbuf_status;
			if (on < 3) {
				num1 = gui_data.sndbuf < 0 ? 15 : 14;
				num2 = snd / 10;
				num3 = snd % 10;
			}
			on_rgb = 0x111111;
			if (on < 0)
				on_rgb = 0xcccc00; // underflow
			else if (on == 2)
				on_rgb = 0xcc0000; // really big overflow
			else if (on == 1)
				on_rgb = 0x0000cc; // "normal" overflow
			off_rgb = 0x111111;
			am = 3;
		} else if (led == LED_MD) {
			// DF3 reused as internal non-volatile ram led (cd32/cdtv)
			if (gui_data.drives[3].drive_disabled && gui_data.md >= 0) {
				pos = 7 + 3;
				if (gui_data.md >= 0) {
					on = gui_data.md;
					on_rgb = on == 2 ? 0xcc0000 : 0x00cc00;
					off_rgb = 0x003300;
				}
				num1 = -1;
				num2 = 17;
				num3 = 19;
			} else {
				continue;
			}
		} else if (led == LED_NET) {
			pos = 6;
			if (gui_data.net >= 0) {
				on = gui_data.net;
				on_rgb = 0;
				if (on & 1)
					on_rgb |= 0x00cc00;
				if (on & 2)
					on_rgb |= 0xcc0000;
				off_rgb = 0x111111;
				num1 = -1;
				num2 = -1;
				num3 = 17;
				am = 1;
			}
		} else {
			continue;
		}
		on_rgb |= 0x33111111;
		off_rgb |= 0x33111111;
		if (half > 0) {
			int halfon = y >= TD_TOTAL_HEIGHT / 2;
			c = ledcolor(on ? (halfon ? on_rgb2 : on_rgb) : off_rgb, rc, gc, bc, alpha);
			if (!halfon && on && extraborder)
				cb = rgbmuldiv(cb, 2, 3);
		} else if (half < 0) {
			int halfon = y < TD_TOTAL_HEIGHT / 2;
			c = ledcolor(on ? (halfon ? on_rgb2 : on_rgb) : off_rgb, rc, gc, bc, alpha);
			if (!halfon && on && extraborder)
				cb = rgbmuldiv(cb, 2, 3);
		} else {
			c = ledcolor(on ? on_rgb : off_rgb, rc, gc, bc, alpha);
		}

		border = 0;
		if (y == 0 || y == TD_TOTAL_HEIGHT - 1) {
			c = cb;
			border = 1;
		}

		x = x_start + pos * td_width * mult;
		for (int xx = 0; xx < mult; xx++) {
			if (!border) {
				putpixel(buf, NULL, bpp, x - mult + xx, cb);
			}
			for (j = 0; j < td_led_width * mult; j += mult) {
				putpixel(buf, NULL, bpp, x + j + xx, c);
			}
			if (!border) {
				putpixel(buf, NULL, bpp, x + j + xx, cb);
			}
		}

		if (y >= td_numbers_pady && y - td_numbers_pady < td_numbers_height) {
			if (num3 >= 0) {
				x += (td_led_width - am * td_numbers_width) * mult / 2;
				if (num1 > 0) {
					write_tdnumber(buf, bpp, x, y - td_numbers_pady, num1, pen_rgb, c2, mult);
					x += td_numbers_width * mult;
				}
				if (num2 >= 0) {
					write_tdnumber(buf, bpp, x, y - td_numbers_pady, num2, pen_rgb, c2, mult);
					x += td_numbers_width * mult;
				} else if (num2 < -1) {
					x += td_numbers_width * mult;
				}
				write_tdnumber(buf, bpp, x, y - td_numbers_pady, num3, pen_rgb, c2, mult);
				x += td_numbers_width * mult;
				if (num4 > 0)
					write_tdnumber(buf, bpp, x, y - td_numbers_pady, num4, pen_rgb, c2, mult);
			}
		}
	}
}

#endif /* __LIBRETRO__ */

#define MAX_STATUSLINE_QUEUE 8
struct statusline_struct
{
	TCHAR *text;
	int type;
};
struct statusline_struct statusline_data[MAX_STATUSLINE_QUEUE];
static TCHAR *statusline_text_active;
static int statusline_delay;
static bool statusline_had_changed;

bool has_statusline_updated(void)
{
	bool v = statusline_had_changed;
	statusline_had_changed = false;
	return v;
}

static void statusline_update_notification(void)
{
	statusline_had_changed = true;
	statusline_updated(0);
}

void statusline_clear(void)
{
	statusline_text_active = NULL;
	statusline_delay = 0;
	for (int i = 0; i < MAX_STATUSLINE_QUEUE; i++) {
		xfree(statusline_data[i].text);
		statusline_data[i].text = NULL;
	}
	statusline_update_notification();
}

const TCHAR *statusline_fetch(void)
{
	return statusline_text_active;
}

void statusline_add_message(int statustype, const TCHAR *format, ...)
{
	va_list parms;
	TCHAR buffer[256];

	if (isguiactive())
		return;

	va_start(parms, format);
	buffer[0] = ' ';
	_vsntprintf(buffer + 1, 256 - 2, format, parms);
	_tcscat(buffer, _T(" "));

	for (int i = 0; i < MAX_STATUSLINE_QUEUE; i++) {
		if (statusline_data[i].text != NULL && statusline_data[i].type == statustype) {
			xfree(statusline_data[i].text);
			statusline_data[i].text = NULL;
			for (int j = i + 1; j < MAX_STATUSLINE_QUEUE; j++) {
				memcpy(&statusline_data[j - 1], &statusline_data[j], sizeof(struct statusline_struct));
			}
			statusline_data[MAX_STATUSLINE_QUEUE - 1].text = NULL;
		}
	}

	if (statusline_data[1].text) {
		for (int i = 0; i < MAX_STATUSLINE_QUEUE; i++) {
			if (statusline_data[i].text && !_tcscmp(statusline_data[i].text, buffer)) {
				xfree(statusline_data[i].text);
				for (int j = i + 1; j < MAX_STATUSLINE_QUEUE; j++) {
					memcpy(&statusline_data[j - 1], &statusline_data[j], sizeof(struct statusline_struct));
				}
				statusline_data[MAX_STATUSLINE_QUEUE - 1].text = NULL;
				i = 0;
			}
		}
	} else if (statusline_data[0].text) {
		if (!_tcscmp(statusline_data[0].text, buffer))
			return;
	}

	for (int i = 0; i < MAX_STATUSLINE_QUEUE; i++) {
		if (statusline_data[i].text == NULL) {
			statusline_data[i].text = my_strdup(buffer);
			statusline_data[i].type = statustype;
			if (i == 0)
				statusline_delay = (int)(STATUSLINE_MS * vblank_hz / 1000.0f);
			statusline_text_active = statusline_data[0].text;
			statusline_update_notification();
			return;
		}
	}
	statusline_text_active = NULL;
	xfree(statusline_data[0].text);
	for (int i = 1; i < MAX_STATUSLINE_QUEUE; i++) {
		memcpy(&statusline_data[i - 1], &statusline_data[i], sizeof(struct statusline_struct));
	}
	statusline_data[MAX_STATUSLINE_QUEUE - 1].text = my_strdup(buffer);
	statusline_data[MAX_STATUSLINE_QUEUE - 1].type = statustype;
	statusline_text_active = statusline_data[0].text;
	statusline_update_notification();

	va_end(parms);
}

void statusline_vsync(void)
{
	if (!statusline_data[0].text)
		return;
	if (statusline_delay == 0)
		statusline_delay = (int)(STATUSLINE_MS * vblank_hz / (1000.0f * 1.0f));
	if (statusline_delay > STATUSLINE_MS * vblank_hz / (1000.0f * 1.0f))
		statusline_delay = (int)(STATUSLINE_MS * vblank_hz / (1000.0f * 1.0f));
	if (statusline_delay > STATUSLINE_MS * vblank_hz / (1000.0f * 3.0f) && statusline_data[1].text)
		statusline_delay = (int)(STATUSLINE_MS * vblank_hz / (1000.0f * 3.0f));
	statusline_delay--;
	if (statusline_delay)
		return;
	statusline_text_active = NULL;
	xfree(statusline_data[0].text);
	for (int i = 1; i < MAX_STATUSLINE_QUEUE; i++) {
		statusline_data[i - 1].text = statusline_data[i].text;
	}
	statusline_data[MAX_STATUSLINE_QUEUE - 1].text = NULL;
	statusline_text_active = statusline_data[0].text;
	statusline_update_notification();
}

void statusline_single_erase(int monid, uae_u8 *buf, int bpp, int y, int totalwidth)
{
	memset(buf, 0, bpp * totalwidth);
}
