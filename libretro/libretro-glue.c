#include "sysconfig.h"
#include "sysdeps.h"

#ifdef __CELLOS_LV2__
#include <ctype.h>
#endif

#include "options.h"
#include "uae.h"
#include "memory_uae.h"
#include "xwin.h"
#include "custom.h"
#include "drawing.h"
#include "hotkeys.h"
#include "hrtimer.h"

#include "inputdevice.h"
void inputdevice_release_all_keys(void);
extern int mouse_port[NORMAL_JPORTS];

#include "libretro-glue.h"
#include "libretro-core.h"
#include "libretro-mapper.h"
#include "encodings/utf.h"
#include "streams/file_stream.h"

extern unsigned int retro_devices[RETRO_DEVICES];
bool inputdevice_finalized = false;

extern int defaultw;
extern int defaulth;
extern int libretro_runloop_active;
extern int libretro_frame_end;

unsigned short int* pixbuf = NULL;
extern unsigned short int retro_bmp[RETRO_BMP_SIZE];
extern char retro_temp_directory[RETRO_PATH_MAX];
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
   if (retro_devices[0] == RETRO_DEVICE_PUAE_CD32PAD)
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
   else if (retro_devices[0] == RETRO_DEVICE_PUAE_ANALOG)
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

   if (retro_devices[1] == RETRO_DEVICE_PUAE_CD32PAD)
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
   else if (retro_devices[1] == RETRO_DEVICE_PUAE_ANALOG)
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

   /* Leave the root directory for RAM disk usage */
   if (strcmp(retro_temp_directory, path))
      rmdir(path);
}

int fcopy(const char *src, const char *dst)
{
   char buf[BUFSIZ] = {0};
   size_t n         = 0;
   int ret          = 0;

   char path_dst[RETRO_PATH_MAX] = {0};
   snprintf(path_dst, sizeof(path_dst), "%s", dst);
   path_basedir(path_dst);

   if (!path_is_directory(path_dst))
   {
      printf("Mkdir: %s\n", path_dst);
      path_mkdir(path_dst);
   }

   FILE *fp_src = fopen(src, "rb");
   FILE *fp_dst = fopen(dst, "wb");
   if (!fp_src)
      ret = -1;
   if (!fp_dst)
      ret = -2;

   if (ret < 0)
      goto close;

   while ((n = fread(buf, sizeof(char), sizeof(buf), fp_src)) > 0 && ret == 0)
   {
      if (fwrite(buf, sizeof(char), n, fp_dst) != n)
         ret = -1;
   }

close:
   if (fp_src)
      fclose(fp_src);
   if (fp_dst);
      fclose(fp_dst);

#if 0
   if (ret == 0)
      printf("fcopy: %s -> %s = %d\n", src, dst, ret);
#endif
   return ret;
}

int fcmp(const char *src, const char *dst)
{
   char buf_src[BUFSIZ] = {0};
   char buf_dst[BUFSIZ] = {0};
   size_t n             = 0;
   int ret              = 0;

   FILE *fp_src = fopen(src, "rb");
   FILE *fp_dst = fopen(dst, "rb");
   if (!fp_src)
      ret = -1;
   if (!fp_dst)
      ret = -2;

   if (ret < 0)
      goto close;

   while ((n = fread(buf_src, sizeof(char), sizeof(buf_src), fp_src)) > 0 && ret == 0)
   {
      fread(buf_dst, sizeof(char), sizeof(buf_dst), fp_dst);
      ret = memcmp(buf_src, buf_dst, sizeof(buf_src));
   }

close:
   if (fp_src)
      fclose(fp_src);
   if (fp_dst);
      fclose(fp_dst);

#if 0
   printf("fcmp: %s -> %s = %d\n", src, dst, ret);
#endif
   return ret;
}

void path_join(char* out, const char* basedir, const char* filename)
{
   snprintf(out, RETRO_PATH_MAX, "%s%s%s", basedir, RETRO_PATH_SEPARATOR, filename);
}

