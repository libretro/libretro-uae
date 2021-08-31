#include "libretro.h"
#include "libretro-core.h"
#include "libretro-mapper.h"
#include "libretro-graph.h"
#include "libretro-vkbd.h"
#include "libretro-dc.h"

#include "uae_types.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "inputdevice.h"
#include "keyboard.h"
#include "gui.h"
#include "xwin.h"
#include "disk.h"
#include "hrtimer.h"

/* Mouse speed flags */
#define MOUSE_SPEED_SLOWER 1
#define MOUSE_SPEED_FASTER 2
/* Mouse speed multipliers */
#define MOUSE_SPEED_SLOW 5
#define MOUSE_SPEED_FAST 2
/* Mouse D-Pad acceleration */
#define MOUSE_DPAD_ACCEL

/* Press durations */
#define SHORT_PRESS 400
#define LONG_PRESS 800

/* Core flags */
int mapper_keys[RETRO_MAPPER_LAST] = {0};
long mapper_keys_pressed_time = 0;

bool retro_capslock = false;
bool retro_mousemode = false;
bool mousemode_locked = false;

int jflag[4][16] = {0};
int kjflag[2][16] = {0};
int mflag[2][16] = {0};
int aflag[2][16] = {0};
static int mapper_flag[RETRO_DEVICES][16] = {0};
static unsigned int mouse_speed[2] = {0};

extern bool request_update_av_info;
extern void retro_reset_soft();
extern bool retro_statusbar;

unsigned retro_key_state[RETROK_LAST] = {0};
unsigned retro_key_state_internal[RETROK_LAST] = {0};
static unsigned retro_key_event_state[RETROK_LAST] = {0};
int16_t joypad_bits[RETRO_DEVICES];
extern bool libretro_supports_bitmasks;
extern bool libretro_ff_enabled;
extern void retro_fastforwarding(bool);
extern dc_storage *dc;
extern unsigned char statusbar_text[RETRO_PATH_MAX];

/* Core options */
extern unsigned int video_config_aspect;
extern bool opt_aspect_ratio_locked;
extern unsigned int zoom_mode_id;
extern unsigned int opt_zoom_mode_id;
extern int opt_statusbar;
extern int opt_statusbar_position;
extern unsigned int opt_analogmouse;
extern unsigned int opt_analogmouse_deadzone;
extern float opt_analogmouse_speed;
extern unsigned int opt_dpadmouse_speed;
extern unsigned int opt_physicalmouse;
extern bool opt_keyboard_pass_through;
extern char opt_joyport_order[5];

bool retro_turbo_fire = false;
bool turbo_fire_locked = false;
unsigned int turbo_fire_button = 0;
unsigned int turbo_pulse = 6;
unsigned int turbo_state[RETRO_DEVICES] = {0};
unsigned int turbo_toggle[RETRO_DEVICES] = {0};

void emu_function(int function)
{
   switch (function)
   {
      case EMU_VKBD:
         toggle_vkbd();
         break;
      case EMU_STATUSBAR:
         retro_statusbar = !retro_statusbar;
         break;
      case EMU_JOYMOUSE:
         retro_mousemode = !retro_mousemode;
         /* Reset flags to prevent sticky keys */
         memset(jflag, 0, 2*16*sizeof jflag[0][0]);
         /* Lock mousemode */
         mousemode_locked = true;
         break;
      case EMU_RESET:
         retro_reset_soft();
         /* Statusbar notification */
         snprintf(statusbar_text, sizeof(statusbar_text), "%c Reset",
               (' ' | 0x80));
         imagename_timer = 50;
         break;
      case EMU_ASPECT_RATIO:
         if (video_config_aspect == 0)
            video_config_aspect = (video_config & PUAE_VIDEO_NTSC) ? PUAE_VIDEO_PAL : PUAE_VIDEO_NTSC;
         else if (video_config_aspect == PUAE_VIDEO_PAL)
            video_config_aspect = PUAE_VIDEO_NTSC;
         else if (video_config_aspect == PUAE_VIDEO_NTSC)
            video_config_aspect = PUAE_VIDEO_PAL;
         request_update_av_info = true;
         /* Lock aspect ratio */
         opt_aspect_ratio_locked = true;
         /* Statusbar notification */
         snprintf(statusbar_text, sizeof(statusbar_text), "%c Pixel Aspect %-40s",
               (' ' | 0x80), (video_config_aspect == PUAE_VIDEO_PAL) ? "PAL" : "NTSC");
         imagename_timer = 50;
         break;
      case EMU_ZOOM_MODE:
         if (zoom_mode_id == 0 && opt_zoom_mode_id == 0)
            break;
         if (zoom_mode_id > 0)
            zoom_mode_id = 0;
         else if (zoom_mode_id == 0)
            zoom_mode_id = opt_zoom_mode_id;
         request_update_av_info = true;
         /* Statusbar notification */
         snprintf(statusbar_text, sizeof(statusbar_text), "%c Zoom Mode %-43s",
               (' ' | 0x80), (zoom_mode_id) ? "ON" : "OFF");
         imagename_timer = 50;
         break;
      case EMU_TURBO_FIRE:
         retro_turbo_fire = !retro_turbo_fire;
         /* Lock turbo fire */
         turbo_fire_locked = true;
         /* Statusbar notification */
         snprintf(statusbar_text, sizeof(statusbar_text), "%c Turbo Fire %-42s",
               (' ' | 0x80), (retro_turbo_fire) ? "ON" : "OFF");
         imagename_timer = 50;
         break;
      case EMU_SAVE_DISK:
         dc_save_disk_toggle(dc, false, true);
         break;
   }
}

static void process_keyrah()
{
   /*** Port 2 ***/
   /* Up / Down */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2))
   {
      retro_joystick(0, 1, -1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_UP] = 1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8))
   {
      retro_joystick(0, 1, 1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_UP])
   {
      retro_joystick(0, 1, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_UP] = 0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN])
   {
      retro_joystick(0, 1, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;
   }

   /* Left / Right */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6))
   {
      retro_joystick(0, 0, -1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4))
   {
      retro_joystick(0, 0, 1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT])
   {
      retro_joystick(0, 0, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT])
   {
      retro_joystick(0, 0, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
   }

   /* Fire */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP5)
   && !kjflag[0][RETRO_DEVICE_ID_JOYPAD_B])
   {
      retro_joystick_button(0, 0, 1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_B] = 1;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP5)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_B])
   {
      retro_joystick_button(0, 0, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_B] = 0;
   }


   /*** Port 1 ***/
   /* Up / Down */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3))
   {
      retro_joystick(1, 1, -1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_UP] = 1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9))
   {
      retro_joystick(1, 1, 1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_UP])
   {
      retro_joystick(1, 1, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_UP] = 0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_DOWN])
   {
      retro_joystick(1, 1, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;
   }

   /* Left / Right */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1))
   {
      retro_joystick(1, 0, -1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7))
   {
      retro_joystick(1, 0, 1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_LEFT])
   {
      retro_joystick(1, 0, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_RIGHT])
   {
      retro_joystick(1, 0, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
   }

   /* Fire */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP0)
   && !kjflag[1][RETRO_DEVICE_ID_JOYPAD_B])
   {
      retro_joystick_button(1, 0, 1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_B] = 1;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP0)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_B])
   {
      retro_joystick_button(1, 0, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_B] = 0;
   }
}

