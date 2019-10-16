#include "libretro.h"
#include "libretro-glue.h"
#include "keyboard.h"
#include "libretro-keymap.h"
#include "graph.h"
#include "vkbd.h"
#include "libretro-mapper.h"

#include "uae_types.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "inputdevice.h"

#include "gui.h"
#include "xwin.h"
#include "disk.h"

#ifdef __CELLOS_LV2__
#include "sys/sys_time.h"
#include "sys/timer.h"
#include <sys/time.h>
#include <time.h>
#define usleep  sys_timer_usleep

void gettimeofday (struct timeval *tv, void *blah)
{
   int64_t time = sys_time_get_system_time();

   tv->tv_sec  = time / 1000000;
   tv->tv_usec = time - (tv->tv_sec * 1000000);  // implicit rounding will take care of this for us
}

#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

unsigned short int bmp[1024*1024];
unsigned short int savebmp[1024*1024];

// Mouse speed flags
#define MOUSE_SPEED_SLOWER 1
#define MOUSE_SPEED_FASTER 2
// Mouse speed multipliers
#define MOUSE_SPEED_SLOW 5
#define MOUSE_SPEED_FAST 2

int NPAGE=-1;
int SHIFTON=-1,ALTON=-1;
int MOUSEMODE=-1,SHOWKEY=-1,SHOWKEYPOS=-1,SHOWKEYTRANS=-1,STATUSON=-1,LEDON=-1;

char RPATH[512];

int analog_left[2];
int analog_right[2];
unsigned int mouse_speed[2]={0};
int slowdown=0;
extern int pix_bytes;
extern bool fake_ntsc;
extern bool real_ntsc;

int vkflag[7]={0};
static int jflag[4][16]={0};
static int kjflag[2][16]={0};
static int mflag[2][16]={0};
static int jbt[2][24]={0};
static int kbt[16]={0};

extern void reset_drawing(void);
extern void retro_key_up(int);
extern void retro_key_down(int);
extern void retro_mouse(int, int, int);
extern void retro_mouse_button(int, int, int);
extern void retro_joystick(int, int, int);
extern void retro_joystick_button(int, int, int);
extern unsigned int uae_devices[4];
extern int mapper_keys[32];
extern int video_config;
extern int video_config_aspect;
extern int zoom_mode_id;
extern bool request_update_av_info;
extern bool opt_enhanced_statusbar;
extern int opt_statusbar_position;
extern unsigned int opt_analogmouse;
extern unsigned int opt_analogmouse_deadzone;
extern float opt_analogmouse_speed;
extern unsigned int opt_dpadmouse_speed;
extern bool opt_multimouse;
extern bool opt_keyrahkeypad;
extern bool opt_keyboard_pass_through;
int turbo_fire_button=-1;
unsigned int turbo_pulse=2;
unsigned int turbo_state[5]={0};
unsigned int turbo_toggle[5]={0};

enum EMU_FUNCTIONS {
   EMU_VKBD = 0,
   EMU_STATUSBAR,
   EMU_MOUSE_TOGGLE,
   EMU_RESET,
   EMU_ASPECT_RATIO_TOGGLE,
   EMU_ZOOM_MODE_TOGGLE,
   EMU_FUNCTION_COUNT
};

/* VKBD_MIN_HOLDING_TIME: Hold a direction longer than this and automatic movement sets in */
/* VKBD_MOVE_DELAY: Delay between automatic movement from button to button */
#define VKBD_MIN_HOLDING_TIME 200
#define VKBD_MOVE_DELAY 50
bool let_go_of_direction = true;
long last_move_time = 0;
long last_press_time = 0;

void emu_function(int function) {
   switch (function)
   {
      case EMU_VKBD:
         SHOWKEY = -SHOWKEY;
         Screen_SetFullUpdate();
         break;
      case EMU_STATUSBAR:
         STATUSON = -STATUSON;
         LEDON = -LEDON;
         Screen_SetFullUpdate();
         break;
      case EMU_MOUSE_TOGGLE:
         MOUSEMODE = -MOUSEMODE;
         break;
      case EMU_RESET:
         uae_reset(0, 1); /* hardreset, keyboardreset */
         fake_ntsc = false;
         break;
      case EMU_ASPECT_RATIO_TOGGLE:
         if (real_ntsc)
            break;
         if (video_config_aspect==0)
            video_config_aspect = (video_config & 0x02) ? 1 : 2;
         else if (video_config_aspect==1)
            video_config_aspect = 2;
         else if (video_config_aspect==2)
            video_config_aspect = 1;
         request_update_av_info = true;
         break;
      case EMU_ZOOM_MODE_TOGGLE:
         zoom_mode_id++;
         if (zoom_mode_id>8) zoom_mode_id = 0;
         request_update_av_info = true;
         break;
   }
}

int STAT_BASEY;
int STAT_DECX=4;
int FONT_WIDTH=1;
int FONT_HEIGHT=1;
int BOX_PADDING=2;
int BOX_Y;
int BOX_WIDTH;
int BOX_HEIGHT=11;

extern char key_state[512];
extern char key_state2[512];

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

#define MDEBUG
#ifdef MDEBUG
#define mprintf printf
#else
#define mprintf(...) 
#endif

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