char* path_join_dup(const char* basedir, const char* filename)
{
    size_t dirlen = strlen(basedir);
    size_t seplen = strlen(RETRO_PATH_SEPARATOR);
    size_t filelen = strlen(filename);
    char* result = (char*)malloc(dirlen + seplen + filelen + 1);
    strcpy(result, basedir);
    strcpy(result + dirlen, RETRO_PATH_SEPARATOR);
    strcpy(result + dirlen + seplen, filename);
    return result;
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
void gz_uncompress(gzFile in, FILE *out)
{
   char gzbuf[16384];
   int len;
   int err;

   for (;;)
   {
      len = gzread(in, gzbuf, sizeof(gzbuf));
      if (len < 0)
         fprintf(stdout, "%s", gzerror(in, &err));
      if (len == 0)
         break;
      if ((int)fwrite(gzbuf, 1, (unsigned)len, out) != len)
         fprintf(stdout, "Write error!\n");
   }
}

void zip_uncompress(char *in, char *out, char *lastfile)
{
   unzFile uf = NULL;
   char *in_local = NULL;
   in_local = utf8_to_local_string_alloc(in);

   uf = unzOpen(in_local);

   free(in_local);
   in_local = NULL;

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
      if (lastfile != NULL &&
            (dc_get_image_type(filename_inzip) == DC_IMAGE_TYPE_FLOPPY ||
             dc_get_image_type(filename_inzip) == DC_IMAGE_TYPE_CD))
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

/* 7zip */
#ifdef HAVE_7ZIP
struct sevenzip_context_t
{
   uint8_t *output;
   CFileInStream archiveStream;
   CLookToRead lookStream;
   ISzAlloc allocImp;
   ISzAlloc allocTempImp;
   CSzArEx db;
   size_t temp_size;
   uint32_t block_index;
   uint32_t parse_index;
   uint32_t decompress_index;
   uint32_t packIndex;
};

static void *sevenzip_stream_alloc_impl(void *p, size_t size)
{
   if (size == 0)
      return 0;
   return malloc(size);
}

static void sevenzip_stream_free_impl(void *p, void *address)
{
   (void)p;

   if (address)
      free(address);
}

static void *sevenzip_stream_alloc_tmp_impl(void *p, size_t size)
{
   (void)p;
   if (size == 0)
      return 0;
   return malloc(size);
}

void sevenzip_uncompress(char *in, char *out, char *lastfile)
{
   CFileInStream archiveStream;
   CLookToRead lookStream;
   ISzAlloc allocImp;
   ISzAlloc allocTempImp;
   CSzArEx db;
   uint8_t *output      = 0;
   int64_t outsize      = -1;

   /*These are the allocation routines.
    * Currently using the non-standard 7zip choices. */
   allocImp.Alloc       = sevenzip_stream_alloc_impl;
   allocImp.Free        = sevenzip_stream_free_impl;
   allocTempImp.Alloc   = sevenzip_stream_alloc_tmp_impl;
   allocTempImp.Free    = sevenzip_stream_free_impl;

   /* Could not open 7zip archive? */
   if (InFile_Open(&archiveStream.file, in))
      return;

   FileInStream_CreateVTable(&archiveStream);
   LookToRead_CreateVTable(&lookStream, false);
   lookStream.realStream = &archiveStream.s;
   LookToRead_Init(&lookStream);
   CrcGenerateTable();

   db.db.PackSizes               = NULL;
   db.db.PackCRCsDefined         = NULL;
   db.db.PackCRCs                = NULL;
   db.db.Folders                 = NULL;
   db.db.Files                   = NULL;
   db.db.NumPackStreams          = 0;
   db.db.NumFolders              = 0;
   db.db.NumFiles                = 0;
   db.startPosAfterHeader        = 0;
   db.dataPos                    = 0;
   db.FolderStartPackStreamIndex = NULL;
   db.PackStreamStartPositions   = NULL;
   db.FolderStartFileIndex       = NULL;
   db.FileIndexToFolderIndexMap  = NULL;
   db.FileNameOffsets            = NULL;
   db.FileNames.data             = NULL;
   db.FileNames.size             = 0;

   SzArEx_Init(&db);

   if (SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp) == SZ_OK)
   {
      uint32_t i;
      uint16_t *temp       = NULL;
      size_t temp_size     = 0;
      uint32_t block_index = 0xFFFFFFFF;
      SRes res             = SZ_OK;
      size_t output_size   = 0;

      for (i = 0; i < db.db.NumFiles; i++)
      {
         size_t j;
         size_t len;
         char infile[RETRO_PATH_MAX];
         size_t offset                = 0;
         size_t outSizeProcessed      = 0;
         const CSzFileItem    *f      = db.db.Files + i;

         len = SzArEx_GetFileNameUtf16(&db, i, NULL);

         if (len > temp_size)
         {
            if (temp)
               free(temp);
            temp_size = len;
            temp = (uint16_t *)malloc(temp_size * sizeof(temp[0]));

            if (temp == 0)
            {
               res = SZ_ERROR_MEM;
               break;
            }
         }

         SzArEx_GetFileNameUtf16(&db, i, temp);
         res       = SZ_ERROR_FAIL;
         infile[0] = '\0';

         if (!temp)
            break;

         res = utf16_to_char_string(temp, infile, sizeof(infile))
               ? SZ_OK : SZ_ERROR_FAIL;

         /* C LZMA SDK does not support chunked extraction - see here:
          * sourceforge.net/p/sevenzip/discussion/45798/thread/6fb59aaf/
          * */
         res = SzArEx_Extract(&db, &lookStream.s, i, &block_index,
               &output, &output_size, &offset, &outSizeProcessed,
               &allocImp, &allocTempImp);

         if (res != SZ_OK)
            break; /* This goes to the error section. */

         outsize = (int64_t)outSizeProcessed;

         char full_path[RETRO_PATH_MAX] = {0};
         snprintf(full_path, RETRO_PATH_MAX, "%s%s%s", out, DIR_SEP_STR, infile);
         if (dc_get_image_type(full_path) == DC_IMAGE_TYPE_FLOPPY && lastfile != NULL)
            snprintf(lastfile, RETRO_PATH_MAX, "%s", path_basename(full_path));

         for (j = 0; full_path[j] != 0; j++)
         {
            if (full_path[j] == '/')
            {
                  full_path[j] = 0;
                  path_mkdir((const char *)full_path);
                  full_path[j] = DIR_SEP_CHR;
            }
         }

         const void *ptr = (const void*)(output + offset);

         if (path_is_valid(full_path))
            continue;
         else if (f->IsDir)
         {
            path_mkdir((const char *)temp);
            fprintf(stdout, "Mkdir: %s\n", full_path);
            continue;
         }

         if (filestream_write_file(full_path, ptr, outsize))
         {
            res = SZ_OK;
            fprintf(stdout, "Un7ip: %s\n", full_path);
         }
         else
         {
            res = SZ_ERROR_FAIL;
            fprintf(stderr, "Un7ip: Error writing extracted file %s\n", full_path);
         }
      }

      if (temp)
         free(temp);
      IAlloc_Free(&allocImp, output);

      if (res == SZ_ERROR_UNSUPPORTED)
         fprintf(stderr, "Un7ip: Decoder doesn't support this archive\n");
      else if (res == SZ_ERROR_MEM)
         fprintf(stderr, "Un7ip: Can not allocate memory\n");
      else if (res == SZ_ERROR_CRC)
         fprintf(stderr, "Un7ip: CRC error\n");
   }

   SzArEx_Free(&db, &allocImp);
   File_Close(&archiveStream.file);
}
#else
void sevenzip_uncompress(char *in, char *out, char *lastfile)
{
}
#endif

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
        char c = (_totupper(*size_spec));

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

/* CHD */
#ifdef WITH_CHD

/***************************************************************************
    INLINE FUNCTIONS
***************************************************************************/

INLINE UINT32 msf_to_lba(UINT32 msf)
{
	return ( ((msf&0x00ff0000)>>16) * 60 * 75) + (((msf&0x0000ff00)>>8) * 75) + ((msf&0x000000ff)>>0);
}

UINT32 lba_to_msf(UINT32 lba)
{
	UINT8 m, s, f;

	m = lba / (60 * 75);
	lba -= m * (60 * 75);
	s = lba / 75;
	f = lba % 75;

	return ((m / 10) << 20) | ((m % 10) << 16) |
			((s / 10) << 12) | ((s % 10) <<  8) |
			((f / 10) <<  4) | ((f % 10) <<  0);
}

// segacd needs it like this.. investigate
// Angelo also says PCE tracks often start playing at the
// wrong address.. related?
INLINE UINT32 lba_to_msf_alt(int lba)
{
	UINT32 ret = 0;

	ret |= ((lba / (60 * 75))&0xff)<<16;
	ret |= (((lba / 75) % 60)&0xff)<<8;
	ret |= ((lba % 75)&0xff)<<0;

	return ret;
}

//-------------------------------------------------
//  be_read - extract a big-endian number from
//  a byte buffer
//-------------------------------------------------

UINT64 be_read(const UINT8 *base, int numbytes)
{
	UINT64 result = 0;
	while (numbytes--)
		result = (result << 8) | *base++;
	return result;
}

/*-------------------------------------------------
    get_bigendian_uint32 - fetch a UINT32 from
    the data stream in bigendian order
-------------------------------------------------*/

INLINE UINT32 get_bigendian_uint32(const UINT8 *base)
{
	return (base[0] << 24) | (base[1] << 16) | (base[2] << 8) | base[3];
}

/*-------------------------------------------------
    get_bigendian_uint64 - fetch a UINT64 from
    the data stream in bigendian order
-------------------------------------------------*/

INLINE UINT64 get_bigendian_uint64(const UINT8 *base)
{
	return ((UINT64)base[0] << 56) | ((UINT64)base[1] << 48) | ((UINT64)base[2] << 40) | ((UINT64)base[3] << 32) |
			((UINT64)base[4] << 24) | ((UINT64)base[5] << 16) | ((UINT64)base[6] << 8) | (UINT64)base[7];
}

// pseudo-codecs returned by hunk_info
const chd_codec_type CHD_CODEC_SELF         = 1;    // copy of another hunk
const chd_codec_type CHD_CODEC_PARENT       = 2;    // copy of a parent's hunk
const chd_codec_type CHD_CODEC_MINI         = 3;    // legacy "mini" 8-byte repeat

// V3-V4 entry types
enum
{
	V34_MAP_ENTRY_TYPE_INVALID = 0,             // invalid type
	V34_MAP_ENTRY_TYPE_COMPRESSED = 1,          // standard compression
	V34_MAP_ENTRY_TYPE_UNCOMPRESSED = 2,        // uncompressed data
	V34_MAP_ENTRY_TYPE_MINI = 3,                // mini: use offset as raw data
	V34_MAP_ENTRY_TYPE_SELF_HUNK = 4,           // same as another hunk in this file
	V34_MAP_ENTRY_TYPE_PARENT_HUNK = 5,         // same as a hunk in the parent file
	V34_MAP_ENTRY_TYPE_2ND_COMPRESSED = 6       // compressed with secondary algorithm (usually FLAC CDDA)
};

// V5 compression types
enum
{
	// these types are live when running
	COMPRESSION_TYPE_0 = 0,                     // codec #0
	COMPRESSION_TYPE_1 = 1,                     // codec #1
	COMPRESSION_TYPE_2 = 2,                     // codec #2
	COMPRESSION_TYPE_3 = 3,                     // codec #3
	COMPRESSION_NONE = 4,                       // no compression; implicit length = hunkbytes
	COMPRESSION_SELF = 5,                       // same as another block in this chd
	COMPRESSION_PARENT = 6,                     // same as a hunk's worth of units in the parent chd

	// these additional pseudo-types are used for compressed encodings:
	COMPRESSION_RLE_SMALL,                      // start of small RLE run (4-bit length)
	COMPRESSION_RLE_LARGE,                      // start of large RLE run (8-bit length)
	COMPRESSION_SELF_0,                         // same as the last COMPRESSION_SELF block
	COMPRESSION_SELF_1,                         // same as the last COMPRESSION_SELF block + 1
	COMPRESSION_PARENT_SELF,                    // same block in the parent
	COMPRESSION_PARENT_0,                       // same as the last COMPRESSION_PARENT block
	COMPRESSION_PARENT_1                        // same as the last COMPRESSION_PARENT block + 1
};

// currently-defined codecs
const chd_codec_type CHD_CODEC_NONE         = 0;

// general codecs
const chd_codec_type CHD_CODEC_ZLIB         = CHD_MAKE_TAG('z','l','i','b');
const chd_codec_type CHD_CODEC_LZMA         = CHD_MAKE_TAG('l','z','m','a');
const chd_codec_type CHD_CODEC_HUFFMAN      = CHD_MAKE_TAG('h','u','f','f');
const chd_codec_type CHD_CODEC_FLAC         = CHD_MAKE_TAG('f','l','a','c');

// general codecs with CD frontend
const chd_codec_type CHD_CODEC_CD_ZLIB      = CHD_MAKE_TAG('c','d','z','l');
const chd_codec_type CHD_CODEC_CD_LZMA      = CHD_MAKE_TAG('c','d','l','z');
const chd_codec_type CHD_CODEC_CD_FLAC      = CHD_MAKE_TAG('c','d','f','l');

// A/V codecs
const chd_codec_type CHD_CODEC_AVHUFF       = CHD_MAKE_TAG('a','v','h','u');

// A/V codec configuration parameters
enum
{
	AVHUFF_CODEC_DECOMPRESS_CONFIG = 1
};

/*-------------------------------------------------
    physical_to_chd_lba - find the CHD LBA
    and the track number
-------------------------------------------------*/

UINT32 physical_to_chd_lba(cdrom_file *file, UINT32 physlba, UINT32 *tracknum)
{
	UINT32 chdlba;
	int track;

	/* loop until our current LBA is less than the start LBA of the next track */
	for (track = 0; track < file->cdtoc.numtrks; track++)
		if (physlba < file->cdtoc.tracks[track + 1].physframeofs)
		{
			chdlba = physlba - file->cdtoc.tracks[track].physframeofs + file->cdtoc.tracks[track].chdframeofs;
			*tracknum = track;
			return chdlba;
		}

	return physlba;
}

/*-------------------------------------------------
    logical_to_chd_lba - find the CHD LBA
    and the track number
-------------------------------------------------*/

UINT32 logical_to_chd_lba(cdrom_file *file, UINT32 loglba, UINT32 *tracknum)
{
	UINT32 chdlba, physlba;
	int track;

	/* loop until our current LBA is less than the start LBA of the next track */
	for (track = 0; track < file->cdtoc.numtrks; track++)
	{
		if (loglba < file->cdtoc.tracks[track + 1].logframeofs)
		{
			// is this a no-pregap-data track?  compensate for the logical offset pointing to the "wrong" sector.
			if ((file->cdtoc.tracks[track].pgdatasize == 0) && (loglba > file->cdtoc.tracks[track].pregap))
			{
				loglba -= file->cdtoc.tracks[track].pregap;
			}

			// convert to physical and proceed
			physlba = file->cdtoc.tracks[track].physframeofs + (loglba - file->cdtoc.tracks[track].logframeofs);
			chdlba = physlba - file->cdtoc.tracks[track].physframeofs + file->cdtoc.tracks[track].chdframeofs;
			*tracknum = track;
			return chdlba;
		}
	}

	return loglba;
}

UINT32 cdrom_read_data(cdrom_file *file, UINT32 lbasector, void *buffer, UINT32 datatype, bool phys)
{
	if (file == NULL)
		return 0;

	// compute CHD sector and tracknumber
	UINT32 tracknum = 0;
	UINT32 chdsector;

	if (phys)
	{
		chdsector = physical_to_chd_lba(file, lbasector, &tracknum);
	}
	else
	{
		chdsector = logical_to_chd_lba(file, lbasector, &tracknum);
	}

	/* copy out the requested sector */
	UINT32 tracktype = file->cdtoc.tracks[tracknum].trktype;

	if ((datatype == tracktype) || (datatype == CD_TRACK_RAW_DONTCARE))
	{
		return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 0, file->cdtoc.tracks[tracknum].datasize) == CHDERR_NONE);
	}
	else
	{
		/* return 2048 bytes of mode 1 data from a 2352 byte mode 1 raw sector */
		if ((datatype == CD_TRACK_MODE1) && (tracktype == CD_TRACK_MODE1_RAW))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 16, 2048) == CHDERR_NONE);
		}

		/* return 2352 byte mode 1 raw sector from 2048 bytes of mode 1 data */
		if ((datatype == CD_TRACK_MODE1_RAW) && (tracktype == CD_TRACK_MODE1))
		{
			UINT8 *bufptr = (UINT8 *)buffer;
			UINT32 msf = lba_to_msf(lbasector);

			static const UINT8 syncbytes[12] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
			memcpy(bufptr, syncbytes, 12);
			bufptr[12] = msf>>16;
			bufptr[13] = msf>>8;
			bufptr[14] = msf&0xff;
			bufptr[15] = 1; // mode 1
			write_log(("CDROM: promotion of mode1/form1 sector to mode1 raw is not complete!\n"));
			return (read_partial_sector(file, bufptr+16, lbasector, chdsector, tracknum, 0, 2048) == CHDERR_NONE);
		}

		/* return 2048 bytes of mode 1 data from a mode2 form1 or raw sector */
		if ((datatype == CD_TRACK_MODE1) && ((tracktype == CD_TRACK_MODE2_FORM1)||(tracktype == CD_TRACK_MODE2_RAW)))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 24, 2048) == CHDERR_NONE);
		}

		/* return mode 2 2336 byte data from a 2352 byte mode 1 or 2 raw sector (skip the header) */
		if ((datatype == CD_TRACK_MODE2) && ((tracktype == CD_TRACK_MODE1_RAW) || (tracktype == CD_TRACK_MODE2_RAW)))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 16, 2336) == CHDERR_NONE);
		}

		write_log("CDROM: Conversion from type %d to type %d not supported!\n", tracktype, datatype);
		return 0;
	}
}

