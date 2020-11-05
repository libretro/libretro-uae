 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the Tcl/Tk GUI
  *
  * Copyright 1996 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gui.h"

#include "libretro.h"
#include "libretro-glue.h"
extern retro_log_printf_t log_cb;

void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected)
{
}

int gui_open (void)
{
    return 1;
}

void gui_notify_state (int state)
{
}

int gui_update (void)
{
    return 0;
}

void gui_exit (void)
{
}

void gui_fps (int fps, int idle, int color)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
    gui_data.fps_color = color;
}

void gui_led (int led, int on)
{
}

void gui_hd_led (int led)
{
}

void gui_cd_led (int led)
{
}

static void gui_flicker_led2 (int led, int unitnum, int status)
{
	static int resetcounter[LED_MAX];
	uae_s8 old;
	uae_s8 *p;

	if (led == LED_HD)
		p = &gui_data.hd;
	else if (led == LED_CD)
		p = &gui_data.cd;
	else if (led == LED_MD)
		p = &gui_data.md;
	else if (led == LED_NET)
		p = &gui_data.net;
	else
		return;
	old = *p;
	if (status < 0) {
		if (old < 0) {
			gui_led (led, -1);
		} else {
			gui_led (led, 0);
		}
		return;
	}
	if (status == 0 && old < 0) {
		*p = 0;
		resetcounter[led] = 0;
		gui_led (led, 0);
		return;
	}
	if (status == 0) {
		resetcounter[led]--;
		if (resetcounter[led] > 0)
			return;
	}
#ifdef RETROPLATFORM
	if (unitnum >= 0) {
		if (led == LED_HD) {
			rp_hd_activity(unitnum, status ? 1 : 0, status == 2 ? 1 : 0);
		} else if (led == LED_CD) {
			rp_cd_activity(unitnum, status);
		}
	}
#endif
	*p = status;
	resetcounter[led] = 4;
	if (old != *p)
		gui_led (led, *p);
}

void gui_flicker_led (int led, int unitnum, int status)
{
    if (led < 0) {
        if (gui_data.hd >= 0)
            gui_flicker_led2(LED_HD, 0, 0);
        if (gui_data.cd >= 0)
            gui_flicker_led2(LED_CD, 0, 0);
        if (gui_data.net >= 0)
            gui_flicker_led2(LED_NET, 0, 0);
        if (gui_data.md >= 0)
            gui_flicker_led2(LED_MD, 0, 0);
    } else {
        gui_flicker_led2(led, unitnum, status);
    }
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
}

void gui_display(int shortcut)
{
}

void gui_message (const char *fmt, ...)
{   
    char text[512];
    va_list ap;

    if (fmt == NULL)
       return;

    va_start (ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    if (text[strlen(text)-1] == '\n')
       text[strlen(text)-1] = '\0';

    log_cb(RETRO_LOG_INFO, "%s\n", text);
}

void gui_gameport_button_change (int port, int button, int onoff)
{
}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
}