char* joystick_value_human(int val[16])
{
    static char str[4];
    sprintf(str, "%3s", "   ");

    if (val[RETRO_DEVICE_ID_JOYPAD_UP])
        str[1] = '^';

    if (val[RETRO_DEVICE_ID_JOYPAD_DOWN])
        str[1] = 'v';

    if (val[RETRO_DEVICE_ID_JOYPAD_LEFT])
        str[0] = '<';

    if (val[RETRO_DEVICE_ID_JOYPAD_RIGHT])
        str[2] = '>';

    if (val[RETRO_DEVICE_ID_JOYPAD_B])
        str[1] = '1';

    if (val[RETRO_DEVICE_ID_JOYPAD_A])
        str[1] = '2';

    if (val[RETRO_DEVICE_ID_JOYPAD_B] && val[RETRO_DEVICE_ID_JOYPAD_A])
        str[1] = '3';

    str[1] = (val[RETRO_DEVICE_ID_JOYPAD_B] || val[RETRO_DEVICE_ID_JOYPAD_A]) ? (str[1] | 0x80) : str[1];
    return str;
}

void Print_Status(void)
{
   if (!opt_enhanced_statusbar)
      return;

   // Statusbar location
   if (video_config & 0x04) // PUAE_VIDEO_HIRES
   {
      if (opt_statusbar_position < 0)
         if (opt_statusbar_position == -1)
             STAT_BASEY=2;
         else
             STAT_BASEY=-opt_statusbar_position+1+BOX_PADDING;
      else
         STAT_BASEY=gfxvidinfo.outheight-BOX_HEIGHT-opt_statusbar_position+2;

      BOX_WIDTH=retrow-146;
   }
   else // PUAE_VIDEO_LORES
   {
      if (opt_statusbar_position < 0)
         if (opt_statusbar_position == -1)
             STAT_BASEY=0;
         else
             STAT_BASEY=-opt_statusbar_position-BOX_HEIGHT+3;
      else
         STAT_BASEY=gfxvidinfo.outheight-opt_statusbar_position+2;

      BOX_WIDTH=retrow;
   }

   BOX_Y=STAT_BASEY-BOX_PADDING;

   // Joy port indicators
   char JOYPORT1[10];
   char JOYPORT2[10];
   char JOYPORT3[10];
   char JOYPORT4[10];

   // Regular joyflags
   sprintf(JOYPORT1, "J1%3s", joystick_value_human(jflag[0]));
   sprintf(JOYPORT2, "J2%3s", joystick_value_human(jflag[1]));
   sprintf(JOYPORT3, "J3%3s", joystick_value_human(jflag[2]));
   sprintf(JOYPORT4, "J4%3s", joystick_value_human(jflag[3]));

   // Mouse flags
   if (strcmp(JOYPORT1, "J1   ") == 0)
      sprintf(JOYPORT1, "J1%3s", joystick_value_human(mflag[1]));
   if (strcmp(JOYPORT2, "J2   ") == 0)
      sprintf(JOYPORT2, "J2%3s", joystick_value_human(mflag[0]));

   // Keyrah joyflags
   if (opt_keyrahkeypad)
   {
      if (strcmp(JOYPORT1, "J1   ") == 0)
         sprintf(JOYPORT1, "J1%3s", joystick_value_human(kjflag[0]));
      if (strcmp(JOYPORT2, "J2   ") == 0)
         sprintf(JOYPORT2, "J2%3s", joystick_value_human(kjflag[1]));
   }

   // Emulated mouse speed
   char MSPD_STR[2] = { 0 };
   switch (opt_dpadmouse_speed)
   {
      case 2:
      case 4:
         MSPD_STR[0]='S';
         break;
      case 6:
         MSPD_STR[0]='M';
         break;
      case 8:
         MSPD_STR[0]='F';
         break;
      case 10:
      case 12:
         MSPD_STR[0]='V';
         break;
   }

   // Zoom mode
   char ZOOM_MODE[10];
   switch (zoom_mode_id)
   {
      default:
      case 0:
         sprintf(ZOOM_MODE, "%s", "None");
         break;
      case 1:
         sprintf(ZOOM_MODE, "%s", "Minimum");
         break;
      case 2:
         sprintf(ZOOM_MODE, "%s", "Smaller");
         break;
      case 3:
         sprintf(ZOOM_MODE, "%s", "Small");
         break;
      case 4:
         sprintf(ZOOM_MODE, "%s", "Medium");
         break;
      case 5:
         sprintf(ZOOM_MODE, "%s", "Large");
         break;
      case 6:
         sprintf(ZOOM_MODE, "%s", "Larger");
         break;
      case 7:
         sprintf(ZOOM_MODE, "%s", "Maximum");
         break;
      case 8:
         sprintf(ZOOM_MODE, "%s", "Automatic");
         break;
   }

   // Statusbar output
   if (pix_bytes == 4)
   {
      DrawFBoxBmp32((uint32_t *)bmp,0,BOX_Y,BOX_WIDTH,BOX_HEIGHT,RGB888(0,0,0));

      Draw_text32((uint32_t *)bmp,STAT_DECX+0,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT1);
      Draw_text32((uint32_t *)bmp,STAT_DECX+40,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT2);
      Draw_text32((uint32_t *)bmp,STAT_DECX+80,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT3);
      Draw_text32((uint32_t *)bmp,STAT_DECX+120,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT4);
      if (MOUSEMODE==-1)
         Draw_text32((uint32_t *)bmp,STAT_DECX+160,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,20,"Joystick");
      else
         Draw_text32((uint32_t *)bmp,STAT_DECX+160,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,20,"Mouse:%s ", MSPD_STR);
      Draw_text32((uint32_t *)bmp,STAT_DECX+240,STAT_BASEY,0xffffff,0x0000,FONT_WIDTH,FONT_HEIGHT,20,"Zoom:%s", ZOOM_MODE);
   }
   else
   {
      DrawFBoxBmp(bmp,0,BOX_Y,BOX_WIDTH,BOX_HEIGHT,RGB565(0,0,0));

      Draw_text(bmp,STAT_DECX+0,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT1);
      Draw_text(bmp,STAT_DECX+40,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT2);
      Draw_text(bmp,STAT_DECX+80,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT3);
      Draw_text(bmp,STAT_DECX+120,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,40,JOYPORT4);
      if (MOUSEMODE==-1)
         Draw_text(bmp,STAT_DECX+160,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,20,"Joystick");
      else
         Draw_text(bmp,STAT_DECX+160,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,20,"Mouse:%s ", MSPD_STR);
      Draw_text(bmp,STAT_DECX+240,STAT_BASEY,0xffff,0x0000,FONT_WIDTH,FONT_HEIGHT,20,"Zoom:%s", ZOOM_MODE);
   }
}