UINT32 cdrom_read_subcode(cdrom_file *file, UINT32 lbasector, void *buffer, bool phys)
{
	if (file == NULL)
		return ~0;

	// compute CHD sector and tracknumber
	UINT32 tracknum = 0;
	UINT32 chdsector;

	if (phys)
	{
		chdsector = physical_to_chd_lba(file, lbasector, &tracknum);
	}
	else
	{
		chdsector = logical_to_chd_lba(file, lbasector, &tracknum);
	}

	if (file->cdtoc.tracks[tracknum].subsize == 0)
		return 0;

	// read the data
	chd_error err = read_partial_sector(file, buffer, lbasector, chdsector, tracknum, file->cdtoc.tracks[tracknum].datasize, file->cdtoc.tracks[tracknum].subsize);
	return (err == CHDERR_NONE);
}

cdrom_file *cdrom_open(chd_file *chd)
{
	int i;
	cdrom_file *file = NULL;
	UINT32 physofs, chdofs, logofs;
	chd_error err;

	/* punt if no CHD */
	if (!chd)
		return NULL;

	/* validate the CHD information */
	if (chd->header.hunkbytes % CD_FRAME_SIZE != 0)
		return NULL;
	if (chd->header.unitbytes != CD_FRAME_SIZE)
		return NULL;

	/* allocate memory for the CD-ROM file */
	file = xmalloc(cdrom_file, 1);
	if (file == NULL)
		return NULL;

	/* fill in the data */
	file->chd = chd;

	/* read the CD-ROM metadata */
	err = cdrom_parse_metadata(chd, &file->cdtoc);
	if (err != CHDERR_NONE)
	{
		if (file)
		   free(file);
		file = NULL;
		return NULL;
	}

	write_log("CD has %d tracks\n", file->cdtoc.numtrks);

	/* calculate the starting frame for each track, keeping in mind that CHDMAN
	   pads tracks out with extra frames to fit 4-frame size boundries
	*/
	physofs = chdofs = logofs = 0;
	for (i = 0; i < file->cdtoc.numtrks; i++)
	{
		file->cdtoc.tracks[i].physframeofs = physofs;
		file->cdtoc.tracks[i].chdframeofs = chdofs;
		file->cdtoc.tracks[i].logframeofs = logofs;

		// if the pregap sectors aren't in the track, add them to the track's logical length
		if (file->cdtoc.tracks[i].pgdatasize == 0)
		{
			logofs += file->cdtoc.tracks[i].pregap;
		}

		// postgap counts against the next track
		logofs += file->cdtoc.tracks[i].postgap;

		physofs += file->cdtoc.tracks[i].frames;
		chdofs  += file->cdtoc.tracks[i].frames;
		chdofs  += file->cdtoc.tracks[i].extraframes - 1;
		logofs  += file->cdtoc.tracks[i].frames;

#if 0
        printf("Track %02d is format %d subtype %d datasize %d subsize %d frames %d extraframes %d pregap %d pgmode %d presize %d postgap %d logofs %d physofs %d chdofs %d\n", i+1,
            file->cdtoc.tracks[i].trktype,
            file->cdtoc.tracks[i].subtype,
            file->cdtoc.tracks[i].datasize,
            file->cdtoc.tracks[i].subsize,
            file->cdtoc.tracks[i].frames,
            file->cdtoc.tracks[i].extraframes,
            file->cdtoc.tracks[i].pregap,
            file->cdtoc.tracks[i].pgtype,
            file->cdtoc.tracks[i].pgdatasize,
            file->cdtoc.tracks[i].postgap,
            file->cdtoc.tracks[i].logframeofs,
            file->cdtoc.tracks[i].physframeofs,
            file->cdtoc.tracks[i].chdframeofs);
#endif
	}

	/* fill out dummy entries for the last track to help our search */
	file->cdtoc.tracks[i].physframeofs = physofs;
	file->cdtoc.tracks[i].logframeofs = logofs;
	file->cdtoc.tracks[i].chdframeofs = chdofs;

	return file;
}

