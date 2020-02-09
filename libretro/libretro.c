#include "libretro.h"
#include "retrodep/retroglue.h"
#include "libretro-mapper.h"
#include "libretro-glue.h"
#include "vkbd.h"
#include "retro_files.h"
#include "retro_strings.h"
#include "retro_disk_control.h"
#include "string/stdstring.h"
#include "file/file_path.h"
#include "deps/zlib/zlib.h"
#include "uae_types.h"

#include "retrodep/WHDLoad_hdf.gz.c"
#include "retrodep/WHDSaves_hdf.gz.c"
#include "retrodep/WHDLoad_prefs.gz.c"

#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "inputdevice.h"
#include "savestate.h"
#include "custom.h"
#include "akiko.h"
#include "blkdev.h"
extern void check_changes(int unitnum);
extern int frame_redraw_necessary;

extern int m68k_go(int may_quit, int resume);

int defaultw = EMULATOR_DEF_WIDTH;
int defaulth = EMULATOR_DEF_HEIGHT;
int retrow = 0;
int retroh = 0;
char key_state[512];
char key_state2[512];
unsigned int opt_mapping_options_display;
unsigned int opt_video_options_display;
unsigned int opt_audio_options_display;
char opt_model[10];
bool opt_use_whdload_hdf = true;
bool opt_use_whdsaves_hdf = true;
unsigned int opt_use_whdload_prefs = 0;
bool opt_shared_nvram = 0;
bool opt_statusbar_enhanced = true;
bool opt_statusbar_minimal = false;
int opt_statusbar_position = 0;
int opt_statusbar_position_old = 0;
int opt_statusbar_position_offset = 0;
unsigned int opt_vkbd_alpha = 204;
bool opt_keyrahkeypad = false;
bool opt_keyboard_pass_through = false;
bool opt_multimouse = false;
unsigned int opt_dpadmouse_speed = 4;
unsigned int opt_analogmouse = 0;
unsigned int opt_analogmouse_deadzone = 15;
float opt_analogmouse_speed = 1.0;
extern int turbo_fire_button;
extern unsigned int turbo_pulse;
int pix_bytes = 2;
static bool pix_bytes_initialized = false;
bool filter_type_update = true;
bool fake_ntsc = false;
bool real_ntsc = false;
bool forced_video = false;
bool request_update_av_info = false;
bool request_reset_drawing = false;
unsigned int zoom_mode_id = 0;
unsigned int opt_zoom_mode_id = 0;
int zoomed_height;

#if defined(NATMEM_OFFSET)
extern uae_u8 *natmem_offset;
extern uae_u32 natmem_size;
#endif

