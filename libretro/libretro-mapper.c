#include "libretro.h"
#include "libretro-core.h"
#include "libretro-mapper.h"
#include "libretro-graph.h"
#include "libretro-vkbd.h"

#include "uae_types.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "inputdevice.h"
#include "keyboard.h"
#include "gui.h"
#include "xwin.h"
#include "disk.h"

#ifdef __CELLOS_LV2__
#include "ps3_headers.h"
#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

extern char retro_key_state[RETROK_LAST];
extern char retro_key_state_old[RETROK_LAST];

static retro_input_state_t input_state_cb;
static retro_input_poll_t input_poll_cb;

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

#ifdef POINTER_DEBUG
int pointer_x = 0;
int pointer_y = 0;
#endif
int last_pointer_x = 0;
int last_pointer_y = 0;

int vkey_pos_x = 0;
int vkey_pos_y = 0;
int vkbd_x_min = 0;
int vkbd_x_max = 0;
int vkbd_y_min = 0;
int vkbd_y_max = 0;

/* Mouse speed flags */
#define MOUSE_SPEED_SLOWER 1
#define MOUSE_SPEED_FASTER 2
/* Mouse speed multipliers */
#define MOUSE_SPEED_SLOW 5
#define MOUSE_SPEED_FAST 2

/* Core flags */
int mapper_keys[RETRO_MAPPER_LAST] = {0};
int vkflag[9] = {0};
int retro_capslock = false;
bool retro_mousemode = false;
bool mousemode_locked = false;
static int jflag[4][16] = {0};
static int kjflag[2][16] = {0};
static int mflag[2][16] = {0};
static int aflag[2][16] = {0};
static int mapper_flag[4][16] = {0};
static unsigned int mouse_speed[2] = {0};

extern bool request_update_av_info;
extern bool request_reset_drawing;
extern void retro_reset_soft();
extern bool retro_statusbar;
extern bool retro_vkbd;
extern bool retro_vkbd_page;
extern bool retro_vkbd_position;
extern bool retro_vkbd_transparent;
extern void retro_key_up(int);
extern void retro_key_down(int);
extern void retro_mouse(int, int, int);
extern void retro_mouse_button(int, int, int);
extern void retro_joystick(int, int, int);
extern void retro_joystick_analog(int, int, int);
extern void retro_joystick_button(int, int, int);
extern unsigned int retro_devices[RETRO_DEVICES];

static int16_t joypad_bits[RETRO_DEVICES];
extern bool libretro_supports_bitmasks;

/* Core options */
extern unsigned int video_config;
extern unsigned int video_config_aspect;
extern unsigned int zoom_mode_id;
extern unsigned int opt_zoom_mode_id;
extern int opt_statusbar;
extern int opt_statusbar_position;
extern unsigned int opt_analogmouse;
extern unsigned int opt_analogmouse_deadzone;
extern float opt_analogmouse_speed;
extern unsigned int opt_dpadmouse_speed;
extern bool opt_multimouse;
extern bool opt_keyrah_keypad;
extern bool opt_keyboard_pass_through;
extern int opt_cd32pad_options;
extern int opt_retropad_options;
extern char opt_joyport_order[5];

int imagename_timer = 0;
static char statusbar_text[RETRO_PATH_MAX] = {0};

int turbo_fire_button_disabled = -1;
int turbo_fire_button = -1;
unsigned int turbo_pulse = 6;
unsigned int turbo_state[RETRO_DEVICES] = {0};
unsigned int turbo_toggle[RETRO_DEVICES] = {0};

enum EMU_FUNCTIONS
{
   EMU_VKBD = 0,
   EMU_STATUSBAR,
   EMU_JOYMOUSE,
   EMU_RESET,
   EMU_ASPECT_RATIO,
   EMU_ZOOM_MODE,
   EMU_TURBO_FIRE,
   EMU_FUNCTION_COUNT
};

/* VKBD_MIN_HOLDING_TIME: Hold a direction longer than this and automatic movement sets in */
/* VKBD_MOVE_DELAY: Delay between automatic movement from button to button */
#define VKBD_MIN_HOLDING_TIME 200
#define VKBD_MOVE_DELAY 50
bool let_go_of_direction = true;
long last_move_time = 0;
long last_press_time = 0;

/* VKBD_STICKY_HOLDING_TIME: Button press longer than this triggers sticky key */
#define VKBD_STICKY_HOLDING_TIME 1000
int let_go_of_button = 1;
long last_press_time_button = 0;
int vkey_pressed = -1;
int vkey_sticky = -1;
int vkey_sticky1 = -1;
int vkey_sticky2 = -1;

void emu_function(int function)
{
   switch (function)
   {
      case EMU_VKBD:
         retro_vkbd = !retro_vkbd;
         break;
      case EMU_STATUSBAR:
         retro_statusbar = !retro_statusbar;
         request_reset_drawing = true;
         break;
      case EMU_JOYMOUSE:
         mousemode_locked = true;
         retro_mousemode = !retro_mousemode;
         /* Reset flags to prevent sticky keys */
         memset(jflag, 0, 2*16*sizeof jflag[0][0]);
         break;
      case EMU_RESET:
         retro_reset_soft();
         break;
      case EMU_ASPECT_RATIO:
         if (video_config_aspect == 0)
            video_config_aspect = (video_config & PUAE_VIDEO_NTSC) ? PUAE_VIDEO_PAL : PUAE_VIDEO_NTSC;
         else if (video_config_aspect == PUAE_VIDEO_PAL)
            video_config_aspect = PUAE_VIDEO_NTSC;
         else if (video_config_aspect == PUAE_VIDEO_NTSC)
            video_config_aspect = PUAE_VIDEO_PAL;
         request_update_av_info = true;
         break;
      case EMU_ZOOM_MODE:
         if (zoom_mode_id == 0 && opt_zoom_mode_id == 0)
            break;
         if (zoom_mode_id > 0)
            zoom_mode_id = 0;
         else if (zoom_mode_id == 0)
            zoom_mode_id = opt_zoom_mode_id;
         request_update_av_info = true;
         break;
      case EMU_TURBO_FIRE:
         if (turbo_fire_button_disabled == -1 && turbo_fire_button == -1)
            break;
         else if (turbo_fire_button_disabled != -1 && turbo_fire_button != -1)
            turbo_fire_button_disabled = -1;

         if (turbo_fire_button_disabled != -1)
         {
            turbo_fire_button = turbo_fire_button_disabled;
            turbo_fire_button_disabled = -1;
         }
         else
         {
            turbo_fire_button_disabled = turbo_fire_button;
            turbo_fire_button = -1;
         }
         break;
   }
}

#ifdef WIIU
#include <features_cpu.h>
#endif

/* in milliseconds */
long GetTicks(void)
{
#ifdef _ANDROID_
   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   return (now.tv_sec*1000000 + now.tv_nsec/1000)/1000;
#elif defined(WIIU)
   return (cpu_features_get_time_usec())/1000;
#else
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return (tv.tv_sec*1000000 + tv.tv_usec)/1000;
#endif
} 