static int retro_button_to_uae_button(int retro_port, int i)
{
   int uae_button = -1;

   /* CD32 Pad */
   if (retro_devices[retro_port] == RETRO_DEVICE_PUAE_CD32PAD)
   {
      /* Face button rotate */
      if (opt_cd32pad_options == 1 || opt_cd32pad_options == 3)
      {
         switch (i)
         {
            case RETRO_DEVICE_ID_JOYPAD_B:
               uae_button = 1;
               break;
            case RETRO_DEVICE_ID_JOYPAD_A:
               uae_button = 3;
               break;
            case RETRO_DEVICE_ID_JOYPAD_Y:
               uae_button = 0;
               break;
            case RETRO_DEVICE_ID_JOYPAD_X:
               uae_button = 2;
               break;
            case RETRO_DEVICE_ID_JOYPAD_L:
               uae_button = 4;
               break;
            case RETRO_DEVICE_ID_JOYPAD_R:
               uae_button = 5;
               break;
            case RETRO_DEVICE_ID_JOYPAD_START:
               uae_button = 6;
               break;
         }
      }
      /* Face button normal */
      else
      {
         switch (i)
         {
            case RETRO_DEVICE_ID_JOYPAD_B:
               uae_button = 0;
               break;
            case RETRO_DEVICE_ID_JOYPAD_A:
               uae_button = 1;
               break;
            case RETRO_DEVICE_ID_JOYPAD_Y:
               uae_button = 2;
               break;
            case RETRO_DEVICE_ID_JOYPAD_X:
               uae_button = 3;
               break;
            case RETRO_DEVICE_ID_JOYPAD_L:
               uae_button = 4;
               break;
            case RETRO_DEVICE_ID_JOYPAD_R:
               uae_button = 5;
               break;
            case RETRO_DEVICE_ID_JOYPAD_START:
               uae_button = 6;
               break;
         }
      }
      /* Face button jump */
      if (opt_cd32pad_options == 2 && i == RETRO_DEVICE_ID_JOYPAD_A)
         uae_button = -2;
      else if (opt_cd32pad_options == 3 && i == RETRO_DEVICE_ID_JOYPAD_B)
         uae_button = -2;
   }
   /* RetroPad + Joystick */
   else if (retro_devices[retro_port] == RETRO_DEVICE_JOYPAD || retro_devices[retro_port] == RETRO_DEVICE_PUAE_JOYSTICK)
   {
      /* Face button rotate */
      if (opt_retropad_options == 1 || opt_retropad_options == 3)
      {
         switch (i)
         {
            case RETRO_DEVICE_ID_JOYPAD_B:
               uae_button = 1;
               break;
            case RETRO_DEVICE_ID_JOYPAD_A:
               uae_button = 3;
               break;
            case RETRO_DEVICE_ID_JOYPAD_Y:
               uae_button = 0;
               break;
            case RETRO_DEVICE_ID_JOYPAD_X:
               uae_button = 2;
               break;
         }
      }
      /* Face button normal */
      else
      {
         switch (i)
         {
            case RETRO_DEVICE_ID_JOYPAD_B:
               uae_button = 0;
               break;
            case RETRO_DEVICE_ID_JOYPAD_A:
               uae_button = 1;
               break;
            case RETRO_DEVICE_ID_JOYPAD_Y:
               uae_button = 2;
               break;
            case RETRO_DEVICE_ID_JOYPAD_X:
               uae_button = 3;
               break;
         }
      }
      /* Face button jump */
      if (opt_retropad_options == 2 && i == RETRO_DEVICE_ID_JOYPAD_A)
         uae_button = -2;
      else if (opt_retropad_options == 3 && i == RETRO_DEVICE_ID_JOYPAD_B)
         uae_button = -2;

      /* Null extra buttons with joystick */
      if (retro_devices[retro_port] == RETRO_DEVICE_PUAE_JOYSTICK && uae_button > 1)
         uae_button = -1;
   }
   /* Analog joystick */
   else
   {
      switch (i)
      {
         case RETRO_DEVICE_ID_JOYPAD_B:
            uae_button = 0;
            break;
         case RETRO_DEVICE_ID_JOYPAD_A:
            uae_button = 1;
            break;
         case RETRO_DEVICE_ID_JOYPAD_Y:
            uae_button = 2;
            break;
         case RETRO_DEVICE_ID_JOYPAD_X:
            uae_button = 3;
            break;
      }
   }

   return uae_button;
}

static void process_controller(int retro_port, int i)
{
   int retro_port_uae = opt_joyport_order[retro_port] - 49;
   int uae_button = -1;
   if (i > 3 && i < 8) /* Directions, need to fight around presses on the same axis */
   {
      if ((i == RETRO_DEVICE_ID_JOYPAD_UP || i == RETRO_DEVICE_ID_JOYPAD_DOWN)
      && !jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_SELECT])
      {
         if (i == RETRO_DEVICE_ID_JOYPAD_UP && !retro_vkbd && !mapper_keys[i])
         {
            if ((joypad_bits[retro_port] & (1 << i))
            && !(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
            && !jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT])
            {
               retro_joystick(retro_port_uae, 1, -1);
               jflag[retro_port_uae][i] = 1;
            }
         }
         else
         if (i == RETRO_DEVICE_ID_JOYPAD_DOWN && !retro_vkbd && !mapper_keys[i])
         {
            if ((joypad_bits[retro_port] & (1 << i))
            && !(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
            && !jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT])
            {
               retro_joystick(retro_port_uae, 1, 1);
               jflag[retro_port_uae][i] = 1;
            }
         }

         if (!(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
         && jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_UP]
         && !jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT])
         {
            retro_joystick(retro_port_uae, 1, 0);
            jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_UP] = 0;
         }
         else
         if (!(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
         && jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_DOWN]
         && !jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT])
         {
            retro_joystick(retro_port_uae, 1, 0);
            jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;
         }
      }

      if (i == RETRO_DEVICE_ID_JOYPAD_LEFT || i == RETRO_DEVICE_ID_JOYPAD_RIGHT)
      {
         if (i == RETRO_DEVICE_ID_JOYPAD_LEFT && !retro_vkbd && !mapper_keys[i])
         {
            if ((joypad_bits[retro_port] & (1 << i))
            && !(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            {
               retro_joystick(retro_port_uae, 0, -1);
               jflag[retro_port_uae][i] = 1;
            }
         }
         else
         if (i == RETRO_DEVICE_ID_JOYPAD_RIGHT && !retro_vkbd && !mapper_keys[i])
         {
            if ((joypad_bits[retro_port] & (1 << i))
            && !(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)))
            {
               retro_joystick(retro_port_uae, 0, 1);
               jflag[retro_port_uae][i] = 1;
            }
         }

         if (!(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
         && jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_LEFT])
         {
            retro_joystick(retro_port_uae, 0, 0);
            jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;
         }
         else
         if (!(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
         && jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_RIGHT])
         {
            retro_joystick(retro_port_uae, 0, 0);
            jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
         }
      }
   }
   else /* Buttons */
   {
      if (retro_turbo_fire && i == turbo_fire_button)
         return;

      uae_button = retro_button_to_uae_button(retro_port, i);

      /* Alternative jump button, hijack Select flag */
      if (uae_button == -2)
      {
         if ((joypad_bits[retro_port] & (1 << i))
         && !jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT] && !retro_vkbd)
         {
            /* Skip RetroPad face button handling if keymapped */
            if (retro_devices[retro_port] == RETRO_DEVICE_JOYPAD
            && mapper_keys[i]
            && (i == RETRO_DEVICE_ID_JOYPAD_A
             || i == RETRO_DEVICE_ID_JOYPAD_B
             || i == RETRO_DEVICE_ID_JOYPAD_X
             || i == RETRO_DEVICE_ID_JOYPAD_Y
            ))
               jflag[retro_port_uae][i] = 1;
            else
               retro_joystick(retro_port_uae, 1, -1);
            jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT] = 1;
         }
         else
         if (!(joypad_bits[retro_port] & (1 << i))
         &&  !(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
         && jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT])
         {
            /* Skip RetroPad face button handling if keymapped */
            if (retro_devices[retro_port] == RETRO_DEVICE_JOYPAD
            && mapper_keys[i]
            && (i == RETRO_DEVICE_ID_JOYPAD_A
             || i == RETRO_DEVICE_ID_JOYPAD_B
             || i == RETRO_DEVICE_ID_JOYPAD_X
             || i == RETRO_DEVICE_ID_JOYPAD_Y
            ))
               jflag[retro_port_uae][i] = 0;
            else
               retro_joystick(retro_port_uae, 1, 0);
            jflag[retro_port_uae][RETRO_DEVICE_ID_JOYPAD_SELECT] = 0;
         }
      }
      /* Normal button */
      else if (uae_button != -1)
      {
         if ((joypad_bits[retro_port] & (1 << i))
         && !jflag[retro_port_uae][i] && !retro_vkbd)
         {
            /* Skip RetroPad face button handling if keymapped */
            if ((retro_devices[retro_port] == RETRO_DEVICE_JOYPAD
              || retro_devices[retro_port] == RETRO_DEVICE_PUAE_ANALOG)
            && mapper_keys[i]
            && (i == RETRO_DEVICE_ID_JOYPAD_A
             || i == RETRO_DEVICE_ID_JOYPAD_B
             || i == RETRO_DEVICE_ID_JOYPAD_X
             || i == RETRO_DEVICE_ID_JOYPAD_Y
            ))
               ;/* no-op */
            else
            {
               retro_joystick_button(retro_port_uae, uae_button, 1);
               jflag[retro_port_uae][i] = 1;
               if (retro_port < 2)
                  aflag[retro_port][i] = 1;
            }
         }
         else
         if (!(joypad_bits[retro_port] & (1 << i))
         && jflag[retro_port_uae][i] && !mapper_flag[retro_port][i])
         {
            /* Skip RetroPad face button handling if keymapped */
            if ((retro_devices[retro_port] == RETRO_DEVICE_JOYPAD
              || retro_devices[retro_port] == RETRO_DEVICE_PUAE_ANALOG)
            && mapper_keys[i]
            && (i == RETRO_DEVICE_ID_JOYPAD_A
             || i == RETRO_DEVICE_ID_JOYPAD_B
             || i == RETRO_DEVICE_ID_JOYPAD_X
             || i == RETRO_DEVICE_ID_JOYPAD_Y
            ))
               ;/* no-op */
            else
            {
               retro_joystick_button(retro_port_uae, uae_button, 0);
               jflag[retro_port_uae][i] = 0;
               if (retro_port < 2)
                  aflag[retro_port][i] = 0;
            }
         }
      }
   }
}