/*-------------------------------------------------
    cdrom_close - "close" a CD-ROM file
-------------------------------------------------*/

void cdrom_close(cdrom_file *file)
{
	if (file == NULL)
		return;

	if (file->chd == NULL)
	{
		for (int i = 0; i < file->cdtoc.numtrks; i++)
		{
			core_fclose(file->fhandle[i]);
		}
	}

	if (file)
	    free(file);
	file = NULL;
}

static const UINT8 V34_MAP_ENTRY_FLAG_TYPE_MASK = 0x0f;     // what type of hunk
static const UINT8 V34_MAP_ENTRY_FLAG_NO_CRC = 0x10;        // no CRC is present

chd_error chd_hunk_info(chd_file *cf, UINT32 hunknum, chd_codec_type *compressor, UINT32 *compbytes)
{
	// error if invalid
	if (hunknum >= cf->header.hunkcount)
		return CHDERR_HUNK_OUT_OF_RANGE;

	// get the map pointer
	UINT8 *rawmap;
	switch (cf->header.version)
	{
		// v3/v4 map entries
		case 3:
		case 4:
			rawmap = cf->header.rawmap + 16 * hunknum;
			switch (rawmap[15] & V34_MAP_ENTRY_FLAG_TYPE_MASK)
			{
				case V34_MAP_ENTRY_TYPE_COMPRESSED:
					*compressor = CHD_CODEC_ZLIB;
					*compbytes = be_read(&rawmap[12], 2) + (rawmap[14] << 16);
					break;

				case V34_MAP_ENTRY_TYPE_UNCOMPRESSED:
					*compressor = CHD_CODEC_NONE;
					*compbytes = cf->header.hunkbytes;
					break;

				case V34_MAP_ENTRY_TYPE_MINI:
					*compressor = CHD_CODEC_MINI;
					*compbytes = 0;
					break;

				case V34_MAP_ENTRY_TYPE_SELF_HUNK:
					*compressor = CHD_CODEC_SELF;
					*compbytes = 0;
					break;

				case V34_MAP_ENTRY_TYPE_PARENT_HUNK:
					*compressor = CHD_CODEC_PARENT;
					*compbytes = 0;
					break;
			}
			break;

		// v5 map entries
		case 5:
			rawmap = cf->header.rawmap + cf->header.mapentrybytes * hunknum;

			// uncompressed case
			if (cf->header.compression[0] == CHD_CODEC_NONE)
			{
				if (be_read(&rawmap[0], 4) == 0)
				{
					*compressor = CHD_CODEC_PARENT;
					*compbytes = 0;
				}
				else
				{
					*compressor = CHD_CODEC_NONE;
					*compbytes = cf->header.hunkbytes;
				}
				break;
			}

			// compressed case
			switch (rawmap[0])
			{
				case COMPRESSION_TYPE_0:
				case COMPRESSION_TYPE_1:
				case COMPRESSION_TYPE_2:
				case COMPRESSION_TYPE_3:
					*compressor = cf->header.compression[rawmap[0]];
					*compbytes = be_read(&rawmap[1], 3);
					break;

				case COMPRESSION_NONE:
					*compressor = CHD_CODEC_NONE;
					*compbytes = cf->header.hunkbytes;
					break;

				case COMPRESSION_SELF:
					*compressor = CHD_CODEC_SELF;
					*compbytes = 0;
					break;

				case COMPRESSION_PARENT:
					*compressor = CHD_CODEC_PARENT;
					*compbytes = 0;
					break;
			}
			break;
	}
	return CHDERR_NONE;
}