static char* joystick_value_human(int val[16], int uae_device)
{
   /*
    * uae_device:
    * 0 = Single button joystick (Keyrah + Parallel)
    * 1 = RetroPad
    * 2 = CD32 Pad
    * 3 = Mouse
    * 4 = Analog joystick
    */

   unsigned str_len = 4;
   char *str = malloc(sizeof(char)*str_len);
   snprintf(str, sizeof(char)*str_len, "%3s", "   ");

   if (val[RETRO_DEVICE_ID_JOYPAD_UP] || val[RETRO_DEVICE_ID_JOYPAD_SELECT]) /* Unused SELECT acts as a jump button */
      str[1] = 30;

   else if (val[RETRO_DEVICE_ID_JOYPAD_DOWN])
      str[1] = 28;

   if (val[RETRO_DEVICE_ID_JOYPAD_LEFT])
      str[0] = 27;

   else if (val[RETRO_DEVICE_ID_JOYPAD_RIGHT])
      str[2] = 29;

   if (val[RETRO_DEVICE_ID_JOYPAD_B])
   {
      switch (uae_device)
      {
         case 1:
            if (opt_retropad_options == 1 || opt_retropad_options == 3)
               str[1] = ('2' | 0x80);
            else
               str[1] = ('1' | 0x80);
            break;
         case 3:
            str[1] = ('L' | 0x80);
            break;
         case 4:
            str[1] = ('1' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_A])
   {
      switch (uae_device)
      {
         case 1:
            if (opt_retropad_options == 1 || opt_retropad_options == 3)
               ; /* no-op */
            else
               str[1] = ('2' | 0x80);
            break;
         case 3:
            str[1] = ('R' | 0x80);
            break;
         case 4:
            str[1] = ('2' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_Y])
   {
      switch (uae_device)
      {
         case 1:
            if (opt_retropad_options == 1 || opt_retropad_options == 3)
               str[1] = ('1' | 0x80);
            break;
         case 3:
            str[1] = ('M' | 0x80);
            break;
         case 4:
            str[1] = ('3' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_X])
   {
      switch (uae_device)
      {
         case 4:
            str[1] = ('4' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_START])
   {
      switch (uae_device)
      {
         case 2:
            str[1] = ('P' | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_L])
   {
      switch (uae_device)
      {
         case 2:
            str[0] = ('R' | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_R])
   {
      switch (uae_device)
      {
         case 2:
            str[2] = ('F' | 0x80);
            break;
      }
   }

   return str;
}

static int joystick_color(int val[16])
{
   unsigned color = 0;

   if (opt_cd32pad_options == 1 || opt_cd32pad_options == 3)
   {
      if (val[RETRO_DEVICE_ID_JOYPAD_Y])
         color |= (pix_bytes == 4) ? RGB888(248,0,0) : RGB565(255,0,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_B])
         color |= (pix_bytes == 4) ? RGB888(0,0,248) : RGB565(0,0,255);

      if (val[RETRO_DEVICE_ID_JOYPAD_X])
         color |= (pix_bytes == 4) ? RGB888(0,248,0) : RGB565(0,255,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_A])
         color |= (pix_bytes == 4) ? RGB888(248,248,0) : RGB565(255,255,0);
   }
   else
   {
      if (val[RETRO_DEVICE_ID_JOYPAD_B])
         color |= (pix_bytes == 4) ? RGB888(248,0,0) : RGB565(255,0,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_A])
         color |= (pix_bytes == 4) ? RGB888(0,0,248) : RGB565(0,0,255);

      if (val[RETRO_DEVICE_ID_JOYPAD_Y])
         color |= (pix_bytes == 4) ? RGB888(0,248,0) : RGB565(0,255,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_X])
         color |= (pix_bytes == 4) ? RGB888(248,248,0) : RGB565(255,255,0);
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_L])
      color |= (pix_bytes == 4) ? RGB888(170,170,170) : RGB565(110,110,110);

   if (val[RETRO_DEVICE_ID_JOYPAD_R])
      color |= (pix_bytes == 4) ? RGB888(170,170,170) : RGB565(110,110,110);

   if (val[RETRO_DEVICE_ID_JOYPAD_START])
      color |= (pix_bytes == 4) ? RGB888(164,164,164) : RGB565(72,72,72);

   if (color == 0)
      color = (pix_bytes == 4) ? 0xffffff : 0xffff;

   return color;
}

static bool flag_empty(int val[16])
{
   for (int x = 0; x < 16; x++)
   {
      if (val[x])
         return false;
   }
   return true;
}

void display_current_image(const char *image, bool inserted)
{
   static char imagename[RETRO_PATH_MAX] = {0};
   static char imagename_prev[RETRO_PATH_MAX] = {0};

   imagename_timer = 150;
   if (strcmp(image, ""))
   {
      snprintf(imagename, sizeof(imagename), "%s%.98s", "  ", path_basename(image));
      snprintf(imagename_prev, sizeof(imagename_prev), "%.100s", imagename);
   }
   else
      snprintf(imagename, sizeof(imagename), "%.100s", imagename_prev);

   int len;
   len = snprintf(&(statusbar_text[0]), sizeof(statusbar_text), "%-100s", imagename);
   statusbar_text[0 + len] = ' ';

   if (inserted)
      statusbar_text[0] = (8 | 0x80);
   else if (!strcmp(image, ""))
      statusbar_text[0] = (9 | 0x80);
}

void print_statusbar(void)
{
   if (opt_statusbar & STATUSBAR_BASIC && !imagename_timer)
      return;

   int BOX_Y                = 0;
   int BOX_WIDTH            = 0;
   int BOX_HEIGHT           = 11;
   int BOX_PADDING          = 2;

   int FONT_WIDTH           = 1;
   if (video_config & PUAE_VIDEO_HIRES)
   {
      if (video_config & PUAE_VIDEO_DOUBLELINE)
         FONT_WIDTH         = 1;
      else
         FONT_WIDTH         = 2;
   }
   else if (video_config & PUAE_VIDEO_SUPERHIRES)
   {
      if (video_config & PUAE_VIDEO_DOUBLELINE)
         FONT_WIDTH         = 2;
      else
         FONT_WIDTH         = 4;
   }
   int FONT_HEIGHT          = 1;
   int FONT_COLOR           = (pix_bytes == 4) ? 0xffffff : 0xffff;;
   int FONT_SLOT            = 34 * FONT_WIDTH;

   int TEXT_X               = 2;
   int TEXT_Y               = 0;
   int TEXT_LENGTH          = (video_config & PUAE_VIDEO_DOUBLELINE) ? 100 : 43;

   /* Statusbar location */
   /* Top */
   if (opt_statusbar_position < 0)
      TEXT_Y = BOX_PADDING;
   /* Bottom */
   else
      TEXT_Y = gfxvidinfo.outheight - opt_statusbar_position - BOX_HEIGHT + BOX_PADDING;
   BOX_Y = TEXT_Y - BOX_PADDING;

   /* Statusbar size */
   BOX_WIDTH = zoomed_width;
   int ZOOMED_WIDTH_OFFSET = retrow - zoomed_width;

   /* Video resolution */
   int TEXT_X_RESOLUTION = TEXT_X + (FONT_SLOT*4) + (FONT_WIDTH*16) - (ZOOMED_WIDTH_OFFSET/2);
   char RESOLUTION[10] = {0};
   snprintf(RESOLUTION, sizeof(RESOLUTION), "%4dx%3d", zoomed_width, zoomed_height);

   /* Model & memory */
   int TEXT_X_MODEL  = TEXT_X + (FONT_SLOT*6) + (FONT_WIDTH*32) - ZOOMED_WIDTH_OFFSET;
   int TEXT_X_MEMORY = TEXT_X + (FONT_SLOT*6) + (FONT_WIDTH*6) - ZOOMED_WIDTH_OFFSET;
   /* Sacrifice memory slot if there is not enough width */
   if (!(video_config & PUAE_VIDEO_DOUBLELINE))
   {
      if (TEXT_X_MEMORY < (TEXT_X_RESOLUTION + FONT_SLOT + (FONT_WIDTH*20)))
         TEXT_X_MEMORY = -1;
   }

   char MODEL[10] = {0};
   char MEMORY[5] = {0};
   float mem_size = 0;
   mem_size = (float)(currprefs.chipmem_size / 0x80000) / 2;
   mem_size += (float)(currprefs.bogomem_size / 0x40000) / 4;
   mem_size += (float)(currprefs.fastmem_size / 0x100000);
   if (TEXT_X_MEMORY > 0)
      snprintf(MEMORY, sizeof(MEMORY), "%2.0fM", floor(mem_size));
   switch (currprefs.cs_compatible)
   {
      case CP_A500:
         snprintf(MODEL, sizeof(MODEL), "%s", " A500");
         break;
      case CP_A500P:
         snprintf(MODEL, sizeof(MODEL), "%s", "A500+");
         break;
      case CP_A600:
         snprintf(MODEL, sizeof(MODEL), "%s", " A600");
         break;
      case CP_A1200:
         snprintf(MODEL, sizeof(MODEL), "%s", "A1200");
         break;
      case CP_A4000:
         snprintf(MODEL, sizeof(MODEL), "%s", "A4000");
         break;
      case CP_CDTV:
         snprintf(MODEL, sizeof(MODEL), "%s", " CDTV");
         break;
      case CP_CD32:
         snprintf(MODEL, sizeof(MODEL), "%s", " CD32");
         break;
   }

   /* Double line positions */
   if (video_config & PUAE_VIDEO_DOUBLELINE)
   {
      TEXT_X_RESOLUTION = TEXT_X + (FONT_SLOT*15) + (FONT_WIDTH*5) - ZOOMED_WIDTH_OFFSET;
      TEXT_X_MODEL      = TEXT_X + (FONT_SLOT*17) + (FONT_WIDTH*15) - ZOOMED_WIDTH_OFFSET;
      TEXT_X_MEMORY     = TEXT_X + (FONT_SLOT*16) + (FONT_WIDTH*25) - ZOOMED_WIDTH_OFFSET;
   }

   /* Joy port indicators */
   char JOYMODE1[5] = {0};
   char JOYMODE2[5] = {0};
   char JOYMODE3[5] = {0};
   char JOYMODE4[5] = {0};

   char JOYPORT1[5] = {0};
   char JOYPORT2[5] = {0};
   char JOYPORT3[5] = {0};
   char JOYPORT4[5] = {0};

   /* Joy port positions */
   int TEXT_X_JOYMODE1 = TEXT_X;
   int TEXT_X_JOYPORT1 = TEXT_X_JOYMODE1 + (FONT_WIDTH*13);

   int TEXT_X_JOYMODE2 = TEXT_X + FONT_SLOT;
   int TEXT_X_JOYPORT2 = TEXT_X_JOYMODE2 + (FONT_WIDTH*13);

   int TEXT_X_JOYMODE3 = TEXT_X + (FONT_SLOT*2);
   int TEXT_X_JOYPORT3 = TEXT_X_JOYMODE3 + (FONT_WIDTH*13);

   int TEXT_X_JOYMODE4 = TEXT_X + (FONT_SLOT*3);
   int TEXT_X_JOYPORT4 = TEXT_X_JOYMODE4 + (FONT_WIDTH*13);

   /* Regular joyflags */
   if (!retro_mousemode)
   {
      switch (retro_devices[0])
      {
         case RETRO_DEVICE_CD32PAD:
            snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "P1");
            break;
         case RETRO_DEVICE_ANALOGJOYSTICK:
            snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "A1");
            break;
         default:
            snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "J1");
            break;
      }

      switch (retro_devices[1])
      {
         case RETRO_DEVICE_CD32PAD:
            snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "P2");
            break;
         case RETRO_DEVICE_ANALOGJOYSTICK:
            snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "A2");
            break;
         default:
            snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "J2");
            break;
      }
   }
   else
   {
      snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "M1");
      snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "M2");
   }

   /* Regular ports */
   switch (retro_devices[0])
   {
      case RETRO_DEVICE_CD32PAD:
         snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(jflag[0], 2));
         break;
      case RETRO_DEVICE_ANALOGJOYSTICK:
         snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(aflag[0], 4));
         break;
      default:
         snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(jflag[0], 1));
         break;
   }

   switch (retro_devices[1])
   {
      case RETRO_DEVICE_CD32PAD:
         snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(jflag[1], 2));
         break;
      case RETRO_DEVICE_ANALOGJOYSTICK:
         snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(aflag[1], 4));
         break;
      default:
         snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(jflag[1], 1));
         break;
   }

   /* Parallel ports, hidden if not connected */
   if (retro_devices[2])
   {
      snprintf(JOYMODE3, sizeof(JOYMODE3), "%2s", "J3");
      snprintf(JOYPORT3, sizeof(JOYPORT3), "%3s", joystick_value_human(jflag[2], 0));
   }
   if (retro_devices[3])
   {
      snprintf(JOYMODE4, sizeof(JOYMODE4), "%2s", "J4");
      snprintf(JOYPORT4, sizeof(JOYPORT4), "%3s", joystick_value_human(jflag[3], 0));
   }

   /* Mouse flags */
   if (!flag_empty(mflag[1]))
   {
      snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "M1");
      snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(mflag[1], 3));
   }
   if (!flag_empty(mflag[0]))
   {
      snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "M2");
      snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(mflag[0], 3));
   }

   /* Keyrah flags */
   if (opt_keyrah_keypad)
   {
      if (!flag_empty(kjflag[0]))
      {
         snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "K1");
         snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(kjflag[0], 0));
      }
      if (!flag_empty(kjflag[1]))
      {
         snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "K2");
         snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(kjflag[1], 0));
      }
   }

   /* Button colorize CD32 Pad */
   int JOY1_COLOR = FONT_COLOR;
   int JOY2_COLOR = FONT_COLOR;
   if (retro_devices[0] == RETRO_DEVICE_CD32PAD)
      JOY1_COLOR = joystick_color(jflag[0]);
   if (retro_devices[1] == RETRO_DEVICE_CD32PAD)
      JOY2_COLOR = joystick_color(jflag[1]);

   /* Statusbar output */
   if (pix_bytes == 4)
   {
      DrawFBoxBmp32((uint32_t *)retro_bmp, 0, BOX_Y, BOX_WIDTH, BOX_HEIGHT, 0, 255);

      if (imagename_timer > 0)
      {
         Draw_text32((uint32_t *)retro_bmp, TEXT_X, TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, TEXT_LENGTH, statusbar_text);
         return;
      }

      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYMODE1,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE1);
      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYPORT1,   TEXT_Y, JOY1_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT1);

      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYMODE2,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE2);
      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYPORT2,   TEXT_Y, JOY2_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT2);

      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYMODE3,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE3);
      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYPORT3,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT3);

      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYMODE4,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE4);
      Draw_text32((uint32_t *)retro_bmp, TEXT_X_JOYPORT4,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT4);

      Draw_text32((uint32_t *)retro_bmp, TEXT_X_RESOLUTION, TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, RESOLUTION);
      Draw_text32((uint32_t *)retro_bmp, TEXT_X_MEMORY,     TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, MEMORY);
      Draw_text32((uint32_t *)retro_bmp, TEXT_X_MODEL,      TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, MODEL);
   }
   else
   {
      DrawFBoxBmp(retro_bmp, 0, BOX_Y, BOX_WIDTH, BOX_HEIGHT, 0, 255);

      if (imagename_timer > 0)
      {
         Draw_text(retro_bmp, TEXT_X, TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, TEXT_LENGTH, statusbar_text);
         return;
      }

      Draw_text(retro_bmp, TEXT_X_JOYMODE1,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE1);
      Draw_text(retro_bmp, TEXT_X_JOYPORT1,   TEXT_Y, JOY1_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT1);

      Draw_text(retro_bmp, TEXT_X_JOYMODE2,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE2);
      Draw_text(retro_bmp, TEXT_X_JOYPORT2,   TEXT_Y, JOY2_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT2);

      Draw_text(retro_bmp, TEXT_X_JOYMODE3,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE3);
      Draw_text(retro_bmp, TEXT_X_JOYPORT3,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT3);

      Draw_text(retro_bmp, TEXT_X_JOYMODE4,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE4);
      Draw_text(retro_bmp, TEXT_X_JOYPORT4,   TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT4);

      Draw_text(retro_bmp, TEXT_X_RESOLUTION, TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, RESOLUTION);
      Draw_text(retro_bmp, TEXT_X_MEMORY,     TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, MEMORY);
      Draw_text(retro_bmp, TEXT_X_MODEL,      TEXT_Y, FONT_COLOR, 0, 255, FONT_WIDTH, FONT_HEIGHT, 10, MODEL);
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
   if (retro_devices[retro_port] == RETRO_DEVICE_CD32PAD)
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
   else if (retro_devices[retro_port] == RETRO_DEVICE_JOYPAD || retro_devices[retro_port] == RETRO_DEVICE_JOYSTICK)
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
      if (retro_devices[retro_port] == RETRO_DEVICE_JOYSTICK && uae_button > 1)
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
         if (i == RETRO_DEVICE_ID_JOYPAD_UP && !retro_vkbd)
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
         if (i == RETRO_DEVICE_ID_JOYPAD_DOWN && !retro_vkbd)
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
         if (i == RETRO_DEVICE_ID_JOYPAD_LEFT && !retro_vkbd)
         {
            if ((joypad_bits[retro_port] & (1 << i))
            && !(joypad_bits[retro_port] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            {
               retro_joystick(retro_port_uae, 0, -1);
               jflag[retro_port_uae][i] = 1;
            }
         }
         else
         if (i == RETRO_DEVICE_ID_JOYPAD_RIGHT && !retro_vkbd)
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
   else if (i != turbo_fire_button) /* Buttons */
   {
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
              || retro_devices[retro_port] == RETRO_DEVICE_ANALOGJOYSTICK)
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
              || retro_devices[retro_port] == RETRO_DEVICE_ANALOGJOYSTICK)
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
   int retro_fire_button = RETRO_DEVICE_ID_JOYPAD_B;

   /* Face button rotate correction for statusbar flag */
   if ((retro_devices[retro_port] == RETRO_DEVICE_JOYPAD && (opt_retropad_options == 1 || opt_retropad_options == 3))
    || (retro_devices[retro_port] == RETRO_DEVICE_CD32PAD && (opt_cd32pad_options == 1 || opt_cd32pad_options == 3)))
      retro_fire_button = RETRO_DEVICE_ID_JOYPAD_Y;

   if (turbo_fire_button != -1 && i == turbo_fire_button)
   {
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
               jflag[retro_port_uae][retro_fire_button] = mapper_flag[retro_port_uae][retro_fire_button] = 0;
            }
            else
            {
               retro_joystick_button(retro_port_uae, 0, 1);
               jflag[retro_port_uae][retro_fire_button] = mapper_flag[retro_port_uae][retro_fire_button] = 1;
            }
         }
         else
         {
            turbo_state[retro_port] = 1;
            retro_joystick_button(retro_port_uae, 0, 1);
            jflag[retro_port_uae][retro_fire_button] = mapper_flag[retro_port_uae][retro_fire_button] = 1;
         }
      }
      else if (!(joypad_bits[retro_port] & (1 << turbo_fire_button)) && turbo_state[retro_port])
      {
         turbo_state[retro_port] = 0;
         turbo_toggle[retro_port] = 1;
         retro_joystick_button(retro_port_uae, 0, 0);
         jflag[retro_port_uae][retro_fire_button] = mapper_flag[retro_port_uae][retro_fire_button] = 0;
      }
   }
}