static void process_turbofire(int retro_port, int i)
{
   int retro_port_uae = opt_joyport_order[retro_port] - 49;
   int fire_button = RETRO_DEVICE_ID_JOYPAD_B;

   /* Face button rotate correction for statusbar flag */
   if ((retro_devices[retro_port] == RETRO_DEVICE_JOYPAD && (opt_retropad_options == 1 || opt_retropad_options == 3))
    || (retro_devices[retro_port] == RETRO_DEVICE_PUAE_CD32PAD && (opt_cd32pad_options == 1 || opt_cd32pad_options == 3)))
      fire_button = RETRO_DEVICE_ID_JOYPAD_Y;

   if (!retro_turbo_fire || i != turbo_fire_button ||
       ((joypad_bits[retro_port] & (1 << fire_button)) && turbo_fire_button != fire_button))
      return;

   if (joypad_bits[retro_port] & (1 << turbo_fire_button))
   {
      if (turbo_state[retro_port])
      {
         if ((turbo_toggle[retro_port]) == (turbo_pulse))
            turbo_toggle[retro_port] = 1;
         else
            turbo_toggle[retro_port]++;

         if (turbo_toggle[retro_port] > (turbo_pulse / 2))
         {
            retro_joystick_button(retro_port_uae, 0, 0);
            jflag[retro_port_uae][fire_button] = mapper_flag[retro_port_uae][fire_button] = 0;
         }
         else
         {
            retro_joystick_button(retro_port_uae, 0, 1);
            jflag[retro_port_uae][fire_button] = mapper_flag[retro_port_uae][fire_button] = 1;
         }
      }
      else
      {
         turbo_state[retro_port] = 1;
         retro_joystick_button(retro_port_uae, 0, 1);
         jflag[retro_port_uae][fire_button] = mapper_flag[retro_port_uae][fire_button] = 1;
      }
   }
   else if (!(joypad_bits[retro_port] & (1 << turbo_fire_button)) && turbo_state[retro_port])
   {
      turbo_state[retro_port] = 0;
      turbo_toggle[retro_port] = 1;
      retro_joystick_button(retro_port_uae, 0, 0);
      jflag[retro_port_uae][fire_button] = mapper_flag[retro_port_uae][fire_button] = 0;
   }
}

static void process_key(unsigned disable_keys)
{
   unsigned i = 0;

   for (i = RETROK_BACKSPACE; i < RETROK_LAST; i++)
   {
      /* CapsLock */
      if (keyboard_translation[i] == AK_CAPSLOCK)
      {
         if (retro_key_event_state[i] && !retro_key_state[i])
         {
            retro_key_down(i);
            retro_key_up(i);
            retro_capslock = !retro_capslock;
            retro_key_state[i] = 1;
         }
         else if (!retro_key_event_state[i] && retro_key_state[i])
            retro_key_state[i] = 0;
      }
      else if (keyboard_translation[i] != -1)
      {
         /* Override cursor keys if used as a RetroPad */
         if (disable_keys == 1 && (i == RETROK_UP || i == RETROK_DOWN || i == RETROK_LEFT || i == RETROK_RIGHT)
          || disable_keys == 2)
            retro_key_event_state[i] = 0;


         /* Skip numpad if Keyrah is active */
         if (opt_keyrah_keypad)
         {
            switch (i)
            {
               case RETROK_KP1:
               case RETROK_KP2:
               case RETROK_KP3:
               case RETROK_KP4:
               case RETROK_KP5:
               case RETROK_KP6:
               case RETROK_KP7:
               case RETROK_KP8:
               case RETROK_KP9:
               case RETROK_KP0:
                  continue;
            }
         }

         if (retro_key_event_state[i] && !retro_key_state[i])
         {
            /* Skip keydown if VKBD is active */
            if (retro_vkbd)
               continue;

            if (retro_capslock)
               retro_key_down(RETROK_LSHIFT);

            retro_key_down(i);
            retro_key_state[i] = 1;
         }
         else if (!retro_key_event_state[i] && retro_key_state[i])
         {
            retro_key_up(i);
            retro_key_state[i] = 0;

            if (retro_capslock)
               retro_key_up(RETROK_LSHIFT);
         }
      }
   }
}

void retro_keyboard_event(bool down, unsigned code,
      uint32_t character, uint16_t mod)
{
   switch (code)
   {
      case RETROK_UNKNOWN:
      case RETROK_PAUSE:
         return;
   }

   retro_key_event_state[code] = down;
}