void Screen_SetFullUpdate(void)
{
   reset_drawing();
}

void ProcessKeyrah()
{
   /*** Port 2 ***/
   /* Up / Down */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2))
   {
      retro_joystick(0, 1, -1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_UP]=1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8))
   {
      retro_joystick(0, 1, 1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN]=1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_UP]==1)
   {
      retro_joystick(0, 1, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_UP]=0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN]==1)
   {
      retro_joystick(0, 1, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN]=0;
   }

   /* Left / Right */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6))
   {
      retro_joystick(0, 0, -1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT]=1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4))
   {
      retro_joystick(0, 0, 1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT]=1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT]==1)
   {
      retro_joystick(0, 0, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT]=0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT]==1)
   {
      retro_joystick(0, 0, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT]=0;
   }

   /* Fire */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP5)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_B]==0)
   {
      retro_joystick_button(0, 0, 1);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_B]=1;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP5)
   && kjflag[0][RETRO_DEVICE_ID_JOYPAD_B]==1)
   {
      retro_joystick_button(0, 0, 0);
      kjflag[0][RETRO_DEVICE_ID_JOYPAD_B]=0;
   }


   /*** Port 1 ***/
   /* Up / Down */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3))
   {
      retro_joystick(1, 1, -1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_UP]=1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9))
   {
      retro_joystick(1, 1, 1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_DOWN]=1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_UP]==1)
   {
      retro_joystick(1, 1, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_UP]=0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_DOWN]==1)
   {
      retro_joystick(1, 1, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_DOWN]=0;
   }

   /* Left / Right */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1))
   {
      retro_joystick(1, 0, -1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_LEFT]=1;
   }
   else
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1)
   && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7))
   {
      retro_joystick(1, 0, 1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_RIGHT]=1;
   }

   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_LEFT]==1)
   {
      retro_joystick(1, 0, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_LEFT]=0;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_RIGHT]==1)
   {
      retro_joystick(1, 0, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_RIGHT]=0;
   }

   /* Fire */
   if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP0)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_B]==0)
   {
      retro_joystick_button(1, 0, 1);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_B]=1;
   }
   else
   if (!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP0)
   && kjflag[1][RETRO_DEVICE_ID_JOYPAD_B]==1)
   {
      retro_joystick_button(1, 0, 0);
      kjflag[1][RETRO_DEVICE_ID_JOYPAD_B]=0;
   }
}

int retro_button_to_uae_button(int i)
{
   int uae_button = -1;
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
   return uae_button;
}

void ProcessController(int retro_port, int i)
{
   int uae_button = -1;

   if (i>3 && i<8) // Directions, need to fight around presses on the same axis
   {
      if (i==RETRO_DEVICE_ID_JOYPAD_UP || i==RETRO_DEVICE_ID_JOYPAD_DOWN)
      {
         if (i==RETRO_DEVICE_ID_JOYPAD_UP && SHOWKEY==-1)
         {
            if (input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, i)
            && !input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
            {
               retro_joystick(retro_port, 1, -1);
               jflag[retro_port][i]=1;
            }
         }
         else
         if (i==RETRO_DEVICE_ID_JOYPAD_DOWN && SHOWKEY==-1)
         {
            if (input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, i)
            && !input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
            {
               retro_joystick(retro_port, 1, 1);
               jflag[retro_port][i]=1;
            }
         }

         if (!input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)
         && jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_UP]==1)
         {
            retro_joystick(retro_port, 1, 0);
            jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_UP]=0;
         }
         else
         if (!input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)
         && jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_DOWN]==1)
         {
            retro_joystick(retro_port, 1, 0);
            jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_DOWN]=0;
         }
      }

      if (i==RETRO_DEVICE_ID_JOYPAD_LEFT || i==RETRO_DEVICE_ID_JOYPAD_RIGHT)
      {
         if (i==RETRO_DEVICE_ID_JOYPAD_LEFT && SHOWKEY==-1)
         {
            if (input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, i)
            && !input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
            {
               retro_joystick(retro_port, 0, -1);
               jflag[retro_port][i]=1;
            }
         }
         else
         if (i==RETRO_DEVICE_ID_JOYPAD_RIGHT && SHOWKEY==-1)
         {
            if (input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, i)
            && !input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
            {
               retro_joystick(retro_port, 0, 1);
               jflag[retro_port][i]=1;
            }
         }

         if (!input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT)
         && jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_LEFT]==1)
         {
            retro_joystick(retro_port, 0, 0);
            jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_LEFT]=0;
         }
         else
         if (!input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)
         && jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_RIGHT]==1)
         {
            retro_joystick(retro_port, 0, 0);
            jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_RIGHT]=0;
         }
      }
   }
   else if (i != turbo_fire_button) // Buttons
   {
      uae_button = retro_button_to_uae_button(i);
      if (uae_button != -1)
      {
         if (input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, i) && jflag[retro_port][i]==0 && SHOWKEY==-1)
         {
            retro_joystick_button(retro_port, uae_button, 1);
            jflag[retro_port][i]=1;
         }
         else
         if (!input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, i) && jflag[retro_port][i]==1)
         {
            retro_joystick_button(retro_port, uae_button, 0);
            jflag[retro_port][i]=0;
         }
      }
   }
}

