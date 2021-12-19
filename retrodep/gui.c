 /*
  * UAE - The Un*x Amiga Emulator
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

bool gui_ask_disk(int drv, TCHAR *name)
{
    return false;
}

int gui_init (void)
{
    return 0;
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

void gui_led (int led, int on, int brightness)
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

    if (status == 0 && old < 0) {
        *p = 0;
        resetcounter[led] = 0;
        return;
    }

    if (status == 0) {
        resetcounter[led]--;
        if (resetcounter[led] > 0)
            return;
    }
    *p = status;

    if (led == LED_CD && status == LED_CD_AUDIO)
        resetcounter[led] = 15;
    else
        resetcounter[led] = 5;
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

#if 0
static const int transla[] = {
	NUMSG_NEEDEXT2, IDS_NUMSG_NEEDEXT2,
	NUMSG_NOROMKEY,IDS_NUMSG_NOROMKEY,
	NUMSG_NOROM,IDS_NUMSG_NOROM,
	NUMSG_KSROMCRCERROR,IDS_NUMSG_KSROMCRCERROR,
	NUMSG_KSROMREADERROR,IDS_NUMSG_KSROMREADERROR,
	NUMSG_NOEXTROM,IDS_NUMSG_NOEXTROM,
	NUMSG_MODRIP_NOTFOUND,IDS_NUMSG_MODRIP_NOTFOUND,
	NUMSG_MODRIP_FINISHED,IDS_NUMSG_MODRIP_FINISHED,
	NUMSG_MODRIP_SAVE,IDS_NUMSG_MODRIP_SAVE,
	NUMSG_KS68EC020,IDS_NUMSG_KS68EC020,
	NUMSG_KS68020,IDS_NUMSG_KS68020,
	NUMSG_KS68030,IDS_NUMSG_KS68030,
	NUMSG_ROMNEED,IDS_NUMSG_ROMNEED,
	NUMSG_EXPROMNEED,IDS_NUMSG_EXPROMNEED,
	NUMSG_NOZLIB,IDS_NUMSG_NOZLIB,
	NUMSG_STATEHD,IDS_NUMSG_STATEHD,
	NUMSG_OLDCAPS, IDS_NUMSG_OLDCAPS,
	NUMSG_NOCAPS, IDS_NUMSG_NOCAPS,
	NUMSG_KICKREP, IDS_NUMSG_KICKREP,
	NUMSG_KICKREPNO, IDS_NUMSG_KICKREPNO,
	NUMSG_KS68030PLUS, IDS_NUMSG_KS68030PLUS,
	NUMSG_NO_PPC, IDS_NUMSG_NO_PPC,
	NUMSG_UAEBOOTROM_PPC, IDS_NUMSG_UAEBOOTROM_PCC,
	NUMSG_NOMEMORY, IDS_NUMSG_NOMEMORY,
	-1
};

static int gettranslation (int msg)
{
	int i;

	i = 0;
	while (transla[i] >= 0) {
		if (transla[i] == msg)
			return transla[i + 1];
		i += 2;
	}
	return -1;
}
#endif

void notify_user (int msg)
{
#if 0
	TCHAR tmp[MAX_DPATH];
	int c = 0;

	c = gettranslation (msg);

	if (c < 0)
		return;

	WIN32GUI_LoadUIString (c, tmp, MAX_DPATH);
	gui_message (tmp);
#endif
}

void notify_user_parms (int msg, const TCHAR *parms, ...)
{
#if 0
	TCHAR msgtxt[MAX_DPATH];
	TCHAR tmp[MAX_DPATH];
	int c = 0;
	va_list parms2;

	c = gettranslation (msg);
	if (c < 0)
		return;
	WIN32GUI_LoadUIString (c, tmp, MAX_DPATH);
	va_start (parms2, parms);
	_vsntprintf (msgtxt, sizeof msgtxt / sizeof (TCHAR), tmp, parms2);
	gui_message (msgtxt);
	va_end (parms2);
#endif
}

int translate_message (int msg,	TCHAR *out)
{
#if 0
	msg = gettranslation (msg);
	out[0] = 0;
	if (msg < 0)
		return 0;
	WIN32GUI_LoadUIString (msg, out, MAX_DPATH);
	return 1;
#endif
}