void update_input(unsigned disable_keys)
{
   /* RETRO  B  Y  SL ST UP DN LT RT A  X  L   R   L2  R2  L3  R3  LR  LL  LD  LU  RR  RL  RD  RU
    * INDEX  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22  23
    */

   unsigned int i = 0, j = 0, mk = 0;
   int LX = 0, LY = 0, RX = 0, RY = 0;
   const int threshold = 20000;
   long now = retro_ticks() / 1000;

   static int jbt[2][RETRO_DEVICE_ID_JOYPAD_LAST] = {0};
   static int kbt[EMU_FUNCTION_COUNT] = {0};

   /* Keyboard hotkeys */
   for (i = 0; i < (RETRO_MAPPER_LAST - RETRO_DEVICE_ID_JOYPAD_LAST); i++)
   {
      /* Skip RetroPad mappings from mapper_keys */
      mk = i + RETRO_DEVICE_ID_JOYPAD_LAST;

      /* Key down */
      if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, mapper_keys[mk]) && !kbt[i] && mapper_keys[mk])
      {
         kbt[i] = 1;
         switch (mk)
         {
            case RETRO_MAPPER_VKBD:
               mapper_keys_pressed_time = now; /* Decide on release */
               break;
            case RETRO_MAPPER_STATUSBAR:
               mapper_keys_pressed_time = now; /* Decide on release */
               break;
            case RETRO_MAPPER_JOYMOUSE:
               emu_function(EMU_JOYMOUSE);
               break;
            case RETRO_MAPPER_RESET:
               emu_function(EMU_RESET);
               break;
            case RETRO_MAPPER_ASPECT_RATIO:
               emu_function(EMU_ASPECT_RATIO);
               break;
            case RETRO_MAPPER_ZOOM_MODE:
               emu_function(EMU_ZOOM_MODE);
               break;
            case RETRO_MAPPER_TURBO_FIRE:
               emu_function(EMU_TURBO_FIRE);
               break;
            case RETRO_MAPPER_SAVE_DISK:
               emu_function(EMU_SAVE_DISK);
               break;
         }
      }
      /* Key up */
      else if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, mapper_keys[mk]) && kbt[i] && mapper_keys[mk])
      {
         kbt[i] = 0;
         switch (mk)
         {
            case RETRO_MAPPER_VKBD:
               if (now - mapper_keys_pressed_time > LONG_PRESS && libretro_ff_enabled)
                  retro_fastforwarding(false);
               else if (now - mapper_keys_pressed_time < SHORT_PRESS)
                  emu_function(EMU_VKBD);
               else
                  emu_function(EMU_STATUSBAR);
               mapper_keys_pressed_time = 0;
               break;
            case RETRO_MAPPER_STATUSBAR:
               if (now - mapper_keys_pressed_time > LONG_PRESS && libretro_ff_enabled)
                  retro_fastforwarding(false);
               else if (now - mapper_keys_pressed_time < SHORT_PRESS)
                  emu_function(EMU_STATUSBAR);
               else
                  emu_function(EMU_VKBD);
               mapper_keys_pressed_time = 0;
               break;
         }
      }
      else if (mapper_keys_pressed_time)
      {
         if (now - mapper_keys_pressed_time > LONG_PRESS && !libretro_ff_enabled)
            retro_fastforwarding(true);
      }
   }

   

   /* The check for kbt[i] here prevents the hotkey from generating key events */
   /* retro_vkbd check is now in process_key() to allow certain keys while retro_vkbd */
   for (i = 0; i < (sizeof(kbt)/sizeof(kbt[0])); i++)
   {
      if (kbt[i])
      {
         disable_keys = 2;
         break;
      }
   }

   process_key(disable_keys);

   if (opt_keyrah_keypad)
      process_keyrah();

   /* RetroPad hotkeys for ports 1 & 2 */
   for (j = 0; j < 2; j++)
   {
      if (retro_devices[j] == RETRO_DEVICE_JOYPAD ||
          retro_devices[j] == RETRO_DEVICE_PUAE_CD32PAD ||
          retro_devices[j] == RETRO_DEVICE_PUAE_ANALOG)
      {
         LX = input_state_cb(j, RETRO_DEVICE_ANALOG, 0, 0);
         LY = input_state_cb(j, RETRO_DEVICE_ANALOG, 0, 1);
         RX = input_state_cb(j, RETRO_DEVICE_ANALOG, 1, 0);
         RY = input_state_cb(j, RETRO_DEVICE_ANALOG, 1, 1);

         /* No keymappings for left analog with analog joystick */
         if (retro_devices[j] == RETRO_DEVICE_PUAE_ANALOG)
            LX = LY = 0;

         for (i = 0; i < RETRO_DEVICE_ID_JOYPAD_LAST; i++)
         {
            int just_pressed  = 0;
            int just_released = 0;
            if (i < 16) /* Remappable RetroPad buttons and directions */
            {
               /* Skip the rest of CD32 pad buttons */
               if (retro_devices[j] == RETRO_DEVICE_PUAE_CD32PAD)
               {
                  switch (i)
                  {
                     case RETRO_DEVICE_ID_JOYPAD_UP:
                     case RETRO_DEVICE_ID_JOYPAD_DOWN:
                     case RETRO_DEVICE_ID_JOYPAD_LEFT:
                     case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                     case RETRO_DEVICE_ID_JOYPAD_B:
                     case RETRO_DEVICE_ID_JOYPAD_Y:
                     case RETRO_DEVICE_ID_JOYPAD_A:
                     case RETRO_DEVICE_ID_JOYPAD_X:
                     case RETRO_DEVICE_ID_JOYPAD_L:
                     case RETRO_DEVICE_ID_JOYPAD_R:
                     case RETRO_DEVICE_ID_JOYPAD_START:
                        continue;
                        break;
                  }
               }

               /* Skip VKBD buttons if VKBD is visible and buttons
                * are mapped to keyboard keys, but allow release */
               if (retro_vkbd)
               {
                  switch (i)
                  {
                     case RETRO_DEVICE_ID_JOYPAD_UP:
                     case RETRO_DEVICE_ID_JOYPAD_DOWN:
                     case RETRO_DEVICE_ID_JOYPAD_LEFT:
                     case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                     case RETRO_DEVICE_ID_JOYPAD_B:
                     case RETRO_DEVICE_ID_JOYPAD_Y:
                     case RETRO_DEVICE_ID_JOYPAD_A:
                     case RETRO_DEVICE_ID_JOYPAD_X:
                     case RETRO_DEVICE_ID_JOYPAD_START:
                        if (mapper_keys[i] >= 0 && !jbt[j][i])
                           continue;
                        break;
                  }
               }

               /* No mappings if button = turbo fire in joystick mode */
               if (retro_turbo_fire && i == turbo_fire_button && !retro_mousemode)
                  continue;

               if ((joypad_bits[j] & (1 << i)) && !jbt[j][i])
                  just_pressed = 1;
               else if (!(joypad_bits[j] & (1 << i)) && jbt[j][i])
                  just_released = 1;
            }
            else if (i >= 16) /* Remappable RetroPad analog stick directions */
            {
               switch (i)
               {
                  case RETRO_DEVICE_ID_JOYPAD_LR:
                     if (LX > threshold && !jbt[j][i]) just_pressed = 1;
                     else if (LX < threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_LL:
                     if (LX < -threshold && !jbt[j][i]) just_pressed = 1;
                     else if (LX > -threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_LD:
                     if (LY > threshold && !jbt[j][i]) just_pressed = 1;
                     else if (LY < threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_LU:
                     if (LY < -threshold && !jbt[j][i]) just_pressed = 1;
                     else if (LY > -threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_RR:
                     if (RX > threshold && !jbt[j][i]) just_pressed = 1;
                     else if (RX < threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_RL:
                     if (RX < -threshold && !jbt[j][i]) just_pressed = 1;
                     else if (RX > -threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_RD:
                     if (RY > threshold && !jbt[j][i]) just_pressed = 1;
                     else if (RY < threshold && jbt[j][i]) just_released = 1;
                     break;
                  case RETRO_DEVICE_ID_JOYPAD_RU:
                     if (RY < -threshold && !jbt[j][i]) just_pressed = 1;
                     else if (RY > -threshold && jbt[j][i]) just_released = 1;
                     break;
                  default:
                     break;
               }
            }

            if (just_pressed)
            {
               jbt[j][i] = 1;
               if (!mapper_keys[i]) /* Unmapped, e.g. set to "---" in core options */
                  continue;

               if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_VKBD])
                  emu_function(EMU_VKBD);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_STATUSBAR])
                  emu_function(EMU_STATUSBAR);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_JOYMOUSE])
                  emu_function(EMU_JOYMOUSE);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_RESET])
                  emu_function(EMU_RESET);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_ASPECT_RATIO])
                  emu_function(EMU_ASPECT_RATIO);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_ZOOM_MODE])
                  emu_function(EMU_ZOOM_MODE);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_TURBO_FIRE])
                  emu_function(EMU_TURBO_FIRE);
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_SAVE_DISK])
                  emu_function(EMU_SAVE_DISK);
               else if (mapper_keys[i] == MOUSE_LEFT_BUTTON)
               {
                  retro_mouse_button(j, 0, 1);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_B] = 1;
               }
               else if (mapper_keys[i] == MOUSE_RIGHT_BUTTON)
               {
                  retro_mouse_button(j, 1, 1);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_A] = 1;
               }
               else if (mapper_keys[i] == MOUSE_MIDDLE_BUTTON)
               {
                  retro_mouse_button(j, 2, 1);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_Y] = 1;
               }
               else if (mapper_keys[i] == MOUSE_SLOWER)
                  mouse_speed[j] |= MOUSE_SPEED_SLOWER;
               else if (mapper_keys[i] == MOUSE_FASTER)
                  mouse_speed[j] |= MOUSE_SPEED_FASTER;
               else if (mapper_keys[i] == JOYSTICK_FIRE)
               {
                  retro_joystick_button(j, 0, 1);
                  if (opt_retropad_options == 1 || opt_retropad_options == 3)
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_Y] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_Y] = 1;
                  else
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_B] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_B] = 1;
               }
               else if (mapper_keys[i] == JOYSTICK_2ND_FIRE)
               {
                  retro_joystick_button(j, 1, 1);
                  if (opt_retropad_options == 1 || opt_retropad_options == 3)
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_B] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_B] = 1;
                  else
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_A] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_A] = 1;
               }
               else if (mapper_keys[i] == TOGGLE_VKBD)
                  mapper_keys_pressed_time = now; /* Decide on release */
               else if (mapper_keys[i] == TOGGLE_STATUSBAR)
                  mapper_keys_pressed_time = now; /* Decide on release */
               else if (mapper_keys[i] == SWITCH_JOYMOUSE)
                  emu_function(EMU_JOYMOUSE);
               else
                  retro_key_down(mapper_keys[i]);
            }
            else if (just_released)
            {
               jbt[j][i] = 0;
               if (!mapper_keys[i]) /* Unmapped, e.g. set to "---" in core options */
                  continue;

               if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_VKBD])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_STATUSBAR])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_JOYMOUSE])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_RESET])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_ASPECT_RATIO])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_ZOOM_MODE])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_TURBO_FIRE])
                  ;/* no-op */
               else if (mapper_keys[i] == mapper_keys[RETRO_MAPPER_SAVE_DISK])
                  ;/* no-op */
               else if (mapper_keys[i] == MOUSE_LEFT_BUTTON)
               {
                  retro_mouse_button(j, 0, 0);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_B] = 0;
               }
               else if (mapper_keys[i] == MOUSE_RIGHT_BUTTON)
               {
                  retro_mouse_button(j, 1, 0);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_A] = 0;
               }
               else if (mapper_keys[i] == MOUSE_MIDDLE_BUTTON)
               {
                  retro_mouse_button(j, 2, 0);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_Y] = 0;
               }
               else if (mapper_keys[i] == MOUSE_SLOWER)
                  mouse_speed[j] &= ~MOUSE_SPEED_SLOWER;
               else if (mapper_keys[i] == MOUSE_FASTER)
                  mouse_speed[j] &= ~MOUSE_SPEED_FASTER;
               else if (mapper_keys[i] == JOYSTICK_FIRE)
               {
                  retro_joystick_button(j, 0, 0);
                  if (opt_retropad_options == 1 || opt_retropad_options == 3)
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_Y] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_Y] = 0;
                  else
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_B] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_B] = 0;
               }
               else if (mapper_keys[i] == JOYSTICK_2ND_FIRE)
               {
                  retro_joystick_button(j, 1, 0);
                  if (opt_retropad_options == 1 || opt_retropad_options == 3)
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_B] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_B] = 0;
                  else
                     jflag[j][RETRO_DEVICE_ID_JOYPAD_A] = mapper_flag[j][RETRO_DEVICE_ID_JOYPAD_A] = 0;
               }
               else if (mapper_keys[i] == TOGGLE_VKBD)
               {
                  if (now - mapper_keys_pressed_time > LONG_PRESS && libretro_ff_enabled)
                     retro_fastforwarding(false);
                  else if (now - mapper_keys_pressed_time < SHORT_PRESS)
                     emu_function(EMU_VKBD);
                  else
                     emu_function(EMU_STATUSBAR);
                  mapper_keys_pressed_time = 0;
               }
               else if (mapper_keys[i] == TOGGLE_STATUSBAR)
               {
                  if (now - mapper_keys_pressed_time > LONG_PRESS && libretro_ff_enabled)
                     retro_fastforwarding(false);
                  else if (now - mapper_keys_pressed_time < SHORT_PRESS)
                     emu_function(EMU_STATUSBAR);
                  else
                     emu_function(EMU_VKBD);
                  mapper_keys_pressed_time = 0;
               }
               else if (mapper_keys[i] == SWITCH_JOYMOUSE)
                  ;/* no-op */
               else
                  retro_key_up(mapper_keys[i]);
            }
            else if (mapper_keys_pressed_time)
            {
               if (now - mapper_keys_pressed_time > LONG_PRESS && !libretro_ff_enabled)
                  retro_fastforwarding(true);
            }
         } /* for i */
      } /* if retro_devices[j]==joypad */
   } /* for j */

   /* Virtual keyboard for ports 1 & 2 */
   input_vkbd();
}