unsigned short int retro_bmp[RETRO_BMP_SIZE];
static char RPATH[512] = {0};
static char full_path[512] = {0};
static int firstpass = 1;
static int restart_pending = 0;
extern int SHIFTON;
extern int STATUSON;
extern void Print_Status(void);
extern void DrawHline(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
extern int prefs_changed;

static char *uae_argv[] = { "puae", RPATH };

int opt_vertical_offset = 0;
bool opt_vertical_offset_auto = true;
extern int minfirstline;
extern int retro_thisframe_first_drawn_line;
static int thisframe_first_drawn_line_old = -1;
extern int retro_thisframe_last_drawn_line;
static int thisframe_last_drawn_line_old = -1;
extern int thisframe_y_adjust;
static int thisframe_y_adjust_old = 0;
static int thisframe_y_adjust_update_frame_timer = 3;

int opt_horizontal_offset = 0;
bool opt_horizontal_offset_auto = true;
static int max_diwlastword_hires = 824;
static int max_diwlastword = 824;
extern int retro_min_diwstart;
static int min_diwstart_old = -1;
extern int retro_max_diwstop;
static int max_diwstop_old = -1;
extern int visible_left_border;
static int visible_left_border_old = 0;
static int visible_left_border_update_frame_timer = 3;

unsigned int video_config = 0;
unsigned int video_config_old = 0;
unsigned int video_config_aspect = 0;
unsigned int video_config_geometry = 0;
unsigned int video_config_allow_hz_change = 0;
unsigned int inputdevice_finalized = 0;

#include "libretro-keyboard.i"
int keyId(const char *val)
{
   int i=0;
   while (keyDesc[i]!=NULL)
   {
      if (!strcmp(keyDesc[i],val))
         return keyVal[i];
      i++;
   }
   return 0;
}

extern void retro_poll_event(void);
unsigned int uae_devices[4];
extern int cd32_pad_enabled[NORMAL_JPORTS];
int mapper_keys[31]={0};

#ifdef WIN32
#define DIR_SEP_STR "\\"
#else
#define DIR_SEP_STR "/"
#endif

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

static char retro_save_directory[RETRO_PATH_MAX] = {0};
// retro_system_directory is extern, used in caps.c and driveclick.c,
// so cannot be static
char retro_system_directory[512] = {0};
static char retro_content_directory[RETRO_PATH_MAX] = {0};

// Disk control context
static dc_storage* dc;

// Configs
static char uae_machine[256];
static char uae_kickstart[RETRO_PATH_MAX];
static char uae_kickstart_ext[RETRO_PATH_MAX];
static char uae_config[1024];

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_controller_description p1_controllers[] = {
      { "CD32 Pad", RETRO_DEVICE_UAE_CD32PAD },
      { "Joystick", RETRO_DEVICE_UAE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_UAE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p2_controllers[] = {
      { "CD32 Pad", RETRO_DEVICE_UAE_CD32PAD },
      { "Joystick", RETRO_DEVICE_UAE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_UAE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p3_controllers[] = {
      { "Joystick", RETRO_DEVICE_UAE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_UAE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p4_controllers[] = {
      { "Joystick", RETRO_DEVICE_UAE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_UAE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };

   static const struct retro_controller_info ports[] = {
      { p1_controllers, 4 }, // port 1
      { p2_controllers, 4 }, // port 2
      { p3_controllers, 3 }, // port 3
      { p4_controllers, 3 }, // port 4
      { NULL, 0 }
   };

   static struct retro_core_option_definition core_options[] =
   {
      {
         "puae_model",
         "Model",
         "Automatic defaults to A500 when booting floppy disks and to A600 when booting hard drives.\nCore restart required.",
         {
            { "auto", "Automatic" },
            { "A500", "A500 (512KB Chip + 512KB Slow)" },
            { "A500OG", "A500 (512KB Chip)" },
            { "A500PLUS", "A500+ (1MB Chip)" },
            { "A600", "A600 (2MB Chip + 8MB Fast)" },
            { "A1200", "A1200 (2MB Chip + 8MB Fast)" },
            { "A1200OG", "A1200 (2MB Chip)" },
            { "CD32", "CD32 (2MB Chip)" },
            { "CD32FR", "CD32 (2MB Chip + 8MB Fast)" },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_cpu_compatibility",
         "CPU Compatibility",
         "",
         {
            { "normal", "Normal" },
            { "compatible", "More compatible" },
            { "exact", "Cycle-exact" },
            { NULL, NULL },
         },
         "exact"
      },
      {
         "puae_cpu_throttle",
         "CPU Speed",
         "Ignored with 'Cycle-exact'.",
         {
            { "-900.0", "-90\%" },
            { "-800.0", "-80\%" },
            { "-700.0", "-70\%" },
            { "-600.0", "-60\%" },
            { "-500.0", "-50\%" },
            { "-400.0", "-40\%" },
            { "-300.0", "-30\%" },
            { "-200.0", "-20\%" },
            { "-100.0", "-10\%" },
            { "0.0", "Normal" },
            { "500.0", "+50\%" },
            { "1000.0", "+100\%" },
            { "1500.0", "+150\%" },
            { "2000.0", "+200\%" },
            { "2500.0", "+250\%" },
            { "3000.0", "+300\%" },
            { "3500.0", "+350\%" },
            { "4000.0", "+400\%" },
            { "4500.0", "+450\%" },
            { "5000.0", "+500\%" },
            { NULL, NULL },
         },
         "0.0"
      },
      {
         "puae_cpu_multiplier",
         "CPU Cycle-exact Speed",
         "Applies only with 'Cycle-exact'.",
         {
            { "0", "Normal" },
            { "1", "1x (3.546895 MHz)" },
            { "2", "2x (7.093790 MHz) A500" },
            { "4", "4x (14.187580 MHz) A1200" },
            { "8", "8x (28.375160 MHz)" },
            { NULL, NULL },
         },
         "0"
      },
      {
         "puae_video_options_display",
         "Show Video Options",
         "Core options page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_video_resolution",
         "Video Resolution",
         "Width:\n- 360px Low\n- 720px High\n- 1440px Super-High\nChanging will issue soft reset.",
         {
            { "lores", "Low" },
            { "hires_single", "High (Single line)" },
            { "hires_double", "High (Double line)" },
            { "superhires_single", "Super-High (Single line)" },
            { "superhires_double", "Super-High (Double line)" },
            { NULL, NULL },
         },
         "hires_double"
      },
      {
         "puae_video_allow_hz_change",
         "Allow PAL/NTSC Hz Change",
         "",
         {
            { "enabled", NULL },
            { "disabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      {
         "puae_video_standard",
         "Video Standard",
         "Height (Single line/Double line):\n- 288px/576px PAL\n- 240px/480px NTSC",
         {
            { "PAL", NULL },
            { "NTSC", NULL },
            { NULL, NULL },
         },
         "PAL"
      },
      {
         "puae_video_aspect",
         "Aspect Ratio",
         "",
         {
            { "auto", "Automatic" },
            { "PAL", NULL },
            { "NTSC", NULL },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_zoom_mode",
         "Zoom Mode",
         "Requirements in RetroArch settings:\nAspect Ratio: Core provided,\nInteger Scale: Off.",
         {
            { "none", "disabled" },
            { "minimum", "Minimum" },
            { "smaller", "Smaller" },
            { "small", "Small" },
            { "medium", "Medium" },
            { "large", "Large" },
            { "larger", "Larger" },
            { "maximum", "Maximum" },
            { "auto", "Automatic" },
            { NULL, NULL },
         },
         "none"
      },
      {
         "puae_vertical_pos",
         "Vertical Position",
         "Automatic keeps zoom modes centered. Positive values force the screen upward and negative values downward.",
         {
            { "auto", "Automatic" },
            { "0", NULL },
            { "2", NULL },
            { "4", NULL },
            { "6", NULL },
            { "8", NULL },
            { "10", NULL },
            { "12", NULL },
            { "14", NULL },
            { "16", NULL },
            { "18", NULL },
            { "20", NULL },
            { "22", NULL },
            { "24", NULL },
            { "26", NULL },
            { "28", NULL },
            { "30", NULL },
            { "32", NULL },
            { "34", NULL },
            { "36", NULL },
            { "38", NULL },
            { "40", NULL },
            { "42", NULL },
            { "44", NULL },
            { "46", NULL },
            { "48", NULL },
            { "50", NULL },
            { "-20", NULL },
            { "-18", NULL },
            { "-16", NULL },
            { "-14", NULL },
            { "-12", NULL },
            { "-10", NULL },
            { "-8", NULL },
            { "-6", NULL },
            { "-4", NULL },
            { "-2", NULL },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_horizontal_pos",
         "Horizontal Position",
         "Automatic keeps screen centered. Positive values force the screen right and negative values left.",
         {
            { "auto", "Automatic" },
            { "0", NULL },
            { "2", NULL },
            { "4", NULL },
            { "6", NULL },
            { "8", NULL },
            { "10", NULL },
            { "12", NULL },
            { "14", NULL },
            { "16", NULL },
            { "18", NULL },
            { "20", NULL },
            { "22", NULL },
            { "24", NULL },
            { "26", NULL },
            { "28", NULL },
            { "30", NULL },
            { "-30", NULL },
            { "-28", NULL },
            { "-26", NULL },
            { "-24", NULL },
            { "-22", NULL },
            { "-20", NULL },
            { "-18", NULL },
            { "-16", NULL },
            { "-14", NULL },
            { "-12", NULL },
            { "-10", NULL },
            { "-8", NULL },
            { "-6", NULL },
            { "-4", NULL },
            { "-2", NULL },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_gfx_colors",
         "Color Depth",
         "24-bit is slower and not available on all platforms. Full restart required.",
         {
            { "16bit", "Thousands (16-bit)" },
            { "24bit", "Millions (24-bit)" },
            { NULL, NULL },
         },
         "16bit"
      },
      {
         "puae_collision_level",
         "Collision Level",
         "'Sprites and Playfields' is recommended.",
         {
            { "playfields", "Sprites and Playfields" },
            { "sprites", "Sprites only" },
            { "full", "Full" },
            { "none", "None" },
            { NULL, NULL },
         },
         "playfields"
      },
      {
         "puae_immediate_blits",
         "Immediate/Waiting Blits",
         "'Immediate Blitter' ignored with 'Cycle-exact'.",
         {
            { "false", "disabled" },
            { "immediate", "Immediate Blitter" },
            { "waiting", "Wait for Blitter" },
            { NULL, NULL },
         },
         "waiting"
      },
      {
         "puae_gfx_framerate",
         "Frameskip",
         "Not compatible with 'Cycle-exact'.",
         {
            { "disabled", NULL },
            { "1", NULL },
            { "2", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_statusbar",
         "Statusbar Position & Mode",
         "",
         {
            { "bottom", "Bottom Full" },
            { "bottom_minimal", "Bottom Full Minimal" },
            { "bottom_basic", "Bottom Basic" },
            { "bottom_basic_minimal", "Bottom Basic Minimal" },
            { "top", "Top Full" },
            { "top_minimal", "Top Full Minimal" },
            { "top_basic", "Top Basic" },
            { "top_basic_minimal", "Top Basic Minimal" },
            { NULL, NULL },
         },
         "bottom"
      },
      {
         "puae_vkbd_alpha",
         "Virtual Keyboard Transparency",
         "",
         {
            { "0\%", NULL },
            { "5\%", NULL },
            { "10\%", NULL },
            { "15\%", NULL },
            { "20\%", NULL },
            { "25\%", NULL },
            { "30\%", NULL },
            { "35\%", NULL },
            { "40\%", NULL },
            { "45\%", NULL },
            { "50\%", NULL },
            { "55\%", NULL },
            { "60\%", NULL },
            { "65\%", NULL },
            { "70\%", NULL },
            { "75\%", NULL },
            { "80\%", NULL },
            { "85\%", NULL },
            { "90\%", NULL },
            { "95\%", NULL },
            { NULL, NULL },
         },
         "20\%"
      },
      {
         "puae_audio_options_display",
         "Show Audio Options",
         "Core options page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_sound_output",
         "Sound Output",
         "",
         {
            { "none", "None" },
            { "interrupts", "Interrupts" },
            { "normal", "Normal" },
            { "exact", "Exact" },
            { NULL, NULL },
         },
         "exact"
      },
      {
         "puae_sound_stereo_separation",
         "Sound Stereo Separation",
         "Paula sound chip channel panning.",
         {
            { "0\%", NULL },
            { "10\%", NULL },
            { "20\%", NULL },
            { "30\%", NULL },
            { "40\%", NULL },
            { "50\%", NULL },
            { "60\%", NULL },
            { "70\%", NULL },
            { "80\%", NULL },
            { "90\%", NULL },
            { "100\%", NULL },
            { NULL, NULL },
         },
         "100\%"
      },
      {
         "puae_sound_interpol",
         "Sound Interpolation",
         "",
         {
            { "none", "None" },
            { "anti", "Anti" },
            { "sinc", "Sinc" },
            { "rh", "RH" },
            { "crux", "Crux" },
            { NULL, NULL },
         },
         "anti"
      },
      {
         "puae_sound_filter",
         "Sound Filter",
         "",
         {
            { "emulated", "Emulated" },
            { "off", "Always off" },
            { "on", "Always on" },
            { NULL, NULL },
         },
         "emulated"
      },
      {
         "puae_sound_filter_type",
         "Sound Filter Type",
         "",
         {
            { "auto", "Automatic" },
            { "standard", "A500" },
            { "enhanced", "A1200" },
            { NULL, NULL },
         },
         "auto",
      },
      {
         "puae_sound_volume_cd",
         "CD Audio Volume",
         "",
         {
            { "0\%", NULL },
            { "5\%", NULL },
            { "10\%", NULL },
            { "15\%", NULL },
            { "20\%", NULL },
            { "25\%", NULL },
            { "30\%", NULL },
            { "35\%", NULL },
            { "40\%", NULL },
            { "45\%", NULL },
            { "50\%", NULL },
            { "55\%", NULL },
            { "60\%", NULL },
            { "65\%", NULL },
            { "70\%", NULL },
            { "75\%", NULL },
            { "80\%", NULL },
            { "85\%", NULL },
            { "90\%", NULL },
            { "95\%", NULL },
            { "100\%", NULL },
            { NULL, NULL },
         },
         "100\%"
      },
      {
         "puae_floppy_sound",
         "Floppy Sound Emulation",
         "",
         {
            { "100", "disabled" },
            { "95", "5\% volume" },
            { "90", "10\% volume" },
            { "85", "15\% volume" },
            { "80", "20\% volume" },
            { "75", "25\% volume" },
            { "70", "30\% volume" },
            { "65", "35\% volume" },
            { "60", "40\% volume" },
            { "55", "45\% volume" },
            { "50", "50\% volume" },
            { "45", "55\% volume" },
            { "40", "60\% volume" },
            { "35", "65\% volume" },
            { "30", "70\% volume" },
            { "25", "75\% volume" },
            { "20", "80\% volume" },
            { "15", "85\% volume" },
            { "10", "90\% volume" },
            { "5", "95\% volume" },
            { "0", "100\% volume" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_floppy_sound_type",
         "Floppy Sound Emulation Type",
         "External file location is 'system/uae_data/'.",
         {
            { "internal", "Internal" },
            { "A500", "External: A500" },
            { "LOUD", "External: LOUD" },
            { NULL, NULL },
         },
         "internal"
      },
      {
         "puae_floppy_speed",
         "Floppy Speed",
         "",
         {
            { "100", "1x" },
            { "200", "2x" },
            { "400", "4x" },
            { "800", "8x" },
            { "0", "Turbo" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_shared_nvram",
         "Shared CD32 NVRAM",
         "Disabled will save separate files per game. Enabled will use one shared file. Core restart required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_use_whdload",
         "Use WHDLoad.hdf",
         "Enables the use of WHDLoad hard drive images which only have the game files. Core restart required.",
         {
            { "enabled", NULL },
            { "disabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      {
         "puae_use_whdsaves",
         "Use WHDSaves.hdf",
         "Enabled will save WHDLoad saves to WHDSaves.hdf or WHDLoad.hdf. Disabled will save files directly to RA saves. Core restart required.",
         {
            { "enabled", NULL },
            { "disabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_use_whdload_prefs",
         "Use WHDLoad.prefs",
         "WHDLoad.prefs in 'system/' required.\n'Config' shows the config screen only if the slave supports it. 'Splash' shows the splash screen always. Space/Enter/Fire work as a start button in 'Config'.\nCore restart required. Will not work with the old WHDLoad.hdf!",
         {
            { "disabled", NULL },
            { "config", "Config" },
            { "splash", "Splash" },
            { "both", "Config + Splash" },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_analogmouse",
         "Analog Stick Mouse",
         "",
         {
            { "disabled", NULL },
            { "left", "Left analog" },
            { "right", "Right analog" },
            { "both", "Both analogs" },
            { NULL, NULL },
         },
         "right"
      },
      {
         "puae_analogmouse_deadzone",
         "Analog Stick Mouse Deadzone",
         "",
         {
            { "0", "0\%" },
            { "5", "5\%" },
            { "10", "10\%" },
            { "15", "15\%" },
            { "20", "20\%" },
            { "25", "25\%" },
            { "30", "30\%" },
            { "35", "35\%" },
            { "40", "40\%" },
            { "45", "45\%" },
            { "50", "50\%" },
            { NULL, NULL },
         },
         "15"
      },
      {
         "puae_analogmouse_speed",
         "Analog Stick Mouse Speed",
         "",
         {
            { "0.5", "50\%" },
            { "0.6", "60\%" },
            { "0.7", "70\%" },
            { "0.8", "80\%" },
            { "0.9", "90\%" },
            { "1.0", "100\%" },
            { "1.1", "110\%" },
            { "1.2", "120\%" },
            { "1.3", "130\%" },
            { "1.4", "140\%" },
            { "1.5", "150\%" },
            { NULL, NULL },
         },
         "1.0"
      },
      {
         "puae_dpadmouse_speed",
         "D-Pad Mouse Speed",
         "",
         {
            { "3", "50\%" },
            { "4", "66\%" },
            { "5", "83\%" },
            { "6", "100\%" },
            { "7", "116\%" },
            { "8", "133\%" },
            { "9", "150\%" },
            { "10", "166\%" },
            { "11", "183\%" },
            { "12", "200\%" },
            { NULL, NULL },
         },
         "6"
      },
      {
         "puae_mouse_speed",
         "Mouse Speed",
         "Affects mouse speed globally.",
         {
            { "10", "10\%" },
            { "20", "20\%" },
            { "30", "30\%" },
            { "40", "40\%" },
            { "50", "50\%" },
            { "60", "60\%" },
            { "70", "70\%" },
            { "80", "80\%" },
            { "90", "90\%" },
            { "100", "100\%" },
            { "110", "110\%" },
            { "120", "120\%" },
            { "130", "130\%" },
            { "140", "140\%" },
            { "150", "150\%" },
            { "160", "160\%" },
            { "170", "170\%" },
            { "180", "180\%" },
            { "190", "190\%" },
            { "200", "200\%" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_multimouse",
         "Multiple Physical Mouse",
         "Requirements: raw/udev input driver and proper mouse index in RA input configs.\nOnly for real mice, not RetroPad emulated.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_keyrah_keypad_mappings",
         "Keyrah Keypad Mappings",
         "Hardcoded keypad to joy mappings for Keyrah hardware.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_physical_keyboard_pass_through",
         "Physical Keyboard Pass-through",
         "Pass all physical keyboard events to the core. Disable this to prevent cursor keys and fire key from generating key events.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_mapping_options_display",
         "Show Mapping Options",
         "Show options for hotkeys & RetroPad mappings.\nCore option page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      /* Hotkeys */
      {
         "puae_mapper_vkbd",
         "Hotkey: Toggle Virtual Keyboard",
         "Press the mapped key to toggle the virtual keyboard.",
         {{ NULL, NULL }},
         "RETROK_F11"
      },
      {
         "puae_mapper_statusbar",
         "Hotkey: Toggle Statusbar",
         "Press the mapped key to toggle the statusbar.",
         {{ NULL, NULL }},
         "RETROK_F12"
      },
      {
         "puae_mapper_mouse_toggle",
         "Hotkey: Toggle Joystick/Mouse",
         "Press the mapped key to toggle between joystick and mouse control.",
         {{ NULL, NULL }},
         "RETROK_RCTRL"
      },
      {
         "puae_mapper_reset",
         "Hotkey: Reset",
         "Press the mapped key to trigger reset (Ctrl-Amiga-Amiga).",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_aspect_ratio_toggle",
         "Hotkey: Toggle Aspect Ratio",
         "Press the mapped key to toggle between PAL/NTSC aspect ratio.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_zoom_mode_toggle",
         "Hotkey: Toggle Zoom Mode",
         "Press the mapped key to toggle zoom mode.",
         {{ NULL, NULL }},
         "---"
      },
      /* Button mappings */
      {
         "puae_mapper_select",
         "RetroPad Select",
         "",
         {{ NULL, NULL }},
         "RETROK_F11"
      },
      {
         "puae_mapper_start",
         "RetroPad Start",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_a",
         "RetroPad A",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_y",
         "RetroPad Y",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_x",
         "RetroPad X",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_l",
         "RetroPad L",
         "",
         {{ NULL, NULL }},
         ""
      },
      {
         "puae_mapper_r",
         "RetroPad R",
         "",
         {{ NULL, NULL }},
         ""
      },
      {
         "puae_mapper_l2",
         "RetroPad L2",
         "",
         {{ NULL, NULL }},
         "MOUSE_LEFT_BUTTON"
      },
      {
         "puae_mapper_r2",
         "RetroPad R2",
         "",
         {{ NULL, NULL }},
         "MOUSE_RIGHT_BUTTON"
      },
      {
         "puae_mapper_l3",
         "RetroPad L3",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_r3",
         "RetroPad R3",
         "",
         {{ NULL, NULL }},
         "---"
      },
      /* Left Stick */
      {
         "puae_mapper_lu",
         "RetroPad L-Up",
         "Mapping for left analog stick up.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_ld",
         "RetroPad L-Down",
         "Mapping for left analog stick down.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_ll",
         "RetroPad L-Left",
         "Mapping for left analog stick left.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_lr",
         "RetroPad L-Right",
         "Mapping for left analog stick right.",
         {{ NULL, NULL }},
         "---"
      },
      /* Right Stick */
      {
         "puae_mapper_ru",
         "RetroPad R-Up",
         "Mapping for right analog stick up.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rd",
         "RetroPad R-Down",
         "Mapping for right analog stick down.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rl",
         "RetroPad R-Left",
         "Mapping for right analog stick left.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rr",
         "RetroPad R-Right",
         "Mapping for right analog stick right.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_turbo_fire_button",
         "RetroPad Turbo Fire",
         "Replaces the mapped button with a turbo fire button.",
         {
            { "disabled", NULL },
            { "A", "RetroPad A" },
            { "Y", "RetroPad Y" },
            { "X", "RetroPad X" },
            { "L", "RetroPad L" },
            { "R", "RetroPad R" },
            { "L2", "RetroPad L2" },
            { "R2", "RetroPad R2" },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_turbo_pulse",
         "RetroPad Turbo Pulse",
         "Frames in a button cycle. 2 equals button press on a frame and release on the next frame.",
         {
            { "2", NULL },
            { "4", NULL },
            { "6", NULL },
            { "8", NULL },
            { "10", NULL },
            { "12", NULL },
            { NULL, NULL },
         },
         "4"
      },
      { NULL, NULL, NULL, {{0}}, NULL },
   };

   /* fill in the values for all the mappers */
   int i = 0;
   int j = 0;
   int hotkey = 0;
   while (core_options[i].key)
   {
      if (strstr(core_options[i].key, "puae_mapper_"))
      {
         /* Show different key list for hotkeys (special negatives removed) */
         if (  strstr(core_options[i].key, "puae_mapper_vkbd")
            || strstr(core_options[i].key, "puae_mapper_statusbar")
            || strstr(core_options[i].key, "puae_mapper_mouse_toggle")
            || strstr(core_options[i].key, "puae_mapper_reset")
            || strstr(core_options[i].key, "puae_mapper_aspect_ratio_toggle")
            || strstr(core_options[i].key, "puae_mapper_zoom_mode_toggle")
         )
            hotkey = 1;
         else
            hotkey = 0;

         j = 0;
         if (hotkey)
         {
             while (keyDescHotkeys[j] && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
             {
                core_options[i].values[j].value = keyDescHotkeys[j];
                core_options[i].values[j].label = NULL;
                ++j;
             };
         }
         else
         {
             while (keyDesc[j] && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
             {
                core_options[i].values[j].value = keyDesc[j];
                core_options[i].values[j].label = NULL;
                ++j;
             };
         }
         core_options[i].values[j].value = NULL;
         core_options[i].values[j].label = NULL;
      };
      ++i;
   }

   environ_cb = cb;
   unsigned version = 0;
   if (cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && (version == 1))
      cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, core_options);
   else
   {
      /* Fallback for older API */
      static char buf[sizeof(core_options) / sizeof(core_options[0])][4096] = { 0 };
      static struct retro_variable variables[sizeof(core_options) / sizeof(core_options[0])] = { 0 };
      i = 0;
      while (core_options[i].key)
      {
         buf[i][0] = 0;
         variables[i].key = core_options[i].key;
         strcpy(buf[i], core_options[i].desc);
         strcat(buf[i], "; ");
         strcat(buf[i], core_options[i].default_value);
         j = 0;
         while (core_options[i].values[j].value && j < RETRO_NUM_CORE_OPTION_VALUES_MAX)
         {
            strcat(buf[i], "|");
            strcat(buf[i], core_options[i].values[j].value);
            ++j;
         };
         variables[i].value = buf[i];
         ++i;
      };
      variables[i].key = NULL;
      variables[i].value = NULL;
      cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
   }

   static bool allowNoGameMode;
   allowNoGameMode = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &allowNoGameMode);
   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

static void update_variables(void)
{
   uae_machine[0] = '\0';
   uae_config[0] = '\0';

   struct retro_variable var = {0};
   struct retro_core_option_display option_display;

   static int video_config_region = 0;

   var.key = "puae_model";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      _tcscpy(opt_model, var.value);
   }

   var.key = "puae_video_standard";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* video_config change only at start */
      if (video_config_old == 0)
      {
         if (strcmp(var.value, "PAL") == 0)
         {
            video_config |= PUAE_VIDEO_PAL;
            strcat(uae_config, "ntsc=false\n");
         }
         else
         {
            video_config |= PUAE_VIDEO_NTSC;
            strcat(uae_config, "ntsc=true\n");
            real_ntsc = true;
         }
         video_config_region = video_config;
      }
      else if (!forced_video)
      {
         if (strcmp(var.value, "PAL") == 0)
            changed_prefs.ntscmode=0;
         else
            changed_prefs.ntscmode=1;
      }
   }

   var.key = "puae_video_aspect";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "PAL") == 0) video_config_aspect = PUAE_VIDEO_PAL;
      else if (strcmp(var.value, "NTSC") == 0) video_config_aspect = PUAE_VIDEO_NTSC;
      else video_config_aspect = 0;
   }

   var.key = "puae_video_allow_hz_change";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0) video_config_allow_hz_change = 1;
      else if (strcmp(var.value, "disabled") == 0) video_config_allow_hz_change = 0;
   }

   var.key = "puae_video_resolution";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int video_config_prev = video_config;
      video_config = video_config_region;

      if (strcmp(var.value, "hires_double") == 0)
      {
         video_config |= PUAE_VIDEO_HIRES;
         video_config |= PUAE_VIDEO_DOUBLELINE;
         max_diwlastword = max_diwlastword_hires;
         if (!firstpass)
         {
            changed_prefs.gfx_resolution=RES_HIRES;
            changed_prefs.gfx_vresolution=VRES_DOUBLE;
         }
      }
      else if (strcmp(var.value, "hires_single") == 0)
      {
         video_config |= PUAE_VIDEO_HIRES;
         max_diwlastword = max_diwlastword_hires;
         if (!firstpass)
         {
            changed_prefs.gfx_resolution=RES_HIRES;
            changed_prefs.gfx_vresolution=VRES_NONDOUBLE;
         }
      }
      else if (strcmp(var.value, "superhires_double") == 0)
      {
         video_config |= PUAE_VIDEO_SUPERHIRES;
         video_config |= PUAE_VIDEO_DOUBLELINE;

         max_diwlastword = max_diwlastword_hires * 2;
         if (!firstpass)
         {
            changed_prefs.gfx_resolution=RES_SUPERHIRES;
            changed_prefs.gfx_vresolution=VRES_DOUBLE;
         }
      }
      else if (strcmp(var.value, "superhires_single") == 0)
      {
         video_config |= PUAE_VIDEO_SUPERHIRES;

         max_diwlastword = max_diwlastword_hires * 2;
         if (!firstpass)
         {
            changed_prefs.gfx_resolution=RES_SUPERHIRES;
            changed_prefs.gfx_vresolution=VRES_NONDOUBLE;
         }
      }
      else if (strcmp(var.value, "lores") == 0)
      {
         max_diwlastword = max_diwlastword_hires / 2;
         if (!firstpass)
         {
            changed_prefs.gfx_resolution=RES_LORES;
            changed_prefs.gfx_vresolution=VRES_NONDOUBLE;
         }
      }

      /* Resolution change requires Amiga reset */
      if (!firstpass && video_config != video_config_prev)
         uae_reset(0, 0);
   }

   var.key = "puae_statusbar";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strstr(var.value, "top"))
         opt_statusbar_position = -1;
      else if (strstr(var.value, "bottom"))
         opt_statusbar_position = 0;

      if (strstr(var.value, "basic"))
         opt_statusbar_enhanced = false;
      else
         opt_statusbar_enhanced = true;

      if (strstr(var.value, "minimal"))
         opt_statusbar_minimal = true;
      else
         opt_statusbar_minimal = false;

      /* Screen refresh required
       * (redundant - will be forced by av_info
       *  geometry update) */
      if (opt_statusbar_position_old != opt_statusbar_position || !opt_statusbar_enhanced)
         request_reset_drawing = true;

      opt_statusbar_position_old = opt_statusbar_position;
   }

   var.key = "puae_vkbd_alpha";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_vkbd_alpha = 255 - (255 * atoi(var.value) / 100);
   }

   var.key = "puae_cpu_compatibility";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "normal") == 0)
      {
         strcat(uae_config, "cpu_compatible=false\n");
         strcat(uae_config, "cycle_exact=false\n");
      }
      else if (strcmp(var.value, "compatible") == 0)
      {
         strcat(uae_config, "cpu_compatible=true\n");
         strcat(uae_config, "cycle_exact=false\n");
      }
      else if (strcmp(var.value, "exact") == 0)
      {
         strcat(uae_config, "cpu_compatible=true\n");
         strcat(uae_config, "cycle_exact=true\n");
      }

      if (!firstpass)
      {
         if (strcmp(var.value, "normal") == 0)
         {
            changed_prefs.cpu_compatible=0;
            changed_prefs.cpu_cycle_exact=0;
            changed_prefs.blitter_cycle_exact=0;
         }
         else if (strcmp(var.value, "compatible") == 0)
         {
            changed_prefs.cpu_compatible=1;
            changed_prefs.cpu_cycle_exact=0;
            changed_prefs.blitter_cycle_exact=0;
         }
         else if (strcmp(var.value, "exact") == 0)
         {
            changed_prefs.cpu_compatible=1;
            changed_prefs.cpu_cycle_exact=1;
            changed_prefs.blitter_cycle_exact=1;
         }
      }
   }

   var.key = "puae_cpu_throttle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cpu_throttle=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
         changed_prefs.m68k_speed_throttle=atof(var.value);
   }

   var.key = "puae_cpu_multiplier";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cpu_multiplier=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
         changed_prefs.cpu_clock_multiplier=atoi(var.value) * 256;
   }

   var.key = "puae_sound_output";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_output=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
      {
         if (strcmp(var.value, "none") == 0) changed_prefs.produce_sound=0;
         else if (strcmp(var.value, "interrupts") == 0) changed_prefs.produce_sound=1;
         else if (strcmp(var.value, "normal") == 0) changed_prefs.produce_sound=2;
         else if (strcmp(var.value, "exact") == 0) changed_prefs.produce_sound=3;
      }
   }

   var.key = "puae_sound_stereo_separation";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int val = atoi(var.value) / 10;
      char valbuf[10];
      snprintf(valbuf, 10, "%d", val);
      strcat(uae_config, "sound_stereo_separation=");
      strcat(uae_config, valbuf);
      strcat(uae_config, "\n");

      if (!firstpass)
         changed_prefs.sound_stereo_separation=val;
   }

   var.key = "puae_sound_interpol";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_interpol=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
      {
         if (strcmp(var.value, "none") == 0) changed_prefs.sound_interpol=0;
         else if (strcmp(var.value, "anti") == 0) changed_prefs.sound_interpol=1;
         else if (strcmp(var.value, "sinc") == 0) changed_prefs.sound_interpol=2;
         else if (strcmp(var.value, "rh") == 0) changed_prefs.sound_interpol=3;
         else if (strcmp(var.value, "crux") == 0) changed_prefs.sound_interpol=4;
      }
   }

   var.key = "puae_sound_filter";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_filter=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");
      
      if (!firstpass)
      {
         if (strcmp(var.value, "emulated") == 0) changed_prefs.sound_filter=FILTER_SOUND_EMUL;
         else if (strcmp(var.value, "off") == 0) changed_prefs.sound_filter=FILTER_SOUND_OFF;
         else if (strcmp(var.value, "on") == 0) changed_prefs.sound_filter=FILTER_SOUND_ON;
      }
   }

   var.key = "puae_sound_filter_type";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "auto"))
      {
         strcat(uae_config, "sound_filter_type=");
         strcat(uae_config, var.value);
         strcat(uae_config, "\n");
      }

      if (!firstpass)
      {
         if (strcmp(var.value, "standard") == 0) changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A500;
         else if (strcmp(var.value, "enhanced") == 0) changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A1200;
         else if (strcmp(var.value, "auto") == 0)
         {
            if (currprefs.cpu_model == 68020)
               changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A1200;
            else
               changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A500;
         }
      }
   }

   var.key = "puae_sound_volume_cd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* 100 is mute, 0 is max */
      int val = 100 - atoi(var.value);
      char valbuf[10];
      snprintf(valbuf, 10, "%d", val);
      strcat(uae_config, "sound_volume_cd=");
      strcat(uae_config, valbuf);
      strcat(uae_config, "\n");

      if (!firstpass)
         changed_prefs.sound_volume_cd=val;
   }

   var.key = "puae_floppy_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "floppy_speed=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
         changed_prefs.floppy_speed=atoi(var.value);
   }

   var.key = "puae_floppy_sound";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* Sound is enabled by default if files are found, so this needs to be set always */
      /* 100 is mute, 0 is max */
      strcat(uae_config, "floppy_volume=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      /* Setting volume in realtime will crash on first pass */
      if (!firstpass)
         changed_prefs.dfxclickvolume=atoi(var.value);
   }

   var.key = "puae_floppy_sound_type";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "internal") == 0)
      {
         strcat(uae_config, "floppy0sound=1\n");
         strcat(uae_config, "floppy1sound=1\n");
         strcat(uae_config, "floppy2sound=1\n");
         strcat(uae_config, "floppy3sound=1\n");
      }
      else
      {
         strcat(uae_config, "floppy0sound=-1\n");
         strcat(uae_config, "floppy1sound=-1\n");
         strcat(uae_config, "floppy2sound=-1\n");
         strcat(uae_config, "floppy3sound=-1\n");

         strcat(uae_config, "floppy0soundext=");
         strcat(uae_config, var.value);
         strcat(uae_config, "\n");
         strcat(uae_config, "floppy1soundext=");
         strcat(uae_config, var.value);
         strcat(uae_config, "\n");
         strcat(uae_config, "floppy2soundext=");
         strcat(uae_config, var.value);
         strcat(uae_config, "\n");
         strcat(uae_config, "floppy3soundext=");
         strcat(uae_config, var.value);
         strcat(uae_config, "\n");
      }

      if (!firstpass)
      {
         if (strcmp(var.value, "internal") == 0)
         {
            for (int i = 0; i < 4; i++)
               changed_prefs.floppyslots[i].dfxclick=1;
         }
         else
         {
            for (int i = 0; i < 4; i++)
            {
               changed_prefs.floppyslots[i].dfxclick=-1;
               _tcscpy(changed_prefs.floppyslots[i].dfxclickexternal, var.value);
            }
         }
      }
   }

   var.key = "puae_mouse_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "input.mouse_speed=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
      {
         int val;
         val = atoi(var.value);
         changed_prefs.input_mouse_speed=val;
      }
   }

   var.key = "puae_immediate_blits";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "false") == 0)
      {
         strcat(uae_config, "immediate_blits=false\n");
         strcat(uae_config, "waiting_blits=false\n");
      }
      else if (strcmp(var.value, "immediate") == 0)
      {
         strcat(uae_config, "immediate_blits=true\n");
         strcat(uae_config, "waiting_blits=disabled\n");
      }
      else if (strcmp(var.value, "waiting") == 0)
      {
         strcat(uae_config, "immediate_blits=false\n");
         strcat(uae_config, "waiting_blits=automatic\n");
      }

      if (!firstpass)
      {
         if (strcmp(var.value, "false") == 0)
         {
            changed_prefs.immediate_blits=0;
            changed_prefs.waiting_blits=0;
         }
         else if (strcmp(var.value, "immediate") == 0)
         {
            changed_prefs.immediate_blits=1;
            changed_prefs.waiting_blits=0;
         }
         else if (strcmp(var.value, "waiting") == 0)
         {
            changed_prefs.immediate_blits=0;
            changed_prefs.waiting_blits=1;
         }
      }
   }

   var.key = "puae_collision_level";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "collision_level=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (!firstpass)
      {
         if (strcmp(var.value, "none") == 0) changed_prefs.collision_level=0;
         else if (strcmp(var.value, "sprites") == 0) changed_prefs.collision_level=1;
         else if (strcmp(var.value, "playfields") == 0) changed_prefs.collision_level=2;
         else if (strcmp(var.value, "full") == 0) changed_prefs.collision_level=3;
      }
   }

   var.key = "puae_gfx_framerate";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int val;
      if (strcmp(var.value, "disabled") == 0) val=1;
      else if (strcmp(var.value, "1") == 0) val=2;
      else if (strcmp(var.value, "2") == 0) val=3;

      if (val>1)
      {
         char valbuf[10];
         snprintf(valbuf, 10, "%d", val);
         strcat(uae_config, "gfx_framerate=");
         strcat(uae_config, valbuf);
         strcat(uae_config, "\n");
      }

      if (!firstpass)
         changed_prefs.gfx_framerate=val;
   }

   var.key = "puae_gfx_colors";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      // Only allow screenmode change after restart
      if (!pix_bytes_initialized)
      {
         if (strcmp(var.value, "16bit") == 0) pix_bytes=2;
         else if (strcmp(var.value, "24bit") == 0) pix_bytes=4;
      }
   }

   var.key = "puae_zoom_mode";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "none") == 0) zoom_mode_id=0;
      else if (strcmp(var.value, "minimum") == 0) zoom_mode_id=1;
      else if (strcmp(var.value, "smaller") == 0) zoom_mode_id=2;
      else if (strcmp(var.value, "small") == 0) zoom_mode_id=3;
      else if (strcmp(var.value, "medium") == 0) zoom_mode_id=4;
      else if (strcmp(var.value, "large") == 0) zoom_mode_id=5;
      else if (strcmp(var.value, "larger") == 0) zoom_mode_id=6;
      else if (strcmp(var.value, "maximum") == 0) zoom_mode_id=7;
      else if (strcmp(var.value, "auto") == 0) zoom_mode_id=8;

      opt_zoom_mode_id = zoom_mode_id;
   }

   var.key = "puae_vertical_pos";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "auto") == 0)
      {
         opt_vertical_offset_auto = true;
         thisframe_y_adjust = minfirstline;
      }
      else
      {
         opt_vertical_offset_auto = false;
         int new_vertical_offset = atoi(var.value);
         if (new_vertical_offset >= -20 && new_vertical_offset <= 50)
         {
            /* This offset is used whenever minfirstline is reset on gfx mode changes in the init_hz() function */
            opt_vertical_offset = new_vertical_offset;
            thisframe_y_adjust = minfirstline + opt_vertical_offset;
         }
      }
   }

   var.key = "puae_horizontal_pos";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "auto") == 0)
      {
         opt_horizontal_offset_auto = true;
      }
      else
      {
         opt_horizontal_offset_auto = false;
         int new_horizontal_offset = atoi(var.value);
         if (new_horizontal_offset >= -30 && new_horizontal_offset <= 30)
         {
            opt_horizontal_offset = new_horizontal_offset;
            visible_left_border = max_diwlastword - retrow - opt_horizontal_offset;
         }
      }
   }

   var.key = "puae_use_whdload";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0) opt_use_whdload_hdf = true;
      if (strcmp(var.value, "disabled") == 0) opt_use_whdload_hdf = false;
   }

   var.key = "puae_use_whdsaves";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0) opt_use_whdsaves_hdf = true;
      if (strcmp(var.value, "disabled") == 0) opt_use_whdsaves_hdf = false;
   }

   var.key = "puae_use_whdload_prefs";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_use_whdload_prefs = 0;
      if (strcmp(var.value, "config") == 0) opt_use_whdload_prefs = 1;
      if (strcmp(var.value, "splash") == 0) opt_use_whdload_prefs = 2;
      if (strcmp(var.value, "both") == 0) opt_use_whdload_prefs = 3;
   }

   var.key = "puae_shared_nvram";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0) opt_shared_nvram = true;
      if (strcmp(var.value, "disabled") == 0) opt_shared_nvram = false;
   }

   var.key = "puae_analogmouse";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_analogmouse=0;
      else if (strcmp(var.value, "left") == 0) opt_analogmouse=1;
      else if (strcmp(var.value, "right") == 0) opt_analogmouse=2;
      else if (strcmp(var.value, "both") == 0) opt_analogmouse=3;
   }

   var.key = "puae_analogmouse_deadzone";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_analogmouse_deadzone = atoi(var.value);
   }

   var.key = "puae_analogmouse_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_analogmouse_speed = atof(var.value);
   }

   var.key = "puae_dpadmouse_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_dpadmouse_speed = atoi(var.value);
   }

   var.key = "puae_multimouse";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_multimouse=false;
      else if (strcmp(var.value, "enabled") == 0) opt_multimouse=true;
   }

   var.key = "puae_keyrah_keypad_mappings";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_keyrahkeypad=false;
      else if (strcmp(var.value, "enabled") == 0) opt_keyrahkeypad=true;
   }

   var.key = "puae_physical_keyboard_pass_through";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_keyboard_pass_through=false;
      else if (strcmp(var.value, "enabled") == 0) opt_keyboard_pass_through=true;
   }

   var.key = "puae_mapping_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_mapping_options_display=0;
      else if (strcmp(var.value, "enabled") == 0) opt_mapping_options_display=1;
   }

   var.key = "puae_video_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_video_options_display=0;
      else if (strcmp(var.value, "enabled") == 0) opt_video_options_display=1;
   }

   var.key = "puae_audio_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_audio_options_display=0;
      else if (strcmp(var.value, "enabled") == 0) opt_audio_options_display=1;
   }

   var.key = "puae_turbo_fire_button";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) turbo_fire_button=-1;
      else if (strcmp(var.value, "A") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_A;
      else if (strcmp(var.value, "Y") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_Y;
      else if (strcmp(var.value, "X") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_X;
      else if (strcmp(var.value, "L") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_L;
      else if (strcmp(var.value, "R") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_R;
      else if (strcmp(var.value, "L2") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_L2;
      else if (strcmp(var.value, "R2") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_R2;
      else if (strcmp(var.value, "L3") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_L3;
      else if (strcmp(var.value, "R3") == 0) turbo_fire_button=RETRO_DEVICE_ID_JOYPAD_R3;
   }

   var.key = "puae_turbo_pulse";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "2") == 0) turbo_pulse=2;
      else if (strcmp(var.value, "4") == 0) turbo_pulse=4;
      else if (strcmp(var.value, "6") == 0) turbo_pulse=6;
      else if (strcmp(var.value, "8") == 0) turbo_pulse=8;
      else if (strcmp(var.value, "10") == 0) turbo_pulse=10;
      else if (strcmp(var.value, "12") == 0) turbo_pulse=12;
   }

   /* Mapper */
   var.key = "puae_mapper_select";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT] = keyId(var.value);
   }

   var.key = "puae_mapper_start";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_START] = keyId(var.value);
   }

   var.key = "puae_mapper_a";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_A] = keyId(var.value);
   }

   var.key = "puae_mapper_y";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_Y] = keyId(var.value);
   }

   var.key = "puae_mapper_x";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_X] = keyId(var.value);
   }

   var.key = "puae_mapper_l";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_L] = keyId(var.value);
   }

   var.key = "puae_mapper_r";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_R] = keyId(var.value);
   }

   var.key = "puae_mapper_l2";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2] = keyId(var.value);
   }

   var.key = "puae_mapper_r2";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2] = keyId(var.value);
   }

   var.key = "puae_mapper_l3";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3] = keyId(var.value);
   }

   var.key = "puae_mapper_r3";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3] = keyId(var.value);
   }

   var.key = "puae_mapper_lr";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[16] = keyId(var.value);
   }

   var.key = "puae_mapper_ll";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[17] = keyId(var.value);
   }

   var.key = "puae_mapper_ld";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[18] = keyId(var.value);
   }

   var.key = "puae_mapper_lu";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[19] = keyId(var.value);
   }

   var.key = "puae_mapper_rr";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[20] = keyId(var.value);
   }

   var.key = "puae_mapper_rl";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[21] = keyId(var.value);
   }

   var.key = "puae_mapper_rd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[22] = keyId(var.value);
   }

   var.key = "puae_mapper_ru";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[23] = keyId(var.value);
   }

   /* Mapper hotkeys */
   var.key = "puae_mapper_vkbd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[24] = keyId(var.value);
   }

   var.key = "puae_mapper_statusbar";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[25] = keyId(var.value);
   }

   var.key = "puae_mapper_mouse_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[26] = keyId(var.value);
   }

   var.key = "puae_mapper_reset";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[27] = keyId(var.value);
   }

   var.key = "puae_mapper_aspect_ratio_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[28] = keyId(var.value);
   }

   var.key = "puae_mapper_zoom_mode_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[29] = keyId(var.value);
   }

   /*** Options display ***/

   /* Mapping options */
   option_display.visible = opt_mapping_options_display;

   option_display.key = "puae_mapper_select";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_start";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_a";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_y";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_x";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_l";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_r";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_l2";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_r2";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_l3";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_r3";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_lu";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_ld";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_ll";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_lr";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_ru";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_rd";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_rl";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_rr";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_vkbd";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_statusbar";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_mouse_toggle";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_reset";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_aspect_ratio_toggle";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_zoom_mode_toggle";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);

   /* Audio options */
   option_display.visible = opt_audio_options_display;

   option_display.key = "puae_sound_output";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_sound_stereo_separation";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_sound_interpol";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_sound_filter";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_sound_filter_type";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_sound_volume_cd";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_floppy_sound";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_floppy_sound_type";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);

   /* Video options */
   option_display.visible = opt_video_options_display;

   option_display.key = "puae_video_resolution";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_allow_hz_change";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_standard";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_aspect";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_zoom_mode";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_vertical_pos";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_horizontal_pos";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_gfx_colors";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_collision_level";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_immediate_blits";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_gfx_framerate";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_statusbar";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_vkbd_alpha";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);

   /* Setting resolution */
   switch (video_config)
   {
		case PUAE_VIDEO_PAL_LO:
			defaultw = PUAE_VIDEO_WIDTH / 2;
			defaulth = PUAE_VIDEO_HEIGHT_PAL / 2;
			strcat(uae_config, "gfx_resolution=lores\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_PAL_HI:
			defaultw = PUAE_VIDEO_WIDTH;
			defaulth = PUAE_VIDEO_HEIGHT_PAL / 2;
			strcat(uae_config, "gfx_resolution=hires\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_PAL_HI_DL:
			defaultw = PUAE_VIDEO_WIDTH;
			defaulth = PUAE_VIDEO_HEIGHT_PAL;
			strcat(uae_config, "gfx_resolution=hires\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
		case PUAE_VIDEO_PAL_SUHI:
			defaultw = PUAE_VIDEO_WIDTH * 2;
			defaulth = PUAE_VIDEO_HEIGHT_PAL / 2;
			strcat(uae_config, "gfx_resolution=superhires\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_PAL_SUHI_DL:
			defaultw = PUAE_VIDEO_WIDTH * 2;
			defaulth = PUAE_VIDEO_HEIGHT_PAL;
			strcat(uae_config, "gfx_resolution=superhires\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;

		case PUAE_VIDEO_NTSC_LO:
			defaultw = PUAE_VIDEO_WIDTH / 2;
			defaulth = PUAE_VIDEO_HEIGHT_NTSC / 2;
			strcat(uae_config, "gfx_resolution=lores\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_NTSC_HI:
			defaultw = PUAE_VIDEO_WIDTH;
			defaulth = PUAE_VIDEO_HEIGHT_NTSC / 2;
			strcat(uae_config, "gfx_resolution=hires\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_NTSC_HI_DL:
			defaultw = PUAE_VIDEO_WIDTH;
			defaulth = PUAE_VIDEO_HEIGHT_NTSC;
			strcat(uae_config, "gfx_resolution=hires\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
		case PUAE_VIDEO_NTSC_SUHI:
			defaultw = PUAE_VIDEO_WIDTH * 2;
			defaulth = PUAE_VIDEO_HEIGHT_NTSC / 2;
			strcat(uae_config, "gfx_resolution=superhires\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_NTSC_SUHI_DL:
			defaultw = PUAE_VIDEO_WIDTH * 2;
			defaulth = PUAE_VIDEO_HEIGHT_NTSC;
			strcat(uae_config, "gfx_resolution=superhires\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
   }

   /* Always update av_info geometry */
   request_update_av_info = true;

   /* Always trigger audio and custom change */
   config_changed = 1;
   check_prefs_changed_audio();
   check_prefs_changed_custom();
   check_prefs_changed_cpu();
   config_changed = 0;
}

//*****************************************************************************
//*****************************************************************************
// Disk control
extern void DISK_reinsert(int num);
extern void disk_eject (int num);

static bool disk_set_eject_state(bool ejected)
{
   if (dc)
   {
      if (dc->eject_state == ejected)
         return true;
      else
         dc->eject_state = ejected;

      if (dc->eject_state)
      {
         if (dc->files[dc->index] > 0)
         {
            if (strendswith(dc->files[dc->index], ADF_FILE_EXT)
             || strendswith(dc->files[dc->index], ADZ_FILE_EXT)
             || strendswith(dc->files[dc->index], FDI_FILE_EXT)
             || strendswith(dc->files[dc->index], DMS_FILE_EXT)
             || strendswith(dc->files[dc->index], IPF_FILE_EXT))
            {
               changed_prefs.floppyslots[0].df[0] = 0;
               disk_eject(0);
            }
            else if (strendswith(dc->files[dc->index], CUE_FILE_EXT)
                  || strendswith(dc->files[dc->index], CCD_FILE_EXT)
                  || strendswith(dc->files[dc->index], NRG_FILE_EXT)
                  || strendswith(dc->files[dc->index], MDS_FILE_EXT)
                  || strendswith(dc->files[dc->index], ISO_FILE_EXT))
            {
               changed_prefs.cdslots[0].name[0] = 0;
               check_changes(0);
            }
         }
      }
      else
      {
         if (dc->files[dc->index] > 0 && file_exists(dc->files[dc->index]))
         {
            if (strendswith(dc->files[dc->index], ADF_FILE_EXT)
             || strendswith(dc->files[dc->index], ADZ_FILE_EXT)
             || strendswith(dc->files[dc->index], FDI_FILE_EXT)
             || strendswith(dc->files[dc->index], DMS_FILE_EXT)
             || strendswith(dc->files[dc->index], IPF_FILE_EXT))
            {
               strcpy (changed_prefs.floppyslots[0].df, dc->files[dc->index]);
               DISK_reinsert(0);
            }
            else if (strendswith(dc->files[dc->index], CUE_FILE_EXT)
                  || strendswith(dc->files[dc->index], CCD_FILE_EXT)
                  || strendswith(dc->files[dc->index], NRG_FILE_EXT)
                  || strendswith(dc->files[dc->index], MDS_FILE_EXT)
                  || strendswith(dc->files[dc->index], ISO_FILE_EXT))
            {
               strcpy (changed_prefs.cdslots[0].name, dc->files[dc->index]);
               check_changes(0);
            }
         }
      }
   }
   return true;
}

static bool disk_get_eject_state(void)
{
   if (dc)
      return dc->eject_state;

   return true;
}

static unsigned disk_get_image_index(void)
{
   if (dc)
      return dc->index;

   return 0;
}

static bool disk_set_image_index(unsigned index)
{
   // Insert disk
   if (dc)
   {
      // Same disk...
      // This can mess things in the emu
      if (index == dc->index)
         return true;

      if ((index < dc->count) && (dc->files[index]))
      {
         dc->index = index;
         fprintf(stdout, "[libretro-uae]: Disk (%d) inserted into drive DF0: '%s'\n", dc->index+1, dc->files[dc->index]);
         return true;
      }
   }

   return false;
}

static unsigned disk_get_num_images(void)
{
   if (dc)
      return dc->count;

   return 0;
}

static bool disk_replace_image_index(unsigned index, const struct retro_game_info *info)
{
   if (dc)
   {
      if (index >= dc->count)
         return false;

      if (dc->files[index])
      {
         free(dc->files[index]);
         dc->files[index] = NULL;
      }

      if (dc->labels[index])
      {
         free(dc->labels[index]);
         dc->labels[index] = NULL;
      }

      // TODO : Handling removing of a disk image when info = NULL
      if (info != NULL)
      {
         if (!string_is_empty(info->path))
         {
            char image_label[RETRO_PATH_MAX];

            image_label[0] = '\0';

            // File path
            dc->files[index] = strdup(info->path);

            // Image label
            fill_short_pathname_representation(image_label, info->path, sizeof(image_label));
            dc->labels[index] = strdup(image_label);

            return true;
         }
      }
   }

   return false;
}

static bool disk_add_image_index(void)
{
   if (dc)
   {
      if (dc->count <= DC_MAX_SIZE)
      {
         dc->files[dc->count]  = NULL;
         dc->labels[dc->count] = NULL;
         dc->count++;
         return true;
      }
   }

   return false;
}

static bool disk_get_image_path(unsigned index, char *path, size_t len)
{
   if (len < 1)
      return false;

   if (dc)
   {
      if (index < dc->count)
      {
         if (!string_is_empty(dc->files[index]))
         {
            strlcpy(path, dc->files[index], len);
            return true;
         }
      }
   }

   return false;
}

static bool disk_get_image_label(unsigned index, char *label, size_t len)
{
   if (len < 1)
      return false;

   if (dc)
   {
      if (index < dc->count)
      {
         if (!string_is_empty(dc->labels[index]))
         {
            strlcpy(label, dc->labels[index], len);
            return true;
         }
      }
   }

   return false;
}

static struct retro_disk_control_callback disk_interface = {
   disk_set_eject_state,
   disk_get_eject_state,
   disk_get_image_index,
   disk_set_image_index,
   disk_get_num_images,
   disk_replace_image_index,
   disk_add_image_index,
};

static struct retro_disk_control_ext_callback disk_interface_ext = {
   disk_set_eject_state,
   disk_get_eject_state,
   disk_get_image_index,
   disk_set_image_index,
   disk_get_num_images,
   disk_replace_image_index,
   disk_add_image_index,
   NULL, // set_initial_image
   disk_get_image_path,
   disk_get_image_label,
};

//*****************************************************************************
//*****************************************************************************
// Init
void retro_init(void)
{
   const char *system_dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
   {
      // if defined, use the system directory
      strlcpy(
            retro_system_directory,
            system_dir,
            sizeof(retro_system_directory));
   }

   const char *content_dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
   {
      // if defined, use the system directory
      strlcpy(
            retro_content_directory,
            content_dir,
            sizeof(retro_content_directory));
   }

   const char *save_dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
   {
      // If save directory is defined use it, otherwise use system directory
      strlcpy(
            retro_save_directory,
            string_is_empty(save_dir) ? retro_system_directory : save_dir,
            sizeof(retro_save_directory));
   }
   else
   {
      // make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend
      strlcpy(
            retro_save_directory,
            retro_system_directory,
            sizeof(retro_save_directory));
   }

   //printf("Retro SYSTEM_DIRECTORY %s\n",retro_system_directory);
   //printf("Retro SAVE_DIRECTORY %s\n",retro_save_directory);
   //printf("Retro CONTENT_DIRECTORY %s\n",retro_content_directory);

   // Disk control interface
   dc = dc_create();

   unsigned dci_version = 0;
   if (environ_cb(RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION, &dci_version) && (dci_version >= 1))
      environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE, &disk_interface_ext);
   else
      environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE, &disk_interface);

   // Savestates
   static uint64_t quirks = RETRO_SERIALIZATION_QUIRK_INCOMPLETE | RETRO_SERIALIZATION_QUIRK_CORE_VARIABLE_SIZE;
   environ_cb(RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS, &quirks);

   // Inputs
   #define RETRO_DESCRIPTOR_BLOCK( _user )                                            \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "A / 2nd fire / Blue" },\
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "B / Fire / Red" },  \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "X / Yellow" },      \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Y / Green" },       \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },     \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start / Play" },\
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },       \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },         \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },             \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },         \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "R / Forward" },         \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "L / Rewind" },         \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "R2" },             \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "L2" },             \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "R3" },             \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "L3" },             \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X, "Left Analog X" },               \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y, "Left Analog Y" },               \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, "Right Analog X" },             \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, "Right Analog Y" }

   static struct retro_input_descriptor input_descriptors[] =
   {
      RETRO_DESCRIPTOR_BLOCK( 0 ),
      RETRO_DESCRIPTOR_BLOCK( 1 ),
      RETRO_DESCRIPTOR_BLOCK( 2 ),
      RETRO_DESCRIPTOR_BLOCK( 3 ),
      { 0 },
   };
   #undef RETRO_DESCRIPTOR_BLOCK
   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &input_descriptors);

   memset(key_state, 0, sizeof(key_state));
   memset(key_state2, 0, sizeof(key_state2));

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      fprintf(stderr, "[libretro-uae]: RGB565 is not supported.\n");
      exit(0);//return false;
   }

   memset(retro_bmp, 0, sizeof(retro_bmp));

   update_variables();
}

