#include "sysconfig.h"
#include "sysdeps.h"

#ifdef __CELLOS_LV2__
#include <ctype.h>
#endif

#include "options.h"
#include "uae.h"
#include "memory.h"
#include "xwin.h"
#include "custom.h"
#include "drawing.h"
#include "hotkeys.h"
#include "hrtimer.h"

#include "inputdevice.h"
void inputdevice_release_all_keys(void);
extern int mouse_port[NORMAL_JPORTS];

#include "libretro-core.h"
#include "libretro-mapper.h"

extern unsigned int retro_devices[RETRO_DEVICES];
bool inputdevice_finalized = false;

extern int defaultw;
extern int defaulth;
extern int libretro_runloop_active;
extern int libretro_frame_end;

unsigned short int* pixbuf = NULL;
extern unsigned short int retro_bmp[RETRO_BMP_SIZE];
void retro_audio_batch_cb(const int16_t *data, size_t frames);

int prefs_changed = 0;

int retro_thisframe_first_drawn_line;
int retro_thisframe_last_drawn_line;
int retro_min_diwstart;
int retro_max_diwstop;

int gui_init (void)
{
   return 0;
}

/*
 * Handle target-specific cfgfile options
 */
void target_save_options (struct zfile* f, struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
   return 0;
}

void target_default_options (struct uae_prefs *p, int type)
{
}

/* --- mouse input --- */
void retro_mouse(int port, int dx, int dy)
{
   mouse_port[port] = 1;
   setmousestate(port, 0, dx, 0);
   setmousestate(port, 1, dy, 0);
}

void retro_mouse_button(int port, int button, int state)
{
   mouse_port[port] = 1;
   setmousebuttonstate(port, button, state);
}

/* --- joystick input --- */
void retro_joystick(int port, int axis, int state)
{
   /* Disable mouse in normal ports, joystick/mouse inverted */
   if (port < 2)
   {
      int m_port = (port == 0) ? 1 : 0;
      mouse_port[m_port] = 0;
   }
   setjoystickstate(port, axis, state, 1);
}

void retro_joystick_analog(int port, int axis, int state)
{
   /* Disable mouse in normal ports, joystick/mouse inverted */
   if (port < 2)
   {
      int m_port = (port == 0) ? 1 : 0;
      mouse_port[m_port] = 0;
   }
   setjoystickstate(port, axis, state, 32768);
}

void retro_joystick_button(int port, int button, int state)
{
   /* Disable mouse in normal ports, joystick/mouse inverted */
   if (port < 2)
   {
      int m_port = (port == 0) ? 1 : 0;
      mouse_port[m_port] = 0;
   }
   setjoybuttonstate(port, button, state);
}

/* --- keyboard input --- */
void retro_key_down(int key)
{
   inputdevice_do_keyboard(key, 1);
}

void retro_key_up(int key)
{
   inputdevice_do_keyboard(key, 0);
}


/* retro */
void retro_renderSound(short* samples, int sampleCount)
{
   if ((sampleCount < 1) || !libretro_runloop_active)
      return;
#if 0
   for (int i=0; i<sampleCount; i+=2)
      retro_audio_cb(samples[i], samples[i+1]);
#else
   retro_audio_batch_cb(samples, sampleCount/2);
#endif
}

void retro_flush_screen (struct vidbuf_description *gfxinfo, int ystart, int yend)
{
   /* These values must be cached here, since the
    * source variables will be reset before the frame
    * ends and control is returned to the frontend */
   retro_thisframe_first_drawn_line = thisframe_first_drawn_line;
   retro_thisframe_last_drawn_line  = thisframe_last_drawn_line;
   retro_min_diwstart               = min_diwstart;
   retro_max_diwstop                = max_diwstop;

   /* Flag that we should end the frame, return out of retro_run */
   libretro_frame_end = 1;
}

void retro_flush_block (struct vidbuf_description *gfxinfo, int ystart, int yend)
{
}

void retro_flush_line (struct vidbuf_description *gfxinfo, int y)
{
}

void retro_flush_clear_screen(struct vidbuf_description *gfxinfo)
{
}