int process_keyboard_pass_through()
{
   unsigned process = 0;

   /* Defaults */
   int fire1_button = RETRO_DEVICE_ID_JOYPAD_B;
   int fire2_button = RETRO_DEVICE_ID_JOYPAD_A;
   int jump_button = -1;

   switch (retro_devices[0])
   {
      case RETRO_DEVICE_JOYPAD:
      case RETRO_DEVICE_PUAE_JOYSTICK:
         /* Fire buttons */
         switch (opt_retropad_options)
         {
            case 1:
            case 3:
               fire1_button = RETRO_DEVICE_ID_JOYPAD_Y;
               fire2_button = RETRO_DEVICE_ID_JOYPAD_B;
               break;
         }

         /* Jump button */
         switch (opt_retropad_options)
         {
            case 2:
               jump_button = RETRO_DEVICE_ID_JOYPAD_A;
               break;
            case 3:
               jump_button = RETRO_DEVICE_ID_JOYPAD_B;
               break;
         }

         /* Null only with RetroPad */
         if (retro_devices[0] == RETRO_DEVICE_JOYPAD)
         {
            if (mapper_keys[fire1_button] || (retro_turbo_fire && fire1_button == turbo_fire_button))
               fire1_button = -1;
            if (mapper_keys[fire2_button] || (retro_turbo_fire && fire2_button == turbo_fire_button))
               fire2_button = -1;

            if (mapper_keys[jump_button] || (retro_turbo_fire && jump_button == turbo_fire_button))
               jump_button = -1;
         }
         break;
   }

   /* Prevent RetroPad from generating keyboard key presses when RetroPad is controlled with keyboard */
   switch (retro_devices[0])
   {
      case RETRO_DEVICE_JOYPAD:
         if ((fire1_button > -1 && (joypad_bits[0] & (1 << fire1_button))) ||
             (fire2_button > -1 && (joypad_bits[0] & (1 << fire2_button))) ||
             (jump_button > -1  && (joypad_bits[0] & (1 << jump_button))) ||
             (retro_turbo_fire  && (joypad_bits[0] & (1 << turbo_fire_button))) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_B))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_B]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_Y))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_Y]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_A))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_A]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_X))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_X]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_START))  && mapper_keys[RETRO_DEVICE_ID_JOYPAD_START]))
            process = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 1; /* Skip cursor keys */
         break;

      case RETRO_DEVICE_PUAE_CD32PAD:
         if ((retro_turbo_fire && (joypad_bits[0] & (1 << turbo_fire_button))) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_B)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_A)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_X)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R)) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT]) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_START)))
            process = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 1; /* Skip cursor keys */
         break;

      case RETRO_DEVICE_PUAE_ANALOG:
         if ( (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_B)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_A)) ||
              (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_X)) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_L3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_R3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT]) ||
             ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_START))  && mapper_keys[RETRO_DEVICE_ID_JOYPAD_START]))
            process = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 1; /* Skip cursor keys */
         break;

      case RETRO_DEVICE_PUAE_JOYSTICK:
         if ((fire1_button > -1 && (joypad_bits[0] & (1 << fire1_button))) ||
             (fire2_button > -1 && (joypad_bits[0] & (1 << fire2_button))) ||
             (jump_button > -1  && (joypad_bits[0] & (1 << jump_button))))
            process = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 1; /* Skip cursor keys */
         break;
   }

   switch (retro_devices[1])
   {
      case RETRO_DEVICE_JOYPAD:
      case RETRO_DEVICE_PUAE_JOYSTICK:
         /* Fire buttons */
         fire1_button = RETRO_DEVICE_ID_JOYPAD_B;
         fire2_button = RETRO_DEVICE_ID_JOYPAD_A;
         switch (opt_retropad_options)
         {
            case 1:
            case 3:
               fire1_button = RETRO_DEVICE_ID_JOYPAD_Y;
               fire2_button = RETRO_DEVICE_ID_JOYPAD_B;
               break;
         }

         /* Jump button */
         jump_button = -1;
         switch (opt_retropad_options)
         {
            case 2:
               jump_button = RETRO_DEVICE_ID_JOYPAD_A;
               break;
            case 3:
               jump_button = RETRO_DEVICE_ID_JOYPAD_B;
               break;
         }

         /* Null only with RetroPad */
         if (retro_devices[1] == RETRO_DEVICE_JOYPAD)
         {
            if (mapper_keys[fire1_button] || (retro_turbo_fire && fire1_button == turbo_fire_button))
               fire1_button = -1;
            if (mapper_keys[fire2_button] || (retro_turbo_fire && fire2_button == turbo_fire_button))
               fire2_button = -1;

            if (mapper_keys[jump_button] || (retro_turbo_fire && jump_button == turbo_fire_button))
               jump_button = -1;
         }
         break;
   }

   switch (retro_devices[1])
   {
      case RETRO_DEVICE_JOYPAD:
         if ((fire1_button > -1 && (joypad_bits[1] & (1 << fire1_button))) ||
             (fire2_button > -1 && (joypad_bits[1] & (1 << fire2_button))) ||
             (jump_button > -1  && (joypad_bits[1] & (1 << jump_button))) ||
             (retro_turbo_fire  && (joypad_bits[1] & (1 << turbo_fire_button))) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_B))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_B]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_Y))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_Y]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_A))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_A]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_X))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_X]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_START))  && mapper_keys[RETRO_DEVICE_ID_JOYPAD_START]) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 2; /* Skip all keyboard input */
         break;

      case RETRO_DEVICE_PUAE_CD32PAD:
         if ((retro_turbo_fire && (joypad_bits[1] & (1 << turbo_fire_button))) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_B)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_A)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_X)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R)) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT]) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_START)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 2; /* Skip all keyboard input */
         break;

      case RETRO_DEVICE_PUAE_ANALOG:
         if ( (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_B)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_A)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_X)) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R))      && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R2))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_L3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_R3))     && mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT]) ||
             ((joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_START))  && mapper_keys[RETRO_DEVICE_ID_JOYPAD_START]) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 2; /* Skip all keyboard input */
         break;

      case RETRO_DEVICE_PUAE_JOYSTICK:
         if ((fire1_button > -1 && (joypad_bits[1] & (1 << fire1_button))) ||
             (fire2_button > -1 && (joypad_bits[1] & (1 << fire2_button))) ||
             (jump_button > -1  && (joypad_bits[1] & (1 << jump_button))) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process = 2; /* Skip all keyboard input */
         break;
   }

   return process;
}