//-------------------------------------------------
//  read_bytes - read from the CHD at a byte level,
//  using the cache to handle partial hunks
//-------------------------------------------------
UINT8 m_cache[CD_FRAME_SIZE*CD_FRAMES_PER_HUNK*2] = {0};
UINT32 m_cachehunk = 0;

chd_error chd_read_bytes(chd_file *chd, UINT64 offset, void *buffer, UINT32 bytes)
{
    UINT32 m_hunkbytes = chd->header.hunkbytes;

	// iterate over hunks
	UINT32 first_hunk = offset / m_hunkbytes;
	UINT32 last_hunk = (offset + bytes - 1) / m_hunkbytes;
	UINT8 *dest = (UINT8 *)buffer;
	for (UINT32 curhunk = first_hunk; curhunk <= last_hunk; curhunk++)
	{
		// determine start/end boundaries
		UINT32 startoffs = (curhunk == first_hunk) ? (offset % m_hunkbytes) : 0;
		UINT32 endoffs = (curhunk == last_hunk) ? ((offset + bytes - 1) % m_hunkbytes) : (m_hunkbytes - 1);

		// if it's a full block, just read directly from disk unless it's the cached hunk
		chd_error err = CHDERR_NONE;
		if (startoffs == 0 && endoffs == m_hunkbytes - 1 && curhunk != m_cachehunk)
			err = chd_read(chd, curhunk, &dest);

		// otherwise, read from the cache
		else
		{
			if (curhunk != m_cachehunk)
			{
				err = chd_read(chd, curhunk, &m_cache);
				if (err != CHDERR_NONE)
					return err;
				m_cachehunk = curhunk;
			}
			memcpy(dest, &m_cache[startoffs], endoffs + 1 - startoffs);
		}

		// handle errors and advance
		if (err != CHDERR_NONE)
			return err;
		dest += endoffs + 1 - startoffs;
	}
	return CHDERR_NONE;
}