void retro_deinit(void)
{	
	// Clean the m3u storage
	if (dc)
		dc_free(dc);
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   if (port<4)
   {
      uae_devices[port]=device;
      int uae_port;
      uae_port = (port==0) ? 1 : 0;
      cd32_pad_enabled[uae_port]=0;
      switch (device)
      {
         case RETRO_DEVICE_JOYPAD:
            fprintf(stdout, "[libretro-uae]: Controller %u: RetroPad\n", (port+1));
            break;

         case RETRO_DEVICE_UAE_CD32PAD:
            fprintf(stdout, "[libretro-uae]: Controller %u: CD32 Pad\n", (port+1));
            cd32_pad_enabled[uae_port]=1;
            break;

         case RETRO_DEVICE_UAE_JOYSTICK:
            fprintf(stdout, "[libretro-uae]: Controller %u: Joystick\n", (port+1));
            break;

         case RETRO_DEVICE_UAE_KEYBOARD:
            fprintf(stdout, "[libretro-uae]: Controller %u: Keyboard\n", (port+1));
            break;

         case RETRO_DEVICE_NONE:
            fprintf(stdout, "[libretro-uae]: Controller %u: Unplugged\n", (port+1));
            break;
      }

      /* After startup input_get_default_joystick will need to be refreshed for cd32<>joystick change to work.
         Doing updateconfig straight from boot will crash, hence inputdevice_finalized */
      if (inputdevice_finalized)
         inputdevice_updateconfig(NULL, &currprefs);
   }
}