void ProcessTurbofire(int retro_port, int i)
{
   if (turbo_fire_button != -1 && i == turbo_fire_button)
   {
      if (input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, turbo_fire_button))
      {
         if (turbo_state[retro_port])
         {
            if ((turbo_toggle[retro_port]) == (turbo_pulse))
               turbo_toggle[retro_port]=1;
            else
               turbo_toggle[retro_port]++;

            if (turbo_toggle[retro_port] > (turbo_pulse / 2))
            {
               retro_joystick_button(retro_port, 0, 0);
               jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_B]=0;
            }
            else
            {
               retro_joystick_button(retro_port, 0, 1);
               jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_B]=1;
            }
         }
         else
         {
            turbo_state[retro_port]=1;
            retro_joystick_button(retro_port, 0, 1);
            jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_B]=1;
         }
      }
      else if (!input_state_cb(retro_port, RETRO_DEVICE_JOYPAD, 0, turbo_fire_button) && turbo_state[retro_port]==1)
      {
         turbo_state[retro_port]=0;
         turbo_toggle[retro_port]=1;
         retro_joystick_button(retro_port, 0, 0);
         jflag[retro_port][RETRO_DEVICE_ID_JOYPAD_B]=0;
      }
   }
}

void ProcessKey(int disable_physical_cursor_keys)
{
   int i;
   for (i=0;i<320;i++)
   {
      key_state[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, i) ? 0x80 : 0;

      /* CapsLock */
      if (keyboard_translation[i]==AK_CAPSLOCK)
      {
         if (key_state[i] && key_state2[i]==0)
         {
            retro_key_down(keyboard_translation[i]);
            retro_key_up(keyboard_translation[i]);
            SHIFTON=-SHIFTON;
            Screen_SetFullUpdate();
            key_state2[i]=1;
         }
         else if (!key_state[i] && key_state2[i]==1)
            key_state2[i]=0;
      }
      /* Special key (Right Alt) for overriding RetroPad cursor override */ 
      else if (keyboard_translation[i]==AK_RALT)
      {
         if (key_state[i] && key_state2[i]==0)
         {
            ALTON=1;
            retro_key_down(keyboard_translation[i]);
            key_state2[i]=1;
         }
         else if (!key_state[i] && key_state2[i]==1)
         {
            ALTON=-1;
            retro_key_up(keyboard_translation[i]);
            key_state2[i]=0;
         }
      }
      else
      {
         /* Override cursor keys if used as a RetroPad */
         if (disable_physical_cursor_keys && (i == RETROK_DOWN || i == RETROK_UP || i == RETROK_LEFT || i == RETROK_RIGHT))
            continue;

         /* Skip numpad if Keyrah is active */
         if (opt_keyrahkeypad)
         {
            switch(i) {
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

         /* Skip keys if VKBD is active */
         if (SHOWKEY==1)
            continue;

         if (key_state[i] && keyboard_translation[i]!=-1 && key_state2[i]==0)
         {
            if (SHIFTON==1)
               retro_key_down(keyboard_translation[RETROK_LSHIFT]);

            retro_key_down(keyboard_translation[i]);
            key_state2[i]=1;
         }
         else if (!key_state[i] && keyboard_translation[i]!=-1 && key_state2[i]==1)
         {
            retro_key_up(keyboard_translation[i]);
            key_state2[i]=0;

            if (SHIFTON==1)
               retro_key_up(keyboard_translation[RETROK_LSHIFT]);
         }
      }
   }
}

void update_input(int disable_physical_cursor_keys)
{
   // RETRO    B   Y   SLT STA UP  DWN LFT RGT A   X   L   R   L2  R2  L3  R3  LR  LL  LD  LU  RR  RL  RD  RU
   // INDEX    0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23

   static int i, j, mk;
   static int oldi=-1;
   static int vkx=0,vky=0;

   static int LX, LY, RX, RY;
   static int threshold=20000;

   /* Keyup only after button is up */
   if (oldi!=-1 && vkflag[4]!=1)
   {
      retro_key_up(oldi);

      if (SHIFTON==1)
         retro_key_up(keyboard_translation[RETROK_LSHIFT]);

      oldi=-1;
   }

   input_poll_cb();

   /* Keyboard hotkeys */
   for (i = 0; i < EMU_FUNCTION_COUNT; i++)
   {
      mk = i + 24;
      /* Key down */
      if (kbt[i]==0 && mapper_keys[mk]!=0 && input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, mapper_keys[mk]))
      {
         //printf("KEYDN: %d\n", mk);
         kbt[i]=1;
         switch (mk)
         {
            case 24:
               emu_function(EMU_VKBD);
               break;
            case 25:
               emu_function(EMU_STATUSBAR);
               break;
            case 26:
               emu_function(EMU_MOUSE_TOGGLE);
               break;
            case 27:
               emu_function(EMU_RESET);
               break;
            case 28:
               emu_function(EMU_ASPECT_RATIO_TOGGLE);
               break;
            case 29:
               emu_function(EMU_ZOOM_MODE_TOGGLE);
               break;
         }
      }
      /* Key up */
      else if (kbt[i]==1 && mapper_keys[mk]!=0 && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, mapper_keys[mk]))
      {
         //printf("KEYUP: %d\n", mk);
         kbt[i]=0;
      }
   }

   

   /* The check for kbt[i] here prevents the hotkey from generating key events */
   /* SHOWKEY check is now in ProcessKey to allow certain keys while SHOWKEY */
   int processkey=1;
   for (i = 0; i < (sizeof(kbt)/sizeof(kbt[0])); i++)
   {
      if (kbt[i] == 1)
      {
         processkey=0;
         break;
      }
   }

   if (processkey && disable_physical_cursor_keys != 2)
      ProcessKey(disable_physical_cursor_keys);

   if (opt_keyrahkeypad)
      ProcessKeyrah();

   /* RetroPad hotkeys for ports 1 & 2 */
   for (j = 0; j < 2; j++)
   {
      if (uae_devices[j] == RETRO_DEVICE_JOYPAD)
      {
         LX = input_state_cb(j, RETRO_DEVICE_ANALOG, 0, 0);
         LY = input_state_cb(j, RETRO_DEVICE_ANALOG, 0, 1);
         RX = input_state_cb(j, RETRO_DEVICE_ANALOG, 1, 0);
         RY = input_state_cb(j, RETRO_DEVICE_ANALOG, 1, 1);

         for (i = 0; i < 24; i++)
         {
            int just_pressed = 0;
            int just_released = 0;
            if (i > 0 && (i<4 || i>7) && i < 16) /* Remappable RetroPad buttons excluding D-Pad + B */
            {
               /* Skip the vkbd extra buttons if vkbd is visible */
               if (SHOWKEY==1 && (i==RETRO_DEVICE_ID_JOYPAD_A || i==RETRO_DEVICE_ID_JOYPAD_X))
                  continue;

               if (input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, i) && jbt[j][i]==0 && i!=turbo_fire_button)
                  just_pressed = 1;
               else if (!input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, i) && jbt[j][i]==1 && i!=turbo_fire_button)
                  just_released = 1;
            }
            else if (i >= 16) /* Remappable RetroPad analog stick directions */
            {
               switch (i)
               {
                  case 16: /* LR */
                     if (LX > threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (LX < threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 17: /* LL */
                     if (LX < -threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (LX > -threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 18: /* LD */
                     if (LY > threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (LY < threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 19: /* LU */
                     if (LY < -threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (LY > -threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 20: /* RR */
                     if (RX > threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (RX < threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 21: /* RL */
                     if (RX < -threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (RX > -threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 22: /* RD */
                     if (RY > threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (RY < threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  case 23: /* RU */
                     if (RY < -threshold && jbt[j][i] == 0) just_pressed = 1;
                     else if (RY > -threshold && jbt[j][i] == 1) just_released = 1;
                     break;
                  default:
                     break;
               }
            }

            if (just_pressed)
            {
               jbt[j][i] = 1;
               if (mapper_keys[i] == 0) /* Unmapped, e.g. set to "---" in core options */
                  continue;

               if (mapper_keys[i] == mapper_keys[24]) /* Virtual keyboard */
                  emu_function(EMU_VKBD);
               else if (mapper_keys[i] == mapper_keys[25]) /* Statusbar */
                  emu_function(EMU_STATUSBAR);
               else if (mapper_keys[i] == mapper_keys[26]) /* Toggle mouse control */
                  emu_function(EMU_MOUSE_TOGGLE);
               else if (mapper_keys[i] == mapper_keys[27]) /* Reset */
                  emu_function(EMU_RESET);
               else if (mapper_keys[i] == mapper_keys[28]) /* Toggle aspect ratio */
                  emu_function(EMU_ASPECT_RATIO_TOGGLE);
               else if (mapper_keys[i] == mapper_keys[29]) /* Toggle zoom mode */
                  emu_function(EMU_ZOOM_MODE_TOGGLE);
               else if (mapper_keys[i] == -2) /* Mouse button left */
               {
                  retro_mouse_button(j, 0, 1);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_B]=1;
               }
               else if (mapper_keys[i] == -3) /* Mouse button right */
               {
                  retro_mouse_button(j, 1, 1);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_A]=1;
               }
               else if (mapper_keys[i] == -4) /* Mouse button middle */
                  retro_mouse_button(j, 2, 1);
               else if (mapper_keys[i] == -5) /* Mouse speed slower */
                  mouse_speed[j] |= MOUSE_SPEED_SLOWER;
               else if (mapper_keys[i] == -6) /* Mouse speed faster */
                  mouse_speed[j] |= MOUSE_SPEED_FASTER;
               else /* Keyboard keys */
                  retro_key_down(keyboard_translation[mapper_keys[i]]);
            }
            else if (just_released)
            {
               jbt[j][i] = 0;
               if (mapper_keys[i] == 0) /* Unmapped, e.g. set to "---" in core options */
                  continue;

               if (mapper_keys[i] == mapper_keys[24]) /* Virtual keyboard */
                  ; /* nop */
               else if (mapper_keys[i] == mapper_keys[25]) /* Statusbar */
                  ; /* nop */
               else if (mapper_keys[i] == mapper_keys[26]) /* Toggle mouse control */
                  ; /* nop */
               else if (mapper_keys[i] == mapper_keys[27]) /* Reset */
                  ; /* nop */
               else if (mapper_keys[i] == mapper_keys[28]) /* Toggle aspect ratio */
                  ; /* nop */
               else if (mapper_keys[i] == mapper_keys[29]) /* Toggle zoom mode */
                  ; /* nop */
               else if (mapper_keys[i] == -2) /* Mouse button left */
               {
                  retro_mouse_button(j, 0, 0);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_B]=0;
               }
               else if (mapper_keys[i] == -3) /* Mouse button right */
               {
                  retro_mouse_button(j, 1, 0);
                  mflag[j][RETRO_DEVICE_ID_JOYPAD_A]=0;
               }
               else if (mapper_keys[i] == -4) /* Mouse button middle */
                  retro_mouse_button(j, 2, 0);
               else if (mapper_keys[i] == -5) /* Mouse speed slower */
                  mouse_speed[j] &= ~MOUSE_SPEED_SLOWER;
               else if (mapper_keys[i] == -6) /* Mouse speed faster */
                  mouse_speed[j] &= ~MOUSE_SPEED_FASTER;
               else /* Keyboard keys */
                  retro_key_up(keyboard_translation[mapper_keys[i]]);
            }
         } /* for i */
      } /* if uae_devices[j]==joypad */
   } /* for j */

   /* Virtual keyboard for ports 1 & 2 */
   if (SHOWKEY==1)
   {
      if (vkflag[4]==0) /* Allow directions when key is not pressed */
      {
         if (vkflag[0]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)))
            vkflag[0]=1;
         else if (vkflag[0]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)))
            vkflag[0]=0;

         if (vkflag[1]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)))
            vkflag[1]=1;
         else if (vkflag[1]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)))
            vkflag[1]=0;

         if (vkflag[2]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT)))
            vkflag[2]=1;
         else if (vkflag[2]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT)))
            vkflag[2]=0;

         if (vkflag[3]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            vkflag[3]=1;
         else if (vkflag[3]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)))
            vkflag[3]=0;
      }
      else /* Release all directions when key is pressed */
      {
         vkflag[0]=0;
         vkflag[1]=0;
         vkflag[2]=0;
         vkflag[3]=0;
      }

      if (vkflag[0] || vkflag[1] || vkflag[2] || vkflag[3])
      {
         /* Movement needs screen refresh in transparent mode */
         if (SHOWKEYTRANS==1)
            Screen_SetFullUpdate();

         long now = GetTicks();
         if (let_go_of_direction)
            /* just pressing down */
            last_press_time = now;

         if ( (now - last_press_time > VKBD_MIN_HOLDING_TIME
            && now - last_move_time > VKBD_MOVE_DELAY)
            || let_go_of_direction)
         {
            last_move_time = now;

            if (vkflag[0])
               vky -= 1;
            else if (vkflag[1])
               vky += 1;

            if (vkflag[2])
               vkx -= 1;
            else if (vkflag[3])
               vkx += 1;
         }
         let_go_of_direction = false;
      }
      else
         let_go_of_direction = true;

      if (vkx < 0)
         vkx=NPLGN-1;
      else if (vkx > NPLGN-1)
         vkx=0;
      if (vky < 0)
         vky=NLIGN-1;
      else if (vky > NLIGN-1)
         vky=0;

      virtual_kbd(bmp,vkx,vky);

      /* Position toggle, RetroPad X */
      i=RETRO_DEVICE_ID_JOYPAD_X;
      if (vkflag[6]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, i)))
      {
         vkflag[6]=1;
         SHOWKEYPOS=-SHOWKEYPOS;
         Screen_SetFullUpdate();
      }
      else if (vkflag[6]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, i)))
      {
         vkflag[6]=0;
      }

      /* Transparency toggle, RetroPad A */
      i=RETRO_DEVICE_ID_JOYPAD_A;
      if (vkflag[5]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, i)))
      {
         vkflag[5]=1;
         SHOWKEYTRANS=-SHOWKEYTRANS;
         Screen_SetFullUpdate();
      }
      else if (vkflag[5]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, i)))
      {
         vkflag[5]=0;
      }

      /* Key press, RetroPad B or Keyboard Enter */
      i=RETRO_DEVICE_ID_JOYPAD_B;
      if (vkflag[4]==0 && (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) || input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, i) || input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN)))
      {
         vkflag[4]=1;
         i=check_vkey2(vkx,vky);

         if (i==-1)
            oldi=-1;
         else if (i==-2)
         {
            oldi=-1;
            NPAGE=-NPAGE;
            Screen_SetFullUpdate();
         }
         else
         {
            if (i==AK_CAPSLOCK)
            {
               retro_key_down(i);
               retro_key_up(i);
               SHIFTON=-SHIFTON;
               Screen_SetFullUpdate();
               oldi=-1;
            }
            else
            {
               oldi=i;
               if (SHIFTON==1)
                  retro_key_down(keyboard_translation[RETROK_LSHIFT]);

               retro_key_down(i);
            }
         }
      }
      else if (vkflag[4]==1 && (!input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && !input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, i) && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN)))
      {
         vkflag[4]=0;
      }
   }
}

