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

void statusline_getpos (int *x, int *y, int width, int height)
{
#ifdef __LIBRETRO__
	currprefs.osd_pos.x=0;
	currprefs.osd_pos.y=(opt_statusbar_position == -1) ? 30000 : opt_statusbar_position; // Have to fake -1 to get -0 as top position
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
			*y = (height - TD_TOTAL_HEIGHT) * (currprefs.osd_pos.y - 30000) / 1000;
		else
			*y = (height - TD_TOTAL_HEIGHT) - ((height - TD_TOTAL_HEIGHT) * (30000 - currprefs.osd_pos.y) / 1000);
	} else {
		if (currprefs.osd_pos.y >= 0)
			*y = height - TD_TOTAL_HEIGHT - currprefs.osd_pos.y;
		else
			*y = -currprefs.osd_pos.y + 1;
	}
}

#ifdef __LIBRETRO__
static const char *numbers = { /* ugly  0123456789CHD%+-PNKV */
	"+++++++--++++-+++++++++++++++++-++++++++++++++++++++++++++++++++++++++-+++++++++-+++++++++--++---+--------------++++++++++-+++++++++++++-+++"
	"+xxxxx+--+xx+-+xxxxx++xxxxx++x+-+x++xxxxx++xxxxx++xxxxx++xxxxx++xxxxx+++xxxx++x+-+x++xxxx+++xx+-+x---+----------+xxxxx++x+++x++x++x+++x+-+x+"
	"+x+++x+--++x+-+++++x++++++x++x+++x++x++++++x++++++++++x++x+++x++x+++x++x++++++x+++x++x+++x+-+x++x+--+x+----+++--+x---x++xx++x++x+x++++x+++x+"
	"+x+-+x+---+x+-+xxxxx++xxxxx++xxxxx++xxxxx++xxxxx+--++x+-+xxxxx++xxxxx++x+----+xxxxx++x+-+x+---+x+--+xxx+--+xxx+-+xxxxx++x+x+x++xx++++++x+x++"
	"+x+++x+---+x+-+x++++++++++x++++++x++++++x++x+++x+--+x+--+x+++x++++++x++x++++++x+++x++x+++x+--+x+x+--+x+----+++--+x++++++x++xx++x+x+++-+x+x+-"
	"+xxxxx+---+x+-+xxxxx++xxxxx+----+x++xxxxx++xxxxx+--+x+--+xxxxx++xxxxx+++xxxx++x+-+x++xxxx++-+x++xx--------------+x+----+x+++x++x++x++-++x++-"
	"+++++++---+++-++++++++++++++----+++++++++++++++++--+++--++++++++++++++-+++++++++-+++++++++----------------------+++----+++-++++++++++--+++--"
};
#else
static const char *numbers = { /* ugly  0123456789CHD%+-PNK */
	"+++++++--++++-+++++++++++++++++-++++++++++++++++++++++++++++++++++++++++++++-++++++-++++----++---+--------------+++++++++++++++++++++"
	"+xxxxx+--+xx+-+xxxxx++xxxxx++x+-+x++xxxxx++xxxxx++xxxxx++xxxxx++xxxxx++xxxx+-+x++x+-+xxx++-+xx+-+x---+----------+xxxxx++x+++x++x++x++"
	"+x+++x+--++x+-+++++x++++++x++x+++x++x++++++x++++++++++x++x+++x++x+++x++x++++-+x++x+-+x++x+--+x++x+--+x+----+++--+x---x++xx++x++x+x+++"
	"+x+-+x+---+x+-+xxxxx++xxxxx++xxxxx++xxxxx++xxxxx+--++x+-+xxxxx++xxxxx++x+----+xxxx+-+x++x+----+x+--+xxx+--+xxx+-+xxxxx++x+x+x++xx++++"
	"+x+++x+---+x+-+x++++++++++x++++++x++++++x++x+++x+--+x+--+x+++x++++++x++x++++-+x++x+-+x++x+---+x+x+--+x+----+++--+x++++++x+x+x++x+x+++"
	"+xxxxx+---+x+-+xxxxx++xxxxx+----+x++xxxxx++xxxxx+--+x+--+xxxxx++xxxxx++xxxx+-+x++x+-+xxx+---+x++xx--------------+x+----+x++xx++x++x++"
	"+++++++---+++-++++++++++++++----+++++++++++++++++--+++--++++++++++++++++++++-++++++-++++------------------------+++----++++++++++++++"
};

#endif

STATIC_INLINE uae_u32 ledcolor (uae_u32 c, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *a)
{
	uae_u32 v = rc[(c >> 16) & 0xff] | gc[(c >> 8) & 0xff] | bc[(c >> 0) & 0xff];
	if (a)
		v |= a[255 - ((c >> 24) & 0xff)];
	return v;
}