int retro_lockscr(struct vidbuf_description *gfxinfo)
{
   return 1;
}

void retro_unlockscr(struct vidbuf_description *gfxinfo)
{
}



int graphics_init(void)
{
   if (pixbuf != NULL)
      return 1;

   currprefs.gfx_size_win.width = defaultw;
   currprefs.gfx_size_win.height = defaulth;

   pixbuf = (unsigned short int*) &retro_bmp[0];
   if (pixbuf == NULL)
   {
      printf("Error: not enough memory to initialize screen buffer!\n");
      return -1;
   }

   gfxvidinfo.width_allocated = currprefs.gfx_size_win.width;
   gfxvidinfo.height_allocated = currprefs.gfx_size_win.height;
   gfxvidinfo.maxblocklines = 1000;
   gfxvidinfo.pixbytes = pix_bytes;
   gfxvidinfo.rowbytes = gfxvidinfo.width_allocated * gfxvidinfo.pixbytes;
   gfxvidinfo.bufmem = (unsigned char*)pixbuf;
   gfxvidinfo.emergmem = 0;
   gfxvidinfo.linemem = 0;

   gfxvidinfo.lockscr = retro_lockscr;
   gfxvidinfo.unlockscr = retro_unlockscr;
   gfxvidinfo.flush_block = retro_flush_block;
   gfxvidinfo.flush_clear_screen = retro_flush_clear_screen;
   gfxvidinfo.flush_screen = retro_flush_screen;
   gfxvidinfo.flush_line = retro_flush_line;

   prefs_changed = 1;
   inputdevice_release_all_keys ();
#if 0
   reset_hotkeys ();
#endif
   reset_drawing ();
   return 1;
}

int is_fullscreen (void)
{
   return 1;
}

int is_vsync (void)
{
   return 0;
}

int mousehack_allowed (void)
{
   return 0;
}

int debuggable (void)
{
   return 0;
}

int graphics_setup(void)
{
   /* 32bit mode
    *                   Rw,Gw,Bw,Rs, Gs,Bs,Aw,As,Avalue,swap */
   if (pix_bytes == 2)
      alloc_colors64k (5, 6, 5, 11, 5, 0, 0, 0, 0, 0);
   else
      alloc_colors64k (8, 8, 8, 16, 8, 0, 0, 0, 0, 0);

   return 1;
}

void graphics_leave(void)
{
}

void graphics_notify_state (int state)
{
}

void gfx_save_options (FILE * f, const struct uae_prefs * p)
{
}

int gfx_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
   return 0;
}

void gfx_default_options(struct uae_prefs *p)
{
}

void screenshot (int type, int f)
{
}

void toggle_fullscreen(int mode)
{
}

int check_prefs_changed_gfx (void)
{
   if (prefs_changed)
      prefs_changed = 0;
   else
      return 0;

   changed_prefs.gfx_size_win.width    = defaultw;
   changed_prefs.gfx_size_win.height   = defaulth;

   if (currprefs.gfx_size_win.width   != changed_prefs.gfx_size_win.width)
       currprefs.gfx_size_win.width    = changed_prefs.gfx_size_win.width;
   if (currprefs.gfx_size_win.height  != changed_prefs.gfx_size_win.height)
       currprefs.gfx_size_win.height   = changed_prefs.gfx_size_win.height;
   if (currprefs.gfx_resolution       != changed_prefs.gfx_resolution)
       currprefs.gfx_resolution        = changed_prefs.gfx_resolution;
   if (currprefs.gfx_vresolution      != changed_prefs.gfx_vresolution)
       currprefs.gfx_vresolution       = changed_prefs.gfx_vresolution;

   if (currprefs.gfx_scandoubler      != changed_prefs.gfx_scandoubler)
       currprefs.gfx_scandoubler       = changed_prefs.gfx_scandoubler;

   gfxvidinfo.width_allocated          = currprefs.gfx_size_win.width;
   gfxvidinfo.height_allocated         = currprefs.gfx_size_win.height;
   gfxvidinfo.rowbytes                 = gfxvidinfo.width_allocated * gfxvidinfo.pixbytes;

#if 0
   printf("check_prefs_changed_gfx: %d:%d, res:%d vres:%d\n", changed_prefs.gfx_size_win.width, changed_prefs.gfx_size_win.height, changed_prefs.gfx_resolution, changed_prefs.gfx_vresolution);
#endif
   return 1;
}