void retro_get_system_info(struct retro_system_info *info)
{
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
   memset(info, 0, sizeof(*info));
   info->library_name     = "PUAE";
   info->library_version  = "2.6.1" GIT_VERSION;
   info->need_fullpath    = true;
   info->block_extract    = false;	
   info->valid_extensions = "adf|adz|dms|fdi|ipf|hdf|hdz|lha|cue|ccd|nrg|mds|iso|uae|m3u|zip";
}

float retro_get_aspect_ratio(int w, int h, int video_config_geometry)
{
   static float ar = 1;

   if (video_config_geometry & PUAE_VIDEO_NTSC)
      ar = ((float)w / (float)h) * (44.0 / 52.0);
   else
      ar = ((float)w / (float)h);

   if (video_config_geometry & PUAE_VIDEO_DOUBLELINE)
   {
      if (video_config_geometry & PUAE_VIDEO_HIRES)
         ;
      else if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
         ar = ar / 2;
   }
   else
   {
      if (video_config_geometry & PUAE_VIDEO_HIRES)
         ar = ar / 2;
      else if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
         ar = ar / 4;
   }

   return ar;
}

bool retro_update_av_info(bool change_geometry, bool change_timing, bool isntsc)
{
   bool av_log = false;
   request_update_av_info = false;
   float hz = currprefs.chipset_refreshrate;
   if (av_log)
      fprintf(stdout, "[libretro-uae]: Trying to update AV geometry:%d timing:%d, to: ntsc:%d hz:%0.4f, from video_config:%d, video_aspect:%d\n", change_geometry, change_timing, isntsc, hz, video_config, video_config_aspect);

   /* Change PAL/NTSC with a twist, thanks to Dyna Blaster

      Early Startup switch looks proper:
         PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
         NTSC mode V=59.8859Hz H=15590.7473Hz (227x262+1) IDX=11 (NTSC) D=0 RTG=0/0
         PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0

      Dyna Blaster switch looks unorthodox:
         PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
         PAL mode V=59.4106Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
         PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
   */

   video_config_old = video_config;
   video_config_geometry = video_config;

   /* When timing & geometry is changed */
   if (change_timing)
   {
      /* Change to NTSC if not NTSC */
      if (isntsc && (video_config & PUAE_VIDEO_PAL) && !fake_ntsc)
      {
         video_config |= PUAE_VIDEO_NTSC;
         video_config &= ~PUAE_VIDEO_PAL;
      }
      /* Change to PAL if not PAL */
      else if (!isntsc && (video_config & PUAE_VIDEO_NTSC) && !fake_ntsc)
      {
         video_config |= PUAE_VIDEO_PAL;
         video_config &= ~PUAE_VIDEO_NTSC;
      }

      /* Main video config will be changed too */
      video_config_geometry = video_config;
   }

   /* Aspect ratio override always changes only temporary video config */
   if (video_config_aspect == PUAE_VIDEO_NTSC)
   {
      video_config_geometry |= PUAE_VIDEO_NTSC;
      video_config_geometry &= ~PUAE_VIDEO_PAL;
   }
   else if (video_config_aspect == PUAE_VIDEO_PAL)
   {
      video_config_geometry |= PUAE_VIDEO_PAL;
      video_config_geometry &= ~PUAE_VIDEO_NTSC;
   }

   /* Do nothing if timing has not changed, unless Hz switched without isntsc */
   if (video_config_old == video_config && change_timing)
   {
      /* Dyna Blaster and the like stays at fake NTSC to prevent pointless switching back and forth */
      if (!isntsc && hz > 55)
      {
         video_config |= PUAE_VIDEO_NTSC;
         video_config &= ~PUAE_VIDEO_PAL;
         video_config_geometry = video_config;
         fake_ntsc=true;
      }

      /* If still no change */
      if (video_config_old == video_config)
      {
         if (av_log)
            fprintf(stdout, "[libretro-uae]: Already at wanted AV\n");
         change_timing = false; // Allow other calculations but don't alter timing
      }
   }

   /* Horizontal centering thresholds */
   static int min_diwstart_limit_hires = 220;
   static int max_diwstop_limit_hires = 600;
   static int min_diwstart_limit = 220;
   static int max_diwstop_limit = 600;

   /* Geometry dimensions */
   switch (video_config_geometry)
   {
      case PUAE_VIDEO_PAL_LO:
         retrow = PUAE_VIDEO_WIDTH / 2;
         retroh = PUAE_VIDEO_HEIGHT_PAL / 2;
         min_diwstart_limit = min_diwstart_limit_hires / 2;
         max_diwstop_limit = max_diwstop_limit_hires / 2;
         break;
      case PUAE_VIDEO_PAL_HI:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_PAL / 2;
         min_diwstart_limit = min_diwstart_limit_hires;
         max_diwstop_limit = max_diwstop_limit_hires;
         break;
      case PUAE_VIDEO_PAL_HI_DL:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_PAL;
         min_diwstart_limit = min_diwstart_limit_hires;
         max_diwstop_limit = max_diwstop_limit_hires;
         break;
      case PUAE_VIDEO_PAL_SUHI:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_PAL / 2;
         min_diwstart_limit = min_diwstart_limit_hires * 2;
         max_diwstop_limit = max_diwstop_limit_hires * 2;
         break;
      case PUAE_VIDEO_PAL_SUHI_DL:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_PAL;
         min_diwstart_limit = min_diwstart_limit_hires * 2;
         max_diwstop_limit = max_diwstop_limit_hires * 2;
         break;

      case PUAE_VIDEO_NTSC_LO:
         retrow = PUAE_VIDEO_WIDTH / 2;
         retroh = PUAE_VIDEO_HEIGHT_NTSC / 2;
         min_diwstart_limit = min_diwstart_limit_hires / 2;
         max_diwstop_limit = max_diwstop_limit_hires / 2;
         break;
      case PUAE_VIDEO_NTSC_HI:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_NTSC / 2;
         min_diwstart_limit = min_diwstart_limit_hires;
         max_diwstop_limit = max_diwstop_limit_hires;
         break;
      case PUAE_VIDEO_NTSC_HI_DL:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_NTSC;
         min_diwstart_limit = min_diwstart_limit_hires;
         max_diwstop_limit = max_diwstop_limit_hires;
         break;
      case PUAE_VIDEO_NTSC_SUHI:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_NTSC / 2;
         min_diwstart_limit = min_diwstart_limit_hires * 2;
         max_diwstop_limit = max_diwstop_limit_hires * 2;
         break;
      case PUAE_VIDEO_NTSC_SUHI_DL:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_NTSC;
         min_diwstart_limit = min_diwstart_limit_hires * 2;
         max_diwstop_limit = max_diwstop_limit_hires * 2;
         break;
   }

   /* When the actual dimensions change and not just the view */
   if (change_timing)
   {
      defaultw = retrow;
      defaulth = retroh;
   }

   static struct retro_system_av_info new_av_info;
   new_av_info.geometry.base_width = retrow;
   new_av_info.geometry.base_height = retroh;
   new_av_info.geometry.aspect_ratio = retro_get_aspect_ratio(retrow, retroh, video_config_geometry);

   /* Disable Hz change if not allowed */
   if (!video_config_allow_hz_change)
      change_timing = 0;

   /* Logging */
   if (av_log)
   {
      if (change_geometry && change_timing) {
         fprintf(stdout, "[libretro-uae]: Update av_info: %dx%d %0.4fHz, video_config:%d\n", retrow, retroh, hz, video_config_geometry);
      } else if (change_geometry && !change_timing) {
         fprintf(stdout, "[libretro-uae]: Update geometry: %dx%d, video_config:%d\n", retrow, retroh, video_config_geometry);
      } else if (!change_geometry && change_timing) {
         fprintf(stdout, "[libretro-uae]: Update timing: %0.4fHz, video_config:%d\n", hz, video_config_geometry);
      }
   }

   if (change_timing) {
      struct retro_system_av_info new_timing;
      retro_get_system_av_info(&new_timing);
      new_timing.timing.fps = hz;
      environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &new_timing);
   }

   if (change_geometry) {
      environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &new_av_info);

      /* Ensure statusbar stays visible at the bottom */
      opt_statusbar_position_offset = 0;
      opt_statusbar_position = opt_statusbar_position_old;
      if (!change_timing)
         if (retroh < defaulth)
            if (opt_statusbar_position >= 0 && (defaulth - retroh) > opt_statusbar_position)
               opt_statusbar_position = defaulth - retroh;

      /* Aspect offset for zoom mode */
      opt_statusbar_position_offset = opt_statusbar_position_old - opt_statusbar_position;

      /* Compensate for the last line in PAL HIRES */
      if (video_config_geometry & PUAE_VIDEO_PAL && video_config_geometry & PUAE_VIDEO_DOUBLELINE && retroh == defaulth && opt_statusbar_position >= 0)
         opt_statusbar_position += 1;

      //fprintf(stdout, "statusbar:%3d old:%3d offset:%3d, retroh:%d defaulth:%d\n", opt_statusbar_position, opt_statusbar_position_old, opt_statusbar_position_offset, retroh, defaulth);
   }

   /* Apply zoom mode if necessary */
   switch (zoom_mode_id)
   {
      case 1:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 480 : 540;
         else
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 240 : 270;
         break;
      case 2:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 474 : 524;
         else
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 237 : 262;
         break;
      case 3:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 470 : 512;
         else
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 235 : 256;
         break;
      case 4:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 460 : 480;
         else
            zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 230 : 240;
         break;
      case 5:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = 448;
         else
            zoomed_height = 224;
         break;
      case 6:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = 432;
         else
            zoomed_height = 216;
         break;
      case 7:
         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = 400;
         else
            zoomed_height = 200;
         break;
      case 8:
         if (retro_thisframe_first_drawn_line != retro_thisframe_last_drawn_line
          && retro_thisframe_first_drawn_line > 0 && retro_thisframe_last_drawn_line > 0
         )
         {
            zoomed_height = retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line + 1;
            zoomed_height = (video_config & PUAE_VIDEO_DOUBLELINE) ? zoomed_height * 2 : zoomed_height;
         }

         if (video_config & PUAE_VIDEO_DOUBLELINE)
            zoomed_height = (zoomed_height < 400) ? 400 : zoomed_height;
         else
            zoomed_height = (zoomed_height < 200) ? 200 : zoomed_height;
         break;
      default:
         zoomed_height = retroh;
         if (fake_ntsc)
            zoomed_height = 460;
         break;
   }

   if (zoomed_height > retroh)
      zoomed_height = retroh;

   if (zoomed_height != retroh)
   {
      new_av_info.geometry.base_height = zoomed_height;
      new_av_info.geometry.aspect_ratio = retro_get_aspect_ratio(retrow, zoomed_height, video_config_geometry);
      environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &new_av_info);

      /* Ensure statusbar stays visible at the bottom */
      if (opt_statusbar_position >= 0 && (retroh - zoomed_height - opt_statusbar_position_offset) > opt_statusbar_position)
         opt_statusbar_position = retroh - zoomed_height - opt_statusbar_position_offset;

      /* Exception for Dyna Blaster */
      if (fake_ntsc)
         opt_statusbar_position -= (retroh - defaulth);

      //fprintf(stdout, "ztatusbar:%3d old:%3d offset:%3d, retroh:%d defaulth:%d\n", opt_statusbar_position, opt_statusbar_position_old, opt_statusbar_position_offset, retroh, defaulth);
   }

   /* If zoom mode should be vertically centered automagically */
   if (opt_vertical_offset_auto && (zoom_mode_id != 0 || zoomed_height != retroh) && !firstpass)
   {
      int zoomed_height_normal = (video_config & PUAE_VIDEO_DOUBLELINE) ? zoomed_height / 2 : zoomed_height;
      int thisframe_y_adjust_new = minfirstline;

      /* Need proper values for calculations */
      if (retro_thisframe_first_drawn_line != retro_thisframe_last_drawn_line
       && retro_thisframe_first_drawn_line > 0 && retro_thisframe_last_drawn_line > 0
       && (retro_thisframe_first_drawn_line < 150 || retro_thisframe_last_drawn_line > 150)
      )
         thisframe_y_adjust_new = (retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line - zoomed_height_normal) / 2 + retro_thisframe_first_drawn_line; // Smart
         //thisframe_y_adjust_new = retro_thisframe_first_drawn_line + ((retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line) - zoomed_height_normal) / 2; // Simple

      /* Sensible limits */
      thisframe_y_adjust_new = (thisframe_y_adjust_new < 0) ? 0 : thisframe_y_adjust_new;
      thisframe_y_adjust_new = (thisframe_y_adjust_new > (minfirstline + 50)) ? (minfirstline + 50) : thisframe_y_adjust_new;
      if (retro_thisframe_first_drawn_line == -1 && retro_thisframe_last_drawn_line == -1)
          thisframe_y_adjust_new = thisframe_y_adjust_old;

      /* Change value only if altered */
      if (thisframe_y_adjust != thisframe_y_adjust_new)
         thisframe_y_adjust = thisframe_y_adjust_new;

      //fprintf(stdout, "FIRSTDRAWN:%6d LASTDRAWN:%6d   yadjust:%d old:%d zoomed_height:%d\n", retro_thisframe_first_drawn_line, retro_thisframe_last_drawn_line, thisframe_y_adjust, thisframe_y_adjust_old, zoomed_height);

      /* Remember the previous value */
      thisframe_y_adjust_old = thisframe_y_adjust;
   }
   else
      thisframe_y_adjust = minfirstline + opt_vertical_offset;

   /* Horizontal centering */
   if (opt_horizontal_offset_auto && !firstpass)
   {
      int visible_left_border_new = max_diwlastword - retrow;
      int diw_multiplier = 1;
      if (video_config_geometry & PUAE_VIDEO_HIRES)
         diw_multiplier = 2;
      else if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
         diw_multiplier = 4;

      /* Need proper values for calculations */
      if (retro_min_diwstart != retro_max_diwstop
       && retro_min_diwstart > 0 && retro_max_diwstop > 0
       && retro_min_diwstart < min_diwstart_limit
       && retro_max_diwstop > max_diwstop_limit
       && (retro_max_diwstop - retro_min_diwstart) <= (retrow + 2*diw_multiplier)
      )
      {
         visible_left_border_new = (retro_max_diwstop - retro_min_diwstart - retrow) / 2 + retro_min_diwstart; // Smart
         //visible_left_border_new = retro_max_diwstop - retrow - (retro_max_diwstop - retro_min_diwstart - retrow) / 2; // Simple
      }
      else if (retro_min_diwstart == 30000 && retro_max_diwstop == 0)
         visible_left_border_new = visible_left_border;

      /* Change value only if altered */
      if (visible_left_border != visible_left_border_new)
         visible_left_border = visible_left_border_new;

      //fprintf(stdout, "DIWSTART  :%6d DIWSTOP  :%6d   left_border:%d old:%d\n", retro_min_diwstart, retro_max_diwstop, visible_left_border, visible_left_border_old);

      /* Remember the previous value */
      visible_left_border_old = visible_left_border;
   }

   /* No need to check changed gfx at startup */
   if (!firstpass)
      prefs_changed = 1; // Triggers check_prefs_changed_gfx() in vsync_handle_check()

   /* Changing any drawing/offset parameters requires
    * a drawing reset - it is safest to just do this
    * whenever retro_update_av_info() is called */
   request_reset_drawing = true;

   return true;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   /* need to do this here because core option values are not available in retro_init */
   if (!pix_bytes_initialized)
   {
      pix_bytes_initialized = true;
      if (pix_bytes == 4)
      {
         enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
         if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
         {
            pix_bytes = 2;
            fprintf(stderr, "[libretro-uae]: XRGB8888 is not supported. Trying RGB565\n");
            fmt = RETRO_PIXEL_FORMAT_RGB565;
            if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
            {
               fprintf(stderr, "[libretro-uae]: RGB565 is not supported\n");
               exit(0);//return false;
            }
         }
      }
   }

   static struct retro_game_geometry geom;
   geom.base_width = retrow;
   geom.base_height = retroh;
   geom.max_width = EMULATOR_DEF_WIDTH;
   geom.max_height = EMULATOR_DEF_HEIGHT;
   geom.aspect_ratio = retro_get_aspect_ratio(retrow, retroh, video_config_geometry);

   info->geometry = geom;
   info->timing.sample_rate = 44100.0;
   info->timing.fps = (retro_get_region() == RETRO_REGION_NTSC) ? PUAE_VIDEO_HZ_NTSC : PUAE_VIDEO_HZ_PAL;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_audio_cb(short l, short r)
{
   audio_cb(l, r);
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_audio_batch_cb(const int16_t *data, size_t frames)
{
   audio_batch_cb(data, frames);
}

bool retro_create_config()
{
   RPATH[0] = '\0';
   path_join((char*)&RPATH, retro_save_directory, LIBRETRO_PUAE_PREFIX ".uae");
   fprintf(stdout, "[libretro-uae]: Generating temporary config file '%s'\n", (const char*)&RPATH);

   if (strcmp(opt_model, "A500") == 0)
   {
      strcat(uae_machine, A500_CONFIG);
      strcpy(uae_kickstart, A500_ROM);
   }
   else if (strcmp(opt_model, "A500OG") == 0)
   {
      strcat(uae_machine, A500OG_CONFIG);
      strcpy(uae_kickstart, A500_ROM);
   }
   else if (strcmp(opt_model, "A500PLUS") == 0)
   {
      strcat(uae_machine, A500PLUS_CONFIG);
      strcpy(uae_kickstart, A500KS2_ROM);
   }
   else if (strcmp(opt_model, "A600") == 0)
   {
      strcat(uae_machine, A600_CONFIG);
      strcpy(uae_kickstart, A600_ROM);
   }
   else if (strcmp(opt_model, "A1200") == 0)
   {
      strcat(uae_machine, A1200_CONFIG);
      strcpy(uae_kickstart, A1200_ROM);
   }
   else if (strcmp(opt_model, "A1200OG") == 0)
   {
      strcat(uae_machine, A1200OG_CONFIG);
      strcpy(uae_kickstart, A1200_ROM);
   }
   else if (strcmp(opt_model, "CD32") == 0)
   {
      strcat(uae_machine, CD32_CONFIG);
      strcpy(uae_kickstart, CD32_ROM);
      strcpy(uae_kickstart_ext, CD32_ROM_EXT);
   }
   else if (strcmp(opt_model, "CD32FR") == 0)
   {
      strcat(uae_machine, CD32FR_CONFIG);
      strcpy(uae_kickstart, CD32_ROM);
      strcpy(uae_kickstart_ext, CD32_ROM_EXT);
   }
   else if (strcmp(opt_model, "auto") == 0)
   {
      strcat(uae_machine, A500_CONFIG);
      strcpy(uae_kickstart, A500_ROM);
   }

   if (!string_is_empty(full_path))
   {
      // If argument is a disk or hard drive image file
      if (strendswith(full_path, ADF_FILE_EXT)
       || strendswith(full_path, ADZ_FILE_EXT)
       || strendswith(full_path, FDI_FILE_EXT)
       || strendswith(full_path, DMS_FILE_EXT)
       || strendswith(full_path, IPF_FILE_EXT)
       || strendswith(full_path, HDF_FILE_EXT)
       || strendswith(full_path, HDZ_FILE_EXT)
       || strendswith(full_path, LHA_FILE_EXT)
       || strendswith(full_path, M3U_FILE_EXT))
      {
	     // Open tmp config file
	     FILE * configfile;
	     if (configfile = fopen(RPATH, "w"))
	     {
	        char kickstart[RETRO_PATH_MAX];

            // If a machine was specified in the name of the game
            if (strstr(full_path, "(A1200OG)") != NULL || strstr(full_path, "(A1200NF)") != NULL)
            {
               // Use A1200 barebone
               fprintf(stdout, "[libretro-uae]: Found '(A1200OG)' or '(A1200NF)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting A1200 NoFast with Kickstart 3.1 r40.068\n");
               fprintf(configfile, A1200OG_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, A1200_ROM);
            }
            else if (strstr(full_path, "(A1200)") != NULL || strstr(full_path, "AGA") != NULL || strstr(full_path, "CD32") != NULL || strstr(full_path, "AmigaCD") != NULL)
            {
               // Use A1200
               fprintf(stdout, "[libretro-uae]: Found '(A1200)', 'AGA', 'CD32', or 'AmigaCD' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting A1200 with Kickstart 3.1 r40.068\n");
               fprintf(configfile, A1200_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, A1200_ROM);
            }
            else if (strstr(full_path, "(A600)") != NULL || strstr(full_path, "ECS") != NULL)
            {
               // Use A600
               fprintf(stdout, "[libretro-uae]: Found '(A600)' or 'ECS' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting A600 with Kickstart 3.1 r40.063\n");
               fprintf(configfile, A600_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, A600_ROM);
            }
            else if (strstr(full_path, "(A500+)") != NULL || strstr(full_path, "(A500PLUS)") != NULL)
            {
               // Use A500+
               fprintf(stdout, "[libretro-uae]: Found '(A500+)' or '(A500PLUS)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting A500+ with Kickstart 2.04 r37.175\n");
               fprintf(configfile, A500PLUS_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, A500KS2_ROM);
            }
            else if (strstr(full_path, "(A500OG)") != NULL || strstr(full_path, "(512K)") != NULL)
            {
               // Use A500 barebone
               fprintf(stdout, "[libretro-uae]: Found '(A500OG)' or '(512K)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting A500 512K with Kickstart 1.3 r34.005\n");
               fprintf(configfile, A500OG_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, A500_ROM);
            }
            else if (strstr(full_path, "(A500)") != NULL || strstr(full_path, "OCS") != NULL)
            {
               // Use A500
               fprintf(stdout, "[libretro-uae]: Found '(A500)' or 'OCS' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting A500 with Kickstart 1.3 r34.005\n");
               fprintf(configfile, A500_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, A500_ROM);
            }
            else
            {
               if (strcmp(opt_model, "auto") == 0)
               {
                  // Hard disk defaults to A600
                  if (  strendswith(full_path, HDF_FILE_EXT)
                     || strendswith(full_path, HDZ_FILE_EXT)
                     || strendswith(full_path, LHA_FILE_EXT))
                  {
                     uae_machine[0] = '\0';
                     strcat(uae_machine, A600_CONFIG);
                     strcpy(uae_kickstart, A600_ROM);
                  }
                  // Floppy disk defaults to A500
                  else
                  {
                     uae_machine[0] = '\0';
                     strcat(uae_machine, A500_CONFIG);
                     strcpy(uae_kickstart, A500_ROM);
                  }
               }

               // No machine specified
               fprintf(stdout, "[libretro-uae]: No machine specified in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting default configuration\n");
               fprintf(configfile, uae_machine);
               path_join((char*)&kickstart, retro_system_directory, uae_kickstart);
            }

            // Write common config
            fprintf(configfile, uae_config);

            // If region was specified in the name of the game
            if (strstr(full_path, "(NTSC)") != NULL)
            {
               fprintf(stdout, "[libretro-uae]: Found '(NTSC)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Forcing NTSC mode\n");
               fprintf(configfile, "ntsc=true\n");
               real_ntsc=true;
               forced_video=true;
            }
            else if (strstr(full_path, "(PAL)") != NULL)
            {
               fprintf(stdout, "[libretro-uae]: Found '(PAL)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Forcing PAL mode\n");
               fprintf(configfile, "ntsc=false\n");
               forced_video=true;
            }

            // Verify Kickstart
            if (!file_exists(kickstart))
            {
               // Kickstart ROM not found
               fprintf(stderr, "Kickstart ROM '%s' not found!\n", (const char*)&kickstart);
               fclose(configfile);
               return false;
            }

            fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

            // If argument is a hard drive image file
            if (strendswith(full_path, HDF_FILE_EXT)
             || strendswith(full_path, HDZ_FILE_EXT)
             || strendswith(full_path, LHA_FILE_EXT))
            {
               char *tmp_str = NULL;

               if (opt_use_whdload_hdf)
               {
                  // Init WHDLoad
                  char whdload_hdf[RETRO_PATH_MAX];
                  path_join((char*)&whdload_hdf, retro_system_directory, "WHDLoad.hdf");

                  // Verify WHDLoad
                  // Windows needs double backslashes when paths are in quotes, hence the string_replace_substring()
                  if (!file_exists(whdload_hdf))
                     path_join((char*)&whdload_hdf, retro_save_directory, "WHDLoad.hdf");
                  if (!file_exists(whdload_hdf))
                  {
                     fprintf(stdout, "[libretro-uae]: WHDLoad image file '%s' not found, attempting to create one\n", (const char*)&whdload_hdf);

                     char whdload_hdf_gz[RETRO_PATH_MAX];
                     path_join((char*)&whdload_hdf_gz, retro_save_directory, "WHDLoad.hdf.gz");

                     FILE *whdload_hdf_gz_fp;
                     if (whdload_hdf_gz_fp = fopen(whdload_hdf_gz, "wb"))
                     {
                        fwrite(___whdload_WHDLoad_hdf_gz, ___whdload_WHDLoad_hdf_gz_len, 1, whdload_hdf_gz_fp);
                        fclose(whdload_hdf_gz_fp);

                        struct gzFile_s *whdload_hdf_gz_fp;
                        if (whdload_hdf_gz_fp = gzopen(whdload_hdf_gz, "r"))
                        {
                           FILE *whdload_hdf_fp;
                           if (whdload_hdf_fp = fopen(whdload_hdf, "wb"))
                           {
                              gz_uncompress(whdload_hdf_gz_fp, whdload_hdf_fp);
                              fclose(whdload_hdf_fp);
                           }
                           gzclose(whdload_hdf_gz_fp);
                        }
                        remove(whdload_hdf_gz);
                     }
                     else
                        fprintf(stderr, "Error creating WHDLoad.hdf '%s'!\n", (const char*)&whdload_hdf);
                  }
                  if (file_exists(whdload_hdf))
                  {
                     tmp_str = string_replace_substring(whdload_hdf, "\\", "\\\\");
                     fprintf(configfile, "hardfile2=rw,WHDLoad:\"%s\",32,1,2,512,0,,uae0\n", (const char*)tmp_str);
                     free(tmp_str);
                     tmp_str = NULL;
                  }
                  else
                     fprintf(stderr, "WHDLoad image file '%s' not found!\n", (const char*)&whdload_hdf);

                  // Attach game image
                  tmp_str = string_replace_substring(full_path, "\\", "\\\\");

                  if (strendswith(full_path, LHA_FILE_EXT))
                     fprintf(configfile, "filesystem2=ro,DH0:LHA:\"%s\",0\n", (const char*)tmp_str);
                  else
                     fprintf(configfile, "hardfile2=rw,DH0:\"%s\",32,1,2,512,0,,uae1\n", (const char*)tmp_str);

                  free(tmp_str);
                  tmp_str = NULL;


                  // Attach retro_system_directory as a read only hard drive for WHDLoad kickstarts/prefs/key
#ifdef WIN32
                  tmp_str = string_replace_substring(retro_system_directory, "\\", "\\\\");
                  fprintf(configfile, "filesystem2=ro,RASystem:RASystem:\"%s\",-128\n", (const char*)tmp_str);
                  free(tmp_str);
                  tmp_str = NULL;
#else
                  // Force the ending slash to make sure the path is not treated as a file
                  fprintf(configfile, "filesystem2=ro,RASystem:RASystem:\"%s%s\",-128\n", retro_system_directory, "/");
#endif

                  if (opt_use_whdsaves_hdf)
                  {
                     // Attach WHDSaves.hdf if available
                     char whdsaves_hdf[RETRO_PATH_MAX];
                     path_join((char*)&whdsaves_hdf, retro_system_directory, "WHDSaves.hdf");
                     if (!file_exists(whdsaves_hdf))
                        path_join((char*)&whdsaves_hdf, retro_save_directory, "WHDSaves.hdf");
                     if (!file_exists(whdsaves_hdf))
                     {
                        fprintf(stdout, "[libretro-uae]: WHDSaves image file '%s' not found, attempting to create one\n", (const char*)&whdsaves_hdf);

                        char whdsaves_hdf_gz[RETRO_PATH_MAX];
                        path_join((char*)&whdsaves_hdf_gz, retro_save_directory, "WHDSaves.hdf.gz");

                        FILE *whdsaves_hdf_gz_fp;
                        if (whdsaves_hdf_gz_fp = fopen(whdsaves_hdf_gz, "wb"))
                        {
                           fwrite(___whdload_WHDSaves_hdf_gz, ___whdload_WHDSaves_hdf_gz_len, 1, whdsaves_hdf_gz_fp);
                           fclose(whdsaves_hdf_gz_fp);

                           struct gzFile_s *whdsaves_hdf_gz_fp;
                           if (whdsaves_hdf_gz_fp = gzopen(whdsaves_hdf_gz, "r"))
                           {
                              FILE *whdsaves_hdf_fp;
                              if (whdsaves_hdf_fp = fopen(whdsaves_hdf, "wb"))
                              {
                                 gz_uncompress(whdsaves_hdf_gz_fp, whdsaves_hdf_fp);
                                 fclose(whdsaves_hdf_fp);
                              }
                              gzclose(whdsaves_hdf_gz_fp);
                           }
                           remove(whdsaves_hdf_gz);
                        }
                        else
                           fprintf(stderr, "Error creating WHDSaves.hdf '%s'!\n", (const char*)&whdsaves_hdf);
                     }
                     if (file_exists(whdsaves_hdf))
                     {
                        tmp_str = string_replace_substring(whdsaves_hdf, "\\", "\\\\");
                        fprintf(configfile, "hardfile2=rw,WHDSaves:\"%s\",32,1,2,512,0,,uae2\n", (const char*)tmp_str);
                        free(tmp_str);
                        tmp_str = NULL;
                     }
                  }
                  else
                  {
                     char whdsaves_path[RETRO_PATH_MAX];
                     path_join((char*)&whdsaves_path, retro_save_directory, "WHDSaves");
                     if (!path_is_directory(whdsaves_path))
                        path_mkdir(whdsaves_path);
                     if (path_is_directory(whdsaves_path))
                     {
                        tmp_str = string_replace_substring(whdsaves_path, "\\", "\\\\");
                        fprintf(configfile, "filesystem2=rw,WHDSaves:WHDSaves:\"%s\",-128\n", (const char*)tmp_str);
                        free(tmp_str);
                        tmp_str = NULL;
                     }
                     else
                        fprintf(stderr, "Error creating WHDSaves directory in '%s'!\n", (const char*)&whdsaves_path);
                  }

                  // Manipulate WHDLoad.prefs
                  int WHDLoad_ConfigDelay = 0;
                  int WHDLoad_SplashDelay = 0;

                  switch (opt_use_whdload_prefs)
                  {
                     case 1:
                        WHDLoad_ConfigDelay = -1;
                        break;
                     case 2:
                        WHDLoad_SplashDelay = 150;
                        break;
                     case 3:
                        WHDLoad_ConfigDelay = -1;
                        WHDLoad_SplashDelay = -1;
                        break;
                  }

                  FILE *whdload_prefs;
                  char whdload_prefs_path[RETRO_PATH_MAX];
                  path_join((char*)&whdload_prefs_path, retro_system_directory, "WHDLoad.prefs");

                  if (!file_exists(whdload_prefs_path))
                  {
                     fprintf(stdout, "[libretro-uae]: WHDLoad prefs '%s' not found, attempting to create one\n", (const char*)&whdload_prefs_path);

                     char whdload_prefs_gz[RETRO_PATH_MAX];
                     path_join((char*)&whdload_prefs_gz, retro_system_directory, "WHDLoad.prefs.gz");

                     FILE *whdload_prefs_gz_fp;
                     if (whdload_prefs_gz_fp = fopen(whdload_prefs_gz, "wb"))
                     {
                        fwrite(___whdload_WHDLoad_prefs_gz, ___whdload_WHDLoad_prefs_gz_len, 1, whdload_prefs_gz_fp);
                        fclose(whdload_prefs_gz_fp);

                        struct gzFile_s *whdload_prefs_gz_fp;
                        if (whdload_prefs_gz_fp = gzopen(whdload_prefs_gz, "r"))
                        {
                           FILE *whdload_prefs_fp;
                           if (whdload_prefs_fp = fopen(whdload_prefs_path, "wb"))
                           {
                              gz_uncompress(whdload_prefs_gz_fp, whdload_prefs_fp);
                              fclose(whdload_prefs_fp);
                           }
                           gzclose(whdload_prefs_gz_fp);
                        }
                        remove(whdload_prefs_gz);
                     }
                     else
                        fprintf(stderr, "Error creating WHDLoad prefs '%s'!\n", (const char*)&whdload_prefs_path);
                  }

                  FILE *whdload_prefs_new;
                  char whdload_prefs_new_path[RETRO_PATH_MAX];
                  path_join((char*)&whdload_prefs_new_path, retro_system_directory, "WHDLoad.prefs_new");

                  char whdload_prefs_backup_path[RETRO_PATH_MAX];
                  path_join((char*)&whdload_prefs_backup_path, retro_system_directory, "WHDLoad.prefs_backup");

                  char whdload_filebuf[4096];
                  if (whdload_prefs = fopen(whdload_prefs_path, "r"))
                  {
                     if (whdload_prefs_new = fopen(whdload_prefs_new_path, "w"))
                     {
                        while (fgets(whdload_filebuf, sizeof(whdload_filebuf), whdload_prefs))
                        {
                           if (strstr(whdload_filebuf, ";ConfigDelay=") || strstr(whdload_filebuf, ";SplashDelay="))
                              fprintf(whdload_prefs_new, whdload_filebuf);
                           else if (strstr(whdload_filebuf, "ConfigDelay="))
                              fprintf(whdload_prefs_new, "%s%d\n", "ConfigDelay=", WHDLoad_ConfigDelay);
                           else if (strstr(whdload_filebuf, "SplashDelay="))
                              fprintf(whdload_prefs_new, "%s%d\n", "SplashDelay=", WHDLoad_SplashDelay);
                           else
                              fprintf(whdload_prefs_new, whdload_filebuf);
                        }
                        fclose(whdload_prefs_new);
                        fclose(whdload_prefs);

                        // Remove backup config
                        remove(whdload_prefs_backup_path);

                        // Replace old and new config
                        rename(whdload_prefs_path, whdload_prefs_backup_path);
                        rename(whdload_prefs_new_path, whdload_prefs_path);
                     }
                     else
                     {
                        fprintf(stderr, "Error creating new WHDLoad.prefs '%s'!\n", (const char*)&whdload_prefs_new_path);
                        fclose(whdload_prefs);
                     }
                  }
                  else
                     fprintf(stderr, "WHDLoad.prefs '%s' not found!\n", (const char*)&whdload_prefs_path);
               }
               else
               {
                  tmp_str = string_replace_substring(full_path, "\\", "\\\\");
                  fprintf(configfile, "hardfile2=rw,DH0:\"%s\",32,1,2,512,0,,uae0\n", (const char*)tmp_str);
                  free(tmp_str);
                  tmp_str = NULL;
               }
            }
            else
            {
               // If argument is a m3u playlist
               if (strendswith(full_path, M3U_FILE_EXT))
               {
                  // Parse the m3u file
                  dc_parse_m3u(dc, full_path, retro_save_directory);

                  // Some debugging
                  fprintf(stdout, "[libretro-uae]: M3U file parsed, %d file(s) found\n", dc->count);
                  //for (unsigned i = 0; i < dc->count; i++)
                     //printf("File %d: %s\n", i+1, dc->files[i]);
               }
               else
               {
                  // Add the file to disk control context
                  char disk_image_label[RETRO_PATH_MAX];
                  disk_image_label[0] = '\0';

                  if (!string_is_empty(full_path))
                     fill_short_pathname_representation(
                           disk_image_label, full_path, sizeof(disk_image_label));

                  // Must reset disk control struct here,
                  // otherwise duplicate entries will be
                  // added when calling retro_reset()
                  dc_reset(dc);
                  dc_add_file(dc, full_path, disk_image_label);
               }

               // Init first disk
               dc->index = 0;
               dc->eject_state = false;
               fprintf(stdout, "[libretro-uae]: Disk (%d) inserted into drive DF0: '%s'\n", dc->index+1, dc->files[dc->index]);
               fprintf(configfile, "floppy0=%s\n", dc->files[0]);

               // Append rest of the disks to the config if m3u is a MultiDrive-m3u
               if (strstr(full_path, "(MD)") != NULL)
               {
                  for (unsigned i = 1; i < dc->count; i++)
                  {
                     dc->index = i;
                     if (i <= 3)
                     {
                        fprintf(stdout, "[libretro-uae]: Disk (%d) inserted into drive DF%d: '%s'\n", dc->index+1, i, dc->files[dc->index]);
                        fprintf(configfile, "floppy%d=%s\n", i, dc->files[i]);

                        // By default only DF0: is enabled, so floppyXtype needs to be set on the extra drives
                        if (i > 0)
                           fprintf(configfile, "floppy%dtype=%d\n", i, 0); // 0 = 3.5" DD
                     }
                     else
                     {
                        fprintf(stderr, "Too many disks for MultiDrive!\n");
                        fclose(configfile);
                        return false;
                     }
                  }
               }
            }
            fclose(configfile);
         }
         else
         {
            // Error
            fprintf(stderr, "Error while writing file '%s'!\n", (const char*)&RPATH);
            return false;
         }
      }
      // If argument is a CD image
      else if (strendswith(full_path, CUE_FILE_EXT)
            || strendswith(full_path, CCD_FILE_EXT)
            || strendswith(full_path, NRG_FILE_EXT)
            || strendswith(full_path, MDS_FILE_EXT)
            || strendswith(full_path, ISO_FILE_EXT))
      {
         // Open tmp config file
         FILE * configfile;
         if (configfile = fopen(RPATH, "w"))
         {
            char kickstart[RETRO_PATH_MAX];
            char kickstart_ext[RETRO_PATH_MAX];

            // If a machine was specified in the name of the game
            if (strstr(full_path, "(CD32FR)") != NULL || strstr(full_path, "FastRAM") != NULL)
            {
               // Use CD32 with Fast RAM
               fprintf(stdout, "[libretro-uae]: Found '(CD32FR)' or 'FastRAM' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting CD32 FastRAM with Kickstart 3.1 r40.060\n");
               fprintf(configfile, CD32FR_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, CD32_ROM);
               path_join((char*)&kickstart_ext, retro_system_directory, CD32_ROM_EXT);
            }
            else if (strstr(full_path, "(CD32)") != NULL || strstr(full_path, "(CD32NF)") != NULL)
            {
               // Use CD32 barebone
               fprintf(stdout, "[libretro-uae]: Found '(CD32)' or '(CD32NF)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting CD32 with Kickstart 3.1 r40.060\n");
               fprintf(configfile, CD32_CONFIG);
               path_join((char*)&kickstart, retro_system_directory, CD32_ROM);
               path_join((char*)&kickstart_ext, retro_system_directory, CD32_ROM_EXT);
            }
            else
            {
               if (strcmp(opt_model, "auto") == 0)
               {
                  uae_machine[0] = '\0';
                  strcat(uae_machine, CD32_CONFIG);
                  strcpy(uae_kickstart, CD32_ROM);
                  strcpy(uae_kickstart_ext, CD32_ROM_EXT);
               }

               // No machine specified
               fprintf(stdout, "[libretro-uae]: No machine specified in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Booting default configuration\n");
               fprintf(configfile, uae_machine);
               path_join((char*)&kickstart, retro_system_directory, uae_kickstart);
               path_join((char*)&kickstart_ext, retro_system_directory, uae_kickstart_ext);
            }

            // Write common config
            fprintf(configfile, uae_config);

            // If region was specified in the name of the game
            if (strstr(full_path, "(NTSC)") != NULL)
            {
               fprintf(stdout, "[libretro-uae]: Found '(NTSC)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Forcing NTSC mode\n");
               fprintf(configfile, "ntsc=true\n");
               real_ntsc=true;
               forced_video=true;
            }
            else if (strstr(full_path, "(PAL)") != NULL)
            {
               fprintf(stdout, "[libretro-uae]: Found '(PAL)' in filename '%s'\n", full_path);
               fprintf(stdout, "[libretro-uae]: Forcing PAL mode\n");
               fprintf(configfile, "ntsc=false\n");
               forced_video=true;
            }

            // Verify Kickstart
            if (!file_exists(kickstart))
            {
               // Kickstart ROM not found
               fprintf(stderr, "Kickstart ROM '%s' not found!\n", (const char*)&kickstart);
               fclose(configfile);
               return false;
            }
            else
               fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

            // Decide if CD32 ROM is combined based on filesize
            struct stat kickstart_st;
            stat(kickstart, &kickstart_st);

            // Verify extended ROM if external
            if (kickstart_st.st_size == 524288)
            {
               if (!file_exists(kickstart_ext))
               {
                  // Kickstart extended ROM not found
                  fprintf(stderr, "Kickstart extended ROM '%s' not found!\n", (const char*)&kickstart_ext);
                  fclose(configfile);
                  return false;
               }
               else
                  fprintf(configfile, "kickstart_ext_rom_file=%s\n", (const char*)&kickstart_ext);
            }

            // NVRAM
            char flash_file[RETRO_PATH_MAX];
            char flash_filepath[RETRO_PATH_MAX];
            if (opt_shared_nvram)
            {
               // Shared
               path_join((char*)&flash_file, retro_save_directory, LIBRETRO_PUAE_PREFIX);
            }
            else
            {
               // Per game
               snprintf(flash_filepath, RETRO_PATH_MAX, "%s", full_path);
               path_remove_extension((char*)flash_filepath);
               path_join((char*)&flash_file, retro_save_directory, path_basename(flash_filepath));
            }
            fprintf(stdout, "[libretro-uae]: Using Flash RAM: '%s.nvr'\n", flash_file);
            fprintf(configfile, "flash_file=%s.nvr\n", (const char*)&flash_file);

            // Add the file to disk control context
            char cd_image_label[RETRO_PATH_MAX];
            cd_image_label[0] = '\0';

            if (!string_is_empty(full_path))
               fill_short_pathname_representation(
                     cd_image_label, full_path, sizeof(cd_image_label));

            // Must reset disk control struct here,
            // otherwise duplicate entries will be
            // added when calling retro_reset()
            dc_reset(dc);
            dc_add_file(dc, full_path, cd_image_label);

            // Init first disk
            dc->index = 0;
            dc->eject_state = false;
            fprintf(stdout, "[libretro-uae]: CD (%d) inserted into drive CD0: '%s'\n", dc->index+1, dc->files[dc->index]);
            fprintf(configfile, "cdimage0=%s,\n", dc->files[0]); // ","-suffix needed if filename contains ","

            fclose(configfile);
         }
         else
         {
            // Error
            fprintf(stderr, "Error while writing file '%s'!\n", (const char*)&RPATH);
            return false;
         }
      }
      // If argument is a config file
	  else if (strendswith(full_path, UAE_FILE_EXT))
	  {
	     // Open tmp config file
	     FILE * configfile;
	     if (configfile = fopen(RPATH, "w"))
	     {
	        char kickstart[RETRO_PATH_MAX];

	        fprintf(configfile, uae_machine);
	        path_join((char*)&kickstart, retro_system_directory, uae_kickstart);

	        // Write common config
	        fprintf(configfile, uae_config);
	        fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

	        // Separator row for clarity
	        fprintf(configfile, "\n");

	        // Iterate parsed file and append all rows to the temporary config
	        FILE * configfile_custom;

	        char filebuf[4096];
	        if (configfile_custom = fopen (full_path, "r"))
	        {
	           while (fgets(filebuf, sizeof(filebuf), configfile_custom))
	           {
	              fprintf(configfile, filebuf);
               }
               fclose(configfile_custom);
            }
            fclose(configfile);
         }
         else
         {
            // Error
            fprintf(stderr, "Error while writing file '%s'!\n", (const char*)&RPATH);
            return false;
         }
      }
	  // Other extensions
	  else
	  {
	     // Unsupported file format
	     fprintf(stderr, "Unsupported file format '%s'!\n", full_path);
	     return false;
	  }
   }
   // Empty content
   else
   {
      // Open tmp config file
      FILE * configfile;
      if (configfile = fopen(RPATH, "w"))
      {
         char kickstart[RETRO_PATH_MAX];

         // No machine specified
         fprintf(stdout, "[libretro-uae]: Booting default configuration\n");
         fprintf(configfile, uae_machine);
         path_join((char*)&kickstart, retro_system_directory, uae_kickstart);

         // Write common config
         fprintf(configfile, uae_config);

         // CD32 exception
         if (strcmp(opt_model, "CD32") == 0 || strcmp(opt_model, "CD32FR") == 0)
         {
            char kickstart_ext[RETRO_PATH_MAX];
            path_join((char*)&kickstart_ext, retro_system_directory, uae_kickstart_ext);

            // Verify kickstart
            if (!file_exists(kickstart))
            {
               // Kickstart ROM not found
               fprintf(stderr, "Kickstart ROM '%s' not found!\n", (const char*)&kickstart);
               fclose(configfile);
               return false;
            }
            else
               fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

            // Decide if CD32 ROM is combined based on filesize
            struct stat kickstart_st;
            stat(kickstart, &kickstart_st);

            // Verify extended ROM if external
            if (kickstart_st.st_size == 524288)
            {
               if (!file_exists(kickstart_ext))
               {
                  // Kickstart extended ROM not found
                  fprintf(stderr, "Kickstart extended ROM '%s' not found!\n", (const char*)&kickstart_ext);
                  fclose(configfile);
                  return false;
               }
               else
                  fprintf(configfile, "kickstart_ext_rom_file=%s\n", (const char*)&kickstart_ext);
            }

            // NVRAM always shared without content
            char flash_file[RETRO_PATH_MAX];
            char flash_filepath[RETRO_PATH_MAX];
            path_join((char*)&flash_file, retro_save_directory, LIBRETRO_PUAE_PREFIX);
            fprintf(stdout, "[libretro-uae]: Using Flash RAM: '%s.nvr'\n", flash_file);
            fprintf(configfile, "flash_file=%s.nvr\n", (const char*)&flash_file);
         }
         else
         {
            // Verify Kickstart
            if (!file_exists(kickstart))
            {
               // Kickstart ROM not found
               fprintf(stderr, "Kickstart ROM '%s' not found!\n", (const char*)&kickstart);
               fclose(configfile);
               return false;
            }
            else
               fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);
         }

         fclose(configfile);
      }
   }
   return true;
}

