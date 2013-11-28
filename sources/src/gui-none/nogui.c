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

int gui_init (void)
{
}

void gui_fps (int fps, int idle, int color)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

void gui_flicker_led (int led, int unitnum, int status)
{
}

void gui_led (int led, int on)
{
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
}

int gui_update (void)
{
	return 0;
}

void gui_exit (void)
{
}

void gui_display(int shortcut)
{
}

void gui_message (const char *format,...)
{
       char msg[2048];
       va_list parms;

       va_start (parms,format);
       vsprintf ( msg, format, parms);
       va_end (parms);

       write_log (msg);
}

void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected) {}
void gui_lock (void) {}
void gui_unlock (void) {}

static int guijoybutton[MAX_JPORTS];
static int guijoyaxis[MAX_JPORTS][4];
static bool guijoychange;

void gui_gameport_button_change (int port, int button, int onoff)
{
        //write_log ("%d %d %d\n", port, button, onoff);
#ifdef RETROPLATFORM
        int mask = 0;
        if (button == JOYBUTTON_CD32_PLAY)
                mask = RP_JOYSTICK_BUTTON5;
        if (button == JOYBUTTON_CD32_RWD)
                mask = RP_JOYSTICK_BUTTON6;
        if (button == JOYBUTTON_CD32_FFW)
                mask = RP_JOYSTICK_BUTTON7;
        if (button == JOYBUTTON_CD32_GREEN)
                mask = RP_JOYSTICK_BUTTON4;
        if (button == JOYBUTTON_3 || button == JOYBUTTON_CD32_YELLOW)
                mask = RP_JOYSTICK_BUTTON3;
        if (button == JOYBUTTON_1 || button == JOYBUTTON_CD32_RED)
                mask = RP_JOYSTICK_BUTTON1;
        if (button == JOYBUTTON_2 || button == JOYBUTTON_CD32_BLUE)
                mask = RP_JOYSTICK_BUTTON2;
        rp_update_gameport (port, mask, onoff);
#endif
        if (onoff)
                guijoybutton[port] |= 1 << button;
        else
                guijoybutton[port] &= ~(1 << button);
        guijoychange = true;
}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
        int onoff = state ? 100 : 0;
        if (axis < 0 || axis > 3)
                return;
        if (max < 0) {
                if (guijoyaxis[port][axis] == 0)
                        return;
                if (guijoyaxis[port][axis] > 0)
                        guijoyaxis[port][axis]--;
        } else {
                if (state > max)
                        state = max;
                if (state < 0)
                        state = 0;
                guijoyaxis[port][axis] = max ? state * 127 / max : onoff;
#ifdef RETROPLATFORM
                if (axis == DIR_LEFT_BIT)
                        rp_update_gameport (port, RP_JOYSTICK_LEFT, onoff);
                if (axis == DIR_RIGHT_BIT)
                        rp_update_gameport (port, DIR_RIGHT_BIT, onoff);
                if (axis == DIR_UP_BIT)
                        rp_update_gameport (port, DIR_UP_BIT, onoff);
                if (axis == DIR_DOWN_BIT)
                        rp_update_gameport (port, DIR_DOWN_BIT, onoff);
#endif
        }
        guijoychange = true;
}