chd_error read_partial_sector(cdrom_file *file, void *dest, UINT32 lbasector, UINT32 chdsector, UINT32 tracknum, UINT32 startoffs, UINT32 length)
{
	chd_error result = CHDERR_NONE;
	bool needswap = false;

	// if this is pregap info that isn't actually in the file, just return blank data
	if ((file->cdtoc.tracks[tracknum].pgdatasize == 0) && (lbasector < (file->cdtoc.tracks[tracknum].logframeofs + file->cdtoc.tracks[tracknum].pregap)))
	{
        write_log("PG missing sector: LBA %d, trklog %d\n", lbasector, file->cdtoc.tracks[tracknum].logframeofs);
		memset(dest, 0, length);
		return result;
	}

	// if a CHD, just read
	if (file->chd != NULL)
	{
#if 0
		result = file->chd->read_bytes(UINT64(chdsector) * UINT64(CD_FRAME_SIZE) + startoffs, dest, length);
#else
		result = chd_read_bytes(file->chd, (UINT64)chdsector * (UINT64)CD_FRAME_SIZE + startoffs, dest, length);
#endif
		/* swap CDDA in the case of LE GDROMs */
		if ((file->cdtoc.flags & CD_FLAG_GDROMLE) && (file->cdtoc.tracks[tracknum].trktype == CD_TRACK_AUDIO))
			needswap = true;
	}
	else
	{
#if 0
		// else read from the appropriate file
		core_file *srcfile = file->fhandle[tracknum];

		UINT64 sourcefileoffset = file->track_info.track[tracknum].offset;
		int bytespersector = file->cdtoc.tracks[tracknum].datasize + file->cdtoc.tracks[tracknum].subsize;

		sourcefileoffset += chdsector * bytespersector + startoffs;

#if 0
		printf("Reading sector %d from track %d at offset %lld\n", chdsector, tracknum, sourcefileoffset);
#endif

		core_fseek(srcfile, sourcefileoffset, SEEK_SET);
		core_fread(srcfile, dest, length);

		needswap = file->track_info.track[tracknum].swap;
#endif
	}

	if (needswap)
	{
		UINT8 *buffer = (UINT8 *)dest - startoffs;
		for (int swapindex = startoffs; swapindex < 2352; swapindex += 2 )
		{
			int swaptemp = buffer[ swapindex ];
			buffer[ swapindex ] = buffer[ swapindex + 1 ];
			buffer[ swapindex + 1 ] = swaptemp;
		}
	}
	return result;
}

/*-------------------------------------------------
    cdrom_get_adr_control - get the ADR | CONTROL
    for a track
-------------------------------------------------*/

int cdrom_get_adr_control(cdrom_file *file, int track)
{
	if (file == NULL)
		return -1;

	if (track == 0xaa || file->cdtoc.tracks[track].trktype == CD_TRACK_AUDIO)
	{
		return 0x10;    // audio track, subchannel is position
	}

	return 0x14;    // data track, subchannel is position
}

/*-------------------------------------------------
    cdrom_get_toc - return the TOC data for a
    CD-ROM
-------------------------------------------------*/

const cdrom_toc *cdrom_get_toc(cdrom_file *file)
{
	if (file == NULL)
		return NULL;

	return &file->cdtoc;
}

/*-------------------------------------------------
    cdrom_get_info_from_type_string
    take a string and convert it into track type
    and track data size
-------------------------------------------------*/

static void cdrom_get_info_from_type_string(const char *typestring, UINT32 *trktype, UINT32 *datasize)
{
	if (!strcmp(typestring, "MODE1"))
	{
		*trktype = CD_TRACK_MODE1;
		*datasize = 2048;
	}
	else if (!strcmp(typestring, "MODE1/2048"))
	{
		*trktype = CD_TRACK_MODE1;
		*datasize = 2048;
	}
	else if (!strcmp(typestring, "MODE1_RAW"))
	{
		*trktype = CD_TRACK_MODE1_RAW;
		*datasize = 2352;
	}
	else if (!strcmp(typestring, "MODE1/2352"))
	{
		*trktype = CD_TRACK_MODE1_RAW;
		*datasize = 2352;
	}
	else if (!strcmp(typestring, "MODE2"))
	{
		*trktype = CD_TRACK_MODE2;
		*datasize = 2336;
	}
	else if (!strcmp(typestring, "MODE2/2336"))
	{
		*trktype = CD_TRACK_MODE2;
		*datasize = 2336;
	}
	else if (!strcmp(typestring, "MODE2_FORM1"))
	{
		*trktype = CD_TRACK_MODE2_FORM1;
		*datasize = 2048;
	}
	else if (!strcmp(typestring, "MODE2/2048"))
	{
		*trktype = CD_TRACK_MODE2_FORM1;
		*datasize = 2048;
	}
	else if (!strcmp(typestring, "MODE2_FORM2"))
	{
		*trktype = CD_TRACK_MODE2_FORM2;
		*datasize = 2324;
	}
	else if (!strcmp(typestring, "MODE2/2324"))
	{
		*trktype = CD_TRACK_MODE2_FORM2;
		*datasize = 2324;
	}
	else if (!strcmp(typestring, "MODE2_FORM_MIX"))
	{
		*trktype = CD_TRACK_MODE2_FORM_MIX;
		*datasize = 2336;
	}
	else if (!strcmp(typestring, "MODE2/2336"))
	{
		*trktype = CD_TRACK_MODE2_FORM_MIX;
		*datasize = 2336;
	}
	else if (!strcmp(typestring, "MODE2_RAW"))
	{
		*trktype = CD_TRACK_MODE2_RAW;
		*datasize = 2352;
	}
	else if (!strcmp(typestring, "MODE2/2352"))
	{
		*trktype = CD_TRACK_MODE2_RAW;
		*datasize = 2352;
	}
	else if (!strcmp(typestring, "AUDIO"))
	{
		*trktype = CD_TRACK_AUDIO;
		*datasize = 2352;
	}
}