void retro_reset(void)
{
   fake_ntsc = false;
   video_config_old = 0;
   update_variables();
   retro_create_config();
   uae_restart(1, (const char*)&RPATH); /* 1=nogui */
}

void retro_run(void)
{
   // Core options
   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();

   // Update audio settings
   if (filter_type_update)
   {
      filter_type_update = false;
      if (currprefs.cpu_model == 68020)
         changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A1200;
      else
         changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A500;
      config_changed = 0;
   }

   // Automatic vertical offset
   if (opt_vertical_offset_auto && zoom_mode_id != 0)
   {
      if ((retro_thisframe_first_drawn_line != thisframe_first_drawn_line_old) ||
          (retro_thisframe_last_drawn_line != thisframe_last_drawn_line_old))
      {
         // Prevent interlace stuttering by requiring a change of at least 2 lines
         if (abs(thisframe_first_drawn_line_old - retro_thisframe_first_drawn_line) > 1)
         {
            thisframe_first_drawn_line_old = retro_thisframe_first_drawn_line;
            request_update_av_info = true;
         }
         if (abs(thisframe_last_drawn_line_old - retro_thisframe_last_drawn_line) > 1)
         {
            thisframe_last_drawn_line_old = retro_thisframe_last_drawn_line;
            request_update_av_info = true;
         }
      }
      // Timer required for unserialize recovery
      else if (retro_thisframe_first_drawn_line == thisframe_first_drawn_line_old)
      {
         if (thisframe_y_adjust_update_frame_timer > 0)
         {
            thisframe_y_adjust_update_frame_timer--;
            if (thisframe_y_adjust_update_frame_timer == 0)
               request_update_av_info = true;
         }
      }
   }
   else
   {
      // Vertical offset must not be set too early
      if (thisframe_y_adjust_update_frame_timer > 0)
      {
         thisframe_y_adjust_update_frame_timer--;
         if ((thisframe_y_adjust_update_frame_timer == 0) && (opt_vertical_offset != 0))
         {
            thisframe_y_adjust = minfirstline + opt_vertical_offset;
            request_reset_drawing = true;
         }
      }
   }

   // Automatic horizontal offset
   if (opt_horizontal_offset_auto)
   {
      if ((retro_min_diwstart != min_diwstart_old) ||
          (retro_max_diwstop != max_diwstop_old))
      {
         min_diwstart_old = retro_min_diwstart;
         max_diwstop_old = retro_max_diwstop;
         request_update_av_info = true;
      }
   }
   else
   {
      // Horizontal offset must not be set too early
      if (visible_left_border_update_frame_timer > 0)
      {
         visible_left_border_update_frame_timer--;
         if (visible_left_border_update_frame_timer == 0)
         {
            visible_left_border = max_diwlastword - retrow - opt_horizontal_offset;
            request_reset_drawing = true;
         }
      }
   }

   // AV info change is requested
   if (request_update_av_info)
      retro_update_av_info(1, 0, 0);

   // Poll inputs
   retro_poll_event();

   // If any drawing parameters/offsets have been modified,
   // must call reset_drawing() to ensure that the changes
   // are 'registered' by center_image() in drawing.c
   // > If we don't do this, the wrong parameters may be
   //   used on the next frame, which can lead to out of
   //   bounds video buffer access (memory corruption)
   // > This check must come *after* horizontal/vertical
   //   offset calculation, retro_update_av_info() and
   //   retro_poll_event()
   if (request_reset_drawing)
   {
      request_reset_drawing = false;
      reset_drawing();
   }

   // Check if a restart is required
   if (restart_pending)
   {
      restart_pending = 0;
      libretro_do_restart(sizeof(uae_argv)/sizeof(*uae_argv), uae_argv);
      // Note that this is set *temporarily*
      // > It will be reset inside the following
      //   'if' statement
      firstpass = 1;
   }

   if (firstpass)
   {
      firstpass = 0;
      // Run emulation first pass
      restart_pending = m68k_go(1, 0);
      video_cb(retro_bmp, retrow, zoomed_height, retrow << (pix_bytes / 2));
      return;
   }

   // Resume emulation for 1 frame
   restart_pending = m68k_go(1, 1);

   if (STATUSON == 1)
      Print_Status();
   if (SHOWKEY == 1)
   {
      // Virtual keyboard transparency requires a graceful redraw, blunt reset_drawing() interferes with zoom
      frame_redraw_necessary=2;
      virtual_kbd(retro_bmp, vkey_pos_x, vkey_pos_y);
   }
   // Maximum 288p/576p PAL shenanigans:
   // Mask the last line(s), since UAE does not refresh the last line, and even its own OSD will leave trails
   if (video_config & PUAE_VIDEO_PAL)
   {
      if (video_config & PUAE_VIDEO_DOUBLELINE)
      {
         DrawHline(retro_bmp, 0, 574, retrow, 0, 0);
         DrawHline(retro_bmp, 0, 575, retrow, 0, 0);
      }
      else
         DrawHline(retro_bmp, 0, 287, retrow, 0, 0);
   }
   video_cb(retro_bmp, retrow, zoomed_height, retrow << (pix_bytes / 2));
}