void retro_poll_event()
{
   /* If RetroPad is controlled with keyboard keys, then prevent up/down/left/right/fire/fire2 from generating keyboard key presses */
   if (uae_devices[0] == RETRO_DEVICE_JOYPAD && ALTON==-1 && 
      (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B) ||
       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A)
       ) &&
       !opt_keyboard_pass_through
   )
      update_input(2); /* Skip all keyboard input when fire/fire2 is pressed */
   else if (
      (uae_devices[0] == RETRO_DEVICE_JOYPAD) && ALTON==-1 &&
      (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) ||
       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) ||
       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ||
       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)
       ) &&
       !opt_keyboard_pass_through
   )
      update_input(1); /* Process all inputs but disable cursor keys */
   else
      update_input(0); /* Process all inputs */

   if (ALTON==-1) /* retro joypad take control over keyboard joy */
   /* override keydown, but allow keyup, to prevent key sticking during keyboard use, if held down on opening keyboard */
   /* keyup allowing most likely not needed on actual keyboard presses even though they get stuck also */
   {
      static float mouse_multiplier=1;
      static int dpadmouse_speed;
      static int uae_mouse_x[2],uae_mouse_y[2];
      static int uae_mouse_l[2]={0},uae_mouse_r[2]={0},uae_mouse_m[2]={0};
      static int mouse_lmb[2]={0},mouse_rmb[2]={0},mouse_mmb[2]={0};
      static int16_t mouse_x[2]={0},mouse_y[2]={0};
      static int i=0,j=0;

      int retro_port;
      for (retro_port = 0; retro_port <= 3; retro_port++)
      {
         switch (uae_devices[retro_port])
         {
            case RETRO_DEVICE_JOYPAD:
               // RetroPad control (ports 0 & 1 joystick disabled if MOUSEMODE is on)
               if (MOUSEMODE==1 && (retro_port==0 || retro_port==1))
                  break;

               for (i=0;i<16;i++) // All buttons
               {
                  if (i==0 || (i>3 && i<9)) // DPAD + B + A
                  {
                     // Skip 2nd fire if keymapped
                     if(retro_port==0 && i==RETRO_DEVICE_ID_JOYPAD_A && mapper_keys[RETRO_DEVICE_ID_JOYPAD_A]!=0)
                        continue;

                     ProcessController(retro_port, i);
                  }
                  ProcessTurbofire(retro_port, i);
               }
               break;

            case RETRO_DEVICE_UAE_CD32PAD:
               for (i=0;i<16;i++) // All buttons
               {
                  if (i<2 || (i>2 && i<12)) // Only skip Select (2)
                  {
                     ProcessController(retro_port, i);
                  }
                  ProcessTurbofire(retro_port, i);
               }
               break;

            case RETRO_DEVICE_UAE_JOYSTICK:
               for (i=0;i<9;i++) // All buttons up to A
               {
                  if (i==0 || (i>3 && i<9)) // DPAD + B + A
                  {
                     ProcessController(retro_port, i);
                  }
               }
               break;
         }
      }
   
      // Mouse control
      uae_mouse_l[0]=uae_mouse_r[0]=uae_mouse_m[0]=0;
      uae_mouse_l[1]=uae_mouse_r[1]=uae_mouse_m[0]=0;
      uae_mouse_x[0]=uae_mouse_y[0]=0;
      uae_mouse_x[1]=uae_mouse_y[1]=0;

      // Joypad buttons only with digital mouse mode and virtual keyboard hidden
      if (MOUSEMODE==1 && SHOWKEY==-1 && (uae_devices[0] == RETRO_DEVICE_JOYPAD || uae_devices[1] == RETRO_DEVICE_JOYPAD))
      {
         for (j = 0; j < 2; j++)
         {
            uae_mouse_l[j] = input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
            uae_mouse_r[j] = input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
         }
      }

      // Real mouse buttons
      if (!uae_mouse_l[0] && !uae_mouse_r[0])
      {
         uae_mouse_l[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
         uae_mouse_r[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
         uae_mouse_m[0] = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
      }

      // Second mouse buttons only when enabled
      if(opt_multimouse)
          if (!uae_mouse_l[1] && !uae_mouse_r[1])
          {
             uae_mouse_l[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
             uae_mouse_r[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
             uae_mouse_m[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
          }

      // Joypad movement only with digital mouse mode and virtual keyboard hidden
      if (MOUSEMODE==1 && SHOWKEY==-1 && (uae_devices[0] == RETRO_DEVICE_JOYPAD || uae_devices[1] == RETRO_DEVICE_JOYPAD))
      {
         for (j = 0; j < 2; j++)
         {
            // Digital mouse speed modifiers
            dpadmouse_speed = opt_dpadmouse_speed;
            if (mouse_speed[j] & MOUSE_SPEED_FASTER)
               dpadmouse_speed = dpadmouse_speed + 4;
            if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
               dpadmouse_speed = dpadmouse_speed - 4;

            // Digital mouse speed limits
            if (dpadmouse_speed<4) dpadmouse_speed = 2;
            if (dpadmouse_speed>10) dpadmouse_speed = 12;

            if (input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
               uae_mouse_x[j] += dpadmouse_speed;
            if (input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
               uae_mouse_x[j] -= dpadmouse_speed;
            if (input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
               uae_mouse_y[j] += dpadmouse_speed;
            if (input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
               uae_mouse_y[j] -= dpadmouse_speed;
         }
      }

      // Left analog movement
      if (opt_analogmouse == 1 || opt_analogmouse == 3)
         for (j = 0; j < 2; j++)
         {
            // No keymappings and mousing at the same time
            if (!uae_mouse_x[j] && !uae_mouse_y[j] && (!mapper_keys[16] && !mapper_keys[17] && !mapper_keys[18] && !mapper_keys[19]))
            {
               analog_left[0] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
               analog_left[1] = (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));

               // Analog stick speed modifiers
               mouse_multiplier = 1;
               if (mouse_speed[j] & MOUSE_SPEED_FASTER)
                  mouse_multiplier = mouse_multiplier * MOUSE_SPEED_FAST;
               if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
                  mouse_multiplier = mouse_multiplier / MOUSE_SPEED_SLOW;

               if (abs(analog_left[0]) <= opt_analogmouse_deadzone * 32768 / 100)
                  analog_left[0] = 0;
               if (abs(analog_left[1]) <= opt_analogmouse_deadzone * 32768 / 100)
                  analog_left[1] = 0;

               if (abs(analog_left[0]) > 0)
               {
                  uae_mouse_x[j] = analog_left[0] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
                  if (uae_mouse_x[j] == 0)
                     uae_mouse_x[j] = (analog_left[0] > 0) ? 1 : -1;
               }

               if (abs(analog_left[1]) > 0)
               {
                  uae_mouse_y[j] = analog_left[1] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
                  if (uae_mouse_y[j] == 0)
                     uae_mouse_y[j] = (analog_left[1] > 0) ? 1 : -1;
               }
            }
         }

      // Right analog movement
      if (opt_analogmouse == 2 || opt_analogmouse == 3)
         for (j = 0; j < 2; j++)
         {
            // No keymappings and mousing at the same time
            if (!uae_mouse_x[j] && !uae_mouse_y[j] && (!mapper_keys[20] && !mapper_keys[21] && !mapper_keys[22] && !mapper_keys[23]))
            {
               analog_right[0] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
               analog_right[1] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));

               // Analog stick speed modifiers
               mouse_multiplier = 1;
               if (mouse_speed[j] & MOUSE_SPEED_FASTER)
                  mouse_multiplier = mouse_multiplier * MOUSE_SPEED_FAST;
               if (mouse_speed[j] & MOUSE_SPEED_SLOWER)
                  mouse_multiplier = mouse_multiplier / MOUSE_SPEED_SLOW;

               if (abs(analog_right[0]) <= opt_analogmouse_deadzone * 32768 / 100)
                  analog_right[0] = 0;
               if (abs(analog_right[1]) <= opt_analogmouse_deadzone * 32768 / 100)
                  analog_right[1] = 0;

               if (abs(analog_right[0]) > 0)
               {
                  uae_mouse_x[j] = analog_right[0] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
                  if (uae_mouse_x[j] == 0)
                     uae_mouse_x[j] = (analog_right[0] > 0) ? 1 : -1;
               }

               if (abs(analog_right[1]) > 0)
               {
                  uae_mouse_y[j] = analog_right[1] * 10 * (opt_analogmouse_speed * opt_analogmouse_speed * 0.7) / (32768 / mouse_multiplier);
                  if (uae_mouse_y[j] == 0)
                     uae_mouse_y[j] = (analog_right[1] > 0) ? 1 : -1;
               }
            }
         }

      // Real mouse movement
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

      // Second mouse movement only when enabled
      if(opt_multimouse)
          if (!uae_mouse_x[1] && !uae_mouse_y[1])
          {
             mouse_x[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
             mouse_y[1] = input_state_cb(1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);

             if (mouse_x[1] || mouse_y[1])
             {
                uae_mouse_x[1] = mouse_x[1];
                uae_mouse_y[1] = mouse_y[1];
             }
          }

      // Ports 1 & 2
      for (j = 0; j < 2; j++)
      {
         // Mouse buttons to UAE
         if (mouse_lmb[j]==0 && uae_mouse_l[j])
         {
            mouse_lmb[j]=1;
            mflag[j][RETRO_DEVICE_ID_JOYPAD_B]=1;
            retro_mouse_button(j, 0, 1);
         }
         else if (mouse_lmb[j]==1 && !uae_mouse_l[j])
         {
            mouse_lmb[j]=0;
            mflag[j][RETRO_DEVICE_ID_JOYPAD_B]=0;
            retro_mouse_button(j, 0, 0);
         }

         if (mouse_rmb[j]==0 && uae_mouse_r[j])
         {
            mouse_rmb[j]=1;
            mflag[j][RETRO_DEVICE_ID_JOYPAD_A]=1;
            retro_mouse_button(j, 1, 1);
         }
         else if (mouse_rmb[j]==1 && !uae_mouse_r[j])
         {
            mouse_rmb[j]=0;
            mflag[j][RETRO_DEVICE_ID_JOYPAD_A]=0;
            retro_mouse_button(j, 1, 0);
         }

         if (mouse_mmb[j]==0 && uae_mouse_m[j])
         {
            mouse_mmb[j]=1;
            mflag[j][RETRO_DEVICE_ID_JOYPAD_Y]=1;
            retro_mouse_button(j, 2, 1);
         }
         else if (mouse_mmb[j]==1 && !uae_mouse_m[j])
         {
            mouse_mmb[j]=0;
            mflag[j][RETRO_DEVICE_ID_JOYPAD_Y]=0;
            retro_mouse_button(j, 2, 0);
         }

         // Mouse movements to UAE
         if (uae_mouse_y[j]<0 && mflag[j][RETRO_DEVICE_ID_JOYPAD_UP]==0)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_UP]=1;
         if (uae_mouse_y[j]>-1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_UP]==1)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_UP]=0;

         if (uae_mouse_y[j]>0 && mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN]==0)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN]=1;
         if (uae_mouse_y[j]<1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN]==1)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_DOWN]=0;

         if (uae_mouse_x[j]<0 && mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT]==0)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT]=1;
         if (uae_mouse_x[j]>-1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT]==1)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_LEFT]=0;

         if (uae_mouse_x[j]>0 && mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT]==0)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT]=1;
         if (uae_mouse_x[j]<1 && mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT]==1)
            mflag[j][RETRO_DEVICE_ID_JOYPAD_RIGHT]=0;

         if (uae_mouse_x[j] || uae_mouse_y[j])
            retro_mouse(j, uae_mouse_x[j], uae_mouse_y[j]);
      }
   }
}