/*-------------------------------------------------
    cdrom_convert_type_string_to_track_info -
    take a string and convert it into track type
    and track data size
-------------------------------------------------*/

void cdrom_convert_type_string_to_track_info(const char *typestring, cdrom_track_info *info)
{
	cdrom_get_info_from_type_string(typestring, &info->trktype, &info->datasize);
}

/*-------------------------------------------------
    cdrom_convert_type_string_to_pregap_info -
    take a string and convert it into pregap type
    and pregap data size
-------------------------------------------------*/

void cdrom_convert_type_string_to_pregap_info(const char *typestring, cdrom_track_info *info)
{
	cdrom_get_info_from_type_string(typestring, &info->pgtype, &info->pgdatasize);
}

/*-------------------------------------------------
    cdrom_convert_subtype_string_to_track_info -
    take a string and convert it into track subtype
    and track subcode data size
-------------------------------------------------*/

void cdrom_convert_subtype_string_to_track_info(const char *typestring, cdrom_track_info *info)
{
	if (!strcmp(typestring, "RW"))
	{
		info->subtype = CD_SUB_NORMAL;
		info->subsize = 96;
	}
	else if (!strcmp(typestring, "RW_RAW"))
	{
		info->subtype = CD_SUB_RAW;
		info->subsize = 96;
	}
}

/*-------------------------------------------------
    cdrom_convert_subtype_string_to_pregap_info -
    take a string and convert it into track subtype
    and track subcode data size
-------------------------------------------------*/

void cdrom_convert_subtype_string_to_pregap_info(const char *typestring, cdrom_track_info *info)
{
	if (!strcmp(typestring, "RW"))
	{
		info->pgsub = CD_SUB_NORMAL;
		info->pgsubsize = 96;
	}
	else if (!strcmp(typestring, "RW_RAW"))
	{
		info->pgsub = CD_SUB_RAW;
		info->pgsubsize = 96;
	}
}

/*-------------------------------------------------
    cdrom_get_type_string - get the string
    associated with the given type
-------------------------------------------------*/

const char *cdrom_get_type_string(UINT32 trktype)
{
	switch (trktype)
	{
		case CD_TRACK_MODE1:            return "MODE1";
		case CD_TRACK_MODE1_RAW:        return "MODE1_RAW";
		case CD_TRACK_MODE2:            return "MODE2";
		case CD_TRACK_MODE2_FORM1:      return "MODE2_FORM1";
		case CD_TRACK_MODE2_FORM2:      return "MODE2_FORM2";
		case CD_TRACK_MODE2_FORM_MIX:   return "MODE2_FORM_MIX";
		case CD_TRACK_MODE2_RAW:        return "MODE2_RAW";
		case CD_TRACK_AUDIO:            return "AUDIO";
		default:                        return "UNKNOWN";
	}
}

/*-------------------------------------------------
    cdrom_get_subtype_string - get the string
    associated with the given subcode type
-------------------------------------------------*/

const char *cdrom_get_subtype_string(UINT32 subtype)
{
	switch (subtype)
	{
		case CD_SUB_NORMAL:             return "RW";
		case CD_SUB_RAW:                return "RW_RAW";
		default:                        return "NONE";
	}
}

chd_error metadata_find_entry(chd_file *chd, UINT32 metatag, UINT32 metaindex, metadata_entry *metaentry)
{
	/* start at the beginning */
	metaentry->offset = chd->header.metaoffset;
	metaentry->prev = 0;

	/* loop until we run out of options */
	while (metaentry->offset != 0)
	{
		UINT8	raw_meta_header[METADATA_HEADER_SIZE];
		UINT32	count;

		/* read the raw header */
		core_fseek(chd->file, metaentry->offset, SEEK_SET);
		count = core_fread(chd->file, raw_meta_header, sizeof(raw_meta_header));
		if (count != sizeof(raw_meta_header))
			break;

		/* extract the data */
		metaentry->metatag = get_bigendian_uint32(&raw_meta_header[0]);
		metaentry->length = get_bigendian_uint32(&raw_meta_header[4]);
		metaentry->next = get_bigendian_uint64(&raw_meta_header[8]);

		/* flags are encoded in the high byte of length */
		metaentry->flags = metaentry->length >> 24;
		metaentry->length &= 0x00ffffff;

		/* if we got a match, proceed */
		if (metatag == CHDMETATAG_WILDCARD || metaentry->metatag == metatag)
			if (metaindex-- == 0)
				return CHDERR_NONE;

		/* no match, fetch the next link */
		metaentry->prev = metaentry->offset;
		metaentry->offset = metaentry->next;
	}

	/* if we get here, we didn't find it */
	return CHDERR_METADATA_NOT_FOUND;
}

/*-------------------------------------------------
    cdrom_parse_metadata - parse metadata into the
    TOC structure
-------------------------------------------------*/