static void process_key(int disable_physical_cursor_keys)
{
   int i;

   for (i = RETROK_BACKSPACE; i < RETROK_LAST; i++)
   {
      retro_key_state[i] = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, i);

      /* CapsLock */
      if (keyboard_translation[i] == AK_CAPSLOCK)
      {
         if (retro_key_state[i] && !retro_key_state_old[i])
         {
            retro_key_down(keyboard_translation[i]);
            retro_key_up(keyboard_translation[i]);
            retro_capslock = !retro_capslock;
            retro_key_state_old[i] = 1;
         }
         else if (!retro_key_state[i] && retro_key_state_old[i])
            retro_key_state_old[i] = 0;
      }
      else if (keyboard_translation[i] != -1)
      {
         /* Override cursor keys if used as a RetroPad */
         if (disable_physical_cursor_keys && (i == RETROK_DOWN || i == RETROK_UP || i == RETROK_LEFT || i == RETROK_RIGHT))
            continue;

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

         if (retro_key_state[i] && !retro_key_state_old[i])
         {
            /* Skip keydown if VKBD is active */
            if (retro_vkbd)
               continue;

            if (retro_capslock)
               retro_key_down(keyboard_translation[RETROK_LSHIFT]);

            retro_key_down(keyboard_translation[i]);
            retro_key_state_old[i] = 1;
         }
         else if (!retro_key_state[i] && retro_key_state_old[i])
         {
            retro_key_up(keyboard_translation[i]);
            retro_key_state_old[i] = 0;

            if (retro_capslock)
               retro_key_up(keyboard_translation[RETROK_LSHIFT]);
         }
      }
   }
}