static void write_tdnumber (uae_u8 *buf, int bpp, int x, int y, int num, uae_u32 c1, uae_u32 c2)
{
	int j;
	const char *numptr;

#ifdef __LIBRETRO__
	numptr = numbers + num * TD_NUM_WIDTH + 20 * TD_NUM_WIDTH * y;
	for (j = 0; j < (TD_NUM_WIDTH * num_multip); j++) {
		if (*numptr == 'x')
		{
			putpixel (buf, bpp, x + j, c1, 1);
            switch (num_multip)
            {
                case 2:
                    putpixel (buf, bpp, x + 1 + j, c1, 1);
                    break;
                case 4:
                    putpixel (buf, bpp, x + 1 + j, c1, 1);
                    putpixel (buf, bpp, x + 2 + j, c1, 1);
                    putpixel (buf, bpp, x + 3 + j, c1, 1);
                    break;
            }
        }
		else if (*numptr == '+')
		{
		    putpixel (buf, bpp, x + j, c2, 0);
		    switch (num_multip)
		    {
		        case 2:
		            putpixel (buf, bpp, x + 1 + j, c2, 0);
		            break;
		        case 4:
		            putpixel (buf, bpp, x + 1 + j, c2, 0);
		            putpixel (buf, bpp, x + 2 + j, c2, 0);
		            putpixel (buf, bpp, x + 3 + j, c2, 0);
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
    numptr = numbers + num * TD_NUM_WIDTH + NUMBERS_NUM * TD_NUM_WIDTH * y;

	for (j = 0; j < TD_NUM_WIDTH; j++) {
		if (*numptr == 'x')
			putpixel (buf, bpp, x + j, c1, 1);
		else if (*numptr == '+')
			putpixel (buf, bpp, x + j, c2, 0);
		numptr++;
	}
#endif
}

#ifdef __LIBRETRO__
#define BLACK           0x000000
#define YELLOW_DISABLED 0x222200
#define YELLOW_DARK     0x444400
#define YELLOW_DIM      0x666600
#define YELLOW_BRIGHT   0x999900
#define GREEN_DISABLED  0x002200
#define GREEN_DARK      0x004400
#define GREEN_DIM       0x006600
#define GREEN_BRIGHT    0x009900
#define RED_DARK        0x440000
#define RED_DIM         0x660000
#define RED_BRIGHT      0x990000

void draw_status_line_single (uae_u8 *buf, int bpp, int y, int totalwidth, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *alpha)
{
    if (!retro_statusbar)
        return;

    totalwidth = zoomed_width;
    num_multip = 1;
    if (currprefs.gfx_resolution == RES_HIRES && currprefs.gfx_vresolution == VRES_NONDOUBLE)
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
    int TD_LED_WIDTH = TD_WIDTH;

    int x_start, j, led, border;
    uae_u32 c1, c2, cb;

    c1 = ledcolor (0x00ffffff, rc, gc, bc, alpha);
    c2 = ledcolor (0x00000000, rc, gc, bc, alpha);
    cb = ledcolor (TD_BORDER, rc, gc, bc, alpha);

    if (td_pos & TD_RIGHT)
        x_start = totalwidth - TD_PADX - VISIBLE_LEDS * TD_WIDTH;
    else
        x_start = TD_PADX;

    int floppies = 1;
    if (gui_data.hd >= 0 || gui_data.cd >= 0 || gui_data.md >= 0)
    {
        floppies = !(opt_statusbar & STATUSBAR_BASIC) ? 1 : 0;
        if (gui_data.df[0][0])
            floppies = 1;
    }

    for (led = 0; led < LED_MAX; led++) {
        int side, pos, num1 = -1, num2 = -1, num3 = -1, num4 = -1;
        int x, c, on = 0, am = 2;
        xcolnr on_rgb = 0, on_rgb2 = 0, off_rgb = 0, pen_rgb = 0;
        int half = 0;

        if (!(currprefs.leds_on_screen_mask[picasso_on ? 1 : 0] & (1 << led)))
            continue;

        pen_rgb = c1;
        if (led >= LED_DF0 && led <= LED_DF3 && floppies) {
            int pled = led - LED_DF0;
            int track = gui_data.drive_track[pled];
            pos = 6 + pled;
            if ((gui_data.hd >= 0 || gui_data.cd >= 0 || gui_data.md >= 0) && !gui_data.df[0][0] && !gui_data.df[1][0])
            {
                on_rgb = BLACK;
                on_rgb2 = BLACK;
                off_rgb = BLACK;
            }
            else
            {
                if (currprefs.chipset_mask & CSMASK_MASK)
                {
                    on_rgb = YELLOW_BRIGHT;
                    on_rgb2 = YELLOW_DIM;
                    off_rgb = YELLOW_DARK;
                }
                else
                {
                    on_rgb = GREEN_BRIGHT;
                    on_rgb2 = GREEN_DIM;
                    off_rgb = GREEN_DARK;
                }

                num1 = -1;
                num2 = track / 10;
                num3 = track % 10;
                on = gui_data.drive_motor[pled];
                if (gui_data.drive_writing[pled]) {
                    on_rgb = RED_BRIGHT;
                    on_rgb2 = RED_DIM;
                }
                half = gui_data.drive_side ? 1 : -1;
                if (!gui_data.df[pled][0])
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
        /*} else if (led == LED_POWER) {
            pos = 3;
            on_rgb = ((gui_data.powerled_brightness * 10 / 16) + 0x33) << 16;
            on = 1;
            off_rgb = 0x330000;*/
        } else if (led == LED_CD && gui_data.cd >= 0) {
            pos = 9;
            if (gui_data.cd >= 0) {
                on = gui_data.cd & (LED_CD_AUDIO | LED_CD_ACTIVE);
                on_rgb = (on & LED_CD_AUDIO) ? YELLOW_DIM : YELLOW_BRIGHT;
                off_rgb = YELLOW_DARK;
                if ((gui_data.cd & LED_CD_ACTIVE2) && !(gui_data.cd & LED_CD_AUDIO)) {
                    on_rgb &= 0xfefefe;
                    on_rgb >>= 1;
                }
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
                on_rgb = on == 2 ? RED_BRIGHT : YELLOW_BRIGHT;
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
                on_rgb = 0xcccccc;
                off_rgb = 0x000000;
                am = 2;
            } else {
                int fps = (gui_data.fps + 5) / 10;
                on_rgb = 0x000000;
                off_rgb = gui_data.fps_color ? 0xcccc00 : 0x000000;
                am = 2;
                if (fps > 99) {
                    num1 = -1;
                    num2 = 9;
                    num3 = 9;
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
                on_rgb = ((gui_data.powerled_brightness * 10 / 16) + 0x33) << 9;
                off_rgb = GREEN_DARK;
            }
            else
            {
                on_rgb = ((gui_data.powerled_brightness * 10 / 16) + 0x33) << 16;
                off_rgb = RED_DARK;
            }
            on = 1;
            if (opt_statusbar & STATUSBAR_MINIMAL)
                num1 = num2 = num3 = -1;
        /*} else if (led == LED_CPU) {
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
            }*/
        /*} else if (led == LED_SND) {
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
            am = 3;*/
        } else if (led == LED_MD && gui_data.drive_disabled[3] && gui_data.md >= 1) {
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
        /*} else if (led == LED_NET) {
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
            }*/
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
            putpixel (buf, bpp, x, cb, 0);
            switch (num_multip)
            {
                case 2:
                    putpixel (buf, bpp, x + 1, cb, 0);
                    break;
                case 4:
                    putpixel (buf, bpp, x + 1, cb, 0);
                    putpixel (buf, bpp, x + 2, cb, 0);
                    putpixel (buf, bpp, x + 3, cb, 0);
                    break;
            }
        }
        x = x + (num_multip);
        for (j = 0; j < TD_LED_WIDTH - num_multip; j++)
            putpixel (buf, bpp, x + j, c, 0);
        if (!border)
        {
            putpixel (buf, bpp, x + j, cb, 0);
            switch (num_multip)
            {
                case 2:
                    putpixel (buf, bpp, x + 1 + j, cb, 0);
                    break;
                case 4:
                    putpixel (buf, bpp, x + 1 + j, cb, 0);
                    putpixel (buf, bpp, x + 2 + j, cb, 0);
                    putpixel (buf, bpp, x + 3 + j, cb, 0);
                    break;
            }
        }

        if (y >= TD_PADY && y - TD_PADY < TD_NUM_HEIGHT) {
            if (num3 >= 0) {
                x += (TD_LED_WIDTH - am * TD_NUM_WIDTH * num_multip) / 2;
                if (num1 > 0) {
                    write_tdnumber (buf, bpp, x, y - TD_PADY, num1, pen_rgb, c2);
                    x += TD_NUM_WIDTH * num_multip;
                }
                if (num2 >= 0) {
                    write_tdnumber (buf, bpp, x, y - TD_PADY, num2, pen_rgb, c2);
                    x += TD_NUM_WIDTH * num_multip;
                }
                x = x - (num_multip);
                write_tdnumber (buf, bpp, x, y - TD_PADY, num3, pen_rgb, c2);
                x += TD_NUM_WIDTH * num_multip;
                if (num4 > -1)
                    write_tdnumber (buf, bpp, x, y - TD_PADY, num4, pen_rgb, c2);
            }
        }
    }
}
#else
void draw_status_line_single (uae_u8 *buf, int bpp, int y, int totalwidth, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *alpha)
{
	int x_start, j, led, border;
	uae_u32 c1, c2, cb;

	c1 = ledcolor (0x00ffffff, rc, gc, bc, alpha);
	c2 = ledcolor (0x00000000, rc, gc, bc, alpha);
	cb = ledcolor (TD_BORDER, rc, gc, bc, alpha);

	if (td_pos & TD_RIGHT)
		x_start = totalwidth - TD_PADX - VISIBLE_LEDS * TD_WIDTH;
	else
		x_start = TD_PADX;

	for (led = 0; led < LED_MAX; led++) {
		int side, pos, num1 = -1, num2 = -1, num3 = -1, num4 = -1;
		int x, c, on = 0, am = 2;
		xcolnr on_rgb = 0, on_rgb2 = 0, off_rgb = 0, pen_rgb = 0;
		int half = 0;

		if (!(currprefs.leds_on_screen_mask[picasso_on ? 1 : 0] & (1 << led)))
			continue;

		pen_rgb = c1;
		if (led >= LED_DF0 && led <= LED_DF3) {
			int pled = led - LED_DF0;
			int track = gui_data.drive_track[pled];
			pos = 7 + pled;
			on_rgb = 0x00cc00;
			on_rgb2 = 0x006600;
			off_rgb = 0x003300;
			if (!gui_data.drive_disabled[pled]) {
				num1 = -1;
				num2 = track / 10;
				num3 = track % 10;
				on = gui_data.drive_motor[pled];
				if (gui_data.drive_writing[pled]) {
					on_rgb = 0xcc0000;
					on_rgb2 = 0x880000;
				}
				half = gui_data.drive_side ? 1 : -1;
				if (gui_data.df[pled][0] == 0)
					pen_rgb = ledcolor (0x00aaaaaa, rc, gc, bc, alpha);
			}
			side = gui_data.drive_side;
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
			    off_rgb = 0x000033;
			    if ((gui_data.cd & LED_CD_ACTIVE2) && !(gui_data.cd & LED_CD_AUDIO)) {
			        on_rgb &= 0xfefefe;
			        on_rgb >>= 1;
                }
                num1 = -1;
                num2 = 10;
                num3 = 12;
            }
		} else if (led == LED_HD) {
			pos = 5;
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
                off_rgb = 0x000000;
                am = 2;
            } else {
                int fps = (gui_data.fps + 5) / 10;
                on_rgb = 0x000000;
                off_rgb = gui_data.fps_color ? 0xcccc00 : 0x000000;
                am = 3;
                if (fps > 999) {
                    fps += 50;
                    fps /= 10;
                    if (fps > 999)
                        fps = 999;
                    num1 = fps / 100;
                    num2 = 18;
                    num3 = (fps - num1 * 100) / 10;
                } else {
                    num1 = fps / 100;
                    num2 = (fps - num1 * 100) / 10;
                    num3 = fps % 10;
                    if (num1 == 0)
                        am = 2;
                }
            }
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
		} else if (led == LED_MD && gui_data.drive_disabled[3]) {
			// DF3 reused as internal non-volatile ram led (cd32/cdtv)
			pos = 7 + 3;
			if (gui_data.md >= 0) {
				on = gui_data.md;
				on_rgb = on == 2 ? 0xcc0000 : 0x00cc00;
				off_rgb = 0x003300;
			}
			num1 = -1;
			num2 = -1;
			num3 = -1;
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
			border = 1;
		}

		x = x_start + pos * TD_WIDTH;
		if (!border)
			putpixel (buf, bpp, x - 1, cb, 0);
		for (j = 0; j < TD_LED_WIDTH; j++)
			putpixel (buf, bpp, x + j, c, 0);
		if (!border)
			putpixel (buf, bpp, x + j, cb, 0);

		if (y >= TD_PADY && y - TD_PADY < TD_NUM_HEIGHT) {
			if (num3 >= 0) {
				x += (TD_LED_WIDTH - am * TD_NUM_WIDTH) / 2;
				if (num1 > 0) {
					write_tdnumber (buf, bpp, x, y - TD_PADY, num1, pen_rgb, c2);
					x += TD_NUM_WIDTH;
				}
				if (num2 >= 0) {
					write_tdnumber (buf, bpp, x, y - TD_PADY, num2, pen_rgb, c2);
					x += TD_NUM_WIDTH;
				}
				write_tdnumber (buf, bpp, x, y - TD_PADY, num3, pen_rgb, c2);
				x += TD_NUM_WIDTH;
				if (num4 > 0)
					write_tdnumber (buf, bpp, x, y - TD_PADY, num4, pen_rgb, c2);
			}
		}
	}
}

#endif // LIBRETRO