/***************************************************************
  Joystick functions
****************************************************************/

static int init_joysticks (void)
{
   return 1;
}

static void close_joysticks (void)
{
}

static int acquire_joystick (int num, int flags)
{
   return 1;
}

static int get_joystick_flags (int num)
{
   return 0;
}
static void unacquire_joystick (int num)
{
}

static void read_joysticks (void)
{
}

static int get_joystick_num (void)
{
   return 4;
}

static TCHAR *get_joystick_friendlyname (int joy)
{
   switch (joy)
   {
      case 0:
         return "RetroPad0";
         break;
      default:
      case 1:
         return "RetroPad1";
         break;
      case 2:
         return "RetroPad2";
         break;
      case 3:
         return "RetroPad3";
         break;
   }
}

static char *get_joystick_uniquename (int joy)
{
   switch (joy)
   {
      case 0:
         return "RetroPad0";
         break;
      default:
      case 1:
         return "RetroPad1";
         break;
      case 2:
         return "RetroPad2";
         break;
      case 3:
         return "RetroPad3";
         break;
   }
}

static int get_joystick_widget_num (int joy)
{
   return 0;
}

static int get_joystick_widget_type (int joy, int num, char *name, uae_u32 *code)
{
   return IDEV_WIDGET_BUTTON;
}

static int get_joystick_widget_first (int joy, int type)
{
   return -1;
}

struct inputdevice_functions inputdevicefunc_joystick = {
   init_joysticks,
   close_joysticks,
   acquire_joystick,
   unacquire_joystick,
   read_joysticks,
   get_joystick_num,
   get_joystick_friendlyname,
   get_joystick_uniquename,
   get_joystick_widget_num,
   get_joystick_widget_type,
   get_joystick_widget_first,
   get_joystick_flags
};

int input_get_default_joystick (struct uae_input_device *uid, int num, int port, int af, int mode, bool gp)
{
   if (retro_devices[0] == RETRO_DEVICE_CD32PAD)
   {
      uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY2_HORIZ;
      uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY2_VERT;
      uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY2_CD32_RED;
      uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY2_CD32_BLUE;
      uid[0].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY2_CD32_GREEN;
      uid[0].eventid[ID_BUTTON_OFFSET + 3][0] = INPUTEVENT_JOY2_CD32_YELLOW;
      uid[0].eventid[ID_BUTTON_OFFSET + 4][0] = INPUTEVENT_JOY2_CD32_RWD;
      uid[0].eventid[ID_BUTTON_OFFSET + 5][0] = INPUTEVENT_JOY2_CD32_FFW;
      uid[0].eventid[ID_BUTTON_OFFSET + 6][0] = INPUTEVENT_JOY2_CD32_PLAY;
   }
   else if (retro_devices[0] == RETRO_DEVICE_ANALOGJOYSTICK)
   {
      uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY2_HORIZ_POT;
      uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY2_VERT_POT;
      uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY2_LEFT;
      uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY2_RIGHT;
      uid[0].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY2_UP;
      uid[0].eventid[ID_BUTTON_OFFSET + 3][0] = INPUTEVENT_JOY2_DOWN;
   }
   else
   {
      uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY2_HORIZ;
      uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY2_VERT;
      uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY2_FIRE_BUTTON;
      uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY2_2ND_BUTTON;
   }

   if (retro_devices[1] == RETRO_DEVICE_CD32PAD)
   {
      uid[1].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY1_HORIZ;
      uid[1].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY1_VERT;
      uid[1].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_CD32_RED;
      uid[1].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_CD32_BLUE;
      uid[1].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY1_CD32_GREEN;
      uid[1].eventid[ID_BUTTON_OFFSET + 3][0] = INPUTEVENT_JOY1_CD32_YELLOW;
      uid[1].eventid[ID_BUTTON_OFFSET + 4][0] = INPUTEVENT_JOY1_CD32_RWD;
      uid[1].eventid[ID_BUTTON_OFFSET + 5][0] = INPUTEVENT_JOY1_CD32_FFW;
      uid[1].eventid[ID_BUTTON_OFFSET + 6][0] = INPUTEVENT_JOY1_CD32_PLAY;
   }
   else if (retro_devices[1] == RETRO_DEVICE_ANALOGJOYSTICK)
   {
      uid[1].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY1_HORIZ_POT;
      uid[1].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY1_VERT_POT;
      uid[1].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_LEFT;
      uid[1].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_RIGHT;
      uid[1].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY1_UP;
      uid[1].eventid[ID_BUTTON_OFFSET + 3][0] = INPUTEVENT_JOY1_DOWN;
   }
   else
   {
      uid[1].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY1_HORIZ;
      uid[1].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY1_VERT;
      uid[1].eventid[ID_AXIS_OFFSET + 2][0]   = INPUTEVENT_JOY1_HORIZ_POT;
      uid[1].eventid[ID_AXIS_OFFSET + 3][0]   = INPUTEVENT_JOY1_VERT_POT;
      uid[1].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_FIRE_BUTTON;
      uid[1].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_2ND_BUTTON;
   }

   uid[2].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_PAR_JOY1_HORIZ;
   uid[2].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_PAR_JOY1_VERT;
   uid[2].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_PAR_JOY1_FIRE_BUTTON;
   uid[2].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_PAR_JOY1_2ND_BUTTON;

   uid[3].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_PAR_JOY2_HORIZ;
   uid[3].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_PAR_JOY2_VERT;
   uid[3].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_PAR_JOY2_FIRE_BUTTON;
   uid[3].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_PAR_JOY2_2ND_BUTTON;

   uid[0].enabled = 1;
   uid[1].enabled = 1;
   uid[2].enabled = 1;
   uid[3].enabled = 1;

   inputdevice_finalized = true;

   currprefs.input_analog_joystick_mult = 100;
   currprefs.input_analog_joystick_offset = 0;
   return 1;
}