bool retro_load_game(const struct retro_game_info *info)
{
   // UAE config
   if (info)
      strcpy(full_path, (char*)info->path);
   static bool retro_return;
   retro_return = retro_create_config();
   if (!retro_return)
      return false;

   // Screen resolution
   fprintf(stderr, "[libretro-uae]: Resolution selected: %dx%d\n", defaultw, defaulth);
   retrow = defaultw;
   retroh = defaulth;

   // Savestate filename
   snprintf(savestate_fname, sizeof(savestate_fname), "%s%s%s.asf", retro_save_directory, DIR_SEP_STR, LIBRETRO_PUAE_PREFIX);

   // Initialise emulation
   umain(sizeof(uae_argv)/sizeof(*uae_argv), uae_argv);

   return true;
}

void retro_unload_game(void)
{
   if (!firstpass)
      leave_program();
}

unsigned retro_get_region(void)
{
   return (video_config & PUAE_VIDEO_NTSC) ? RETRO_REGION_NTSC : RETRO_REGION_PAL;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   (void)type;
   (void)info;
   (void)num;
   return false;
}

size_t retro_serialize_size(void)
{
   if (!firstpass)
   {
      if (save_state(savestate_fname, "libretro") >= 0)
      {
#if 0
         FILE *file = fopen(savestate_fname, "rb");
         if (file)
         {
            size_t size = 0;
            fseek(file, 0L, SEEK_END);
            size = ftell(file);
            fclose(file);
            return size;
         }
#endif
         if (file_exists(savestate_fname))
         {
            struct stat savestate_st;
            stat(savestate_fname, &savestate_st);
            remove(savestate_fname);
            return savestate_st.st_size;
         }
      }
   }
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   if (!firstpass)
   {
      if (save_state(savestate_fname, "libretro") >= 0)
      {
         struct stat savestate_st;
         stat(savestate_fname, &savestate_st);
         size = savestate_st.st_size;

         FILE *file = fopen(savestate_fname, "rb");
         if (file)
         {
            if (fread(data_, size, 1, file) == 1)
            {
               fclose(file);
               remove(savestate_fname);
               return true;
            }
            fclose(file);
         }
      }
   }
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   if (!firstpass)
   {
      thisframe_y_adjust_update_frame_timer = 3;
      FILE *file = fopen(savestate_fname, "wb");
      if (file)
      {
         if (fwrite(data_, size, 1, file) == 1)
         {
            fclose(file);
            savestate_state = STATE_DORESTORE;
            return true;
         }
         else
            fclose(file);
      }
   }
   return false;
}

void *retro_get_memory_data(unsigned id)
{
#if defined(NATMEM_OFFSET)
   if ( id == RETRO_MEMORY_SYSTEM_RAM )
      return natmem_offset;
#endif
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
#if defined(NATMEM_OFFSET)
   if ( id == RETRO_MEMORY_SYSTEM_RAM )
      return natmem_size;
#endif
   return 0;
}

void retro_cheat_reset(void) {}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

#if defined(ANDROID) || defined(__SWITCH__) || defined(WIIU)
#include <sys/timeb.h>

int ftime(struct timeb *tb)
{
    struct timeval  tv;
    struct timezone tz;

    if (gettimeofday (&tv, &tz) < 0)
        return -1;

    tb->time    = tv.tv_sec;
    tb->millitm = (tv.tv_usec + 500) / 1000;

    if (tb->millitm == 1000)
    {
        ++tb->time;
        tb->millitm = 0;
    }
    tb->timezone = tz.tz_minuteswest;
    tb->dstflag  = tz.tz_dsttime;

    return 0;
}
#endif