void update_input(int disable_physical_cursor_keys)
{
   /* RETRO  B  Y  SL ST UP DN LT RT A  X  L   R   L2  R2  L3  R3  LR  LL  LD  LU  RR  RL  RD  RU
    * INDEX  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22  23
    */

   static long now = 0;
   static long last_vkey_pressed_time = 0;
   static int last_vkey_pressed = -1;
   static int vkey_sticky1_release = 0;
   static int vkey_sticky2_release = 0;

   static int i = 0, j = 0, mk = 0;
   static int LX = 0, LY = 0, RX = 0, RY = 0;
   const int threshold = 20000;

   static int jbt[2][24] = {0};
   static int kbt[EMU_FUNCTION_COUNT] = {0};

   now = GetTicks();

   if (vkey_sticky && last_vkey_pressed != -1 && last_vkey_pressed > 0)
   {
      if (vkey_sticky1 > -1 && vkey_sticky1 != last_vkey_pressed)
      {
         if (vkey_sticky2 > -1 && vkey_sticky2 != last_vkey_pressed)
            retro_key_up(vkey_sticky2);
         vkey_sticky2 = last_vkey_pressed;
      }
      else
         vkey_sticky1 = last_vkey_pressed;
   }

   /* Keyup only after button is up */
   if (last_vkey_pressed != -1 && !vkflag[4])
   {
      if (last_vkey_pressed < -10)
      {
         if (last_vkey_pressed == -15)
         {
            retro_mouse_button(0, 0, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_B] = 0;
         }
         else if (last_vkey_pressed == -16)
         {
            retro_mouse_button(0, 1, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_A] = 0;
         }
         else if (last_vkey_pressed == -17)
         {
            retro_mouse_button(0, 2, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_Y] = 0;
         }
      }
      else
      {
         if (vkey_pressed == -1 && last_vkey_pressed >= 0 && last_vkey_pressed != vkey_sticky1 && last_vkey_pressed != vkey_sticky2)
            retro_key_up(last_vkey_pressed);

         if (retro_capslock)
            retro_key_up(keyboard_translation[RETROK_LSHIFT]);
      }

      last_vkey_pressed = -1;
   }

   if (vkey_sticky1_release)
   {
      vkey_sticky1_release = 0;
      vkey_sticky1 = -1;
      retro_key_up(vkey_sticky1);
   }
   if (vkey_sticky2_release)
   {
      vkey_sticky2_release = 0;
      vkey_sticky2 = -1;
      retro_key_up(vkey_sticky2);
   }

   /* Keyboard hotkeys */
   for (i = 0; i < EMU_FUNCTION_COUNT; i++)
   {
      mk = i + 24;

      /* Key down */
      if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, mapper_keys[mk]) && !kbt[i] && mapper_keys[mk])
      {
         kbt[i] = 1;
         switch (mk)
         {
            case RETRO_MAPPER_VKBD:
               emu_function(EMU_VKBD);
               break;
            case RETRO_MAPPER_STATUSBAR:
               emu_function(EMU_STATUSBAR);
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
         }
      }
      /* Key up */
      else if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, mapper_keys[mk]) && kbt[i] && mapper_keys[mk])
      {
         kbt[i] = 0;
      }
   }

   

   /* The check for kbt[i] here prevents the hotkey from generating key events */
   /* retro_vkbd check is now in process_key() to allow certain keys while retro_vkbd */
   int processkey = 1;
   for (i = 0; i < (sizeof(kbt)/sizeof(kbt[0])); i++)
   {
      if (kbt[i])
      {
         processkey = 0;
         break;
      }
   }

   if (processkey && disable_physical_cursor_keys != 2)
      process_key(disable_physical_cursor_keys);

   if (opt_keyrah_keypad)
      process_keyrah();

   /* RetroPad hotkeys for ports 1 & 2 */
   for (j = 0; j < 2; j++)
   {
      if (retro_devices[j] == RETRO_DEVICE_JOYPAD || retro_devices[j] == RETRO_DEVICE_CD32PAD || retro_devices[j] == RETRO_DEVICE_ANALOGJOYSTICK)
      {
         LX = input_state_cb(j, RETRO_DEVICE_ANALOG, 0, 0);
         LY = input_state_cb(j, RETRO_DEVICE_ANALOG, 0, 1);
         RX = input_state_cb(j, RETRO_DEVICE_ANALOG, 1, 0);
         RY = input_state_cb(j, RETRO_DEVICE_ANALOG, 1, 1);

         /* No keymappings for left analog with analog joystick */
         if (retro_devices[j] == RETRO_DEVICE_ANALOGJOYSTICK)
            LX = LY = 0;

         for (i = 0; i < RETRO_DEVICE_ID_JOYPAD_LAST; i++)
         {
            int just_pressed  = 0;
            int just_released = 0;
            if ((i < 4 || i > 7) && i < 16) /* Remappable RetroPad buttons excluding D-Pad */
            {
               /* Skip the rest of CD32 pad buttons */
               if (retro_devices[j] == RETRO_DEVICE_CD32PAD)
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
                        continue;
                        break;
                  }
               }

               /* Skip the VKBD buttons if VKBD is visible and buttons are mapped to keyboard keys */
               if (retro_vkbd)
               {
                  switch (i)
                  {
                     case RETRO_DEVICE_ID_JOYPAD_B:
                     case RETRO_DEVICE_ID_JOYPAD_Y:
                     case RETRO_DEVICE_ID_JOYPAD_A:
                     case RETRO_DEVICE_ID_JOYPAD_X:
                     case RETRO_DEVICE_ID_JOYPAD_START:
                        if (mapper_keys[i] >= 0)
                           continue;
                        break;
                  }
               }

               /* No mappings if button = turbo fire in joystick mode */
               if (i == turbo_fire_button && !retro_mousemode)
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
                  emu_function(EMU_VKBD);
               else if (mapper_keys[i] == TOGGLE_STATUSBAR)
                  emu_function(EMU_STATUSBAR);
               else if (mapper_keys[i] == SWITCH_JOYMOUSE)
                  emu_function(EMU_JOYMOUSE);
               else
                  retro_key_down(keyboard_translation[mapper_keys[i]]);
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
                  ;/* no-op */
               else if (mapper_keys[i] == TOGGLE_STATUSBAR)
                  ;/* no-op */
               else if (mapper_keys[i] == SWITCH_JOYMOUSE)
                  ;/* no-op */
               else
                  retro_key_up(keyboard_translation[mapper_keys[i]]);
            }
         } /* for i */
      } /* if retro_devices[j]==joypad */
   } /* for j */

   /* Virtual keyboard for ports 1 & 2 */
   if (retro_vkbd)
   {
      /* Mouse acceleration */
      const int mspeed_default = 3;
      static int mspeed;
      if (!vkflag[4])
         mspeed = mspeed_default;

      if (!vkflag[4]) /* Allow directions when key is not pressed */
      {
         if (!vkflag[0] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
                            (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
                            input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_UP)))
            vkflag[0] = 1;
         else
         if (vkflag[0] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) &&
                           !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) &&
                           !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_UP)))
            vkflag[0] = 0;

         if (!vkflag[1] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
                            (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
                            input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_DOWN)))
            vkflag[1] = 1;
         else
         if (vkflag[1] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) &&
                           !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) &&
                           !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_DOWN)))
            vkflag[1] = 0;

         if (!vkflag[2] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
                            (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
                            input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LEFT)))
            vkflag[2] = 1;
         else
         if (vkflag[2] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) &&
                           !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) &&
                           !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LEFT)))
            vkflag[2] = 0;

         if (!vkflag[3] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) ||
                            (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) ||
                            input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RIGHT)))
            vkflag[3] = 1;
         else
         if (vkflag[3] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) &&
                           !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) &&
                           !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RIGHT)))
            vkflag[3] = 0;
      }
      else /* Release all directions when key is pressed */
      {
         vkflag[0] = 0;
         vkflag[1] = 0;
         vkflag[2] = 0;
         vkflag[3] = 0;
      }

      if (vkflag[0] || vkflag[1] || vkflag[2] || vkflag[3])
      {
         if (let_go_of_direction)
            /* just pressing down */
            last_press_time = now;

         if ((now - last_press_time > VKBD_MIN_HOLDING_TIME
           && now - last_move_time > VKBD_MOVE_DELAY)
           || let_go_of_direction)
         {
            last_move_time = now;

            if (vkflag[0])
               vkey_pos_y -= 1;
            else if (vkflag[1])
               vkey_pos_y += 1;

            if (vkflag[2])
               vkey_pos_x -= 1;
            else if (vkflag[3])
               vkey_pos_x += 1;
         }
         let_go_of_direction = false;
      }
      else
         let_go_of_direction = true;

      if (vkey_pos_x < 0)
         vkey_pos_x = VKBDX - 1;
      else if (vkey_pos_x > VKBDX - 1)
         vkey_pos_x = 0;
      if (vkey_pos_y < 0)
         vkey_pos_y = VKBDY - 1;
      else if (vkey_pos_y > VKBDY - 1)
         vkey_pos_y = 0;

      /* Absolute pointer */
      int p_x = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
      int p_y = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);

      if (p_x != 0 && p_y != 0 && (p_x != last_pointer_x || p_y != last_pointer_y))
      {
         int px = (int)((p_x + 0x7fff) * zoomed_width / 0xffff);
         int py = (int)((p_y + 0x7fff) * zoomed_height / 0xffff);
         last_pointer_x = p_x;
         last_pointer_y = p_y;
#ifdef POINTER_DEBUG
         pointer_x = px;
         pointer_y = py;
#endif
         if (px >= vkbd_x_min && px <= vkbd_x_max && py >= vkbd_y_min && py <= vkbd_y_max)
         {
            float vkey_width = (float)(vkbd_x_max - vkbd_x_min) / VKBDX;
            vkey_pos_x = ((px - vkbd_x_min) / vkey_width);

            float vkey_height = (float)(vkbd_y_max - vkbd_y_min) / VKBDY;
            vkey_pos_y = ((py - vkbd_y_min) / vkey_height);

            vkey_pos_x = (vkey_pos_x < 0) ? 0 : vkey_pos_x;
            vkey_pos_x = (vkey_pos_x > VKBDX - 1) ? VKBDX - 1 : vkey_pos_x;
            vkey_pos_y = (vkey_pos_y < 0) ? 0 : vkey_pos_y;
            vkey_pos_y = (vkey_pos_y > VKBDY - 1) ? VKBDY - 1 : vkey_pos_y;

#ifdef POINTER_DEBUG
            printf("px:%d py:%d (%d,%d) vkey:%dx%d\n", p_x, p_y, px, py, vkey_pos_x, vkey_pos_y);
#endif
         }
      }

      /* Press Return, RetroPad Start */
      i = RETRO_DEVICE_ID_JOYPAD_START;
      if (!vkflag[8] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                                (joypad_bits[1] & (1 << i))))
      {
         vkflag[8] = 1;
         retro_key_down(AK_RET);
      }
      else
      if (vkflag[8] && (!(joypad_bits[0] & (1 << i)) &&
                        !(joypad_bits[1] & (1 << i))))
      {
         vkflag[8] = 0;
         retro_key_up(AK_RET);
      }

      /* CapsLock, RetroPad Y */
      i = RETRO_DEVICE_ID_JOYPAD_Y;
      if (!vkflag[7] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                                (joypad_bits[1] & (1 << i))))
      {
         vkflag[7] = 1;
         retro_capslock = !retro_capslock;
      }
      else
      if (vkflag[7] && (!(joypad_bits[0] & (1 << i)) &&
                        !(joypad_bits[1] & (1 << i))))
      {
         vkflag[7] = 0;
      }

      /* Position toggle, RetroPad X */
      i = RETRO_DEVICE_ID_JOYPAD_X;
      if (!vkflag[6] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                                (joypad_bits[1] & (1 << i))))
      {
         vkflag[6] = 1;
         retro_vkbd_position = !retro_vkbd_position;
      }
      else
      if (vkflag[6] && (!(joypad_bits[0] & (1 << i)) &&
                        !(joypad_bits[1] & (1 << i))))
      {
         vkflag[6] = 0;
      }

      /* Transparency toggle, RetroPad A */
      i = RETRO_DEVICE_ID_JOYPAD_A;
      if (!vkflag[5] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                                (joypad_bits[1] & (1 << i))))
      {
         vkflag[5] = 1;
         retro_vkbd_transparent = !retro_vkbd_transparent;
      }
      else
      if (vkflag[5] && (!(joypad_bits[0] & (1 << i)) &&
                        !(joypad_bits[1] & (1 << i))))
      {
         vkflag[5] = 0;
      }

      /* Key press, RetroPad B joyports 1+2 / Keyboard Enter / Pointer */
      i = RETRO_DEVICE_ID_JOYPAD_B;
      if (!vkflag[4] && ((joypad_bits[0] & (1 << i)) ||
                         (joypad_bits[1] & (1 << i)) ||
                         input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN) ||
                         input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)))
      {
         vkey_pressed = check_vkey(vkey_pos_x, vkey_pos_y);
         vkflag[4] = 1;

         if (vkey_pressed == -1)
            last_vkey_pressed = -1;
         else if (vkey_pressed == -2)
         {
            last_vkey_pressed = -1;
            retro_vkbd_page = !retro_vkbd_page;
         }
         else if (vkey_pressed < -10)
         {
            last_vkey_pressed = vkey_pressed;
            last_vkey_pressed_time = now;

            if (vkey_pressed == -11) /* Mouse up */
            {
               retro_mouse(0, 0, -3);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_UP] = 1;
            }
            else if (vkey_pressed == -12) /* Mouse down */
            {
               retro_mouse(0, 0, 3);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
            }
            else if (vkey_pressed == -13) /* Mouse left */
            {
               retro_mouse(0, -3, 0);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
            }
            else if (vkey_pressed == -14) /* Mouse right */
            {
               retro_mouse(0, 3, 0);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
            }
            else if (vkey_pressed == -15) /* LMB */
            {
               retro_mouse_button(0, 0, 1);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_B] = 1;
            }
            else if (vkey_pressed == -16) /* RMB */
            {
               retro_mouse_button(0, 1, 1);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_A] = 1;
            }
            else if (vkey_pressed == -17) /* MMB */
            {
               retro_mouse_button(0, 2, 1);
               mflag[0][RETRO_DEVICE_ID_JOYPAD_Y] = 1;
            }
            else if (vkey_pressed == -18) /* Toggle joystick/mouse */
            {
               emu_function(EMU_JOYMOUSE);
            }
            else if (vkey_pressed == -19) /* Toggle turbo fire */
            {
               emu_function(EMU_TURBO_FIRE);
            }
            else if (vkey_pressed == -20) /* Reset */
            {
               emu_function(EMU_RESET);
            }
            else if (vkey_pressed == -21) /* Toggle statusbar */
            {
               emu_function(EMU_STATUSBAR);
            }
            else if (vkey_pressed == -22) /* Toggle aspect ratio */
            {
               emu_function(EMU_ASPECT_RATIO);
            }
         }
         else
         {
            if (vkey_pressed == AK_CAPSLOCK)
            {
               retro_key_down(vkey_pressed);
               retro_key_up(vkey_pressed);
               retro_capslock = !retro_capslock;
               last_vkey_pressed = -1;
            }
            else
            {
               last_vkey_pressed = vkey_pressed;
               if (retro_capslock)
                  retro_key_down(keyboard_translation[RETROK_LSHIFT]);

               if (!vkey_sticky)
               {
                  if (vkey_pressed == vkey_sticky1)
                     vkey_sticky1_release = 1;
                  if (vkey_pressed == vkey_sticky2)
                     vkey_sticky2_release = 1;
               }
               retro_key_down(vkey_pressed);
            }
         }
      }
      else
      if (vkflag[4] && (!(joypad_bits[0] & (1 << i)) &&
                        !(joypad_bits[1] & (1 << i)) &&
                        !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN) &&
                        !input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)))
      {
         vkey_pressed = -1;
         vkflag[4] = 0;

         if (last_vkey_pressed > -10 && last_vkey_pressed < -15)
         {
            mspeed = mspeed_default;

            if (vkey_pressed == -11) /* Mouse up */
               mflag[0][RETRO_DEVICE_ID_JOYPAD_UP] = 0;
            else if (vkey_pressed == -12) /* Mouse down */
               mflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;
            else if (vkey_pressed == -13) /* Mouse left */
               mflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;
            else if (vkey_pressed == -14) /* Mouse right */
               mflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
         }
      }
      else
      if (vkflag[4] && ((joypad_bits[0] & (1 << i)) ||
                        (joypad_bits[1] & (1 << i)) ||
                        input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN) ||
                        input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)))
      {
         if (now - last_vkey_pressed_time > 100)
         {
            mspeed++;
            last_vkey_pressed_time = now;
         }

         if (vkey_pressed == -11) /* Mouse up */
            retro_mouse(0, 0, -mspeed);
         else if (vkey_pressed == -12) /* Mouse down */
            retro_mouse(0, 0, mspeed);
         else if (vkey_pressed == -13) /* Mouse left */
            retro_mouse(0, -mspeed, 0);
         else if (vkey_pressed == -14) /* Mouse right */
            retro_mouse(0, mspeed, 0);
      }

      if (vkflag[4] && vkey_pressed > 0)
      {
         if (let_go_of_button)
            last_press_time_button = now;
         if (now - last_press_time_button > VKBD_STICKY_HOLDING_TIME)
            vkey_sticky = 1;
         let_go_of_button = 0;
      }
      else
      {
         let_go_of_button = 1;
         vkey_sticky = 0;
      }
   }