int input_get_default_joystick_analog (struct uae_input_device *uid, int num, int port, int af, bool gp)
{
   uid[num].eventid[ID_AXIS_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_HORIZ_POT : INPUTEVENT_JOY1_HORIZ_POT;
   uid[num].eventid[ID_AXIS_OFFSET + 1][0] = port ? INPUTEVENT_JOY2_VERT_POT : INPUTEVENT_JOY1_VERT_POT;
   uid[num].eventid[ID_BUTTON_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_LEFT : INPUTEVENT_JOY1_LEFT;
   uid[num].eventid[ID_BUTTON_OFFSET + 1][0] = port ? INPUTEVENT_JOY2_RIGHT : INPUTEVENT_JOY1_RIGHT;
   uid[num].eventid[ID_BUTTON_OFFSET + 2][0] = port ? INPUTEVENT_JOY2_UP : INPUTEVENT_JOY1_UP;
   uid[num].eventid[ID_BUTTON_OFFSET + 3][0] = port ? INPUTEVENT_JOY2_DOWN : INPUTEVENT_JOY1_DOWN;
   return 0;
}



/***************************************************************
  Mouse functions
****************************************************************/
/*
 * Mouse inputdevice functions
 */

/* Hardwire for 3 axes and 3 buttons
 * There is no 3rd axis as such - mousewheel events are
 * supplied by X on buttons 4 and 5.
 */
#define MAX_BUTTONS     3
#define MAX_AXES        2
#define FIRST_AXIS      0
#define FIRST_BUTTON    MAX_AXES

static int init_mouse (void)
{
   return 1;
}

static void close_mouse (void)
{
   return;
}

static int acquire_mouse (int num, int flags)
{
   return 1;
}

static void unacquire_mouse (int num)
{
   return;
}

static int get_mouse_num (void)
{
   return 1;
}

static TCHAR *get_mouse_friendlyname (int mouse)
{
   return "RetroMouse";
}

static TCHAR *get_mouse_uniquename (int mouse)
{
   return "RetroMouse";
}

static int get_mouse_widget_num (int mouse)
{
   return MAX_AXES + MAX_BUTTONS;
}

static int get_mouse_widget_first (int mouse, int type)
{
   switch (type)
   {
      case IDEV_WIDGET_BUTTON:
         return FIRST_BUTTON;
      case IDEV_WIDGET_AXIS:
         return FIRST_AXIS;
   }
   return -1;
}

static int get_mouse_widget_type (int mouse, int num, TCHAR *name, uae_u32 *code)
{
   if (num >= MAX_AXES && num < MAX_AXES + MAX_BUTTONS)
   {
      if (name)
         sprintf (name, "Button %d", num + 1 + MAX_AXES);
      return IDEV_WIDGET_BUTTON;
   }
   else if (num < MAX_AXES)
   {
      if (name)
         sprintf (name, "Axis %d", num + 1);
      return IDEV_WIDGET_AXIS;
   }
   return IDEV_WIDGET_NONE;
}

static void read_mouse (void)
{
   /* We handle mouse input in handle_events() */
}

static int get_mouse_flags (int num)
{
   return 0;
}

struct inputdevice_functions inputdevicefunc_mouse = {
   init_mouse,
   close_mouse,
   acquire_mouse,
   unacquire_mouse,
   read_mouse,
   get_mouse_num,
   get_mouse_friendlyname,
   get_mouse_uniquename,
   get_mouse_widget_num,
   get_mouse_widget_type,
   get_mouse_widget_first,
   get_mouse_flags
};

int input_get_default_mouse (struct uae_input_device *uid, int num, int port, int af, bool gp, bool wheel)
{
   uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_MOUSE1_HORIZ;
   uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_MOUSE1_VERT;
   uid[0].eventid[ID_AXIS_OFFSET + 2][0]   = INPUTEVENT_MOUSE1_WHEEL;
   uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_FIRE_BUTTON;
   uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_2ND_BUTTON;
   uid[0].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY1_3RD_BUTTON;

   uid[1].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_MOUSE2_HORIZ;
   uid[1].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_MOUSE2_VERT;
   uid[1].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY2_FIRE_BUTTON;
   uid[1].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY2_2ND_BUTTON;
   uid[1].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY2_3RD_BUTTON;

   uid[0].enabled = 1;
   uid[1].enabled = 1;
   return 0;
}

int input_get_default_lightpen (struct uae_input_device *uid, int num, int port, int af, bool gp)
{
   uid[num].eventid[ID_AXIS_OFFSET + 0][0] = INPUTEVENT_LIGHTPEN_HORIZ;
   uid[num].eventid[ID_AXIS_OFFSET + 1][0] = INPUTEVENT_LIGHTPEN_VERT;
   uid[num].eventid[ID_BUTTON_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_3RD_BUTTON : INPUTEVENT_JOY1_3RD_BUTTON;
   return 0;
}


/***************************************************************
  Keyboard functions
****************************************************************/

static int init_kb (void)
{
   return 1;
}

static void close_kb (void)
{
}

static int acquire_kb (int num, int flags)
{
   return 1;
}

static void unacquire_kb (int num)
{
}

static void read_kb (void)
{
}

static int get_kb_num (void)
{
   return 1;
}

static char *get_kb_friendlyname (int mouse)
{
   return "RetroKeyboard";
}

static char *get_kb_uniquename (int mouse)
{
   return "RetroKeyboard";
}

static int get_kb_widget_num (int mouse)
{
   return 255;
}

static int get_kb_widget_type (int mouse, int num, char *name, uae_u32 *code)
{
   if (code)
      *code = ~0;
   return IDEV_WIDGET_NONE;
}

static int get_kb_widget_first (int mouse, int type)
{
   return 0;
}

static int get_kb_flags (int num)
{
   return 0;
}

struct inputdevice_functions inputdevicefunc_keyboard = {
   init_kb,
   close_kb,
   acquire_kb,
   unacquire_kb,
   read_kb,
   get_kb_num,
   get_kb_friendlyname,
   get_kb_uniquename,
   get_kb_widget_num,
   get_kb_widget_type,
   get_kb_widget_first,
   get_kb_flags
};

int getcapslockstate (void)
{
   return 0;
}

void setcapslockstate (int state)
{
}

#if 0
static struct uae_input_device_kbr_default keytrans_amiga[] = {
   { -1, {{0}} }
};

static struct uae_input_device_kbr_default keytrans_pc1[] = {
   { -1, {{0, 0}} }
};

static struct uae_input_device_kbr_default *keytrans[] = {
   keytrans_amiga,
   keytrans_pc1,
   keytrans_pc1
};

static int *kbmaps[] = {
};

void clearallkeys (void)
{
   inputdevice_updateconfig (&changed_prefs, &currprefs);
}
#endif

void keyboard_settrans (void)
{
#if 0
   inputdevice_setkeytranslation (keytrans, kbmaps);
#endif
}


/********************************************************************
    Misc fuctions
*********************************************************************/

int needmousehack(void)
{
   return 0;
}

void toggle_mousegrab(void)
{
}

/* handle pads in the "options" dialog */
int handle_options_events()
{
   return 0;
}

bool handle_events()
{
   return 0;
}

void uae_pause (void)
{
}

void uae_resume (void)
{
}



/* Libretro misc */
int sensible_strcmp(char *a, char *b)
{
   int i;
   for (i = 0; a[i] == b[i]; i++)
      if (a[i] == '\0')
         return 0;
   /* Replace " " (32) with "/" (47) when comparing for more natural sorting, such as:
    * 1. Turrican
    * 2. Turrican II
    * 3. Turrican III
    * Because "/" (47) is bigger than "," (44) and "." (46), and it is not used in filenames */
   if (a[i] == 32)
      return (47 < (unsigned char)b[i]) ? -1 : 1;
   if (b[i] == 32)
      return ((unsigned char)a[i] < 47) ? -1 : 1;
   return ((unsigned char)a[i] < (unsigned char)b[i]) ? -1 : 1;
}

int qstrcmp(const void *a, const void *b)
{
   char *pa = (char *)a;
   char *pb = (char *)b;
   return sensible_strcmp(pa, pb);
}

void remove_recurse(const char *path)
{
   struct dirent *dirp;
   char filename[RETRO_PATH_MAX];
   DIR *dir = opendir(path);
   if (dir == NULL)
      return;

   while ((dirp = readdir(dir)) != NULL)
   {
      if (dirp->d_name[0] == '.')
         continue;

      sprintf(filename, "%s%s%s", path, DIR_SEP_STR, dirp->d_name);
      fprintf(stdout, "Clean: %s\n", filename);

      if (path_is_directory(filename))
         remove_recurse(filename);
      else
         remove(filename);
   }

   closedir(dir);
   rmdir(path);
}

void path_join(char* out, const char* basedir, const char* filename)
{
   snprintf(out, RETRO_PATH_MAX, "%s%s%s", basedir, RETRO_PATH_SEPARATOR, filename);
}

/* Note: This function returns a pointer to a substring_left of the original string.
 * If the given string was allocated dynamically, the caller must not overwrite
 * that pointer with the returned value, since the original pointer must be
 * deallocated using the same allocator with which it was allocated.  The return
 * value must NOT be deallocated using free() etc. */
char* trimwhitespace(char *str)
{
   char *end;

   /* Trim leading space */
   while (isspace((unsigned char)*str)) str++;

   if (*str == 0) /* All spaces? */
      return str;

   /* Trim trailing space */
   end = str + strlen(str) - 1;
   while (end > str && isspace((unsigned char)*end)) end--;

   /* Write new null terminator character */
   end[1] = '\0';

   return str;
}

/* Returns a substring of 'str' that contains the 'len' leftmost characters of 'str' */
char* strleft(const char* str, int len)
{
   char* result = calloc(len + 1, sizeof(char));
   strncpy(result, str, len);
   return result;
}

/* Returns a substring of 'str' that contains the 'len' rightmost characters of 'str' */
char* strright(const char* str, int len)
{
   int pos = strlen(str) - len;
   char* result = calloc(len + 1, sizeof(char));
   strncpy(result, str + pos, len);
   return result;
}

/* Returns true if 'str' starts with 'start' */
bool strstartswith(const char* str, const char* start)
{
   if (strlen(str) >= strlen(start))
      if(!strncasecmp(str, start, strlen(start)))
         return true;

   return false;
}

/* Returns true if 'str' ends with 'end' */
bool strendswith(const char* str, const char* end)
{
   if (strlen(str) >= strlen(end))
      if(!strcasecmp((char*)&str[strlen(str)-strlen(end)], end))
         return true;

   return false;
}

/* zlib */
#include "deps/zlib/zlib.h"
void gz_uncompress(gzFile in, FILE *out)
{
   char gzbuf[16384];
   int len;
   int err;

   for (;;)
   {
      len = gzread(in, gzbuf, sizeof(gzbuf));
      if (len < 0)
         fprintf(stderr, gzerror(in, &err));
      if (len == 0)
         break;
      if ((int)fwrite(gzbuf, 1, (unsigned)len, out) != len)
         fprintf(stderr, "Write error!\n");
   }
}

#include "deps/zlib/unzip.h"
#include "file/file_path.h"
void zip_uncompress(char *in, char *out, char *lastfile)
{
   unzFile uf = NULL;
   uf = unzOpen(in);

   uLong i;
   unz_global_info gi;
   int err;
   err = unzGetGlobalInfo (uf, &gi);

   const char* password = NULL;
   int size_buf = 8192;

   for (i = 0; i < gi.number_entry; i++)
   {
      char filename_inzip[256];
      char filename_withpath[512];
      filename_inzip[0] = '\0';
      filename_withpath[0] = '\0';
      char* filename_withoutpath;
      char* p;
      unz_file_info file_info;
      FILE *fout = NULL;
      void* buf;

      buf = (void*)malloc(size_buf);
      if (buf == NULL)
      {
         fprintf(stderr, "Unzip: Error allocating memory\n");
         return;
      }

      err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
      snprintf(filename_withpath, sizeof(filename_withpath), "%s%s%s", out, DIR_SEP_STR, filename_inzip);
      if (dc_get_image_type(filename_inzip) == DC_IMAGE_TYPE_FLOPPY && lastfile != NULL)
         snprintf(lastfile, RETRO_PATH_MAX, "%s", filename_inzip);

      p = filename_withoutpath = filename_inzip;
      while ((*p) != '\0')
      {
         if (((*p) == '/') || ((*p) == '\\'))
            filename_withoutpath = p+1;
         p++;
      }

      if ((*filename_withoutpath) == '\0')
      {
         fprintf(stdout, "Mkdir: %s\n", filename_withpath);
         path_mkdir(filename_withpath);
      }
      else if (!path_is_valid(filename_withpath))
      {
         const char* write_filename;
         int skip = 0;

         write_filename = filename_withpath;

         err = unzOpenCurrentFilePassword(uf, password);
         if (err != UNZ_OK)
         {
            fprintf(stderr, "Unzip: Error %d with zipfile in unzOpenCurrentFilePassword: %s\n", err, write_filename);
         }

         if ((skip == 0) && (err == UNZ_OK))
         {
            fout = fopen(write_filename, "wb");
            if (fout == NULL)
            {
               fprintf(stderr, "Unzip: Error opening %s\n", write_filename);
            }
         }

         if (fout != NULL)
         {
            fprintf(stdout, "Unzip: %s\n", write_filename);

            do
            {
               err = unzReadCurrentFile(uf, buf, size_buf);
               if (err < 0)
               {
                  fprintf(stderr, "Unzip: Error %d with zipfile in unzReadCurrentFile\n", err);
                  break;
               }
               if (err > 0)
               {
                  if (!fwrite(buf, err, 1, fout))
                  {
                     fprintf(stderr, "Unzip: Error in writing extracted file\n");
                     err = UNZ_ERRNO;
                     break;
                  }
               }
            }
            while (err > 0);
            if (fout)
               fclose(fout);
         }

         if (err == UNZ_OK)
         {
            err = unzCloseCurrentFile(uf);
            if (err != UNZ_OK)
            {
               fprintf(stderr, "Unzip: Error %d with zipfile in unzCloseCurrentFile\n", err);
            }
         }
         else
            unzCloseCurrentFile(uf);
      }

      free(buf);

      if ((i+1) < gi.number_entry)
      {
         err = unzGoToNextFile(uf);
         if (err != UNZ_OK)
         {
            fprintf(stderr, "Unzip: Error %d with zipfile in unzGoToNextFile\n", err);
            break;
         }
      }
   }

   if (uf)
   {
      unzCloseCurrentFile(uf);
      unzClose(uf);
      uf = NULL;
   }
}

/* HDF tools */
static int create_hdf (const char *path, off_t size)
{
    FILE *f;
    void *buf;
    const size_t CHUNK_SIZE = 4096;

    if (size == 0)
        return 0;

    f = fopen (path, "wb+");
    if (f) {
        /*
         * Try it the easy way.
         */
        if (fseeko (f, size - 1, SEEK_SET) == 0) {
            fputc (0, f);
            if (fseeko (f, 0, SEEK_SET) == 0) {
                fclose (f);
                return 0;
            }
        }

        /*
         * Okay. That failed. Let's assume seeking passed
         * the end of a file ain't supported. Do it the
         * hard way.
         */
        fseeko (f, 0, SEEK_SET);
        buf = calloc (1, CHUNK_SIZE);

        while (size >= (off_t) CHUNK_SIZE) {
            if (fwrite (buf, CHUNK_SIZE, 1, f) != 1)
                break;
            size -= CHUNK_SIZE;
        }

        if (size < (off_t) CHUNK_SIZE) {
            if (size == 0 || fwrite (buf, (size_t)size, 1, f) == 1) {
                fclose (f);
                return 0;
            }
        }
    }

    if (f) {
        fclose (f);
    }

    return -1;
}

int make_hdf (char *hdf_path, char *hdf_size, char *device_name)
{
    uae_u64 size;
    char *size_spec;

    uae_u32 block_size = 512;
    uae_u64 num_blocks;

    uae_u32 cylinders;
    uae_u32 blocks_per_track;
    uae_u32 surfaces;

    size = strtoll(hdf_size, &size_spec, 10);

    /* Munge size specifier */
    if (size > 0) {
        char c = (toupper(*size_spec));

        if (c == 'K')
            size *= 1024;
        else if (c == 'M' || c == '\0')
            size *= 1024 * 1024;
        else if (c == 'G')
            size *= 1024 * 1024 * 1024;
        else
            size = 0;
    }

    if (size <= 0) {
        fprintf (stderr, "Invalid size\n");
        exit (EXIT_FAILURE);
    }

    if ((size >= (1LL << 31)) && (sizeof (off_t) < sizeof (uae_u64))) {
        fprintf (stderr, "Specified size too large (2GB file size is maximum).\n");
        exit (EXIT_FAILURE);
    }

    num_blocks = size / block_size;

    /* We don't want more than (2^32)-1 blocks */
    if (num_blocks >= (1LL << 32)) {
        fprintf (stderr, "Specified size too large (too many blocks).\n");
        exit (EXIT_FAILURE);
    }

    /*
     * Try and work out some plausible geometry
     *
     * We try and set surfaces and blocks_per_track to keep
     * cylinders < 65536. Prior to OS 3.9, FFS had problems with
     * more cylinders than that.
     */

    /* The default practice in UAE hardfiles, so let's start there. */
    blocks_per_track = 32;
    surfaces = 1;

    cylinders = num_blocks / (blocks_per_track * surfaces);

    if (cylinders == 0) {
        fprintf (stderr, "Specified size is too small.\n");
        exit (EXIT_FAILURE);
    }

    while (cylinders > 65535 && surfaces < 255) {
        surfaces++;
        cylinders = num_blocks / (blocks_per_track * surfaces);
    }

    while (cylinders > 65535 && blocks_per_track < 255) {
        blocks_per_track++;
        cylinders = num_blocks / (blocks_per_track * surfaces);
    }

    /* Calculate size based on above geometry */
    num_blocks = (uae_u64)cylinders * surfaces * blocks_per_track;

    /* make file */
    if (create_hdf (hdf_path, num_blocks * block_size) < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