void retro_poll_event()
{
   unsigned i, j;
   for (j = 0; j < RETRO_DEVICES; j++)
   {
      if (libretro_supports_bitmasks)
         joypad_bits[j] = input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
      else
      {
         joypad_bits[j] = 0;
         for (i = 0; i < RETRO_DEVICE_ID_JOYPAD_LR; i++)
            joypad_bits[j] |= input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, i) ? (1 << i) : 0;
      }
   }

   /* Keyboard pass-through */
   unsigned disable_keys = 0;
   if (!opt_keyboard_pass_through)
      disable_keys = process_keyboard_pass_through();
   update_input(disable_keys);

   /* retro joypad take control over keyboard joy */
   /* override keydown, but allow keyup, to prevent key sticking during keyboard use, if held down on opening keyboard */
   /* keyup allowing most likely not needed on actual keyboard presses even though they get stuck also */
   static float mouse_multiplier = 1;
   static int dpadmouse_speed[2] = {0};
   static int dpadmouse_pressed[2] = {0};
#ifdef MOUSE_DPAD_ACCEL
   long now = 0;
   now = retro_ticks() / 1000;
#endif

   int uae_mouse_x[2] = {0}, uae_mouse_y[2] = {0};
   unsigned int uae_mouse_l[2] = {0}, uae_mouse_r[2] = {0}, uae_mouse_m[2] = {0};
   static unsigned int mouse_lmb[2] = {0}, mouse_rmb[2] = {0}, mouse_mmb[2] = {0};
   static int16_t mouse_x[2] = {0}, mouse_y[2] = {0};

   int analog_left[2] = {0};
   int analog_right[2] = {0};
   double analog_left_magnitude = 0;
   double analog_right_magnitude = 0;
   int analog_deadzone = 0;
   analog_deadzone = (opt_analogmouse_deadzone * 32768 / 100);

   int retro_port;
   for (retro_port = 0; retro_port <= 3; retro_port++)
   {
      if (retro_mousemode)
         continue;

      switch (retro_devices[retro_port])
      {
         case RETRO_DEVICE_JOYPAD:
            for (i = 0; i < 16; i++) /* All buttons */
            {
               switch (i)
               {
                  case RETRO_DEVICE_ID_JOYPAD_B:
                  case RETRO_DEVICE_ID_JOYPAD_Y:
                  case RETRO_DEVICE_ID_JOYPAD_A:
                  case RETRO_DEVICE_ID_JOYPAD_X:
                  case RETRO_DEVICE_ID_JOYPAD_UP:
                  case RETRO_DEVICE_ID_JOYPAD_DOWN:
                  case RETRO_DEVICE_ID_JOYPAD_LEFT:
                  case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                     process_controller(retro_port, i);
                     break;
               }
               process_turbofire(retro_port, i);
            }
            break;

         case RETRO_DEVICE_PUAE_ANALOG:
            for (i = 0; i < 16; i++) /* All buttons */
            {
               switch (i)
               {
                  case RETRO_DEVICE_ID_JOYPAD_B:
                  case RETRO_DEVICE_ID_JOYPAD_Y:
                  case RETRO_DEVICE_ID_JOYPAD_A:
                  case RETRO_DEVICE_ID_JOYPAD_X:
                     process_controller(retro_port, i);
                     break;
               }
               process_turbofire(retro_port, i);
            }
            break;

         case RETRO_DEVICE_PUAE_CD32PAD:
            for (i = 0; i < 16; i++) /* All buttons */
            {
               switch (i)
               {
                  case RETRO_DEVICE_ID_JOYPAD_B:
                  case RETRO_DEVICE_ID_JOYPAD_Y:
                  case RETRO_DEVICE_ID_JOYPAD_A:
                  case RETRO_DEVICE_ID_JOYPAD_X:
                  case RETRO_DEVICE_ID_JOYPAD_L:
                  case RETRO_DEVICE_ID_JOYPAD_R:
                  case RETRO_DEVICE_ID_JOYPAD_START:
                  case RETRO_DEVICE_ID_JOYPAD_UP:
                  case RETRO_DEVICE_ID_JOYPAD_DOWN:
                  case RETRO_DEVICE_ID_JOYPAD_LEFT:
                  case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                     process_controller(retro_port, i);
                     break;
               }
               process_turbofire(retro_port, i);
            }
            break;

         case RETRO_DEVICE_PUAE_JOYSTICK:
            for (i = 0; i < 9; i++) /* All buttons up to A */
            {
               switch (i)
               {
                  case RETRO_DEVICE_ID_JOYPAD_B:
                  case RETRO_DEVICE_ID_JOYPAD_Y:
                  case RETRO_DEVICE_ID_JOYPAD_A:
                  case RETRO_DEVICE_ID_JOYPAD_X:
                  case RETRO_DEVICE_ID_JOYPAD_UP:
                  case RETRO_DEVICE_ID_JOYPAD_DOWN:
                  case RETRO_DEVICE_ID_JOYPAD_LEFT:
                  case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                     process_controller(retro_port, i);
                     break;
               }
            }
            break;
      }
   }

   /* Mouse control */
   uae_mouse_l[0] = uae_mouse_r[0] = uae_mouse_m[0] = 0;
   uae_mouse_l[1] = uae_mouse_r[1] = uae_mouse_m[0] = 0;
   uae_mouse_x[0] = uae_mouse_y[0] = 0;
   uae_mouse_x[1] = uae_mouse_y[1] = 0;

   /* Joypad buttons only with digital mouse mode and virtual keyboard hidden */
   if (retro_mousemode && !retro_vkbd)
   {
      for (j = 0; j < 2; j++)
      {
         if ((retro_devices[j] == RETRO_DEVICE_PUAE_CD32PAD && (opt_cd32pad_options == 1 || opt_cd32pad_options == 3))
          || (retro_devices[j] == RETRO_DEVICE_JOYPAD && (opt_retropad_options == 1 || opt_retropad_options == 3))
         )
         {
            uae_mouse_l[j] = (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_Y));
            uae_mouse_r[j] = (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_B));
         }
         else
         {
            uae_mouse_l[j] = (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_B));
            uae_mouse_r[j] = (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_A));
         }
      }
   }

   /* Real mouse buttons only when virtual keyboard hidden */
   if (!retro_vkbd)
   {
      if (opt_physicalmouse > 0 && !uae_mouse_l[0] && !uae_mouse_r[0])
      {
         uae_mouse_l[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
         uae_mouse_r[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
         uae_mouse_m[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
      }

      /* Second real mouse buttons only when enabled */
      if (opt_physicalmouse > 1 && !uae_mouse_l[1] && !uae_mouse_r[1])
      {
         uae_mouse_l[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
         uae_mouse_r[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
         uae_mouse_m[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
      }
   }

   /* Joypad movement only with digital mouse mode or analog joystick, and virtual keyboard hidden */
   if (!retro_vkbd && (retro_mousemode || retro_devices[0] == RETRO_DEVICE_PUAE_ANALOG ||
                                          retro_devices[1] == RETRO_DEVICE_PUAE_ANALOG))
   {
      for (j = 0; j < 2; j++)
      {
         /* Digital mouse speed modifiers */
         if (!dpadmouse_pressed[j])
#ifdef MOUSE_DPAD_ACCEL
            dpadmouse_speed[j] = opt_dpadmouse_speed - 3;
#else
            dpadmouse_speed[j] = opt_dpadmouse_speed;
#endif

         if (mouse_speed[j] & MOUSE_SPEED_FASTER)
            dpadmouse_speed[j] = dpadmouse_speed[j] + 3;
         if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
            dpadmouse_speed[j] = dpadmouse_speed[j] - 4;

#ifdef MOUSE_DPAD_ACCEL
         /* Digital mouse acceleration */
         if (dpadmouse_pressed[j] && (now - dpadmouse_pressed[j] > 400))
         {
            dpadmouse_speed[j]++;
            dpadmouse_pressed[j] = now;
         }
#endif

         /* Digital mouse speed limits */
         if (dpadmouse_speed[j] < 2) dpadmouse_speed[j] = 2;
         if (dpadmouse_speed[j] > 20) dpadmouse_speed[j] = 20;

         if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
            uae_mouse_x[j] += dpadmouse_speed[j];
         else if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
            uae_mouse_x[j] -= dpadmouse_speed[j];
         if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
            uae_mouse_y[j] += dpadmouse_speed[j];
         else if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
            uae_mouse_y[j] -= dpadmouse_speed[j];

#ifdef MOUSE_DPAD_ACCEL
         /* Acceleration timestamps */
         if ((uae_mouse_x[j] != 0 || uae_mouse_y[j] != 0) && !dpadmouse_pressed[j])
            dpadmouse_pressed[j] = now;
         else if ((uae_mouse_x[j] == 0 && uae_mouse_y[j] == 0) && dpadmouse_pressed[j])
            dpadmouse_pressed[j] = 0;
#endif
      }
   }

   /* Left analog movement */
   /* Analog joystick is prioritized over mouse and keymappings */
   if (retro_devices[0] == RETRO_DEVICE_PUAE_ANALOG ||
       retro_devices[1] == RETRO_DEVICE_PUAE_ANALOG)
   {
      for (j = 0; j < 2; j++)
      {
         analog_left[0] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
         analog_left[1] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));
         analog_left_magnitude = sqrt((analog_left[0]*analog_left[0]) + (analog_left[1]*analog_left[1]));
         if (analog_left_magnitude <= analog_deadzone)
         {
            analog_left[0] = 0;
            analog_left[1] = 0;
         }

         if (abs(analog_left[0]) > analog_deadzone)
            retro_joystick_analog(j, 0, analog_left[0]);

         if (abs(analog_left[1]) > analog_deadzone)
            retro_joystick_analog(j, 1, analog_left[1]);

         /* Statusbar flags */
         if (analog_left[1] < analog_deadzone && !aflag[j][RETRO_DEVICE_ID_JOYPAD_UP])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_UP] = 1;
         if (analog_left[1] > -1 && aflag[j][RETRO_DEVICE_ID_JOYPAD_UP])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_UP] = 0;

         if (analog_left[1] > analog_deadzone && !aflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
         if (analog_left[1] < 1 && aflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;

         if (analog_left[0] < analog_deadzone && !aflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
         if (analog_left[0] > -1 && aflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;

         if (analog_left[0] > analog_deadzone && !aflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
         if (analog_left[0] < 1 && aflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT])
            aflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
      }
   }
   /* Analog mouse is secondary */
   else if (opt_analogmouse == 1 || opt_analogmouse == 3)
   {
      for (j = 0; j < 2; j++)
      {
         /* No keymappings and mousing at the same time */
         if (!uae_mouse_x[j] && !uae_mouse_y[j] &&
               (!mapper_keys[RETRO_DEVICE_ID_JOYPAD_LR] &&
                !mapper_keys[RETRO_DEVICE_ID_JOYPAD_LL] &&
                !mapper_keys[RETRO_DEVICE_ID_JOYPAD_LD] &&
                !mapper_keys[RETRO_DEVICE_ID_JOYPAD_LU]))
         {
            analog_left[0] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
            analog_left[1] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));
            analog_left_magnitude = sqrt((analog_left[0]*analog_left[0]) + (analog_left[1]*analog_left[1]));
            if (analog_left_magnitude <= analog_deadzone)
            {
               analog_left[0] = 0;
               analog_left[1] = 0;
            }

            /* Analog stick speed modifiers */
            mouse_multiplier = 1;
            if (mouse_speed[j] & MOUSE_SPEED_FASTER)
               mouse_multiplier = mouse_multiplier * MOUSE_SPEED_FAST;
            if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
               mouse_multiplier = mouse_multiplier / MOUSE_SPEED_SLOW;

            if (abs(analog_left[0]) > 0)
            {
               uae_mouse_x[j] = analog_left[0] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
               if (!uae_mouse_x[j] && abs(analog_left[0]) > analog_deadzone)
                  uae_mouse_x[j] = (analog_left[0] > 0) ? 1 : -1;
            }

            if (abs(analog_left[1]) > 0)
            {
               uae_mouse_y[j] = analog_left[1] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
               if (!uae_mouse_y[j] && abs(analog_left[1]) > analog_deadzone)
                  uae_mouse_y[j] = (analog_left[1] > 0) ? 1 : -1;
            }
         }
      }
   }

   /* Right analog movement only for mouse */
   if (opt_analogmouse == 2 || opt_analogmouse == 3)
   {
      for (j = 0; j < 2; j++)
      {
         /* No keymappings and mousing at the same time */
         if (!uae_mouse_x[j] && !uae_mouse_y[j] &&
               (!mapper_keys[RETRO_DEVICE_ID_JOYPAD_RR] &&
                !mapper_keys[RETRO_DEVICE_ID_JOYPAD_RL] &&
                !mapper_keys[RETRO_DEVICE_ID_JOYPAD_RD] &&
                !mapper_keys[RETRO_DEVICE_ID_JOYPAD_RU]))
         {
            analog_right[0] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
            analog_right[1] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));
            analog_right_magnitude = sqrt((analog_right[0]*analog_right[0]) + (analog_right[1]*analog_right[1]));
            if (analog_right_magnitude <= analog_deadzone)
            {
               analog_right[0] = 0;
               analog_right[1] = 0;
            }

            /* Analog stick speed modifiers */
            mouse_multiplier = 1;
            if (mouse_speed[j] & MOUSE_SPEED_FASTER)
               mouse_multiplier = mouse_multiplier * MOUSE_SPEED_FAST;
            if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
               mouse_multiplier = mouse_multiplier / MOUSE_SPEED_SLOW;

            if (abs(analog_right[0]) > 0)
            {
               uae_mouse_x[j] = analog_right[0] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
               if (!uae_mouse_x[j] && abs(analog_right[0]) > analog_deadzone)
                  uae_mouse_x[j] = (analog_right[0] > 0) ? 1 : -1;
            }

            if (abs(analog_right[1]) > 0)
            {
               uae_mouse_y[j] = analog_right[1] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
               if (!uae_mouse_y[j] && abs(analog_right[1]) > analog_deadzone)
                  uae_mouse_y[j] = (analog_right[1] > 0) ? 1 : -1;
            }
         }
      }
   }

   /* Real mouse movement only when virtual keyboard hidden */
   if (!retro_vkbd)
   {
      if (opt_physicalmouse > 0 && !uae_mouse_x[0] && !uae_mouse_y[0])
      {
         mouse_x[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
         mouse_y[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);

         if (mouse_x[0] || mouse_y[0])
         {
            uae_mouse_x[0] = mouse_x[0];
            uae_mouse_y[0] = mouse_y[0];
         }
      }

      /* Second real mouse movement only when enabled */
      if (opt_physicalmouse > 1 && !uae_mouse_x[1] && !uae_mouse_y[1])
      {
         mouse_x[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
         mouse_y[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);

         if (mouse_x[1] || mouse_y[1])
         {
            uae_mouse_x[1] = mouse_x[1];
            uae_mouse_y[1] = mouse_y[1];
         }
      }
   }

   /* Ports 1 & 2 */
   for (j = 0; j < 2; j++)
   {
      /* Mouse buttons to UAE */
      if (!mouse_lmb[j] && uae_mouse_l[j])
      {
         mouse_lmb[j] = 1;
         mflag[j][RETRO_DEVICE_ID_JOYPAD_B] = 1;
         retro_mouse_button(j, 0, 1);
      }
      else if (mouse_lmb[j] && !uae_mouse_l[j])
      {
         mouse_lmb[j] = 0;
         mflag[j][RETRO_DEVICE_ID_JOYPAD_B] = 0;
         retro_mouse_button(j, 0, 0);
      }

      if (!mouse_rmb[j] && uae_mouse_r[j])
      {
         mouse_rmb[j] = 1;
         mflag[j][RETRO_DEVICE_ID_JOYPAD_A] = 1;
         retro_mouse_button(j, 1, 1);
      }
      else if (mouse_rmb[j] && !uae_mouse_r[j])
      {
         mouse_rmb[j] = 0;
         mflag[j][RETRO_DEVICE_ID_JOYPAD_A] = 0;
         retro_mouse_button(j, 1, 0);
      }

      if (!mouse_mmb[j] && uae_mouse_m[j])
      {
         mouse_mmb[j] = 1;
         mflag[j][RETRO_DEVICE_ID_JOYPAD_Y] = 1;
         retro_mouse_button(j, 2, 1);
      }
      else if (mouse_mmb[j] && !uae_mouse_m[j])
      {
         mouse_mmb[j] = 0;
         mflag[j][RETRO_DEVICE_ID_JOYPAD_Y] = 0;
         retro_mouse_button(j, 2, 0);
      }

      /* Mouse movements to UAE */
      if (uae_mouse_y[j] < 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_UP])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_UP] = 1;
      if (uae_mouse_y[j] > -1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_UP] && !vkflag[RETRO_DEVICE_ID_JOYPAD_B])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_UP] = 0;

      if (uae_mouse_y[j] > 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
      if (uae_mouse_y[j] < 1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] && !vkflag[RETRO_DEVICE_ID_JOYPAD_B])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;

      if (uae_mouse_x[j] < 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
      if (uae_mouse_x[j] > -1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] && !vkflag[RETRO_DEVICE_ID_JOYPAD_B])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;

      if (uae_mouse_x[j] > 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
      if (uae_mouse_x[j] < 1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] && !vkflag[RETRO_DEVICE_ID_JOYPAD_B])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;

      if (uae_mouse_x[j] || uae_mouse_y[j])
         retro_mouse(j, uae_mouse_x[j], uae_mouse_y[j]);
   }
}