#if 0
   printf("vkey:%d sticky:%d sticky1:%d sticky2:%d, now:%d last:%d\n", vkey_pressed, vkey_sticky, vkey_sticky1, vkey_sticky2, now, last_press_time_button);
#endif
}

int process_keyboard_pass_through()
{
   unsigned process_key = 0;

   /* Defaults */
   int fire1_button = RETRO_DEVICE_ID_JOYPAD_B;
   int fire2_button = RETRO_DEVICE_ID_JOYPAD_A;
   int jump_button = -1;

   switch (retro_devices[0])
   {
      case RETRO_DEVICE_JOYPAD:
      case RETRO_DEVICE_JOYSTICK:
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
            if (mapper_keys[fire1_button] || fire1_button == turbo_fire_button)
               fire1_button = -1;
            if (mapper_keys[fire2_button] || fire2_button == turbo_fire_button)
               fire2_button = -1;

            if (mapper_keys[jump_button] || jump_button == turbo_fire_button)
               jump_button = -1;
         }
         break;
   }

   /* Prevent RetroPad from generating keyboard key presses when RetroPad is controlled with keyboard */
   switch (retro_devices[0])
   {
      case RETRO_DEVICE_JOYPAD:
         if ((fire1_button > -1      && (joypad_bits[0] & (1 << fire1_button))) ||
             (fire2_button > -1      && (joypad_bits[0] & (1 << fire2_button))) ||
             (jump_button > -1       && (joypad_bits[0] & (1 << jump_button))) ||
             (turbo_fire_button > -1 && (joypad_bits[0] & (1 << turbo_fire_button))) ||
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
            process_key = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process_key = 1; /* Skip cursor keys */
         break;

      case RETRO_DEVICE_CD32PAD:
         if ((turbo_fire_button > -1 && (joypad_bits[0] & (1 << turbo_fire_button))) ||
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
            process_key = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process_key = 1; /* Skip cursor keys */
         break;

      case RETRO_DEVICE_ANALOGJOYSTICK:
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
            process_key = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process_key = 1; /* Skip cursor keys */
         break;

      case RETRO_DEVICE_JOYSTICK:
         if ((fire1_button > -1 && (joypad_bits[0] & (1 << fire1_button))) ||
             (fire2_button > -1 && (joypad_bits[0] & (1 << fire2_button))) ||
             (jump_button > -1  && (joypad_bits[0] & (1 << jump_button))))
            process_key = 2; /* Skip all keyboard input */
         else
         if ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
             (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process_key = 1; /* Skip cursor keys */
         break;
   }

   switch (retro_devices[1])
   {
      case RETRO_DEVICE_JOYPAD:
      case RETRO_DEVICE_JOYSTICK:
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
            if (mapper_keys[fire1_button] || fire1_button == turbo_fire_button)
               fire1_button = -1;
            if (mapper_keys[fire2_button] || fire2_button == turbo_fire_button)
               fire2_button = -1;

            if (mapper_keys[jump_button] || jump_button == turbo_fire_button)
               jump_button = -1;
         }
         break;
   }

   switch (retro_devices[1])
   {
      case RETRO_DEVICE_JOYPAD:
         if ((fire1_button > -1      && (joypad_bits[1] & (1 << fire1_button))) ||
             (fire2_button > -1      && (joypad_bits[1] & (1 << fire2_button))) ||
             (jump_button > -1       && (joypad_bits[1] & (1 << jump_button))) ||
             (turbo_fire_button > -1 && (joypad_bits[1] & (1 << turbo_fire_button))) ||
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
            process_key = 2; /* Skip all keyboard input */
         break;

      case RETRO_DEVICE_CD32PAD:
         if ((turbo_fire_button > -1 && (joypad_bits[1] & (1 << turbo_fire_button))) ||
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
            process_key = 2; /* Skip all keyboard input */
         break;

      case RETRO_DEVICE_ANALOGJOYSTICK:
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
            process_key = 2; /* Skip all keyboard input */
         break;

      case RETRO_DEVICE_JOYSTICK:
         if ((fire1_button > -1 && (joypad_bits[1] & (1 << fire1_button))) ||
             (fire2_button > -1 && (joypad_bits[1] & (1 << fire2_button))) ||
             (jump_button > -1  && (joypad_bits[1] & (1 << jump_button))) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
              (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            process_key = 2; /* Skip all keyboard input */
         break;
   }

   return process_key;
}

void retro_poll_event()
{
   unsigned i, j;

   input_poll_cb();

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
   unsigned process_key = 0;
   if (!opt_keyboard_pass_through)
      process_key = process_keyboard_pass_through();
   update_input(process_key);

   /* retro joypad take control over keyboard joy */
   /* override keydown, but allow keyup, to prevent key sticking during keyboard use, if held down on opening keyboard */
   /* keyup allowing most likely not needed on actual keyboard presses even though they get stuck also */
   static float mouse_multiplier = 1;
   static int dpadmouse_speed[2] = {0};
   static long dpadmouse_press[2] = {0};
   static int dpadmouse_pressed[2] = {0};
   static long now = 0;
   now = GetTicks();

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

         case RETRO_DEVICE_ANALOGJOYSTICK:
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

         case RETRO_DEVICE_CD32PAD:
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

         case RETRO_DEVICE_JOYSTICK:
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
         if ((retro_devices[j] == RETRO_DEVICE_CD32PAD && (opt_cd32pad_options == 1 || opt_cd32pad_options == 3))
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
      if (!uae_mouse_l[0] && !uae_mouse_r[0])
      {
         uae_mouse_l[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
         uae_mouse_r[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
         uae_mouse_m[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
      }

      /* Second real mouse buttons only when enabled */
      if (opt_multimouse && !uae_mouse_l[1] && !uae_mouse_r[1])
      {
         uae_mouse_l[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
         uae_mouse_r[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
         uae_mouse_m[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
      }
   }

   /* Joypad movement only with digital mouse mode or analog joystick, and virtual keyboard hidden */
   if ((retro_mousemode || retro_devices[0] == RETRO_DEVICE_ANALOGJOYSTICK || retro_devices[1] == RETRO_DEVICE_ANALOGJOYSTICK) && !retro_vkbd)
   {
      for (j = 0; j < 2; j++)
      {
         /* Digital mouse speed modifiers */
         if (dpadmouse_pressed[j] == 0)
            dpadmouse_speed[j] = opt_dpadmouse_speed;

         if (mouse_speed[j] & MOUSE_SPEED_FASTER)
            dpadmouse_speed[j] = dpadmouse_speed[j] + 3;
         if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
            dpadmouse_speed[j] = dpadmouse_speed[j] - 4;

         /* Digital mouse acceleration */
         if (dpadmouse_pressed[j] == 1)
            if (now - dpadmouse_press[j] > 200)
            {
               dpadmouse_speed[j]++;
               dpadmouse_press[j] = now;
            }

         /* Digital mouse speed limits */
         if (dpadmouse_speed[j] < 3) dpadmouse_speed[j] = 2;
         if (dpadmouse_speed[j] > 13) dpadmouse_speed[j] = 14;

         if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
            uae_mouse_x[j] += dpadmouse_speed[j];
         else if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
            uae_mouse_x[j] -= dpadmouse_speed[j];
         if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
            uae_mouse_y[j] += dpadmouse_speed[j];
         else if (joypad_bits[j] & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
            uae_mouse_y[j] -= dpadmouse_speed[j];

         /* Acceleration timestamps */
         if ((uae_mouse_x[j] != 0 || uae_mouse_y[j] != 0) && dpadmouse_pressed[j] == 0)
         {
            dpadmouse_press[j] = now;
            dpadmouse_pressed[j] = 1;
         }
         else if ((uae_mouse_x[j] == 0 && uae_mouse_y[j] == 0) && dpadmouse_pressed[j] == 1)
         {
            dpadmouse_press[j] = 0;
            dpadmouse_pressed[j] = 0;
         }
      }
   }

   /* Left analog movement */
   /* Analog joystick is prioritized over mouse and keymappings */
   if (retro_devices[0] == RETRO_DEVICE_ANALOGJOYSTICK || retro_devices[1] == RETRO_DEVICE_ANALOGJOYSTICK)
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
         if (!uae_mouse_x[j] && !uae_mouse_y[j] && (!mapper_keys[16] && !mapper_keys[17] && !mapper_keys[18] && !mapper_keys[19]))
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
         if (!uae_mouse_x[j] && !uae_mouse_y[j] && (!mapper_keys[20] && !mapper_keys[21] && !mapper_keys[22] && !mapper_keys[23]))
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
      if (!uae_mouse_x[0] && !uae_mouse_y[0])
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
      if (opt_multimouse && !uae_mouse_x[1] && !uae_mouse_y[1])
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
      if (uae_mouse_y[j] > -1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_UP] && !vkflag[4])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_UP] = 0;

      if (uae_mouse_y[j] > 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
      if (uae_mouse_y[j] < 1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] && !vkflag[4])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;

      if (uae_mouse_x[j] < 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
      if (uae_mouse_x[j] > -1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] && !vkflag[4])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;

      if (uae_mouse_x[j] > 0 && !mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
      if (uae_mouse_x[j] < 1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] && !vkflag[4])
         mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;

      if (uae_mouse_x[j] || uae_mouse_y[j])
         retro_mouse(j, uae_mouse_x[j], uae_mouse_y[j]);
   }
}