chd_error cdrom_parse_metadata(chd_file *chd, cdrom_toc *toc)
{
	char metadata[256];
	chd_error err;
	int i;

	toc->flags = 0;

	/* start with no tracks */
	for (toc->numtrks = 0; toc->numtrks < CD_MAX_TRACKS; toc->numtrks++)
	{
		int tracknum = -1, frames = 0, pregap, postgap, padframes;
		char type[16], subtype[16], pgtype[16], pgsub[16];
		cdrom_track_info *track;

		pregap = postgap = padframes = 0;

		/* fetch the metadata for this track */
		err = chd_get_metadata(chd, CDROM_TRACK_METADATA_TAG, toc->numtrks, metadata, sizeof(metadata), NULL, NULL, NULL);
		if (err == CHDERR_NONE)
		{
			/* parse the metadata */
			type[0] = subtype[0] = 0;
			pgtype[0] = pgsub[0] = 0;
			if (sscanf(metadata, CDROM_TRACK_METADATA_FORMAT, &tracknum, type, subtype, &frames) != 4)
				return CHDERR_INVALID_DATA;
			if (tracknum == 0 || tracknum > CD_MAX_TRACKS)
				return CHDERR_INVALID_DATA;
			track = &toc->tracks[tracknum - 1];

		}
		else
		{
			err = chd_get_metadata(chd, CDROM_TRACK_METADATA2_TAG, toc->numtrks, metadata, sizeof(metadata), NULL, NULL, NULL);
			if (err == CHDERR_NONE)
			{
				/* parse the metadata */
				type[0] = subtype[0] = 0;
				pregap = postgap = 0;
				if (sscanf(metadata, CDROM_TRACK_METADATA2_FORMAT, &tracknum, type, subtype, &frames, &pregap, pgtype, pgsub, &postgap) != 8)
					return CHDERR_INVALID_DATA;
				if (tracknum == 0 || tracknum > CD_MAX_TRACKS)
					return CHDERR_INVALID_DATA;
				track = &toc->tracks[tracknum - 1];
			}
			else
			{
				err = chd_get_metadata(chd, GDROM_OLD_METADATA_TAG, toc->numtrks, metadata, sizeof(metadata), NULL, NULL, NULL);
				if (err == CHDERR_NONE)
					/* legacy GDROM track was detected */
					toc->flags |= CD_FLAG_GDROMLE;
				else
					err = chd_get_metadata(chd, GDROM_TRACK_METADATA_TAG, toc->numtrks, metadata, sizeof(metadata), NULL, NULL, NULL);

				if (err == CHDERR_NONE)
				{
					/* parse the metadata */
					type[0] = subtype[0] = 0;
					pregap = postgap = 0;
					if (sscanf(metadata, GDROM_TRACK_METADATA_FORMAT, &tracknum, type, subtype, &frames, &padframes, &pregap, pgtype, pgsub, &postgap) != 9)
						return CHDERR_INVALID_DATA;
					if (tracknum == 0 || tracknum > CD_MAX_TRACKS)
						return CHDERR_INVALID_DATA;
					track = &toc->tracks[tracknum - 1];
					toc->flags |= CD_FLAG_GDROM;
				}
				else
				{
					break;
				}
			}
		}

		/* extract the track type and determine the data size */
		track->trktype = CD_TRACK_MODE1;
		track->datasize = 0;
		cdrom_convert_type_string_to_track_info(type, track);
		if (track->datasize == 0)
			return CHDERR_INVALID_DATA;

		/* extract the subtype and determine the subcode data size */
		track->subtype = CD_SUB_NONE;
		track->subsize = 0;
		cdrom_convert_subtype_string_to_track_info(subtype, track);

		/* set the frames and extra frames data */
		track->frames = frames;
		track->padframes = padframes;
		int padded = (frames + CD_TRACK_PADDING - 1) / CD_TRACK_PADDING;
		track->extraframes = padded * CD_TRACK_PADDING - frames;

		/* set the pregap info */
		track->pregap = pregap;
		track->pgtype = CD_TRACK_MODE1;
		track->pgsub = CD_SUB_NONE;
		track->pgdatasize = 0;
		track->pgsubsize = 0;
		if (track->pregap > 0)
		{
			if (pgtype[0] == 'V')
			{
				cdrom_convert_type_string_to_pregap_info(&pgtype[1], track);
			}

			cdrom_convert_subtype_string_to_pregap_info(pgsub, track);
		}

		/* set the postgap info */
		track->postgap = postgap;
	}

	/* if we got any tracks this way, we're done */
	if (toc->numtrks > 0)
		return CHDERR_NONE;

	printf("toc->numtrks = %d?!\n", toc->numtrks);

	/* look for old-style metadata */
	UINT8 *oldmetadata;
	err = chd_get_metadata(chd, CDROM_OLD_METADATA_TAG, 0, oldmetadata, sizeof(oldmetadata), NULL, NULL, NULL);
	if (err != CHDERR_NONE)
		return err;

	/* reconstruct the TOC from it */
	UINT32 *mrp = (UINT32 *)(&oldmetadata[0]);
	toc->numtrks = *mrp++;

	for (i = 0; i < CD_MAX_TRACKS; i++)
	{
		toc->tracks[i].trktype = *mrp++;
		toc->tracks[i].subtype = *mrp++;
		toc->tracks[i].datasize = *mrp++;
		toc->tracks[i].subsize = *mrp++;
		toc->tracks[i].frames = *mrp++;
		toc->tracks[i].extraframes = *mrp++;
		toc->tracks[i].pregap = 0;
		toc->tracks[i].postgap = 0;
		toc->tracks[i].pgtype = 0;
		toc->tracks[i].pgsub = 0;
		toc->tracks[i].pgdatasize = 0;
		toc->tracks[i].pgsubsize = 0;
	}

	/* TODO: I don't know why sometimes the data is one endian and sometimes another */
	if (toc->numtrks > CD_MAX_TRACKS)
	{
		toc->numtrks = FLIPENDIAN_INT32(toc->numtrks);
		for (i = 0; i < CD_MAX_TRACKS; i++)
		{
			toc->tracks[i].trktype = FLIPENDIAN_INT32(toc->tracks[i].trktype);
			toc->tracks[i].subtype = FLIPENDIAN_INT32(toc->tracks[i].subtype);
			toc->tracks[i].datasize = FLIPENDIAN_INT32(toc->tracks[i].datasize);
			toc->tracks[i].subsize = FLIPENDIAN_INT32(toc->tracks[i].subsize);
			toc->tracks[i].frames = FLIPENDIAN_INT32(toc->tracks[i].frames);
			toc->tracks[i].padframes = FLIPENDIAN_INT32(toc->tracks[i].padframes);
			toc->tracks[i].extraframes = FLIPENDIAN_INT32(toc->tracks[i].extraframes);
		}
	}

	return CHDERR_NONE;
}

#endif /* WITH_CHD */
