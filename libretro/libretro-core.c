#include "libretro.h"
#include "libretro-core.h"
#include "libretro-mapper.h"
#include "libretro-graph.h"
#include "encodings/utf.h"

#include "retrodep/WHDLoad_files.zip.c"
#include "retrodep/WHDLoad_hdf.gz.c"
#include "retrodep/WHDSaves_hdf.gz.c"
#include "retrodep/WHDLoad_prefs.gz.c"

#include "sysdeps.h"
#include "uae.h"
#include "uae_types.h"
#include "options.h"
#include "inputdevice.h"
#include "savestate.h"
#include "custom.h"
#include "xwin.h"
#include "drawing.h"
#include "akiko.h"
#include "blkdev.h"
#include "disk.h"

int libretro_runloop_active = 0;
int retrow = 0;
int retroh = 0;
int defaultw = EMULATOR_DEF_WIDTH;
int defaulth = EMULATOR_DEF_HEIGHT;
char retro_key_state[RETROK_LAST] = {0};
char retro_key_state_old[RETROK_LAST] = {0};
unsigned short int retro_bmp[RETRO_BMP_SIZE] = {0};

extern int frame_redraw_necessary;
extern int bplcon0;
extern int diwlastword_total;
extern int diwfirstword_total;
extern int interlace_seen;
extern int m68k_go(int may_quit, int resume);
extern int prefs_changed;

unsigned int opt_model_options_display = 0;
unsigned int opt_audio_options_display = 0;
unsigned int opt_video_options_display = 0;
unsigned int opt_mapping_options_display = 1;
char opt_model[10] = {0};
char opt_model_fd[10] = {0};
char opt_model_hd[10] = {0};
char opt_model_cd[10] = {0};
bool opt_region_auto = true;
bool opt_video_resolution_auto = false;
bool opt_video_vresolution_auto = false;
bool opt_floppy_sound_empty_mute = false;
bool opt_floppy_multidrive = false;
unsigned int opt_use_whdload = 1;
unsigned int opt_use_whdload_prefs = 0;
unsigned int opt_use_boot_hd = 0;
bool opt_shared_nvram = false;
bool opt_cd_startup_delayed_insert = false;
int opt_statusbar = 0;
int opt_statusbar_position = 0;
int opt_statusbar_position_old = 0;
int opt_statusbar_position_offset = 0;
unsigned int opt_vkbd_theme = 0;
unsigned int opt_vkbd_alpha = 204;
bool opt_keyrah_keypad = false;
bool opt_keyboard_pass_through = false;
bool opt_multimouse = false;
unsigned int opt_dpadmouse_speed = 4;
unsigned int opt_analogmouse = 0;
unsigned int opt_analogmouse_deadzone = 20;
float opt_analogmouse_speed = 1.0;
unsigned int opt_cd32pad_options = 0;
unsigned int opt_retropad_options = 0;
char opt_joyport_order[5] = "1234";

#if defined(NATMEM_OFFSET)
extern uae_u8 *natmem_offset;
extern uae_u32 natmem_size;
#endif

static char RPATH[RETRO_PATH_MAX] = {0};
char full_path[RETRO_PATH_MAX] = {0};
static char *uae_argv[] = { "puae", RPATH };
static int restart_pending = 0;
static char* core_options_legacy_strings = NULL;

bool retro_message = false;
char retro_message_msg[1024] = {0};
bool retro_statusbar = false;

extern bool retro_mousemode;
extern bool mousemode_locked;
extern bool retro_vkbd;
extern void print_vkbd(unsigned short int *pixels);

extern int turbo_fire_button;
extern unsigned int turbo_pulse;
extern bool inputdevice_finalized;
unsigned int pix_bytes = 2;
static bool pix_bytes_initialized = false;
static bool cpu_cycle_exact_force = false;
static bool automatic_sound_filter_type_update = true;
static bool fake_ntsc = false;
static bool real_ntsc = false;
static bool forced_video = false;
static bool locked_video_horizontal = false;
bool request_update_av_info = false;
bool retro_av_info_change_timing = false;
bool retro_av_info_change_geometry = true;
bool retro_av_info_is_ntsc = false;
bool request_reset_drawing = false;
bool request_reset_soft = false;
unsigned int request_init_custom_timer = 0;
unsigned int request_check_prefs_timer = 0;
unsigned int zoom_mode_id = 0;
unsigned int opt_zoom_mode_id = 0;
unsigned int zoom_mode_crop_id = 0;
int zoomed_width = 0;
int zoomed_height = 0;
unsigned int width_multiplier = 1;

static int opt_vertical_offset = 0;
static bool opt_vertical_offset_auto = true;
extern int minfirstline;
static int retro_thisframe_counter = 0;
extern int retro_thisframe_first_drawn_line;
static int retro_thisframe_first_drawn_line_old = -1;
static int retro_thisframe_first_drawn_line_start = -1;
extern int retro_thisframe_last_drawn_line;
static int retro_thisframe_last_drawn_line_old = -1;
static int retro_thisframe_last_drawn_line_start = -1;
extern int thisframe_y_adjust;
static int thisframe_y_adjust_old = -1;
static int thisframe_y_adjust_update_frame_timer = 3;

static int opt_horizontal_offset = 0;
static bool opt_horizontal_offset_auto = true;
static int retro_max_diwlastword_hires = 824;
static int retro_max_diwlastword = 824;
extern int retro_min_diwstart;
static int retro_min_diwstart_old = -1;
extern int retro_max_diwstop;
static int retro_max_diwstop_old = -1;
static int retro_diwstartstop_counter = 0;
extern int visible_left_border;
static int visible_left_border_old = 0;
static int visible_left_border_update_frame_timer = 3;

unsigned int video_config = 0;
unsigned int video_config_old = 0;
unsigned int video_config_aspect = 0;
unsigned int video_config_geometry = 0;
unsigned int video_config_allow_hz_change = 0;

struct zfile *retro_deserialize_file = NULL;
static size_t save_state_file_size = 0;

static int retro_keymap_id(const char *val)
{
   int i = 0;
   while (retro_keys[i].value != NULL)
   {
      if (!strcmp(retro_keys[i].value, val))
         return retro_keys[i].id;
      i++;
   }
   return 0;
}

unsigned int retro_devices[RETRO_DEVICES];
extern void retro_poll_event();
extern int cd32_pad_enabled[NORMAL_JPORTS];
extern int mapper_keys[RETRO_MAPPER_LAST];
extern void display_current_image(const char *image, bool inserted);

retro_log_printf_t log_cb = NULL;
static retro_set_led_state_t led_state_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_t audio_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;
bool libretro_supports_bitmasks = false;

char retro_save_directory[RETRO_PATH_MAX] = {0};
char retro_temp_directory[RETRO_PATH_MAX] = {0};
char retro_system_directory[RETRO_PATH_MAX] = {0};
static char retro_content_directory[RETRO_PATH_MAX] = {0};

/* Disk Control context */
dc_storage *dc = NULL;

/* Configs */
static char uae_model[256] = {0};
static char uae_kickstart[RETRO_PATH_MAX] = {0};
static char uae_kickstart_ext[RETRO_PATH_MAX] = {0};
static char uae_config[4096] = {0};
static char uae_custom_config[4096] = {0};

void retro_set_led(unsigned led)
{
   led_state_cb(0, led);
}

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_controller_description p1_controllers[] = {
      { "CD32 Pad", RETRO_DEVICE_CD32PAD },
      { "Analog Joystick", RETRO_DEVICE_ANALOGJOYSTICK },
      { "Joystick", RETRO_DEVICE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p2_controllers[] = {
      { "CD32 Pad", RETRO_DEVICE_CD32PAD },
      { "Analog Joystick", RETRO_DEVICE_ANALOGJOYSTICK },
      { "Joystick", RETRO_DEVICE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p3_controllers[] = {
      { "Joystick", RETRO_DEVICE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p4_controllers[] = {
      { "Joystick", RETRO_DEVICE_JOYSTICK },
      { "Keyboard", RETRO_DEVICE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };
   static const struct retro_controller_description p5_controllers[] = {
      { "Keyboard", RETRO_DEVICE_KEYBOARD },
      { "None", RETRO_DEVICE_NONE },
   };

   static const struct retro_controller_info ports[] = {
      { p1_controllers, 5 }, /* port 1 */
      { p2_controllers, 5 }, /* port 2 */
      { p3_controllers, 3 }, /* port 3 */
      { p4_controllers, 3 }, /* port 4 */
      { p5_controllers, 2 }, /* port 5 */
      { NULL, 0 }
   };

   static struct retro_core_option_definition core_options[] =
   {
      {
         "puae_model",
         "Model",
         "'Automatic' defaults to 'A500' with floppy disks, 'A600' with hard drives and 'CD32' with CD images. 'Automatic' can be overridden with file path tags.\nCore restart required.",
         {
            { "auto", "Automatic" },
            { "A500OG", "A500 (512KB Chip)" },
            { "A500", "A500 (512KB Chip + 512KB Slow)" },
            { "A500PLUS", "A500+ (1MB Chip)" },
            { "A600", "A600 (2MB Chip + 8MB Fast)" },
            { "A1200OG", "A1200 (2MB Chip)" },
            { "A1200", "A1200 (2MB Chip + 8MB Fast)" },
            { "A4030", "A4000/030 (2MB Chip + 8MB Fast)" },
            { "A4040", "A4000/040 (2MB Chip + 8MB Fast)" },
            { "CDTV", "CDTV (1MB Chip)" },
            { "CD32", "CD32 (2MB Chip)" },
            { "CD32FR", "CD32 (2MB Chip + 8MB Fast)" },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_model_options_display",
         "Show Automatic Model Options",
         "Shows/hides default model options (Floppy/HD/CD) for 'Automatic' model.\nCore options page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_model_fd",
         "Model > Automatic Floppy",
         "Default model when floppies are launched with 'Automatic' model.\nCore restart required.",
         {
            { "A500OG", "A500 (512KB Chip)" },
            { "A500", "A500 (512KB Chip + 512KB Slow)" },
            { "A500PLUS", "A500+ (1MB Chip)" },
            { "A600", "A600 (2MB Chip + 8MB Fast)" },
            { "A1200OG", "A1200 (2MB Chip)" },
            { "A1200", "A1200 (2MB Chip + 8MB Fast)" },
            { "A4030", "A4000/030 (2MB Chip + 8MB Fast)" },
            { "A4040", "A4000/040 (2MB Chip + 8MB Fast)" },
            { NULL, NULL },
         },
         "A500"
      },
      {
         "puae_model_hd",
         "Model > Automatic HD",
         "Default model when HD interface is used with 'Automatic' model. Affects WHDLoad installs and other hard drive images.\nCore restart required.",
         {
            { "A600", "A600 (2MB Chip + 8MB Fast)" },
            { "A1200OG", "A1200 (2MB Chip)" },
            { "A1200", "A1200 (2MB Chip + 8MB Fast)" },
            { "A4030", "A4000/030 (2MB Chip + 8MB Fast)" },
            { "A4040", "A4000/040 (2MB Chip + 8MB Fast)" },
            { NULL, NULL },
         },
         "A600"
      },
      {
         "puae_model_cd",
         "Model > Automatic CD",
         "Default model when compact discs are launched with 'Automatic' model.\nCore restart required.",
         {
            { "CDTV", "CDTV (1MB Chip)" },
            { "CD32", "CD32 (2MB Chip)" },
            { "CD32FR", "CD32 (2MB Chip + 8MB Fast)" },
            { NULL, NULL },
         },
         "CD32"
      },
      {
         "puae_cpu_compatibility",
         "System > CPU Compatibility",
         "Some games require 'Cycle-exact'. 'Cycle-exact' can be forced with '(CE)' file path tag.",
         {
            { "normal", "Normal" },
            { "compatible", "More compatible" },
            { "exact", "Cycle-exact" },
            { NULL, NULL },
         },
         "normal"
      },
      {
         "puae_cpu_throttle",
         "System > CPU Speed",
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
            { "0.0", "Default" },
            { "1000.0", "+100\%" },
            { "2000.0", "+200\%" },
            { "3000.0", "+300\%" },
            { "4000.0", "+400\%" },
            { "5000.0", "+500\%" },
            { "6000.0", "+600\%" },
            { "7000.0", "+700\%" },
            { "8000.0", "+800\%" },
            { "9000.0", "+900\%" },
            { "10000.0", "+1000\%" },
            { NULL, NULL },
         },
         "0.0"
      },
      {
         "puae_cpu_multiplier",
         "System > CPU Cycle-exact Speed",
         "Applies only with 'Cycle-exact'.",
         {
            { "0", "Default" },
            { "1", "3.546895 MHz" },
            { "2", "7.093790 MHz (A500)" },
            { "4", "14.187580 MHz (A1200)" },
            { "8", "28.375160 MHz" },
            { "10", "35.468950 MHz" },
            { "12", "42.562740 MHz" },
            { "16", "56.750320 MHz" },
            { NULL, NULL },
         },
         "0"
      },
      {
         "puae_floppy_speed",
         "Media > Floppy Speed",
         "'Turbo' removes disk rotation emulation.",
         {
            { "100", "Default (300RPM)" },
            { "200", "2x (600RPM)" },
            { "400", "4x (1200RPM)" },
            { "800", "8x (2400RPM)" },
            { "0", "Turbo" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_floppy_multidrive",
         "Media > Floppy MultiDrive",
         "Inserts each disk in different drives. Can be forced with '(MD)' file path tag. Maximum is 4 disks due to external drive limit! Not all games support external drives!\nCore restart required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_floppy_write_protection",
         "Media > Floppy Write Protection",
         "Makes all drives read only. Changing this while emulation is running ejects and reinserts all disks.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_cd_speed",
         "Media > CD Speed",
         "Transfer rate in CD32 is 300KB/s (double-speed), CDTV is 150KB/s (single-speed). 'Turbo' removes seek delay emulation.",
         {
            { "100", "Default" },
            { "0", "Turbo" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_cd_startup_delayed_insert",
         "Media > CD Startup Delayed Insert",
         "Some games fail to load if CD32/CDTV is powered on with CD inserted. 'ON' inserts CD during boot animation.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_shared_nvram",
         "Media > CD32/CDTV Shared NVRAM",
         "'OFF' saves separate files per content. Starting without content uses the shared file. CD32 and CDTV use separate shared files.\nCore restart required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_use_boot_hd",
         "Media > Global Boot HD",
         "Attach a hard disk meant for Workbench usage, not for WHDLoad! Enabling forces a model with HD interface. Changing HDF size will not replace or edit the existing HDF.\nCore restart required.",
         {
            { "disabled", NULL },
            { "files", "Files" },
            { "hdf20", "HDF 20MB" },
            { "hdf40", "HDF 40MB" },
            { "hdf80", "HDF 80MB" },
            { "hdf128", "HDF 128MB" },
            { "hdf256", "HDF 256MB" },
            { "hdf512", "HDF 512MB" },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_use_whdload",
         "Media > WHDLoad Support",
         "Enable launching pre-installed WHDLoad installs. Creates a helper image for loading content and an empty image for saving. Core restart required.\n- 'Files' creates data in directories\n- 'HDFs' creates data in images",
         {
            { "disabled", NULL },
            { "files", "Files" },
            { "hdfs", "HDFs" },
            { NULL, NULL },
         },
         "files"
      },
      {
         "puae_use_whdload_prefs",
         "Media > WHDLoad Splash Screen",
         "Space/Enter/Fire works as WHDLoad Start-button. Core restart required.\nOverride with buttons while booting:\n- 'Config': Hold 2nd fire / Blue\n- 'Splash': Hold LMB\n- 'Config + Splash': Hold RMB\n- ReadMe + MkCustom: Hold Red+Blue",
         {
            { "disabled", NULL },
            { "config", "Config (Show only if available)" },
            { "splash", "Splash (Show briefly)" },
            { "both", "Config + Splash (Wait for user input)" },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_video_options_display",
         "Show Video Options",
         "Shows/hides video related options.\nCore options page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_video_allow_hz_change",
         "Video > Allow PAL/NTSC Hz Change",
         "Lets Amiga decide the exact output Hz.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      {
         "puae_video_standard",
         "Video > Standard",
         "Output Hz & height:\n- 'PAL': 50Hz - 288px / 576px\n- 'NTSC': 60Hz - 240px / 480px\n- 'Automatic' switches region per file path tags.",
         {
            { "PAL auto", "Automatic PAL" },
            { "NTSC auto", "Automatic NTSC" },
            { "PAL", NULL },
            { "NTSC", NULL },
            { NULL, NULL },
         },
         "PAL auto"
      },
      {
         "puae_video_resolution",
         "Video > Resolution",
         "Output width:\n- 'Automatic' defaults to 'High' and switches to 'Super-High' when needed.",
         {
            { "auto", "Automatic" },
            { "lores", "Low 360px" },
            { "hires", "High 720px" },
            { "superhires", "Super-High 1440px" },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_video_vresolution",
         "Video > Line Mode",
         "Output height:\n- 'Automatic' defaults to 'Single Line' and switches to 'Double Line' on interlaced screens.",
         {
            { "auto", "Automatic" },
            { "single", "Single Line" },
            { "double", "Double Line" },
            { NULL, NULL },
         },
         "auto"
      },
      {
         "puae_video_aspect",
         "Video > Pixel Aspect Ratio",
         "- 'PAL': 1/1 = 1.000\n- 'NTSC': 44/52 = 0.846",
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
         "Video > Zoom Mode",
         "Crops the borders to fit various host screens. Requirements in RetroArch settings:\n- Aspect Ratio: Core provided,\n- Integer Scale: Off.",
         {
            { "disabled", NULL },
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
         "disabled"
      },
      {
         "puae_zoom_mode_crop",
         "Video > Zoom Mode Crop",
         "Use 'Horizontal + Vertical' & 'Automatic' to remove borders completely.",
         {
            { "both", "Horizontal + Vertical" },
            { "horizontal", "Horizontal" },
            { "vertical", "Vertical" },
            { "16:9", "16:9" },
            { "16:10", "16:10" },
            { "4:3", "4:3" },
            { "5:4", "5:4" },
            { NULL, NULL },
         },
         "both"
      },
      {
         "puae_vertical_pos",
         "Video > Vertical Position",
         "'Automatic' keeps only zoomed screens centered. Positive values move upward and negative values move downward.",
         {
            { "auto", "Automatic" },
            { "0", "Default" },
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
            { "52", NULL },
            { "54", NULL },
            { "56", NULL },
            { "58", NULL },
            { "60", NULL },
            { "62", NULL },
            { "64", NULL },
            { "66", NULL },
            { "68", NULL },
            { "70", NULL },
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
         "Video > Horizontal Position",
         "'Automatic' keeps screen centered. Positive values move right and negative values move left.",
         {
            { "auto", "Automatic" },
            { "0", "Default" },
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
            { "-40", NULL },
            { "-38", NULL },
            { "-36", NULL },
            { "-34", NULL },
            { "-32", NULL },
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
         "puae_gfx_flickerfixer",
         "Video > Remove Interlace Artifacts",
         "Best suited for stationary screens, Workbench etc.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_immediate_blits",
         "Video > Immediate/Waiting Blits",
         "'Immediate Blitter' is ignored with 'Cycle-exact'.",
         {
            { "false", "disabled" },
            { "immediate", "Immediate Blitter" },
            { "waiting", "Wait for Blitter" },
            { NULL, NULL },
         },
         "waiting"
      },
      {
         "puae_collision_level",
         "Video > Collision Level",
         "'Sprites and Playfields' is recommended.",
         {
            { "none", "None" },
            { "sprites", "Sprites only" },
            { "playfields", "Sprites and Playfields" },
            { "full", "Full" },
            { NULL, NULL },
         },
         "playfields"
      },
      {
         "puae_gfx_framerate",
         "Video > Frameskip",
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
         "Video > Statusbar Mode",
         "- 'Full': Joyports + Current image + LEDs\n- 'Basic': Current image + LEDs\n- 'Minimal': Track number + FPS hidden",
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
         "puae_vkbd_theme",
         "Video > Virtual KBD Theme",
         "By default, the keyboard comes up with RetroPad Select.",
         {
            { "0", "Classic" },
            { "1", "CD32" },
            { "2", "Dark" },
            { "3", "Light" },
            { NULL, NULL },
         },
         "0"
      },
      {
         "puae_vkbd_alpha",
         "Video > Virtual KBD Transparency",
         "Keyboard transparency can be toggled with RetroPad A.",
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
         "puae_gfx_colors",
         "Video > Color Depth",
         "'24-bit' is slower and not available on all platforms. Full restart required.",
         {
            { "16bit", "Thousands (16-bit)" },
            { "24bit", "Millions (24-bit)" },
            { NULL, NULL },
         },
         "16bit"
      },
      {
         "puae_audio_options_display",
         "Show Audio Options",
         "Shows/hides audio related options.\nCore options page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_sound_stereo_separation",
         "Audio > Stereo Separation",
         "Paula sound chip channel panning. Does not affect CD audio.",
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
         "Audio > Interpolation",
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
         "Audio > Filter",
         "",
         {
            { "emulated", "Emulated" },
            { "off", "Off" },
            { "on", "On" },
            { NULL, NULL },
         },
         "emulated"
      },
      {
         "puae_sound_filter_type",
         "Audio > Filter Type",
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
         "Audio > CD Audio Volume",
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
         "Audio > Floppy Sound Emulation",
         "",
         {
            { "100", "disabled" },
            { "95", "5\%" },
            { "90", "10\%" },
            { "85", "15\%" },
            { "80", "20\%" },
            { "75", "25\%" },
            { "70", "30\%" },
            { "65", "35\%" },
            { "60", "40\%" },
            { "55", "45\%" },
            { "50", "50\%" },
            { "45", "55\%" },
            { "40", "60\%" },
            { "35", "65\%" },
            { "30", "70\%" },
            { "25", "75\%" },
            { "20", "80\%" },
            { "15", "85\%" },
            { "10", "90\%" },
            { "5", "95\%" },
            { "0", "100\%" },
            { NULL, NULL },
         },
         "80"
      },
      {
         "puae_floppy_sound_empty_mute",
         "Audio > Floppy Sound Mute Ejected",
         "Mutes drive head clicking when floppy is not inserted.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_floppy_sound_type",
         "Audio > Floppy Sound Type",
         "External files go in 'system/uae_data/'.",
         {
            { "internal", "Internal" },
            { "A500", "External: A500" },
            { "LOUD", "External: LOUD" },
            { NULL, NULL },
         },
         "internal"
      },
      {
         "puae_analogmouse",
         "Input > Analog Stick Mouse",
         "",
         {
            { "disabled", NULL },
            { "left", "Left Analog" },
            { "right", "Right Analog" },
            { "both", "Both Analogs" },
            { NULL, NULL },
         },
         "both"
      },
      {
         "puae_analogmouse_deadzone",
         "Input > Analog Stick Mouse Deadzone",
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
         "20"
      },
      {
         "puae_analogmouse_speed",
         "Input > Analog Stick Mouse Speed",
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
         "Input > D-Pad Mouse Speed",
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
         "Input > Mouse Speed",
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
         "Input > Multiple Physical Mouse",
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
         "Input > Keyrah Keypad Mappings",
         "Hardcoded keypad to joyport mappings for Keyrah hardware.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_physical_keyboard_pass_through",
         "Input > Keyboard Pass-through",
         "'ON' passes all physical keyboard events to the core. 'OFF' prevents RetroPad keys from generating keyboard events.",
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
         "Shows/hides hotkey & RetroPad mapping options.\nCore options page refresh required.",
         {
            { "disabled", NULL },
            { "enabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      /* Hotkeys */
      {
         "puae_mapper_vkbd",
         "Hotkey > Toggle Virtual Keyboard",
         "Press the mapped key to toggle the virtual keyboard.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_statusbar",
         "Hotkey > Toggle Statusbar",
         "Press the mapped key to toggle the statusbar.",
         {{ NULL, NULL }},
         "RETROK_F12"
      },
      {
         "puae_mapper_mouse_toggle",
         "Hotkey > Switch Joystick/Mouse",
         "Press the mapped key to switch between joystick and mouse control.",
         {{ NULL, NULL }},
         "RETROK_RCTRL"
      },
      {
         "puae_mapper_reset",
         "Hotkey > Reset",
         "Press the mapped key to trigger soft reset (Ctrl-Amiga-Amiga).",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_aspect_ratio_toggle",
         "Hotkey > Toggle Aspect Ratio",
         "Press the mapped key to toggle between PAL/NTSC pixel aspect ratio.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_zoom_mode_toggle",
         "Hotkey > Toggle Zoom Mode",
         "Press the mapped key to toggle zoom mode.",
         {{ NULL, NULL }},
         "---"
      },
      /* Button mappings */
      {
         "puae_mapper_select",
         "RetroPad > Select",
         "",
         {{ NULL, NULL }},
         "TOGGLE_VKBD"
      },
      {
         "puae_mapper_start",
         "RetroPad > Start",
         "VKBD: Press 'Return'. Remapping to non-keyboard keys overrides VKBD function!",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_b",
         "RetroPad > B",
         "Unmapped defaults to fire button.\nVKBD: Press selected key.\n",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_a",
         "RetroPad > A",
         "Unmapped defaults to 2nd fire button.\nVKBD: Toggle transparency. Remapping to non-keyboard keys overrides VKBD function!",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_y",
         "RetroPad > Y",
         "VKBD: Toggle 'CapsLock'. Remapping to non-keyboard keys overrides VKBD function!",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_x",
         "RetroPad > X",
         "VKBD: Toggle position. Remapping to non-keyboard keys overrides VKBD function!",
         {{ NULL, NULL }},
         "RETROK_SPACE"
      },
      {
         "puae_mapper_l",
         "RetroPad > L",
         "",
         {{ NULL, NULL }},
         ""
      },
      {
         "puae_mapper_r",
         "RetroPad > R",
         "",
         {{ NULL, NULL }},
         ""
      },
      {
         "puae_mapper_l2",
         "RetroPad > L2",
         "",
         {{ NULL, NULL }},
         "MOUSE_LEFT_BUTTON"
      },
      {
         "puae_mapper_r2",
         "RetroPad > R2",
         "",
         {{ NULL, NULL }},
         "MOUSE_RIGHT_BUTTON"
      },
      {
         "puae_mapper_l3",
         "RetroPad > L3",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_r3",
         "RetroPad > R3",
         "",
         {{ NULL, NULL }},
         "---"
      },
      /* Left Stick */
      {
         "puae_mapper_lu",
         "RetroPad > Left Analog > Up",
         "Mapping for left analog stick up.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_ld",
         "RetroPad > Left Analog > Down",
         "Mapping for left analog stick down.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_ll",
         "RetroPad > Left Analog > Left",
         "Mapping for left analog stick left.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_lr",
         "RetroPad > Left Analog > Right",
         "Mapping for left analog stick right.",
         {{ NULL, NULL }},
         "---"
      },
      /* Right Stick */
      {
         "puae_mapper_ru",
         "RetroPad > Right Analog > Up",
         "Mapping for right analog stick up.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rd",
         "RetroPad > Right Analog > Down",
         "Mapping for right analog stick down.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rl",
         "RetroPad > Right Analog > Left",
         "Mapping for right analog stick left.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rr",
         "RetroPad > Right Analog > Right",
         "Mapping for right analog stick right.",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_turbo_fire_button",
         "RetroPad > Turbo Fire",
         "Replaces the mapped button with turbo fire button.",
         {
            { "disabled", NULL },
            { "B", "RetroPad B" },
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
         "RetroPad > Turbo Pulse",
         "Frames in a button cycle.\n- '2' = 1 frame down, 1 frame up\n- '4' = 2 frames down, 2 frames up\n- '6' = 3 frames down, 3 frames up\netc.",
         {
            { "2", "2 frames" },
            { "4", "4 frames" },
            { "6", "6 frames" },
            { "8", "8 frames" },
            { "10", "10 frames" },
            { "12", "12 frames" },
            { NULL, NULL },
         },
         "6"
      },
      {
         "puae_joyport",
         "RetroPad > Joystick/Mouse",
         "Changes D-Pad control between joyports. Hotkey toggling disables this option until core restart.",
         {
            { "joystick", "Joystick (Port 1)" },
            { "mouse", "Mouse (Port 2)" },
            { NULL, NULL },
         },
         "Joystick"
      },
      {
         "puae_joyport_order",
         "RetroPad > Joyport Order",
         "Plugs RetroPads in different ports. Useful for Arcadia system and games that use the 4-player adapter.",
         {
            { "1234", "1-2-3-4" },
            { "2143", "2-1-4-3" },
            { "3412", "3-4-1-2" },
            { "4321", "4-3-2-1" },
            { NULL, NULL },
         },
         "1234"
      },
      {
         "puae_retropad_options",
         "RetroPad > Face Button Options",
         "Rotates face buttons clockwise and/or makes 2nd fire press up.",
         {
            { "disabled", "B = Fire, A = 2nd fire" },
            { "jump", "B = Fire, A = Up" },
            { "rotate", "Y = Fire, B = 2nd fire" },
            { "rotate_jump", "Y = Fire, B = Up" },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_cd32pad_options",
         "CD32 Pad > Face Button Options",
         "Rotates face buttons clockwise and/or makes blue button press up.",
         {
            { "disabled", "B = Red, A = Blue" },
            { "jump", "B = Red, A = Up" },
            { "rotate", "Y = Red, B = Blue" },
            { "rotate_jump", "Y = Red, B = Up" },
            { NULL, NULL },
         },
         "disabled"
      },
      { NULL, NULL, NULL, {{0}}, NULL },
   };

   /* Fill in the values for all the mappers */
   int i = 0;
   int j = 0;
   int hotkey = 0;
   int hotkeys_skipped = 0;
   /* Count special hotkeys */
   while (retro_keys[j].value && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
   {
      if (retro_keys[j].id < 0)
         hotkeys_skipped++;
      ++j;
   }
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
            while (retro_keys[j].value && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
            {
               if (j == 0) /* "---" unmapped */
               {
                  core_options[i].values[j].value = retro_keys[j].value;
                  core_options[i].values[j].label = retro_keys[j].label;
               }
               else
               {
                  core_options[i].values[j].value = retro_keys[j + hotkeys_skipped + 1].value;

                  /* Append "Keyboard " for keyboard keys */
                  if (retro_keys[j + hotkeys_skipped + 1].id > 0)
                  {
                     char key_label[25] = {0};
                     sprintf(key_label, "Keyboard %s", retro_keys[j + hotkeys_skipped + 1].label);
                     core_options[i].values[j].label = strdup(key_label);
                  }
                  else
                     core_options[i].values[j].label = retro_keys[j + hotkeys_skipped + 1].label;
               }
               ++j;
            };
         }
         else
         {
            while (retro_keys[j].value && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
            {
               core_options[i].values[j].value = retro_keys[j].value;

               /* Append "Keyboard " for keyboard keys */
               if (retro_keys[j].id > 0)
               {
                  char key_label[25] = {0};
                  sprintf(key_label, "Keyboard %s", retro_keys[j].label);
                  core_options[i].values[j].label = strdup(key_label);
               }
               else
                  core_options[i].values[j].label = retro_keys[j].label;

               ++j;
            };
         }
         core_options[i].values[j].value = NULL;
         core_options[i].values[j].label = NULL;
      };
      ++i;
   }

   environ_cb = cb;
   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);

   unsigned version = 0;
   if (!cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version))
   {
      if (log_cb)
         log_cb(RETRO_LOG_INFO, "retro_set_environment: GET_CORE_OPTIONS_VERSION failed, not setting CORE_OPTIONS now.\n");
   }
   else if (version == 1)
   {
      cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, core_options);
   }
   else
   {
      /* Fallback for older API */
      /* Use define because C doesn't care about const. */
#define NUM_CORE_OPTIONS ( sizeof(core_options)/sizeof(core_options[0])-1 )
      static struct retro_variable variables[NUM_CORE_OPTIONS+1];

      /* Only generate variables once, it's as static as core_options */
      if (!core_options_legacy_strings)
      {
         /* First pass: Calculate size of string-buffer required */
         unsigned buf_len;
         char *buf;
         {
            unsigned alloc_len = 0;
            struct retro_core_option_definition *o = core_options + NUM_CORE_OPTIONS - 1;
            struct retro_variable *rv = variables + NUM_CORE_OPTIONS - 1;
            for (; o >= core_options; --o, --rv)
            {
               int l = snprintf(0, 0, "%s; %s", o->desc, o->default_value);
               for (struct retro_core_option_value *v = o->values; v->value; ++v)
                  l += snprintf(0, 0, "|%s", v->value);
               alloc_len += l + 1;
            }
            buf = core_options_legacy_strings = (char *)malloc(alloc_len);
            buf_len = alloc_len;
         }
         /* Second pass: Fill string-buffer */
         struct retro_core_option_definition *o = core_options + NUM_CORE_OPTIONS - 1;
         struct retro_variable *rv = variables + NUM_CORE_OPTIONS;
         rv->key = rv->value = 0;
         --rv;
         for (; o >= core_options; --o, --rv)
         {
            int l = snprintf(buf, buf_len, "%s; %s", o->desc, o->default_value);
            for (struct retro_core_option_value *v = o->values; v->value; ++v)
               if (v->value != o->default_value)
                  l += snprintf(buf+l, buf_len, "|%s", v->value);
            rv->key = o->key;
            rv->value = buf;
            ++l;
            buf += l;
            buf_len -= l;
         }
      }
      cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
#undef NUM_CORE_OPTIONS
   }

   static bool allowNoGameMode;
   allowNoGameMode = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &allowNoGameMode);

   static struct retro_led_interface led_interface;
   cb(RETRO_ENVIRONMENT_GET_LED_INTERFACE, &led_interface);
   if (led_interface.set_led_state)
      led_state_cb = led_interface.set_led_state;
}

static void update_variables(void)
{
   uae_model[0] = '\0';
   uae_config[0] = '\0';

   struct retro_variable var = {0};
   struct retro_core_option_display option_display;

   var.key = "puae_model";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      _tcscpy(opt_model, var.value);
   }

   var.key = "puae_model_fd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      _tcscpy(opt_model_fd, var.value);
   }

   var.key = "puae_model_hd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      _tcscpy(opt_model_hd, var.value);
   }

   var.key = "puae_model_cd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      _tcscpy(opt_model_cd, var.value);
   }

   var.key = "puae_video_standard";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* video_config change only at start */
      if (video_config_old == 0 && !forced_video)
      {
         if (strstr(var.value, "PAL"))
         {
            video_config |= PUAE_VIDEO_PAL;
            strcat(uae_config, "ntsc=false\n");
            real_ntsc = false;
         }
         else
         {
            video_config |= PUAE_VIDEO_NTSC;
            strcat(uae_config, "ntsc=true\n");
            real_ntsc = true;
         }
      }
      else if (!forced_video)
      {
         if (strstr(var.value, "PAL")) changed_prefs.ntscmode = 0;
         else                          changed_prefs.ntscmode = 1;
      }

      if (strstr(var.value, "auto")) opt_region_auto = true;
      else                           opt_region_auto = false;
   }

   var.key = "puae_video_aspect";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "PAL"))  video_config_aspect = PUAE_VIDEO_PAL;
      else if (!strcmp(var.value, "NTSC")) video_config_aspect = PUAE_VIDEO_NTSC;
      else                                 video_config_aspect = 0;
   }

   var.key = "puae_video_allow_hz_change";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) video_config_allow_hz_change = 0;
      else                                video_config_allow_hz_change = 1;
   }

   var.key = "puae_video_resolution";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_video_resolution_auto = false;

      if (!strcmp(var.value, "lores"))
      {
         video_config &= ~PUAE_VIDEO_HIRES;
         video_config &= ~PUAE_VIDEO_SUPERHIRES;
         retro_max_diwlastword = retro_max_diwlastword_hires / 2;
         width_multiplier = 1;
         if (libretro_runloop_active)
            changed_prefs.gfx_resolution = RES_LORES;
      }
      else if (!strcmp(var.value, "hires"))
      {
         video_config &= ~PUAE_VIDEO_SUPERHIRES;
         video_config |= PUAE_VIDEO_HIRES;
         retro_max_diwlastword = retro_max_diwlastword_hires;
         width_multiplier = 2;
         if (libretro_runloop_active)
            changed_prefs.gfx_resolution = RES_HIRES;
      }
      else if (!strcmp(var.value, "superhires"))
      {
         video_config &= ~PUAE_VIDEO_HIRES;
         video_config |= PUAE_VIDEO_SUPERHIRES;
         retro_max_diwlastword = retro_max_diwlastword_hires * 2;
         width_multiplier = 4;
         if (libretro_runloop_active)
            changed_prefs.gfx_resolution = RES_SUPERHIRES;
      }
      else if (!strcmp(var.value, "auto"))
      {
         opt_video_resolution_auto = true;

         if (video_config_old & PUAE_VIDEO_SUPERHIRES)
         {
            video_config &= ~PUAE_VIDEO_HIRES;
            video_config |= PUAE_VIDEO_SUPERHIRES;
            retro_max_diwlastword = retro_max_diwlastword_hires * 2;
            width_multiplier = 4;
            if (libretro_runloop_active)
               changed_prefs.gfx_resolution = RES_SUPERHIRES;
         }
         else
         {
            video_config &= ~PUAE_VIDEO_SUPERHIRES;
            video_config |= PUAE_VIDEO_HIRES;
            retro_max_diwlastword = retro_max_diwlastword_hires;
            width_multiplier = 2;
            if (libretro_runloop_active)
               changed_prefs.gfx_resolution = RES_HIRES;
         }
      }

      /* Resolution change needs init_custom() to be done after reset_drawing() is done */
      if (libretro_runloop_active && video_config != video_config_old)
         request_init_custom_timer = 3;
   }

   var.key = "puae_video_vresolution";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_video_vresolution_auto = false;

      if (!strcmp(var.value, "double"))
      {
         video_config |= PUAE_VIDEO_DOUBLELINE;
         if (libretro_runloop_active)
            changed_prefs.gfx_vresolution = VRES_DOUBLE;
      }
      else if (!strcmp(var.value, "single"))
      {
         video_config &= ~PUAE_VIDEO_DOUBLELINE;
         if (libretro_runloop_active)
            changed_prefs.gfx_vresolution = VRES_NONDOUBLE;
      }
      else if (!strcmp(var.value, "auto"))
      {
         opt_video_vresolution_auto = true;

         if (libretro_runloop_active)
         {
            if (video_config_old & PUAE_VIDEO_DOUBLELINE)
            {
               video_config |= PUAE_VIDEO_DOUBLELINE;
               changed_prefs.gfx_vresolution = VRES_DOUBLE;
            }
            else
            {
               video_config &= ~PUAE_VIDEO_DOUBLELINE;
               changed_prefs.gfx_vresolution = VRES_NONDOUBLE;
            }
         }
      }

      /* Lores can not be double lined */
      if (retro_max_diwlastword < retro_max_diwlastword_hires)
      {
         video_config &= ~PUAE_VIDEO_DOUBLELINE;
         changed_prefs.gfx_vresolution = VRES_NONDOUBLE;
      }

      /* Resolution change needs init_custom() to be done after reset_drawing() is done */
      if (libretro_runloop_active && video_config != video_config_old)
         request_init_custom_timer = 3;
   }

   var.key = "puae_statusbar";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_statusbar = 0;

      if (strstr(var.value, "top"))
      {
         opt_statusbar |= STATUSBAR_TOP;
         opt_statusbar_position = -1;
      }
      else
      {
         opt_statusbar |= STATUSBAR_BOTTOM;
         opt_statusbar_position = 0;
      }

      if (strstr(var.value, "basic"))
         opt_statusbar |= STATUSBAR_BASIC;

      if (strstr(var.value, "minimal"))
         opt_statusbar |= STATUSBAR_MINIMAL;

      opt_statusbar_position_old = opt_statusbar_position;
   }

   var.key = "puae_vkbd_theme";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_vkbd_theme = atoi(var.value);
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
      if (!strcmp(var.value, "normal"))
      {
         strcat(uae_config, "cpu_compatible=false\n");
         strcat(uae_config, "cycle_exact=false\n");
      }
      else if (!strcmp(var.value, "compatible"))
      {
         strcat(uae_config, "cpu_compatible=true\n");
         strcat(uae_config, "cycle_exact=false\n");
      }
      else if (!strcmp(var.value, "exact"))
      {
         strcat(uae_config, "cpu_compatible=true\n");
         strcat(uae_config, "cycle_exact=true\n");
      }

      if (libretro_runloop_active && !cpu_cycle_exact_force)
      {
         if (!strcmp(var.value, "normal"))
         {
            changed_prefs.cpu_compatible = 0;
            changed_prefs.cpu_cycle_exact = 0;
            changed_prefs.blitter_cycle_exact = 0;
         }
         else if (!strcmp(var.value, "compatible"))
         {
            changed_prefs.cpu_compatible = 1;
            changed_prefs.cpu_cycle_exact = 0;
            changed_prefs.blitter_cycle_exact = 0;
         }
         else if (!strcmp(var.value, "exact"))
         {
            changed_prefs.cpu_compatible = 1;
            changed_prefs.cpu_cycle_exact = 1;
            changed_prefs.blitter_cycle_exact = 1;
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

      if (libretro_runloop_active)
         changed_prefs.m68k_speed_throttle = atof(var.value);
   }

   var.key = "puae_cpu_multiplier";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cpu_multiplier=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (libretro_runloop_active)
         changed_prefs.cpu_clock_multiplier = atoi(var.value) * 256;
   }

   var.key = "puae_sound_stereo_separation";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int val = atoi(var.value) / 10;
      char valbuf[4];
      snprintf(valbuf, 4, "%d", val);

      strcat(uae_config, "sound_stereo_separation=");
      strcat(uae_config, valbuf);
      strcat(uae_config, "\n");

      if (libretro_runloop_active)
         changed_prefs.sound_stereo_separation = val;
   }

   var.key = "puae_sound_interpol";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_interpol=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (libretro_runloop_active)
      {
         if      (!strcmp(var.value, "none")) changed_prefs.sound_interpol = 0;
         else if (!strcmp(var.value, "anti")) changed_prefs.sound_interpol = 1;
         else if (!strcmp(var.value, "sinc")) changed_prefs.sound_interpol = 2;
         else if (!strcmp(var.value, "rh"))   changed_prefs.sound_interpol = 3;
         else if (!strcmp(var.value, "crux")) changed_prefs.sound_interpol = 4;
      }
   }

   var.key = "puae_sound_filter";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_filter=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");
      
      if (libretro_runloop_active)
      {
         if      (!strcmp(var.value, "emulated")) changed_prefs.sound_filter = FILTER_SOUND_EMUL;
         else if (!strcmp(var.value, "off"))      changed_prefs.sound_filter = FILTER_SOUND_OFF;
         else if (!strcmp(var.value, "on"))       changed_prefs.sound_filter = FILTER_SOUND_ON;
      }
   }

   var.key = "puae_sound_filter_type";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "auto"))
      {
         automatic_sound_filter_type_update = false;
         strcat(uae_config, "sound_filter_type=");
         strcat(uae_config, var.value);
         strcat(uae_config, "\n");
      }

      if (libretro_runloop_active)
      {
         if      (!strcmp(var.value, "standard")) changed_prefs.sound_filter_type = FILTER_SOUND_TYPE_A500;
         else if (!strcmp(var.value, "enhanced")) changed_prefs.sound_filter_type = FILTER_SOUND_TYPE_A1200;
         else if (!strcmp(var.value, "auto"))     automatic_sound_filter_type_update = true;
      }
   }

   var.key = "puae_sound_volume_cd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* 100 is mute, 0 is max */
      int val = 100 - atoi(var.value);
      char valbuf[4];
      snprintf(valbuf, 4, "%d", val);

      strcat(uae_config, "sound_volume_cd=");
      strcat(uae_config, valbuf);
      strcat(uae_config, "\n");

      if (libretro_runloop_active)
         changed_prefs.sound_volume_cd = val;
   }

   var.key = "puae_cd_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cd_speed=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (libretro_runloop_active)
         changed_prefs.cd_speed = atoi(var.value);
   }

   var.key = "puae_floppy_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "floppy_speed=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (libretro_runloop_active)
         changed_prefs.floppy_speed = atoi(var.value);
   }

   var.key = "puae_floppy_multidrive";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_floppy_multidrive = false;
      else                                opt_floppy_multidrive = true;
   }

   var.key = "puae_floppy_write_protection";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int val = 0;
      if (!strcmp(var.value, "enabled")) val = 1;

      if (val)
         strcat(uae_config, "floppy_write_protected=true\n");

      if (libretro_runloop_active)
      {
         changed_prefs.floppy_read_only = val;
         if (changed_prefs.floppy_read_only != currprefs.floppy_read_only)
         {
            currprefs.floppy_read_only = val;
            for (unsigned i = 0; i < 4; i++)
               DISK_reinsert(i);
         }
      }
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
      if (libretro_runloop_active)
         changed_prefs.dfxclickvolume = atoi(var.value);
   }

   var.key = "puae_floppy_sound_empty_mute";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_floppy_sound_empty_mute = false;
      else                                opt_floppy_sound_empty_mute = true;
   }

   var.key = "puae_floppy_sound_type";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "internal"))
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

      if (libretro_runloop_active)
      {
         for (unsigned i = 0; i < 4; i++)
         {
            if (!strcmp(var.value, "internal"))
            {
               changed_prefs.floppyslots[i].dfxclick = 1;
            }
            else
            {
               changed_prefs.floppyslots[i].dfxclick = -1;
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

      if (libretro_runloop_active)
      {
         int val = 0;
         val = atoi(var.value);
         changed_prefs.input_mouse_speed = val;
      }
   }

   var.key = "puae_immediate_blits";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "false"))
      {
         strcat(uae_config, "immediate_blits=false\n");
         strcat(uae_config, "waiting_blits=false\n");
      }
      else if (!strcmp(var.value, "immediate"))
      {
         strcat(uae_config, "immediate_blits=true\n");
         strcat(uae_config, "waiting_blits=disabled\n");
      }
      else if (!strcmp(var.value, "waiting"))
      {
         strcat(uae_config, "immediate_blits=false\n");
         strcat(uae_config, "waiting_blits=automatic\n");
      }

      if (libretro_runloop_active)
      {
         if (!strcmp(var.value, "false"))
         {
            changed_prefs.immediate_blits = 0;
            changed_prefs.waiting_blits = 0;
         }
         else if (!strcmp(var.value, "immediate"))
         {
            changed_prefs.immediate_blits = 1;
            changed_prefs.waiting_blits = 0;
         }
         else if (!strcmp(var.value, "waiting"))
         {
            changed_prefs.immediate_blits = 0;
            changed_prefs.waiting_blits = 1;
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

      if (libretro_runloop_active)
      {
         if      (!strcmp(var.value, "none"))       changed_prefs.collision_level = 0;
         else if (!strcmp(var.value, "sprites"))    changed_prefs.collision_level = 1;
         else if (!strcmp(var.value, "playfields")) changed_prefs.collision_level = 2;
         else if (!strcmp(var.value, "full"))       changed_prefs.collision_level = 3;
      }
   }

   var.key = "puae_gfx_framerate";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int val = 0;
      if      (!strcmp(var.value, "disabled")) val = 1;
      else if (!strcmp(var.value, "1"))        val = 2;
      else if (!strcmp(var.value, "2"))        val = 3;

      if (val > 1)
      {
         char valbuf[4];
         snprintf(valbuf, 4, "%d", val);
         strcat(uae_config, "gfx_framerate=");
         strcat(uae_config, valbuf);
         strcat(uae_config, "\n");
      }

      if (libretro_runloop_active && !strstr(uae_custom_config, "gfx_framerate="))
         changed_prefs.gfx_framerate = val;
   }

   var.key = "puae_gfx_colors";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* Only allow screenmode change after restart */
      if (!pix_bytes_initialized)
      {
         if (!strcmp(var.value, "16bit"))      pix_bytes = 2;
         else if (!strcmp(var.value, "24bit")) pix_bytes = 4;
      }
   }

   var.key = "puae_gfx_flickerfixer";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled"))
         strcat(uae_config, "gfx_flickerfixer=false\n");
      else
         strcat(uae_config, "gfx_flickerfixer=true\n");

      if (libretro_runloop_active)
      {
         if (!strcmp(var.value, "disabled")) changed_prefs.gfx_scandoubler = 0;
         else                                changed_prefs.gfx_scandoubler = 1;

         /* Change requires init_custom() */
         if (changed_prefs.gfx_scandoubler != currprefs.gfx_scandoubler)
            request_init_custom_timer = 3;
      }
   }

   var.key = "puae_zoom_mode";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled")) zoom_mode_id = 0;
      else if (!strcmp(var.value, "minimum"))  zoom_mode_id = 1;
      else if (!strcmp(var.value, "smaller"))  zoom_mode_id = 2;
      else if (!strcmp(var.value, "small"))    zoom_mode_id = 3;
      else if (!strcmp(var.value, "medium"))   zoom_mode_id = 4;
      else if (!strcmp(var.value, "large"))    zoom_mode_id = 5;
      else if (!strcmp(var.value, "larger"))   zoom_mode_id = 6;
      else if (!strcmp(var.value, "maximum"))  zoom_mode_id = 7;
      else if (!strcmp(var.value, "auto"))     zoom_mode_id = 8;

      opt_zoom_mode_id = zoom_mode_id;
   }

   var.key = "puae_zoom_mode_crop";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "both"))       zoom_mode_crop_id = 0;
      else if (!strcmp(var.value, "vertical"))   zoom_mode_crop_id = 1;
      else if (!strcmp(var.value, "horizontal")) zoom_mode_crop_id = 2;
      else if (!strcmp(var.value, "16:9"))       zoom_mode_crop_id = 3;
      else if (!strcmp(var.value, "16:10"))      zoom_mode_crop_id = 4;
      else if (!strcmp(var.value, "4:3"))        zoom_mode_crop_id = 5;
      else if (!strcmp(var.value, "5:4"))        zoom_mode_crop_id = 6;
   }

   var.key = "puae_vertical_pos";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      opt_vertical_offset = 0;
      if (!strcmp(var.value, "auto"))
      {
         opt_vertical_offset_auto = true;
         thisframe_y_adjust = minfirstline;
      }
      else
      {
         opt_vertical_offset_auto = false;
         int new_vertical_offset = atoi(var.value);
         if (new_vertical_offset >= -20 && new_vertical_offset <= 70)
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
      opt_horizontal_offset = 0;
      if (!strcmp(var.value, "auto"))
      {
         opt_horizontal_offset_auto = true;
      }
      else
      {
         opt_horizontal_offset_auto = false;
         int new_horizontal_offset = atoi(var.value);
         if (new_horizontal_offset >= -40 && new_horizontal_offset <= 40)
         {
            opt_horizontal_offset = new_horizontal_offset;
            visible_left_border = retro_max_diwlastword - retrow - (opt_horizontal_offset * width_multiplier);
         }
      }
   }

   var.key = "puae_use_whdload";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled")) opt_use_whdload = 0;
      else if (!strcmp(var.value, "files"))    opt_use_whdload = 1;
      else if (!strcmp(var.value, "hdfs"))     opt_use_whdload = 2;
   }

   var.key = "puae_use_whdload_prefs";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled")) opt_use_whdload_prefs = 0;
      else if (!strcmp(var.value, "config"))   opt_use_whdload_prefs = 1;
      else if (!strcmp(var.value, "splash"))   opt_use_whdload_prefs = 2;
      else if (!strcmp(var.value, "both"))     opt_use_whdload_prefs = 3;
   }

   var.key = "puae_cd_startup_delayed_insert";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_cd_startup_delayed_insert = false;
      else                                opt_cd_startup_delayed_insert = true;
   }

   var.key = "puae_shared_nvram";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_shared_nvram = false;
      else                                opt_shared_nvram = true;
   }

   var.key = "puae_use_boot_hd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled")) opt_use_boot_hd = 0;
      else if (!strcmp(var.value, "files"))    opt_use_boot_hd = 1;
      else if (!strcmp(var.value, "hdf20"))    opt_use_boot_hd = 2;
      else if (!strcmp(var.value, "hdf40"))    opt_use_boot_hd = 3;
      else if (!strcmp(var.value, "hdf80"))    opt_use_boot_hd = 4;
      else if (!strcmp(var.value, "hdf128"))   opt_use_boot_hd = 5;
      else if (!strcmp(var.value, "hdf256"))   opt_use_boot_hd = 6;
      else if (!strcmp(var.value, "hdf512"))   opt_use_boot_hd = 7;
   }

   var.key = "puae_analogmouse";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled")) opt_analogmouse = 0;
      else if (!strcmp(var.value, "left"))     opt_analogmouse = 1;
      else if (!strcmp(var.value, "right"))    opt_analogmouse = 2;
      else if (!strcmp(var.value, "both"))     opt_analogmouse = 3;
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
      if (!strcmp(var.value, "disabled")) opt_multimouse = false;
      else                                opt_multimouse = true;
   }

   var.key = "puae_keyrah_keypad_mappings";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_keyrah_keypad = false;
      else                                opt_keyrah_keypad = true;
   }

   var.key = "puae_physical_keyboard_pass_through";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_keyboard_pass_through = false;
      else                                opt_keyboard_pass_through = true;
   }

   var.key = "puae_model_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_model_options_display = 0;
      else                                opt_model_options_display = 1;
   }

   var.key = "puae_mapping_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_mapping_options_display = 0;
      else                                opt_mapping_options_display = 1;
   }

   var.key = "puae_video_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_video_options_display = 0;
      else                                opt_video_options_display = 1;
   }

   var.key = "puae_audio_options_display";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!strcmp(var.value, "disabled")) opt_audio_options_display = 0;
      else                                opt_audio_options_display = 1;
   }

   var.key = "puae_joyport";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (!mousemode_locked)
      {
         if      (!strcmp(var.value, "joystick")) retro_mousemode = false;
         else if (!strcmp(var.value, "mouse"))    retro_mousemode = true;
      }
   }

   var.key = "puae_joyport_order";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      snprintf(opt_joyport_order, sizeof(opt_joyport_order), "%s", var.value);
   }

   var.key = "puae_retropad_options";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled"))    opt_retropad_options = 0;
      else if (!strcmp(var.value, "rotate"))      opt_retropad_options = 1;
      else if (!strcmp(var.value, "jump"))        opt_retropad_options = 2;
      else if (!strcmp(var.value, "rotate_jump")) opt_retropad_options = 3;
   }

   var.key = "puae_cd32pad_options";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled"))    opt_cd32pad_options = 0;
      else if (!strcmp(var.value, "rotate"))      opt_cd32pad_options = 1;
      else if (!strcmp(var.value, "jump"))        opt_cd32pad_options = 2;
      else if (!strcmp(var.value, "rotate_jump")) opt_cd32pad_options = 3;
   }

   var.key = "puae_turbo_fire_button";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if      (!strcmp(var.value, "disabled")) turbo_fire_button = -1;
      else if (!strcmp(var.value, "B"))        turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_B;
      else if (!strcmp(var.value, "A"))        turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_A;
      else if (!strcmp(var.value, "Y"))        turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_Y;
      else if (!strcmp(var.value, "X"))        turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_X;
      else if (!strcmp(var.value, "L"))        turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_L;
      else if (!strcmp(var.value, "R"))        turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_R;
      else if (!strcmp(var.value, "L2"))       turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_L2;
      else if (!strcmp(var.value, "R2"))       turbo_fire_button = RETRO_DEVICE_ID_JOYPAD_R2;
   }

   var.key = "puae_turbo_pulse";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      turbo_pulse = atoi(var.value);
   }

   /* Mapper */
   var.key = "puae_mapper_select";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_SELECT] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_start";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_START] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_b";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_B] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_a";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_A] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_y";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_Y] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_x";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_X] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_l";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_L] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_r";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_R] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_l2";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_L2] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_r2";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_R2] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_l3";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_L3] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_r3";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_R3] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_lr";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_LR] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_ll";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_LL] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_ld";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_LD] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_lu";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_LU] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_rr";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_RR] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_rl";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_RL] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_rd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_RD] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_ru";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_DEVICE_ID_JOYPAD_RU] = retro_keymap_id(var.value);
   }

   /* Mapper hotkeys */
   var.key = "puae_mapper_vkbd";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_MAPPER_VKBD] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_statusbar";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_MAPPER_STATUSBAR] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_mouse_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_MAPPER_JOYMOUSE] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_reset";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_MAPPER_RESET] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_aspect_ratio_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_MAPPER_ASPECT_RATIO] = retro_keymap_id(var.value);
   }

   var.key = "puae_mapper_zoom_mode_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[RETRO_MAPPER_ZOOM_MODE] = retro_keymap_id(var.value);
   }

   /*** Options display ***/

   /* Model options */
   option_display.visible = opt_model_options_display;

   option_display.key = "puae_model_fd";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_model_hd";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_model_cd";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);

   /* Video options */
   option_display.visible = opt_video_options_display;

   option_display.key = "puae_video_resolution";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_vresolution";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_allow_hz_change";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_standard";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_video_aspect";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_zoom_mode";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_zoom_mode_crop";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_vertical_pos";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_horizontal_pos";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_collision_level";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_immediate_blits";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_gfx_flickerfixer";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_gfx_framerate";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_gfx_colors";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_statusbar";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_vkbd_theme";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_vkbd_alpha";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);

   /* Audio options */
   option_display.visible = opt_audio_options_display;

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
   option_display.key = "puae_floppy_sound_empty_mute";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_floppy_sound_type";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);

   /* Mapping options */
   option_display.visible = opt_mapping_options_display;

   option_display.key = "puae_mapper_select";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_start";
   environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
   option_display.key = "puae_mapper_b";
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

   /* Always trigger changed prefs */
   config_changed = 1;
   check_prefs_changed_audio();
   check_prefs_changed_custom();
   check_prefs_changed_cpu();
   config_changed = 0;
}

/*****************************************************************************/
/* Disk Control */
static bool retro_disk_set_eject_state(bool ejected)
{
   if (dc)
   {
      if (dc->eject_state == ejected)
         return true;
      else
         dc->eject_state = ejected;

      if (dc->files[dc->index] > 0 && path_is_valid(dc->files[dc->index]))
          display_current_image(((!dc->eject_state) ? dc->labels[dc->index] : ""), !dc->eject_state);

      if (dc->eject_state)
      {
         if (dc->files[dc->index] > 0)
         {
            if (dc->types[dc->index] == DC_IMAGE_TYPE_FLOPPY)
            {
               changed_prefs.floppyslots[0].df[0] = 0;
               disk_eject(0);
            }
            else if (dc->types[dc->index] == DC_IMAGE_TYPE_CD)
            {
               changed_prefs.cdslots[0].name[0] = 0;
            }
         }
      }
      else
      {
         if (dc->files[dc->index] > 0 && path_is_valid(dc->files[dc->index]))
         {
            if (dc->types[dc->index] == DC_IMAGE_TYPE_FLOPPY)
            {
               strcpy(changed_prefs.floppyslots[0].df, dc->files[dc->index]);
               DISK_reinsert(0);
            }
            else if (dc->types[dc->index] == DC_IMAGE_TYPE_CD)
            {
               strcpy(changed_prefs.cdslots[0].name, dc->files[dc->index]);
            }
         }
      }
   }

   return true;
}

static bool retro_disk_get_eject_state(void)
{
   if (dc)
      return dc->eject_state;

   return true;
}

static unsigned retro_disk_get_image_index(void)
{
   if (dc)
      return dc->index;

   return 0;
}

bool retro_disk_set_image_index(unsigned index)
{
   if (dc)
   {
      if (index == dc->index)
         return true;

      if (dc->replace)
      {
         dc->replace = false;
         index = 0;
      }

      if (index < dc->count && dc->files[index])
      {
         dc->index = index;
         display_current_image(dc->labels[dc->index], false);
         log_cb(RETRO_LOG_INFO, "Disk (%d) inserted in drive DF0: '%s'\n", dc->index+1, dc->files[dc->index]);
         return true;
      }
   }

   return false;
}

static unsigned retro_disk_get_num_images(void)
{
   if (dc)
      return dc->count;

   return 0;
}

static bool retro_disk_replace_image_index(unsigned index, const struct retro_game_info *info)
{
   if (dc)
   {
      if (info != NULL)
         dc_replace_file(dc, index, info->path);
      else
         dc_remove_file(dc, index);
      return true;
   }

   return false;
}

static bool retro_disk_add_image_index(void)
{
   if (dc)
   {
      if (dc->count <= DC_MAX_SIZE)
      {
         dc->files[dc->count]  = NULL;
         dc->labels[dc->count] = NULL;
         dc->types[dc->count]  = DC_IMAGE_TYPE_NONE;
         dc->count++;
         return true;
      }
   }

   return false;
}

static bool retro_disk_get_image_path(unsigned index, char *path, size_t len)
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

static bool retro_disk_get_image_label(unsigned index, char *label, size_t len)
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
   retro_disk_set_eject_state,
   retro_disk_get_eject_state,
   retro_disk_get_image_index,
   retro_disk_set_image_index,
   retro_disk_get_num_images,
   retro_disk_replace_image_index,
   retro_disk_add_image_index,
};

static struct retro_disk_control_ext_callback disk_interface_ext = {
   retro_disk_set_eject_state,
   retro_disk_get_eject_state,
   retro_disk_get_image_index,
   retro_disk_set_image_index,
   retro_disk_get_num_images,
   retro_disk_replace_image_index,
   retro_disk_add_image_index,
   NULL, /* set_initial_image */
   retro_disk_get_image_path,
   retro_disk_get_image_label,
};

/*****************************************************************************/

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

void retro_init(void)
{
   struct retro_log_callback log;
   log_cb = fallback_log;
   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;

   const char *system_dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
   {
      strlcpy(retro_system_directory,
              system_dir,
              sizeof(retro_system_directory));
   }

   const char *content_dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
   {
      strlcpy(retro_content_directory,
              content_dir,
              sizeof(retro_content_directory));
   }

   const char *save_dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
   {
      /* If save directory is defined use it, otherwise use system directory */
      strlcpy(retro_save_directory,
              string_is_empty(save_dir) ? retro_system_directory : save_dir,
              sizeof(retro_save_directory));
   }
   else
   {
      /* Make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend */
      strlcpy(retro_save_directory,
              retro_system_directory,
              sizeof(retro_save_directory));
   }

   /* Temp directory for ZIPs */
   snprintf(retro_temp_directory, sizeof(retro_temp_directory), "%s%s%s", retro_save_directory, DIR_SEP_STR, "TEMP");

   /* Clean ZIP temp */
   if (!string_is_empty(retro_temp_directory) && path_is_directory(retro_temp_directory))
      remove_recurse(retro_temp_directory);

   /* Disk Control interface */
   dc = dc_create();
   unsigned dci_version = 0;
   if (environ_cb(RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION, &dci_version) && (dci_version >= 1))
      environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE, &disk_interface_ext);
   else
      environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE, &disk_interface);

   /* Savestates
    * > Considered incomplete because runahead cannot
    *   be enabled until content is full loaded */
   static uint64_t quirks = RETRO_SERIALIZATION_QUIRK_INCOMPLETE;
   environ_cb(RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS, &quirks);

   /* > Ensure save state de-serialization file
    *   is closed/NULL
    *   (redundant safety check, possibly required
    *   for static builds...) */
   if (retro_deserialize_file)
   {
      zfile_fclose(retro_deserialize_file);
      retro_deserialize_file = NULL;
   }

   /* Inputs */
   #define RETRO_DESCRIPTOR_BLOCK(_user)                                                                        \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },                                          \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },                                      \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },                                      \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },                                    \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "B / Fire / Red" },                               \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "A / 2nd fire / Blue" },                          \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Y / Green" },                                    \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "X / Yellow" },                                   \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },                                  \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start / Play" },                             \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "L / Rewind" },                                   \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "R / Forward" },                                  \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "L2" },                                          \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "R2" },                                          \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "L3" },                                          \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "R3" },                                          \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X, "Left Analog X" },   \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y, "Left Analog Y" },   \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, "Right Analog X" }, \
   { _user, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, "Right Analog Y" }

   static struct retro_input_descriptor input_descriptors[] =
   {
      RETRO_DESCRIPTOR_BLOCK(0),
      RETRO_DESCRIPTOR_BLOCK(1),
      RETRO_DESCRIPTOR_BLOCK(2),
      RETRO_DESCRIPTOR_BLOCK(3),
      RETRO_DESCRIPTOR_BLOCK(4),
      {0},
   };
   #undef RETRO_DESCRIPTOR_BLOCK
   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &input_descriptors);

   if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
      libretro_supports_bitmasks = true;

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      log_cb(RETRO_LOG_ERROR, "RGB565 is not supported.\n");
      environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
      return;
   }

   memset(retro_bmp, 0, sizeof(retro_bmp));
   memset(retro_key_state, 0, sizeof(retro_key_state));
   memset(retro_key_state_old, 0, sizeof(retro_key_state_old));

   libretro_runloop_active = 0;
   update_variables();

   /* Screen resolution */
   log_cb(RETRO_LOG_DEBUG, "Resolution selected: %dx%d\n", defaultw, defaulth);
   retrow = defaultw;
   retroh = defaulth;
}

void retro_deinit(void)
{
   /* Clean the M3U storage */
   if (dc)
      dc_free(dc);

   /* Clean legacy strings */
   if (core_options_legacy_strings)
      free(core_options_legacy_strings);

   /* Clean ZIP temp */
   if (!string_is_empty(retro_temp_directory) && path_is_directory(retro_temp_directory))
      remove_recurse(retro_temp_directory);

   /* 'Reset' troublesome static variables */
   pix_bytes_initialized = false;
   cpu_cycle_exact_force = false;
   automatic_sound_filter_type_update = true;
   fake_ntsc = false;
   real_ntsc = false;
   forced_video = false;
   locked_video_horizontal = false;
   libretro_supports_bitmasks = false;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   if (port < RETRO_DEVICES)
   {
      int uae_port;
      uae_port = (port == 0) ? 1 : 0;
      cd32_pad_enabled[uae_port] = 0;
      retro_devices[port] = device;
#if 0
      switch (device)
      {
         case RETRO_DEVICE_JOYPAD:
            log_cb(RETRO_LOG_INFO, "Controller %u: RetroPad\n", (port+1));
            break;

         case RETRO_DEVICE_CD32PAD:
            log_cb(RETRO_LOG_INFO, "Controller %u: CD32 Pad\n", (port+1));
            cd32_pad_enabled[uae_port] = 1;
            break;

         case RETRO_DEVICE_ANALOGJOYSTICK:
            log_cb(RETRO_LOG_INFO, "Controller %u: Analog Joystick\n", (port+1));
            break;

         case RETRO_DEVICE_JOYSTICK:
            log_cb(RETRO_LOG_INFO, "Controller %u: Joystick\n", (port+1));
            break;

         case RETRO_DEVICE_KEYBOARD:
            log_cb(RETRO_LOG_INFO, "Controller %u: Keyboard\n", (port+1));
            break;

         case RETRO_DEVICE_NONE:
            log_cb(RETRO_LOG_INFO, "Controller %u: None\n", (port+1));
            break;
      }
#else
      if (device == RETRO_DEVICE_CD32PAD)
         cd32_pad_enabled[uae_port] = 1;
#endif

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
   info->block_extract    = true;
   info->valid_extensions = "adf|adz|dms|fdi|ipf|hdf|hdz|lha|slave|info|cue|ccd|nrg|mds|iso|uae|m3u|zip";
}

double retro_get_aspect_ratio(unsigned int width, unsigned int height, bool pixel_aspect)
{
   double ar = 1;
   double par = 1;

   if (video_config_geometry & PUAE_VIDEO_NTSC || video_config_aspect == PUAE_VIDEO_NTSC)
      par = (double)44.0 / (double)52.0;
   ar = ((double)width / (double)height) * par;

   if (video_config_geometry & PUAE_VIDEO_DOUBLELINE)
   {
      if (video_config_geometry & PUAE_VIDEO_HIRES)
         ;
      else if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
         ar /= 2;
   }
   else
   {
      if (video_config_geometry & PUAE_VIDEO_HIRES)
         ar /= 2;
      else if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
         ar /= 4;
   }

   if (pixel_aspect)
      return par;
   return ar;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   /* Need to do this here because core option values are not available in retro_init */
   if (!pix_bytes_initialized)
   {
      pix_bytes_initialized = true;
      if (pix_bytes == 4)
      {
         enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
         if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
         {
            pix_bytes = 2;
            log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported. Trying RGB565.\n");
            fmt = RETRO_PIXEL_FORMAT_RGB565;
            if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
            {
               log_cb(RETRO_LOG_INFO, "RGB565 is not supported.\n");
               exit(0);
            }
         }
      }
   }

   info->geometry.base_width   = retrow;
   info->geometry.base_height  = retroh;
   info->geometry.max_width    = EMULATOR_MAX_WIDTH;
   info->geometry.max_height   = EMULATOR_MAX_HEIGHT;
   info->geometry.aspect_ratio = retro_get_aspect_ratio(retrow, retroh, false);

   info->timing.sample_rate    = 44100.0;
   info->timing.fps            = (retro_get_region() == RETRO_REGION_NTSC) ? PUAE_VIDEO_HZ_NTSC : PUAE_VIDEO_HZ_PAL;
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

static void retro_force_region(FILE** configfile)
{
   /* If region was specified in the path */
   if (strstr(full_path, "NTSC") || strstr(full_path, "(USA)"))
   {
      log_cb(RETRO_LOG_INFO, "Forcing NTSC mode\n");
      fprintf(*configfile, "ntsc=true\n");
      real_ntsc = true;
      forced_video = true;
   }
   else if (strstr(full_path, "PAL") || strstr(full_path, "(Europe)")
         || strstr(full_path, "(France)") || strstr(full_path, "(Germany)")
         || strstr(full_path, "(Italy)") || strstr(full_path, "(Spain)")
         || strstr(full_path, "(Finland)") || strstr(full_path, "(Denmark)")
         || strstr(full_path, "(Sweden)"))
   {
      log_cb(RETRO_LOG_INFO, "Forcing PAL mode\n");
      fprintf(*configfile, "ntsc=false\n");
      real_ntsc = false;
      forced_video = true;
   }
}

static void retro_use_boot_hd(FILE** configfile)
{
   char *tmp_str = NULL;
   char boothd_size[5] = {0};

   snprintf(boothd_size, sizeof(boothd_size), "%dM", 0);
   if (opt_use_boot_hd > 1)
   {
      switch (opt_use_boot_hd)
      {
         case 2:
            snprintf(boothd_size, sizeof(boothd_size), "%dM", 20);
            break;
         case 3:
            snprintf(boothd_size, sizeof(boothd_size), "%dM", 40);
            break;
         case 4:
            snprintf(boothd_size, sizeof(boothd_size), "%dM", 80);
            break;
         case 5:
            snprintf(boothd_size, sizeof(boothd_size), "%dM", 128);
            break;
         case 6:
            snprintf(boothd_size, sizeof(boothd_size), "%dM", 256);
            break;
         case 7:
            snprintf(boothd_size, sizeof(boothd_size), "%dM", 512);
            break;
      }
   }

   /* HDF mode */
   if (opt_use_boot_hd > 1)
   {
      /* Init Boot HD */
      char boothd_hdf[RETRO_PATH_MAX];
      path_join((char*)&boothd_hdf, retro_save_directory, LIBRETRO_PUAE_PREFIX ".hdf");
      if (!path_is_valid(boothd_hdf))
      {
         log_cb(RETRO_LOG_INFO, "Boot HD image file '%s' not found, attempting to create one\n", (const char*)&boothd_hdf);
         if (make_hdf(boothd_hdf, boothd_size, "BOOT"))
            log_cb(RETRO_LOG_ERROR, "Unable to create Boot HD image: '%s'\n", (const char*)&boothd_hdf);
      }
      if (path_is_valid(boothd_hdf))
      {
         tmp_str = string_replace_substring(boothd_hdf, "\\", "\\\\");
         fprintf(*configfile, "hardfile2=rw,BOOT:\"%s\",32,1,2,512,0,,uae0\n", (const char*)tmp_str);
         free(tmp_str);
         tmp_str = NULL;
      }
   }
   /* Directory mode */
   else if (opt_use_boot_hd == 1)
   {
      char boothd_path[RETRO_PATH_MAX];
      path_join((char*)&boothd_path, retro_save_directory, "BootHD");

      if (!path_is_directory(boothd_path))
      {
         log_cb(RETRO_LOG_INFO, "Boot HD image directory '%s' not found, attempting to create one\n", (const char*)&boothd_path);
         path_mkdir(boothd_path);
      }
      if (path_is_directory(boothd_path))
      {
         tmp_str = string_replace_substring(boothd_path, "\\", "\\\\");
         fprintf(*configfile, "filesystem2=rw,BOOT:Boot:\"%s\",0\n", (const char*)tmp_str);
         free(tmp_str);
         tmp_str = NULL;
      }
      else
         log_cb(RETRO_LOG_ERROR, "Unable to create Boot HD directory: '%s'\n", (const char*)&boothd_path);
   }
}

static void retro_print_kickstart(FILE** configfile)
{
   char kickstart[RETRO_PATH_MAX];
   path_join((char*)&kickstart, retro_system_directory, uae_kickstart);

   /* Main Kickstart */
   if (!path_is_valid(kickstart))
   {
      /* Kickstart ROM not found */
      log_cb(RETRO_LOG_ERROR, "Kickstart ROM '%s' not found!\n", (const char*)&kickstart);
      snprintf(retro_message_msg, sizeof(retro_message_msg),
               "Kickstart ROM '%s' not found!", path_basename((const char*)kickstart));
      retro_message = true;
   }
   else
      fprintf(*configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

   /* Extended KS + NVRAM */
   if (!string_is_empty(uae_kickstart_ext))
   {
      char kickstart_ext[RETRO_PATH_MAX];
      path_join((char*)&kickstart_ext, retro_system_directory, uae_kickstart_ext);

      /* Decide if CD32 ROM is combined based on filesize */
      struct stat kickstart_st;
      stat(kickstart, &kickstart_st);

      /* Verify extended ROM if external */
      if (kickstart_st.st_size <= 524288)
      {
         if (!path_is_valid(kickstart_ext))
         {
            /* Kickstart extended ROM not found */
            log_cb(RETRO_LOG_ERROR, "Kickstart extended ROM '%s' not found!\n", (const char*)&kickstart_ext);
            snprintf(retro_message_msg, sizeof(retro_message_msg),
                     "Kickstart extended ROM '%s' not found!", path_basename((const char*)kickstart_ext));
            retro_message = true;
         }
         else
            fprintf(*configfile, "kickstart_ext_rom_file=%s\n", (const char*)&kickstart_ext);
      }

      /* NVRAM */
      char flash_filename[RETRO_PATH_MAX];
      char flash_filepath[RETRO_PATH_MAX];
      /* Shared */
      if (opt_shared_nvram || string_is_empty(full_path))
      {
         snprintf(flash_filename, sizeof(flash_filename), "%s.nvr", LIBRETRO_PUAE_PREFIX);
         /* CDTV suffix */
         if (kickstart_st.st_size == 262144)
            snprintf(flash_filename, sizeof(flash_filename), "%s_cdtv.nvr", LIBRETRO_PUAE_PREFIX);
      }
      /* Per game */
      else
      {
         snprintf(flash_filename, sizeof(flash_filename), "%s", path_basename(full_path));
         snprintf(flash_filename, sizeof(flash_filename), "%s.nvr", path_remove_extension(flash_filename));
      }
      path_join((char*)&flash_filepath, retro_save_directory, flash_filename);
      log_cb(RETRO_LOG_INFO, "Using NVRAM: '%s'\n", flash_filepath);
      fprintf(*configfile, "flash_file=%s\n", (const char*)&flash_filepath);
   }
}

static char* emu_config_string(char* mode, int config)
{
   if (!strcmp(mode, "model"))
   {
      switch (config)
      {
         case EMU_CONFIG_A500:      return "A500";
         case EMU_CONFIG_A500OG:    return "A500OG";
         case EMU_CONFIG_A500PLUS:  return "A500PLUS";
         case EMU_CONFIG_A600:      return "A600";
         case EMU_CONFIG_A1200:     return "A1200";
         case EMU_CONFIG_A1200OG:   return "A1200OG";
         case EMU_CONFIG_A4030:     return "A4030";
         case EMU_CONFIG_A4040:     return "A4040";
         case EMU_CONFIG_CDTV:      return "CDTV";
         case EMU_CONFIG_CD32:      return "CD32";
         case EMU_CONFIG_CD32FR:    return "CD32FR";
      }
   }
   else if (!strcmp(mode, "kickstart"))
   {
      switch (config)
      {
         case EMU_CONFIG_A500:      return A500_ROM;
         case EMU_CONFIG_A500OG:    return A500_ROM;
         case EMU_CONFIG_A500PLUS:  return A500KS2_ROM;
         case EMU_CONFIG_A600:      return A600_ROM;
         case EMU_CONFIG_A1200:     return A1200_ROM;
         case EMU_CONFIG_A1200OG:   return A1200_ROM;
         case EMU_CONFIG_A4030:     return A4000_ROM;
         case EMU_CONFIG_A4040:     return A4000_ROM;
         case EMU_CONFIG_CDTV:      return A500_ROM;
         case EMU_CONFIG_CD32:      return CD32_ROM;
         case EMU_CONFIG_CD32FR:    return CD32_ROM;
      }
   }
   else if (!strcmp(mode, "kickstart_ext"))
   {
      switch (config)
      {
         case EMU_CONFIG_A500:      return "";
         case EMU_CONFIG_A500OG:    return "";
         case EMU_CONFIG_A500PLUS:  return "";
         case EMU_CONFIG_A600:      return "";
         case EMU_CONFIG_A1200:     return "";
         case EMU_CONFIG_A1200OG:   return "";
         case EMU_CONFIG_A4030:     return "";
         case EMU_CONFIG_A4040:     return "";
         case EMU_CONFIG_CDTV:      return CDTV_ROM;
         case EMU_CONFIG_CD32:      return CD32_ROM_EXT;
         case EMU_CONFIG_CD32FR:    return CD32_ROM_EXT;
      }
   }
   return "";
}

static int emu_config_int(char* model)
{
   if (!strcmp(model, "A500"))          return EMU_CONFIG_A500;
   else if (!strcmp(model, "A500OG"))   return EMU_CONFIG_A500OG;
   else if (!strcmp(model, "A500PLUS")) return EMU_CONFIG_A500PLUS;
   else if (!strcmp(model, "A600"))     return EMU_CONFIG_A600;
   else if (!strcmp(model, "A1200"))    return EMU_CONFIG_A1200;
   else if (!strcmp(model, "A1200OG"))  return EMU_CONFIG_A1200OG;
   else if (!strcmp(model, "A4030"))    return EMU_CONFIG_A4030;
   else if (!strcmp(model, "A4040"))    return EMU_CONFIG_A4040;
   else if (!strcmp(model, "CDTV"))     return EMU_CONFIG_CDTV;
   else if (!strcmp(model, "CD32"))     return EMU_CONFIG_CD32;
   else if (!strcmp(model, "CD32FR"))   return EMU_CONFIG_CD32FR;
   else return -1;
}

static char* emu_config(int config)
{
   char custom_config_path[RETRO_PATH_MAX] = {0};
   char custom_config_file[RETRO_PATH_MAX] = {0};

   snprintf(custom_config_file, sizeof(custom_config_file),
            "%s_%s.%s", LIBRETRO_PUAE_PREFIX, emu_config_string("model", config), "uae");
   path_join(custom_config_path, retro_save_directory, custom_config_file);

   if (path_is_valid(custom_config_path))
   {
      log_cb(RETRO_LOG_INFO, "Replacing model preset with: '%s'\n", custom_config_path);

      char filebuf[512] = {0};
      FILE * custom_config_fp;
      if (custom_config_fp = fopen(custom_config_path, "r"))
      {
         while (fgets(filebuf, sizeof(filebuf), custom_config_fp))
            strcat(uae_custom_config, filebuf);
         fclose(custom_config_fp);
      }
      return uae_custom_config;
   }

   /* chipmem_size (default 1): 1 = 0.5MB, 2 = 1MB, 4 = 2MB
    * bogomem_size (default 0): 2 = 0.5MB, 4 = 1MB, 6 = 1.5MB, 7 = 1.8MB
    * fastmem_size (default 0): 1 = 1.0MB, ...
    */
   switch (config)
   {
      case EMU_CONFIG_A500: return
         "cpu_model=68000\n"
         "chipset=ocs\n"
         "chipset_compatible=A500\n"
         "chipmem_size=1\n"
         "bogomem_size=2\n"
         "fastmem_size=0\n";

      case EMU_CONFIG_A500OG: return
         "cpu_model=68000\n"
         "chipset=ocs\n"
         "chipset_compatible=A500\n"
         "chipmem_size=1\n"
         "bogomem_size=0\n"
         "fastmem_size=0\n";

      case EMU_CONFIG_A500PLUS: return
         "cpu_model=68000\n"
         "chipset=ecs\n"
         "chipset_compatible=A500+\n"
         "chipmem_size=2\n"
         "bogomem_size=0\n"
         "fastmem_size=0\n";

      case EMU_CONFIG_A600: return
         "cpu_model=68000\n"
         "chipset=ecs\n"
         "chipset_compatible=A600\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=8\n";

      case EMU_CONFIG_A1200: return
         "cpu_model=68020\n"
         "chipset=aga\n"
         "chipset_compatible=A1200\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=8\n";

      case EMU_CONFIG_A1200OG: return
         "cpu_model=68020\n"
         "chipset=aga\n"
         "chipset_compatible=A1200\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=0\n";

      case EMU_CONFIG_A4030: return
         "cpu_model=68030\n"
         "fpu_model=68882\n"
         "chipset=aga\n"
         "chipset_compatible=A4000\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=8\n";

      case EMU_CONFIG_A4040: return
         "cpu_model=68040\n"
         "fpu_model=68040\n"
         "chipset=aga\n"
         "chipset_compatible=A4000\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=8\n";

      case EMU_CONFIG_CDTV: return
         "cpu_model=68000\n"
         "chipset=ecs_agnus\n"
         "chipset_compatible=CDTV\n"
         "chipmem_size=2\n"
         "bogomem_size=0\n"
         "fastmem_size=0\n"
         "floppy0type=-1\n";

      case EMU_CONFIG_CD32: return
         "cpu_model=68020\n"
         "chipset=aga\n"
         "chipset_compatible=CD32\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=0\n"
         "floppy0type=-1\n";

      case EMU_CONFIG_CD32FR: return
         "cpu_model=68020\n"
         "chipset=aga\n"
         "chipset_compatible=CD32\n"
         "chipmem_size=4\n"
         "bogomem_size=0\n"
         "fastmem_size=8\n"
         "floppy0type=-1\n";

      default: return "";
   }
}

static void retro_build_preset(char* model)
{
   int model_int = emu_config_int(model);
   strcpy(uae_model, emu_config(model_int));
   strcpy(uae_kickstart, emu_config_string("kickstart", model_int));
   strcpy(uae_kickstart_ext, emu_config_string("kickstart_ext", model_int));
}

static bool retro_create_config()
{
   RPATH[0] = '\0';
   path_join((char*)&RPATH, retro_save_directory, LIBRETRO_PUAE_PREFIX ".uae");
   log_cb(RETRO_LOG_INFO, "Generating config file: '%s'\n", (const char*)&RPATH);

   /* Model preset */
   if (!strcmp(opt_model, "auto"))
   {
      if (opt_use_boot_hd)
         retro_build_preset(opt_model_hd);
      else
         retro_build_preset(opt_model_fd);
   }
   else
      retro_build_preset(opt_model);

   /* "Browsed" file in ZIP */
   char browsed_file[RETRO_PATH_MAX] = {0};
   if (!string_is_empty(full_path) && strstr(full_path, ".zip#"))
   {
      char *token = strtok((char*)full_path, "#");
      while (token != NULL)
      {
         snprintf(browsed_file, sizeof(browsed_file), "%s", token);
         token = strtok(NULL, "#");
      }
   }

   /* Open tmp config file */
   FILE * configfile;
   if (!(configfile = fopen(RPATH, "w")))
   {
      log_cb(RETRO_LOG_ERROR, "Unable to write file: '%s'\n", (const char*)&RPATH);
      return false;
   }

   if (!string_is_empty(full_path) && (path_is_valid(full_path) || path_is_directory(full_path)))
   {
      /* Extract ZIP for examination */
      if (strendswith(full_path, "zip"))
      {
         char zip_basename[RETRO_PATH_MAX] = {0};
         snprintf(zip_basename, sizeof(zip_basename), "%s", path_basename(full_path));
         snprintf(zip_basename, sizeof(zip_basename), "%s", path_remove_extension(zip_basename));

         path_mkdir(retro_temp_directory);
         zip_uncompress(full_path, retro_temp_directory, NULL);

         /* Default to directory mode */
         int zip_mode = 0;
         snprintf(full_path, sizeof(full_path), "%s", retro_temp_directory);

         FILE *zip_m3u;
         char zip_m3u_list[20][RETRO_PATH_MAX] = {0};
         char zip_m3u_path[RETRO_PATH_MAX];
         snprintf(zip_m3u_path, sizeof(zip_m3u_path), "%s%s%s.m3u", retro_temp_directory, DIR_SEP_STR, zip_basename);
         int zip_m3u_num = 0;

         DIR *zip_dir;
         struct dirent *zip_dirp;
         zip_dir = opendir(retro_temp_directory);
         while ((zip_dirp = readdir(zip_dir)) != NULL)
         {
            if (zip_dirp->d_name[0] == '.' || strendswith(zip_dirp->d_name, "m3u") || zip_mode > 1 || browsed_file[0] != '\0')
               continue;

            /* Multi file mode, generate playlist */
            if (dc_get_image_type(zip_dirp->d_name) == DC_IMAGE_TYPE_FLOPPY)
            {
               zip_mode = 1;
               zip_m3u_num++;
               snprintf(zip_m3u_list[zip_m3u_num-1], RETRO_PATH_MAX, "%s", zip_dirp->d_name);
            }
            /* Single file image mode */
            else if (dc_get_image_type(zip_dirp->d_name) == DC_IMAGE_TYPE_CD
                  || dc_get_image_type(zip_dirp->d_name) == DC_IMAGE_TYPE_HD)
            {
               zip_mode = 2;
               snprintf(full_path, sizeof(full_path), "%s%s%s", retro_temp_directory, DIR_SEP_STR, zip_dirp->d_name);
            }
         }
         closedir(zip_dir);

         switch (zip_mode)
         {
            case 0: /* Extracted path */
            case 2: /* Single image */
               if (browsed_file[0] != '\0')
                  snprintf(full_path, sizeof(full_path), "%s%s%s", retro_temp_directory, DIR_SEP_STR, browsed_file);
               break;
            case 1: /* Generated playlist */
               zip_m3u = fopen(zip_m3u_path, "w");
               qsort(zip_m3u_list, zip_m3u_num, RETRO_PATH_MAX, qstrcmp);
               for (int l = 0; l < zip_m3u_num; l++)
                  fprintf(zip_m3u, "%s\n", zip_m3u_list[l]);
               fclose(zip_m3u);
               snprintf(full_path, sizeof(full_path), "%s", zip_m3u_path);
               break;
         }
      }

      /* Floppy disk, hard drive, WHDLoad or playlist */
      if (dc_get_image_type(full_path) == DC_IMAGE_TYPE_FLOPPY
       || dc_get_image_type(full_path) == DC_IMAGE_TYPE_HD
       || dc_get_image_type(full_path) == DC_IMAGE_TYPE_WHDLOAD
       || strendswith(full_path, "m3u"))
      {
         /* Check if model is specified in the path on 'Automatic' */
         if (!strcmp(opt_model, "auto"))
         {
            if (strstr(full_path, "(A4030)") || strstr(full_path, "(030)"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A4030)' or '(030)' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A4000/030: '%s'\n", A4000_ROM);
               retro_build_preset("A4030");
            }
            else if (strstr(full_path, "(A4040)") || strstr(full_path, "(040)"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A4040)' or '(040)' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A4000/040: '%s'\n", A4000_ROM);
               retro_build_preset("A4040");
            }
            else if (strstr(full_path, "(A1200OG)") || strstr(full_path, "(A1200NF)"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A1200OG)' or '(A1200NF)' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A1200 NoFast: '%s'\n", A1200_ROM);
               retro_build_preset("A1200OG");
            }
            else if (strstr(full_path, "(A1200)") || strstr(full_path, "AGA") || strstr(full_path, "CD32") || strstr(full_path, "AmigaCD"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A1200)', 'AGA', 'CD32', or 'AmigaCD' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A1200: '%s'\n", A1200_ROM);
               retro_build_preset("A1200");
            }
            else if (strstr(full_path, "(A600)") || strstr(full_path, "ECS"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A600)' or 'ECS' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A600: '%s'\n", A600_ROM);
               retro_build_preset("A600");
            }
            else if (strstr(full_path, "(A500+)") || strstr(full_path, "(A500PLUS)"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A500+)' or '(A500PLUS)' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A500+: '%s'\n", A500KS2_ROM);
               retro_build_preset("A500PLUS");
            }
            else if (strstr(full_path, "(A500OG)") || strstr(full_path, "(512K)"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A500OG)' or '(512K)' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A500 512K: '%s'\n", A500_ROM);
               retro_build_preset("A500OG");
            }
            else if (strstr(full_path, "(A500)") || strstr(full_path, "OCS"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(A500)' or 'OCS' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting A500: '%s'\n", A500_ROM);
               retro_build_preset("A500");
            }
            else
            {
               /* Hard disks must default to a machine with HD interface */
               if (!opt_use_boot_hd &&
                   (dc_get_image_type(full_path) == DC_IMAGE_TYPE_HD ||
                    dc_get_image_type(full_path) == DC_IMAGE_TYPE_WHDLOAD))
                  retro_build_preset(opt_model_hd);

               /* No model specified */
               log_cb(RETRO_LOG_INFO, "No model specified in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting default model: '%s'\n", uae_kickstart);
            }
         }
         else
            /* Core option model */
            log_cb(RETRO_LOG_INFO, "Booting model: '%s'\n", uae_kickstart);

         /* Write model preset */
         fprintf(configfile, uae_model);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Verify and write Kickstart */
         retro_print_kickstart(&configfile);

         /* Bootable HD exception */
         if (opt_use_boot_hd)
            retro_use_boot_hd(&configfile);

         /* Hard drive or WHDLoad image */
         if (dc_get_image_type(full_path) == DC_IMAGE_TYPE_HD
          || dc_get_image_type(full_path) == DC_IMAGE_TYPE_WHDLOAD)
         {
            char *tmp_str = NULL;

            /* WHDLoad support */
            if (opt_use_whdload)
            {
               /* WHDLoad HDF mode */
               if (opt_use_whdload == 2)
               {
                  char whdload_hdf[RETRO_PATH_MAX];
                  path_join((char*)&whdload_hdf, retro_save_directory, "WHDLoad.hdf");

                  /* Verify WHDLoad */
                  if (!path_is_valid(whdload_hdf))
                  {
                     log_cb(RETRO_LOG_INFO, "WHDLoad image file '%s' not found, attempting to create one\n", (const char*)&whdload_hdf);

                     char whdload_hdf_gz[RETRO_PATH_MAX];
                     path_join((char*)&whdload_hdf_gz, retro_save_directory, "WHDLoad.hdf.gz");

                     FILE *whdload_hdf_gz_fp;
                     if (whdload_hdf_gz_fp = fopen(whdload_hdf_gz, "wb"))
                     {
                        /* Write GZ */
                        fwrite(___whdload_WHDLoad_hdf_gz, ___whdload_WHDLoad_hdf_gz_len, 1, whdload_hdf_gz_fp);
                        fclose(whdload_hdf_gz_fp);

                        /* Extract GZ */
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
                        log_cb(RETRO_LOG_ERROR, "Unable to create WHDLoad image file: '%s'\n", (const char*)&whdload_hdf);
                  }
                  /* Attach HDF */
                  if (path_is_valid(whdload_hdf))
                  {
                     tmp_str = string_replace_substring(whdload_hdf, "\\", "\\\\");
                     fprintf(configfile, "hardfile2=rw,WHDLoad:\"%s\",32,1,2,512,0,,uae0\n", (const char*)tmp_str);
                     free(tmp_str);
                     tmp_str = NULL;
                  }
               }
               /* WHDLoad file mode */
               else
               {
                  char whdload_path[RETRO_PATH_MAX];
                  path_join((char*)&whdload_path, retro_save_directory, "WHDLoad");

                  char whdload_c_path[RETRO_PATH_MAX];
                  path_join((char*)&whdload_c_path, retro_save_directory, "WHDLoad/C");

                  /* Verify WHDLoad */
                  if (!path_is_directory(whdload_path) || (path_is_directory(whdload_path) && !path_is_directory(whdload_c_path)))
                  {
                     log_cb(RETRO_LOG_INFO, "WHDLoad image directory '%s' not found, attempting to create one\n", (const char*)&whdload_path);
                     path_mkdir(whdload_path);

                     char whdload_files_zip[RETRO_PATH_MAX];
                     path_join((char*)&whdload_files_zip, retro_save_directory, "WHDLoad_files.zip");

                     FILE *whdload_files_zip_fp;
                     if (whdload_files_zip_fp = fopen(whdload_files_zip, "wb"))
                     {
                        /* Write ZIP */
                        fwrite(___whdload_WHDLoad_files_zip, ___whdload_WHDLoad_files_zip_len, 1, whdload_files_zip_fp);
                        fclose(whdload_files_zip_fp);

                        /* Extract ZIP */
                        zip_uncompress(whdload_files_zip, whdload_path, NULL);
                        remove(whdload_files_zip);
                     }
                     else
                        log_cb(RETRO_LOG_ERROR, "Unable to create WHDLoad image directory: '%s'\n", (const char*)&whdload_path);
                  }
                  /* Attach directory */
                  if (path_is_directory(whdload_path) && path_is_directory(whdload_c_path))
                  {
                     tmp_str = string_replace_substring(whdload_path, "\\", "\\\\");
                     fprintf(configfile, "filesystem2=rw,WHDLoad:WHDLoad:\"%s\",0\n", (const char*)tmp_str);
                     free(tmp_str);
                     tmp_str = NULL;
                  }
               }

               /* Attach hard drive */
               tmp_str = string_replace_substring(full_path, "\\", "\\\\");
               char tmp_str_name[RETRO_PATH_MAX];
               char tmp_str_path[RETRO_PATH_MAX];
               if (strendswith(full_path, "slave") || strendswith(full_path, "info"))
               {
                  snprintf(tmp_str_name, sizeof(tmp_str_name), "%s", path_basename(tmp_str));
                  snprintf(tmp_str_name, sizeof(tmp_str_name), "%s", path_remove_extension(tmp_str_name));
                  path_parent_dir(tmp_str);
                  snprintf(tmp_str_path, sizeof(tmp_str_path), "%s%s", tmp_str, tmp_str_name);
                  if (!path_is_directory(tmp_str_path))
                     snprintf(tmp_str_path, sizeof(tmp_str_path), "%s", tmp_str);
               }
               if (strendswith(full_path, "lha"))
                  fprintf(configfile, "filesystem2=ro,DH0:LHA:\"%s\",0\n", (const char*)tmp_str);
               else if (path_is_directory(full_path))
                  fprintf(configfile, "filesystem2=rw,DH0:%s:\"%s\",0\n", path_basename(tmp_str), (const char*)tmp_str);
               else if (strendswith(full_path, "slave") || strendswith(full_path, "info"))
                  fprintf(configfile, "filesystem2=rw,DH0:%s:\"%s\",0\n", tmp_str_name, tmp_str_path);
               else
                  fprintf(configfile, "hardfile2=rw,DH0:\"%s\",32,1,2,512,0,,uae1\n", (const char*)tmp_str);
               free(tmp_str);
               tmp_str = NULL;

               /* Attach retro_system_directory as a read only hard drive for WHDLoad kickstarts/prefs/key */
#ifdef WIN32
               tmp_str = string_replace_substring(retro_system_directory, "\\", "\\\\");
               fprintf(configfile, "filesystem2=ro,RASystem:RASystem:\"%s\",-128\n", (const char*)tmp_str);
               free(tmp_str);
               tmp_str = NULL;
#else
               /* Force the ending slash to make sure the path is not treated as a file */
               fprintf(configfile, "filesystem2=ro,RASystem:RASystem:\"%s%s\",-128\n", retro_system_directory, "/");
#endif
               /* WHDSaves HDF mode */
               if (opt_use_whdload == 2)
               {
                  /* Attach WHDSaves.hdf if available */
                  char whdsaves_hdf[RETRO_PATH_MAX];
                  path_join((char*)&whdsaves_hdf, retro_save_directory, "WHDSaves.hdf");
                  if (!path_is_valid(whdsaves_hdf))
                  {
                     log_cb(RETRO_LOG_INFO, "WHDSaves image file '%s' not found, attempting to create one\n", (const char*)&whdsaves_hdf);

                     char whdsaves_hdf_gz[RETRO_PATH_MAX];
                     path_join((char*)&whdsaves_hdf_gz, retro_save_directory, "WHDSaves.hdf.gz");

                     FILE *whdsaves_hdf_gz_fp;
                     if (whdsaves_hdf_gz_fp = fopen(whdsaves_hdf_gz, "wb"))
                     {
                        /* Write GZ */
                        fwrite(___whdload_WHDSaves_hdf_gz, ___whdload_WHDSaves_hdf_gz_len, 1, whdsaves_hdf_gz_fp);
                        fclose(whdsaves_hdf_gz_fp);

                        /* Extract GZ */
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
                        log_cb(RETRO_LOG_ERROR, "Unable to create WHDSaves image file: '%s'\n", (const char*)&whdsaves_hdf);
                  }
                  /* Attach HDF */
                  if (path_is_valid(whdsaves_hdf))
                  {
                     tmp_str = string_replace_substring(whdsaves_hdf, "\\", "\\\\");
                     fprintf(configfile, "hardfile2=rw,WHDSaves:\"%s\",32,1,2,512,0,,uae2\n", (const char*)tmp_str);
                     free(tmp_str);
                     tmp_str = NULL;
                  }
               }
               /* WHDSaves file mode */
               else
               {
                  char whdsaves_path[RETRO_PATH_MAX];
                  path_join((char*)&whdsaves_path, retro_save_directory, "WHDSaves");
                  if (!path_is_directory(whdsaves_path))
                     path_mkdir(whdsaves_path);
                  /* Attach directory */
                  if (path_is_directory(whdsaves_path))
                  {
                     tmp_str = string_replace_substring(whdsaves_path, "\\", "\\\\");
                     fprintf(configfile, "filesystem2=rw,WHDSaves:WHDSaves:\"%s\",-128\n", (const char*)tmp_str);
                     free(tmp_str);
                     tmp_str = NULL;
                  }
                  else
                     log_cb(RETRO_LOG_ERROR, "Unable to create WHDSaves image directory: '%s'\n", (const char*)&whdsaves_path);
               }

               /* Manipulate WHDLoad.prefs */
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

               if (!path_is_valid(whdload_prefs_path))
               {
                  log_cb(RETRO_LOG_INFO, "WHDLoad.prefs '%s' not found, attempting to create one\n", (const char*)&whdload_prefs_path);

                  char whdload_prefs_gz[RETRO_PATH_MAX];
                  path_join((char*)&whdload_prefs_gz, retro_system_directory, "WHDLoad.prefs.gz");

                  FILE *whdload_prefs_gz_fp;
                  if (whdload_prefs_gz_fp = fopen(whdload_prefs_gz, "wb"))
                  {
                     /* Write GZ */
                     fwrite(___whdload_WHDLoad_prefs_gz, ___whdload_WHDLoad_prefs_gz_len, 1, whdload_prefs_gz_fp);
                     fclose(whdload_prefs_gz_fp);

                     /* Extract GZ */
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
                     log_cb(RETRO_LOG_ERROR, "Unable to create WHDLoad.prefs: '%s'\n", (const char*)&whdload_prefs_path);
               }

               FILE *whdload_prefs_new;
               char whdload_prefs_new_path[RETRO_PATH_MAX];
               path_join((char*)&whdload_prefs_new_path, retro_system_directory, "WHDLoad.prefs_new");

               char whdload_prefs_backup_path[RETRO_PATH_MAX];
               path_join((char*)&whdload_prefs_backup_path, retro_system_directory, "WHDLoad.prefs_backup");

               char whdload_filebuf[512];
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

                     /* Remove backup config */
                     remove(whdload_prefs_backup_path);

                     /* Replace old and new config */
                     rename(whdload_prefs_path, whdload_prefs_backup_path);
                     rename(whdload_prefs_new_path, whdload_prefs_path);
                  }
                  else
                  {
                     log_cb(RETRO_LOG_ERROR, "Unable to create new WHDLoad.prefs: '%s'\n", (const char*)&whdload_prefs_new_path);
                     fclose(whdload_prefs);
                  }
               }
            }
            else
            {
               tmp_str = string_replace_substring(full_path, "\\", "\\\\");
               if (path_is_directory(full_path))
                  fprintf(configfile, "filesystem2=rw,DH0:%s:\"%s\",0\n", path_basename(tmp_str), (const char*)tmp_str);
               else
                  fprintf(configfile, "hardfile2=rw,DH0:\"%s\",32,1,2,512,0,,uae0\n", (const char*)tmp_str);
               free(tmp_str);
               tmp_str = NULL;
            }
         }
         else
         {
            /* M3U playlist */
            if (strendswith(full_path, "m3u"))
            {
               /* Parse the M3U file */
               dc_parse_m3u(dc, full_path, retro_save_directory);

               /* Some debugging */
               log_cb(RETRO_LOG_INFO, "M3U parsed, %d file(s) found\n", dc->count);
               for (unsigned i = 0; i < dc->count; i++)
                  log_cb(RETRO_LOG_DEBUG, "File %d: %s\n", i+1, dc->files[i]);
            }
            /* Single file */
            else
            {
               /* Add the file to disk control context */
               char disk_image_label[RETRO_PATH_MAX];
               disk_image_label[0] = '\0';

               if (!string_is_empty(full_path))
                  fill_short_pathname_representation(
                        disk_image_label, full_path, sizeof(disk_image_label));

               /* Must reset disk control struct here,
                * otherwise duplicate entries will be
                * added when calling retro_reset() */
               dc_reset(dc);
               dc_add_file(dc, full_path, disk_image_label);
            }

            /* Init only existing disks */
            if (dc->count)
            {
               /* Init first disk */
               dc->index = 0;
               dc->eject_state = false;
               display_current_image(dc->labels[dc->index], true);
               log_cb(RETRO_LOG_INFO, "Disk (%d) inserted in drive DF0: '%s'\n", dc->index+1, dc->files[dc->index]);
               fprintf(configfile, "floppy0=%s\n", dc->files[0]);

               /* Append rest of the disks to the config if M3U is a MultiDrive-M3U */
               if (strstr(full_path, "(MD)") != NULL || opt_floppy_multidrive)
               {
                  for (unsigned i = 1; i < dc->count; i++)
                  {
                     if (i < 4)
                     {
                        log_cb(RETRO_LOG_INFO, "Disk (%d) inserted in drive DF%d: '%s'\n", i+1, i, dc->files[i]);
                        fprintf(configfile, "floppy%d=%s\n", i, dc->files[i]);

                        /* By default only DF0: is enabled, so floppyXtype needs to be set on the extra drives */
                        fprintf(configfile, "floppy%dtype=%d\n", i, 0); /* 0 = 3.5" DD */
                     }
                     else
                     {
                        log_cb(RETRO_LOG_WARN, "Too many disks for MultiDrive!\n");
                        snprintf(retro_message_msg, sizeof(retro_message_msg), "Too many disks for MultiDrive!");
                        retro_message = true;
                     }
                  }
               }
            }
         }

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Write common config */
         fprintf(configfile, uae_config);
      }
      /* CD image */
      else if (dc_get_image_type(full_path) == DC_IMAGE_TYPE_CD)
      {
         /* Check if model is specified in the path on 'Automatic' */
         if (!strcmp(opt_model, "auto"))
         {
            if (strstr(full_path, "(CD32FR)") || strstr(full_path, "FastRAM"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(CD32FR)' or 'FastRAM' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting CD32 FastRAM: '%s'\n", CD32_ROM);
               retro_build_preset("CD32FR");
            }
            else if (strstr(full_path, "(CD32)") || strstr(full_path, "(CD32NF)"))
            {
               log_cb(RETRO_LOG_INFO, "Found '(CD32)' or '(CD32NF)' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting CD32: '%s'\n", CD32_ROM);
               retro_build_preset("CD32");
            }
            else if (strstr(full_path, "CDTV"))
            {
               log_cb(RETRO_LOG_INFO, "Found 'CDTV' in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting CDTV: '%s'\n", CDTV_ROM);
               retro_build_preset("CDTV");
            }
            else
            {
               /* Default CD model */
               retro_build_preset(opt_model_cd);

               /* No model specified */
               log_cb(RETRO_LOG_INFO, "No model specified in: '%s'\n", full_path);
               log_cb(RETRO_LOG_INFO, "Booting default model: '%s'\n", uae_kickstart);
            }
         }
         else
            /* Core option model */
            log_cb(RETRO_LOG_INFO, "Booting model: '%s'\n", uae_kickstart);

         /* Write model preset */
         fprintf(configfile, uae_model);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Verify and write Kickstart */
         retro_print_kickstart(&configfile);

         /* Add the file to disk control context */
         char cd_image_label[RETRO_PATH_MAX];
         cd_image_label[0] = '\0';

         if (!string_is_empty(full_path))
            fill_short_pathname_representation(
                  cd_image_label, full_path, sizeof(cd_image_label));

         /* Must reset disk control struct here,
          * otherwise duplicate entries will be
          * added when calling retro_reset() */
         dc_reset(dc);
         dc_add_file(dc, full_path, cd_image_label);

         /* Init first disk */
         dc->index = 0;
         dc->eject_state = false;
         display_current_image(dc->labels[dc->index], true);
         log_cb(RETRO_LOG_INFO, "CD (%d) inserted in drive CD0: '%s'\n", dc->index+1, dc->files[dc->index]);
         fprintf(configfile, "cdimage0=%s,%s\n", dc->files[0], (opt_cd_startup_delayed_insert ? "delay" : "")); /* ","-suffix needed if filename contains "," */

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Write common config */
         fprintf(configfile, uae_config);
      }
      /* UAE config file */
      else if (strendswith(full_path, "uae"))
      {
         char disk_image[RETRO_PATH_MAX] = {0};

         /* Write model preset */
         fprintf(configfile, uae_model);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Verify and write Kickstart */
         retro_print_kickstart(&configfile);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Write common config */
         fprintf(configfile, uae_config);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Must reset disk control struct here,
          * otherwise duplicate entries will be
          * added when calling retro_reset() */
         dc_reset(dc);

         /* Iterate parsed file and append all rows to the temporary config */
         FILE * configfile_custom;
         char filebuf[512];
         if (configfile_custom = fopen(full_path, "r"))
         {
            while (fgets(filebuf, sizeof(filebuf), configfile_custom))
            {
               fprintf(configfile, filebuf);

               /* Parse diskimage-rows for disk control */
               if (strstr(filebuf, "diskimage") && filebuf[0] == 'd')
               {
                  char *token = strtok((char*)filebuf, "=");
                  while (token != NULL)
                  {
                     snprintf(disk_image, sizeof(disk_image), "%s", token);
                     token = strtok(NULL, "=");
                  }
                  strtok(disk_image, "\n");
                  if (!string_is_empty(disk_image) && path_is_valid(disk_image))
                  {
                     /* Add the file to disk control context */
                     char disk_image_label[RETRO_PATH_MAX];
                     disk_image_label[0] = '\0';
                     fill_short_pathname_representation(disk_image_label, disk_image, sizeof(disk_image_label));
                     dc_add_file(dc, disk_image, disk_image_label);
                  }
               }
            }
            fclose(configfile_custom);
         }

         /* Init only existing disks */
         if (!string_is_empty(disk_image) && dc->count)
         {
            /* Init first disk */
            dc->index = 0;
            dc->eject_state = false;
            display_current_image(dc->labels[dc->index], true);
            log_cb(RETRO_LOG_INFO, "Disk (%d) inserted in drive DF0: '%s'\n", dc->index+1, dc->files[dc->index]);
         }
      }
      /* Unknown extensions */
      else
      {
         /* Write model preset */
         fprintf(configfile, uae_model);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Verify and write Kickstart */
         retro_print_kickstart(&configfile);

         /* Separator row for clarity */
         fprintf(configfile, "\n");

         /* Write common config */
         fprintf(configfile, uae_config);

         /* Unsupported file format */
         log_cb(RETRO_LOG_ERROR, "Unsupported file format: '%s'\n", full_path);
         snprintf(retro_message_msg, sizeof(retro_message_msg), "Unsupported file format: '%s'", path_get_extension(full_path));
         retro_message = true;
      }
   }
   /* Empty content */
   else
   {
      /* No model specified */
      log_cb(RETRO_LOG_INFO, "Booting model: '%s'\n", uae_kickstart);

      /* Write model preset */
      fprintf(configfile, uae_model);

      /* Separator row for clarity */
      fprintf(configfile, "\n");

      /* Verify and write Kickstart */
      retro_print_kickstart(&configfile);

      /* Bootable HD exception, not for CD systems */
      if (opt_use_boot_hd)
         if (strcmp(opt_model, "CD32") && strcmp(opt_model, "CD32FR") && strcmp(opt_model, "CDTV"))
            retro_use_boot_hd(&configfile);

      /* Separator row for clarity */
      fprintf(configfile, "\n");

      /* Write common config */
      fprintf(configfile, uae_config);
   }

   /* Separator row for clarity */
   fprintf(configfile, "\n");

   /* Iterate global config file and append all rows to the temporary config */
   char configfile_global_path[RETRO_PATH_MAX];
   path_join((char*)&configfile_global_path, retro_save_directory, LIBRETRO_PUAE_PREFIX "_global.uae");
   if (path_is_valid(configfile_global_path))
   {
      log_cb(RETRO_LOG_INFO, "Appending global configuration: '%s'\n", configfile_global_path);
      /* Separator row for clarity */
      fprintf(configfile, "\n");

      FILE * configfile_global;
      char filebuf[512];
      if (configfile_global = fopen(configfile_global_path, "r"))
      {
         while (fgets(filebuf, sizeof(filebuf), configfile_global))
            fprintf(configfile, filebuf);
         fclose(configfile_global);
      }
   }

   /* Scan region tags only with automatic region */
   if (opt_region_auto)
      retro_force_region(&configfile);

   /* Forced Cycle-exact */
   if (strstr(full_path, "(CE)"))
      fprintf(configfile, "cycle_exact=true\n");

   /* Close tmp config */
   fclose(configfile);

   /* Scan for specific rows */
   char filebuf[512];
   if (configfile = fopen(RPATH, "r"))
   {
      while (fgets(filebuf, sizeof(filebuf), configfile))
      {
         if (strstr(filebuf, "ntsc=true") && filebuf[0] == 'n')
            real_ntsc = true;
         if (strstr(filebuf, "ntsc=false") && filebuf[0] == 'n')
            real_ntsc = false;
         if (strstr(filebuf, "cycle_exact=true") && filebuf[0] == 'c')
            cpu_cycle_exact_force = true;
      }
      fclose(configfile);
   }

   return true;
}

void retro_reset(void)
{
   fake_ntsc = false;
   video_config_old = 0;
   update_variables();
   retro_create_config();
   uae_restart(1, RPATH); /* 1=nogui */
}

void retro_reset_soft()
{
   request_reset_soft = false;
   fake_ntsc = false;
   uae_reset(0, 0); /* hardreset, keyboardreset */
}

/* Vertical centering */
void update_video_center_vertical(void)
{
   int zoomed_height_normal   = (video_config & PUAE_VIDEO_DOUBLELINE) ? zoomed_height / 2 : zoomed_height;
   int thisframe_y_adjust_new = minfirstline;

   /* Need proper values for calculations */
   if (retro_thisframe_first_drawn_line != retro_thisframe_last_drawn_line
    && retro_thisframe_first_drawn_line > 0 && retro_thisframe_last_drawn_line > 0
    && (retro_thisframe_first_drawn_line < 150 || retro_thisframe_last_drawn_line > 150)
   )
      thisframe_y_adjust_new = (retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line - zoomed_height_normal) / 2 + retro_thisframe_first_drawn_line;
   else if (retro_thisframe_first_drawn_line == -1 && retro_thisframe_last_drawn_line == -1)
      thisframe_y_adjust_new = thisframe_y_adjust_old;

   /* Sensible limits */
   thisframe_y_adjust_new = (thisframe_y_adjust_new < 0) ? 0 : thisframe_y_adjust_new;
   thisframe_y_adjust_new = (thisframe_y_adjust_new > (minfirstline + 70)) ? (minfirstline + 70) : thisframe_y_adjust_new;

   /* Change value only if altered */
   if (thisframe_y_adjust != thisframe_y_adjust_new)
      thisframe_y_adjust = thisframe_y_adjust_new;

#if 0
   fprintf(stdout, "FIRSTDRAWN:%6d LASTDRAWN:%6d   yadjust:%3d old:%3d zoomed_h:%d\n", retro_thisframe_first_drawn_line, retro_thisframe_last_drawn_line, thisframe_y_adjust, thisframe_y_adjust_old, zoomed_height);
#endif

   /* Remember the previous value */
   thisframe_y_adjust_old = thisframe_y_adjust;

   /* Counter reset */
   retro_thisframe_counter = 0;
}

/* Horizontal centering */
void update_video_center_horizontal(void)
{
   int visible_left_border_new = retro_max_diwlastword - retrow + (retrow - zoomed_width) / 2;

   /* Horizontal centering thresholds */
   int min_diwstart_limit = 110;
   int max_diwstop_limit  = 300;

   if (locked_video_horizontal)
      return;

   min_diwstart_limit *= width_multiplier;
   max_diwstop_limit  *= width_multiplier;

   /* Need proper values for calculations */
   if (retro_min_diwstart != retro_max_diwstop
    && retro_min_diwstart > 0
    && retro_max_diwstop  > 0
    && retro_min_diwstart < min_diwstart_limit
    && retro_max_diwstop  > max_diwstop_limit
    && (retro_max_diwstop - retro_min_diwstart) <= (zoomed_width + (2 * width_multiplier)))
      visible_left_border_new = (retro_max_diwstop - retro_min_diwstart - zoomed_width) / 2 + retro_min_diwstart;
   else if (retro_min_diwstart == 30000 && retro_max_diwstop == 0)
      visible_left_border_new = visible_left_border;

   /* Sensible limits */
   visible_left_border_new = (visible_left_border_new < 0) ? 0 : visible_left_border_new;
   visible_left_border_new = ((visible_left_border_new / width_multiplier) > 150) ? (150 * width_multiplier) : visible_left_border_new;

   /* Change value only if altered */
   if (visible_left_border != visible_left_border_new)
      visible_left_border = visible_left_border_new;

#if 0
   fprintf(stdout, "DIWSTART  :%6d DIWSTOP  :%6d   lborder:%3d old:%3d width:%3d\n", retro_min_diwstart, retro_max_diwstop, visible_left_border, visible_left_border_old, (retro_max_diwstop - retro_min_diwstart));
#endif

   /* Remember the previous value */
   visible_left_border_old = visible_left_border;

   /* Counter reset */
   retro_diwstartstop_counter = 0;
}

void update_audiovideo(void)
{
   /* Statusbar disk display timer */
   if (imagename_timer > 0)
      imagename_timer--;
   if (retro_statusbar && !imagename_timer)
      request_reset_drawing = true;

   /* Update audio settings */
   if (automatic_sound_filter_type_update)
   {
      automatic_sound_filter_type_update = false;
      config_changed = 1;
      if (currprefs.cpu_model >= 68020)
         changed_prefs.sound_filter_type = FILTER_SOUND_TYPE_A1200;
      else
         changed_prefs.sound_filter_type = FILTER_SOUND_TYPE_A500;
   }

   /* Automatic video resolution */
   if (opt_video_resolution_auto)
   {
      int current_resolution = GET_RES_DENISE (bplcon0);
#if 0
      printf("BPLCON0: %d, %d, %d %d\n", bplcon0, current_resolution, diwfirstword_total, diwlastword_total);
#endif

      /* Super Skidmarks force to SuperHires */
      if (current_resolution == 1 && bplcon0 == 0xC201 && ((diwfirstword_total == 210 && diwlastword_total && 786) || (diwfirstword_total == 420 && diwlastword_total && 1572)))
         current_resolution = 2;
      /* Super Stardust force to SuperHires, rather pointless and causes a false positive on The Settlers */
#if 0
      else if (current_resolution == 0 && (bplcon0 == 0 /*CD32*/|| bplcon0 == 512 /*AGA*/) && ((diwfirstword_total == 114 && diwlastword_total && 818) || (diwfirstword_total == 228 && diwlastword_total && 1636)))
         current_resolution = 2;
#endif
      /* Lores force to Hires */
      else if (current_resolution == 0)
         current_resolution = 1;

      switch (current_resolution)
      {
         case 1:
            if (!(video_config & PUAE_VIDEO_HIRES))
            {
               changed_prefs.gfx_resolution = RES_HIRES;
               video_config |= PUAE_VIDEO_HIRES;
               video_config &= ~PUAE_VIDEO_SUPERHIRES;
               defaultw = retrow = PUAE_VIDEO_WIDTH;
               retro_max_diwlastword = retro_max_diwlastword_hires;
               request_init_custom_timer = 3;
            }
            break;
         case 2:
            if (!(video_config & PUAE_VIDEO_SUPERHIRES))
            {
               changed_prefs.gfx_resolution = RES_SUPERHIRES;
               video_config |= PUAE_VIDEO_SUPERHIRES;
               video_config &= ~PUAE_VIDEO_HIRES;
               defaultw = retrow = PUAE_VIDEO_WIDTH * 2;
               retro_max_diwlastword = retro_max_diwlastword_hires * 2;
               request_init_custom_timer = 3;
            }
            break;
      }

      /* Horizontal centering calculation needs to be forced due to
       * retro_max_diwlastword change which is crucial for visible_left_border */
      if (request_init_custom_timer > 0)
      {
         retro_min_diwstart_old = -1;
         retro_max_diwstop_old  = -1;
         visible_left_border    = retro_max_diwlastword - retrow;
      }
   }

   /* Automatic video vresolution (Line Mode) */
   if (opt_video_vresolution_auto)
   {
      int retro_interlace_seen = interlace_seen;

      /* Lores force to single line */
      if (!(video_config & PUAE_VIDEO_HIRES) && !(video_config & PUAE_VIDEO_SUPERHIRES))
         retro_interlace_seen = 0;

      switch (retro_interlace_seen)
      {
         case -1:
         case 1:
            if (!(video_config & PUAE_VIDEO_DOUBLELINE))
            {
               changed_prefs.gfx_vresolution = VRES_DOUBLE;
               video_config |= PUAE_VIDEO_DOUBLELINE;
               defaulth = retroh = (video_config & PUAE_VIDEO_NTSC) ? PUAE_VIDEO_HEIGHT_NTSC : PUAE_VIDEO_HEIGHT_PAL;
               request_init_custom_timer = 3;
            }
            break;
         case 0:
            if ((video_config & PUAE_VIDEO_DOUBLELINE))
            {
               changed_prefs.gfx_vresolution = VRES_NONDOUBLE;
               video_config &= ~PUAE_VIDEO_DOUBLELINE;
               defaulth = retroh = (video_config & PUAE_VIDEO_NTSC) ? PUAE_VIDEO_HEIGHT_NTSC / 2 : PUAE_VIDEO_HEIGHT_PAL / 2;
               request_init_custom_timer = 3;
            }
            break;
      }
   }

   /* Update av_info */
   if (request_init_custom_timer > 0)
      request_update_av_info = true;

   /* Automatic vertical offset */
   if (opt_vertical_offset_auto && zoom_mode_id != 0 && retro_thisframe_first_drawn_line != retro_thisframe_last_drawn_line)
   {
      int retro_thisframe_first_drawn_line_delta = abs(retro_thisframe_first_drawn_line_old - retro_thisframe_first_drawn_line);
      int retro_thisframe_last_drawn_line_delta  = abs(retro_thisframe_last_drawn_line_old - retro_thisframe_last_drawn_line);

#if 0
      printf("thisrun   first:%3d old:%3d start:%3d last:%3d old:%3d start:%3d\n", retro_thisframe_first_drawn_line, retro_thisframe_first_drawn_line_old, retro_thisframe_first_drawn_line_start, retro_thisframe_last_drawn_line, retro_thisframe_last_drawn_line_old, retro_thisframe_last_drawn_line_start);
#endif
      if (( retro_thisframe_first_drawn_line != retro_thisframe_first_drawn_line_old
         || retro_thisframe_last_drawn_line  != retro_thisframe_last_drawn_line_old)
         && retro_thisframe_first_drawn_line != -1
         && retro_thisframe_last_drawn_line  != -1
         && retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line > 40
         && (retro_thisframe_first_drawn_line_delta > 1 || retro_thisframe_last_drawn_line_delta > 1)
      )
      {
         if (retro_thisframe_first_drawn_line_start == -1 && retro_thisframe_last_drawn_line_start == -1)
            request_update_av_info = true;

         if (retro_thisframe_first_drawn_line_delta > 1)
            retro_thisframe_first_drawn_line_start = (retro_thisframe_first_drawn_line_old == -1) ? retro_thisframe_first_drawn_line : retro_thisframe_first_drawn_line_old;
         if (retro_thisframe_last_drawn_line_delta > 1)
            retro_thisframe_last_drawn_line_start  = (retro_thisframe_last_drawn_line_old == -1) ? retro_thisframe_last_drawn_line : retro_thisframe_last_drawn_line_old;

         if (retro_thisframe_first_drawn_line_start != retro_thisframe_first_drawn_line
          || retro_thisframe_last_drawn_line_start  != retro_thisframe_last_drawn_line)
             retro_thisframe_counter = 1;

         retro_thisframe_first_drawn_line_old = retro_thisframe_first_drawn_line;
         retro_thisframe_last_drawn_line_old  = retro_thisframe_last_drawn_line;
      }
      else if (retro_thisframe_counter > 0
            && retro_thisframe_first_drawn_line != -1
            && retro_thisframe_last_drawn_line  != -1
            && retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line > 40
      )
      {
#if 0
         printf("frmcnt %d, first:%3d old:%3d start:%3d last:%3d old:%3d start:%3d\n", retro_thisframe_counter, retro_thisframe_first_drawn_line, retro_thisframe_first_drawn_line_old, retro_thisframe_first_drawn_line_start, retro_thisframe_last_drawn_line, retro_thisframe_last_drawn_line_old, retro_thisframe_last_drawn_line_start);
#endif
         /* Reset counter if the first drawn line changes while the last line stays the same */
         if (retro_thisframe_first_drawn_line != retro_thisframe_first_drawn_line_start
          && retro_thisframe_last_drawn_line  == retro_thisframe_last_drawn_line_start
          && abs(retro_thisframe_first_drawn_line_start - retro_thisframe_first_drawn_line) < 40)
            retro_thisframe_counter = 0;

         /* Prevent geometry change but allow vertical centering if the values return to the starting point during counting */
         if (retro_thisframe_first_drawn_line == retro_thisframe_first_drawn_line_start
          && retro_thisframe_last_drawn_line  == retro_thisframe_last_drawn_line_start)
            retro_av_info_change_geometry = false;

         if (retro_thisframe_counter > 0)
            retro_thisframe_counter++;

         if (retro_thisframe_counter > 4)
            request_update_av_info = true;
      }

      if (request_update_av_info)
      {
         retro_thisframe_first_drawn_line_start = retro_thisframe_first_drawn_line;
         retro_thisframe_last_drawn_line_start  = retro_thisframe_last_drawn_line;
      }
   }
   else
   {
      /* Vertical offset must not be set too early */
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

   /* Automatic horizontal offset */
   if (opt_horizontal_offset_auto)
   {
      if ( (retro_min_diwstart != retro_min_diwstart_old
         || retro_max_diwstop  != retro_max_diwstop_old)
         && retro_min_diwstart != 30000
         && retro_max_diwstop  != 0)
      {
#if 0
         printf("start:%3d old:%3d stop:%3d old:%3d\n", retro_min_diwstart, retro_min_diwstart_old, retro_max_diwstop, retro_max_diwstop_old);
#endif
         /* Game specific hacks: */
         /* Zool */
         if (retro_min_diwstart == 0 && retro_max_diwstop == retro_max_diwstop_old)
            retro_diwstartstop_counter = 0;
         /* North & South */
         else if (retro_min_diwstart == (73 * width_multiplier) && retro_min_diwstart_old == retro_min_diwstart
               && retro_max_diwstop  == (402 * width_multiplier) && retro_max_diwstop_old == (393 * width_multiplier))
         {
            retro_max_diwstop = retro_max_diwstop_old;
            retro_diwstartstop_counter = 0;
         }
         /* Toki */
         else if (retro_min_diwstart == (89 * width_multiplier) && retro_min_diwstart_old == (57 * width_multiplier)
               && retro_max_diwstop  == (345 * width_multiplier) && retro_max_diwstop_old == (409 * width_multiplier))
         {
            retro_diwstartstop_counter = 0;
            locked_video_horizontal = true;
         }
         else
            retro_diwstartstop_counter = 1;

         retro_min_diwstart_old = retro_min_diwstart;
         retro_max_diwstop_old  = retro_max_diwstop;
      }
      /* Prevent centering of horizontal animations by requiring the change to stabilize */
      else if (retro_diwstartstop_counter > 0
            && retro_min_diwstart == retro_min_diwstart_old
            && retro_max_diwstop  == retro_max_diwstop_old)
      {
         update_video_center_horizontal();
         request_reset_drawing = true;
      }
   }
   else
   {
      /* Horizontal offset must not be set too early */
      if (visible_left_border_update_frame_timer > 0)
      {
         visible_left_border_update_frame_timer--;
         if (visible_left_border_update_frame_timer == 0)
         {
            visible_left_border = retro_max_diwlastword - retrow - opt_horizontal_offset;
            request_reset_drawing = true;
         }
      }
   }
}

static bool retro_update_av_info(void)
{
   bool av_log          = false;
   bool isntsc          = retro_av_info_is_ntsc;
   bool change_timing   = retro_av_info_change_timing;
   bool change_geometry = retro_av_info_change_geometry;
   double hz            = currprefs.chipset_refreshrate;

   /* Reset global parameters ready for the next update */
   request_update_av_info        = false;
   retro_av_info_is_ntsc         = false;
   retro_av_info_change_timing   = false;
   retro_av_info_change_geometry = true;

   if (av_log)
      fprintf(stdout, "* Trying to update AV timing:%d to: ntsc:%d hz:%0.4f, from video_config:%d, video_aspect:%d\n", change_timing, isntsc, hz, video_config, video_config_aspect);

   /* Change PAL/NTSC with a twist, thanks to Dyna Blaster
    *
    * Early Startup switch looks proper:
    *    PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
    *    NTSC mode V=59.8859Hz H=15590.7473Hz (227x262+1) IDX=11 (NTSC) D=0 RTG=0/0
    *    PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
    *
    * Dyna Blaster switch looks unorthodox:
    *    PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
    *    PAL mode V=59.4106Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
    *    PAL mode V=49.9201Hz H=15625.0881Hz (227x312+1) IDX=10 (PAL) D=0 RTG=0/0
    */

   video_config_old      = video_config;
   video_config_geometry = video_config;

   /* When timing & geometry is changed */
   if (change_timing)
   {
      /* Change to NTSC if not NTSC */
      if (isntsc && (video_config & PUAE_VIDEO_PAL) && !fake_ntsc)
      {
         video_config |= PUAE_VIDEO_NTSC;
         video_config &= ~PUAE_VIDEO_PAL;
         real_ntsc = true;
      }
      /* Change to PAL if not PAL */
      else if (!isntsc && (video_config & PUAE_VIDEO_NTSC) && !fake_ntsc)
      {
         video_config |= PUAE_VIDEO_PAL;
         video_config &= ~PUAE_VIDEO_NTSC;
         real_ntsc = false;
      }

      /* Request init_custom() on change and update temporary video config */
      if (video_config_geometry != video_config)
         request_init_custom_timer = 1;
      video_config_geometry = video_config;
   }

   /* Aspect ratio override changes only the temporary video config. Don't change at the same time with region change. */
   if (!change_timing && video_config_aspect != 0)
   {
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
   }
   /* Forced aspect needs to be postponed for the next run to keep the actual dimension intact */
   else if (change_timing && video_config_aspect != 0)
      request_update_av_info = true;

   /* Do nothing if timing has not changed, unless Hz switched without isntsc */
   if (video_config_old == video_config && change_timing)
   {
      /* Dyna Blaster and the like stays at fake NTSC to prevent pointless switching back and forth */
      if (!isntsc && hz > 55)
      {
         video_config |= PUAE_VIDEO_NTSC;
         video_config &= ~PUAE_VIDEO_PAL;
         video_config_geometry = video_config;
         fake_ntsc = true;
      }

      /* If still no change */
      if (video_config_old == video_config)
      {
         if (av_log)
            fprintf(stdout, "* Already at wanted AV\n");
         change_timing = false; /* Allow other calculations but don't alter timing */
      }
   }


   /* Geometry dimensions */
   switch (video_config_geometry)
   {
      case PUAE_VIDEO_PAL_LO:
         retrow = PUAE_VIDEO_WIDTH / 2;
         retroh = PUAE_VIDEO_HEIGHT_PAL / 2;
         break;
      case PUAE_VIDEO_PAL_HI:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_PAL / 2;
         break;
      case PUAE_VIDEO_PAL_HI_DL:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_PAL;
         break;
      case PUAE_VIDEO_PAL_SUHI:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_PAL / 2;
         break;
      case PUAE_VIDEO_PAL_SUHI_DL:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_PAL;
         break;

      case PUAE_VIDEO_NTSC_LO:
         retrow = PUAE_VIDEO_WIDTH / 2;
         retroh = PUAE_VIDEO_HEIGHT_NTSC / 2;
         break;
      case PUAE_VIDEO_NTSC_HI:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_NTSC / 2;
         break;
      case PUAE_VIDEO_NTSC_HI_DL:
         retrow = PUAE_VIDEO_WIDTH;
         retroh = PUAE_VIDEO_HEIGHT_NTSC;
         break;
      case PUAE_VIDEO_NTSC_SUHI:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_NTSC / 2;
         break;
      case PUAE_VIDEO_NTSC_SUHI_DL:
         retrow = PUAE_VIDEO_WIDTH * 2;
         retroh = PUAE_VIDEO_HEIGHT_NTSC;
         break;
   }

   /* Width multiplier */
   if (video_config & PUAE_VIDEO_SUPERHIRES)
      width_multiplier = 4;
   else if (video_config & PUAE_VIDEO_HIRES)
      width_multiplier = 2;
   else
      width_multiplier = 1;

   /* Restore actual canvas height for aspect ratio toggling in real NTSC, otherwise toggling is broken */
   if (!change_timing && real_ntsc && video_config_aspect == PUAE_VIDEO_PAL)
      retroh = defaulth;

   /* Exception for Dyna Blaster */
   if (fake_ntsc)
      retroh = (video_config & PUAE_VIDEO_DOUBLELINE) ? 476 : 238;

   /* When the actual dimensions change and not just the view */
   if (change_timing)
   {
      defaultw = retrow;
      defaulth = retroh;
   }

   /* Disable Hz change if not allowed */
   if (!video_config_allow_hz_change)
      change_timing = false;

   /* Ensure statusbar stays visible at the bottom */
   opt_statusbar_position_offset = 0;
   opt_statusbar_position = opt_statusbar_position_old;
   if (!change_timing)
      if (retroh < defaulth)
         if (opt_statusbar_position >= 0 && (defaulth - retroh) >= opt_statusbar_position)
            opt_statusbar_position = defaulth - retroh;

   /* Aspect offset for zoom mode */
   opt_statusbar_position_offset = opt_statusbar_position_old - opt_statusbar_position;

   /* Compensate for the PAL last line, aargh */
   if (video_config & PUAE_VIDEO_PAL && !real_ntsc && opt_statusbar_position >= 0)
   {
      if (interlace_seen)
      {
         if (video_config_geometry & PUAE_VIDEO_DOUBLELINE)
         {
            if (video_config_geometry & PUAE_VIDEO_PAL)
            {
               opt_statusbar_position += 2;
               opt_statusbar_position_offset += 2;
            }
            else
            {
               opt_statusbar_position -= 2;
               opt_statusbar_position_offset -= 2;
            }
         }
         else
         {
            if (video_config_geometry & PUAE_VIDEO_PAL)
            {
               opt_statusbar_position += 1;
               opt_statusbar_position_offset += 1;
            }
         }
      }
      else
      {
         if (video_config_geometry & PUAE_VIDEO_DOUBLELINE && video_config_geometry & PUAE_VIDEO_PAL)
         {
            opt_statusbar_position += 1;
            opt_statusbar_position_offset += 1;
         }
      }
   }

#if 0
   fprintf(stdout, "statusbar:%3d old:%3d offset:%3d, defaulth:%d retroh:%d\n", opt_statusbar_position, opt_statusbar_position_old, opt_statusbar_position_offset, defaulth, retroh);
#endif

   /* Apply zoom mode croppings */
   switch (zoom_mode_id)
   {
      case 1:
         zoomed_width  = 360;
         zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 240 : 270;
         break;
      case 2:
         zoomed_width  = 348;
         zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 240 : 264;
         break;
      case 3:
         zoomed_width  = 332;
         zoomed_height = (video_config_geometry & PUAE_VIDEO_NTSC) ? 240 : 256;
         break;
      case 4:
         zoomed_width  = 320;
         zoomed_height = 240;
         break;
      case 5:
         zoomed_width  = 320;
         zoomed_height = 224;
         break;
      case 6:
         zoomed_width  = 320;
         zoomed_height = 216;
         break;
      case 7:
         zoomed_width  = 320;
         zoomed_height = 200;
         break;
      case 8:
         if (retro_min_diwstart != retro_max_diwstop
          && retro_min_diwstart > 0
          && retro_max_diwstop > 0)
            zoomed_width = (retro_max_diwstop / width_multiplier) - (retro_min_diwstart / width_multiplier);
         zoomed_width = (zoomed_width < 320) ? 320 : zoomed_width;

         if (retro_thisframe_first_drawn_line != retro_thisframe_last_drawn_line
          && retro_thisframe_first_drawn_line > 0
          && retro_thisframe_last_drawn_line > 0)
            zoomed_height = retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line + 1;
         zoomed_height = (zoomed_height < 200) ? 200 : zoomed_height;
         break;
      default:
         zoomed_width  = retrow;
         zoomed_height = retroh;
         break;
   }

   /* Zoom mode preset calculations */
   if (zoom_mode_id > 0)
   {
      double zoom_dar = 0;
      double zoom_par = retro_get_aspect_ratio(0, 0, true);
      int zoomed_height_original = zoomed_height;

      switch (zoom_mode_crop_id)
      {
         case 0: /* Both */
            break;
         case 1: /* Vertical disables horizontal crop */
            zoomed_width = retrow;
            break;
         case 2: /* Horizontal disables vertical crop */
            zoomed_height = retroh;
            break;
         case 3: /* 16:9 */
            zoom_dar = (double)16/9;
            zoomed_width = retrow;
            if (zoomed_height < (int)(zoomed_width * width_multiplier / zoom_dar * zoom_par))
               zoomed_width = (int)(zoomed_height * zoom_dar / zoom_par);
            break;
         case 4: /* 16:10 */
            zoom_dar = (double)16/10;
            zoomed_width = retrow;
            if (zoomed_height < (int)(zoomed_width * width_multiplier / zoom_dar * zoom_par))
               zoomed_width = (int)(zoomed_height * zoom_dar / zoom_par);
            break;
         case 5: /* 4:3 */
            zoom_dar = (double)4/3;
            if (zoomed_height < (int)(zoomed_width * width_multiplier / zoom_dar * zoom_par))
            {
               zoomed_height = (int)(zoomed_width / zoom_dar * zoom_par);
               if (zoomed_height < zoomed_height_original)
                  zoomed_height = zoomed_height_original;
               zoomed_width = (int)(zoomed_height * zoom_dar / zoom_par);
            }
            break;
         case 6: /* 5:4 */
            zoom_dar = (double)5/4;
            if (zoomed_height < (int)(zoomed_width * width_multiplier / zoom_dar * zoom_par))
            {
               zoomed_height = (int)(zoomed_width / zoom_dar * zoom_par);
               if (zoomed_height < zoomed_height_original)
                  zoomed_height = zoomed_height_original;
               zoomed_width = (int)(zoomed_height * zoom_dar / zoom_par);
            }
            break;
      }

      if (video_config & PUAE_VIDEO_DOUBLELINE)
         zoomed_height *= 2;

      zoomed_width = (zoomed_width < 320) ? 320 : zoomed_width;
      zoomed_width *= width_multiplier;

      if (zoomed_height > retroh)
         zoomed_height = retroh;

      if (zoomed_width > retrow)
         zoomed_width = retrow;
   }

   struct retro_system_av_info new_av_info;
   retro_get_system_av_info(&new_av_info);

   if (zoomed_height != retroh || zoomed_width != retrow)
   {
      new_av_info.geometry.base_width   = zoomed_width;
      new_av_info.geometry.base_height  = zoomed_height;
      new_av_info.geometry.aspect_ratio = retro_get_aspect_ratio(zoomed_width, zoomed_height, false);

      /* Ensure statusbar stays visible at the bottom */
      if (opt_statusbar_position >= 0 && (retroh - zoomed_height - opt_statusbar_position_offset) >= opt_statusbar_position)
         opt_statusbar_position = retroh - zoomed_height - opt_statusbar_position_offset;

#if 0
      fprintf(stdout, "ztatusbar:%3d old:%3d offset:%3d, defaulth:%d retroz:%d\n", opt_statusbar_position, opt_statusbar_position_old, opt_statusbar_position_offset, defaulth, zoomed_height);
#endif
   }

   /* Timing or geometry update */
   if (change_timing)
   {
      new_av_info.timing.fps = hz;
      environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &new_av_info);
   }
   else if (change_geometry)
      environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &new_av_info);

   /* If zoom mode should be vertically centered automagically */
   if (opt_vertical_offset_auto && (zoom_mode_id != 0 || zoomed_height != retroh))
      update_video_center_vertical();
   else
      thisframe_y_adjust = minfirstline + opt_vertical_offset;

   /* Horizontal centering needs to be done also after geometry change */
   if (opt_horizontal_offset_auto)
      update_video_center_horizontal();

   /* Logging */
   if (av_log)
   {
      if (change_timing)
         fprintf(stdout, "* Update av_info : %dx%d %0.4fHz, zoomed: %dx%d, aspect:%0.3f, video_config:%d\n", retrow, retroh, hz, zoomed_width, zoomed_height, retro_get_aspect_ratio(zoomed_width, zoomed_height, false), video_config_geometry);
      else if (change_geometry)
         fprintf(stdout, "* Update geometry: %dx%d, zoomed: %dx%d, aspect:%0.3f, video_config:%d\n", retrow, retroh, zoomed_width, zoomed_height, retro_get_aspect_ratio(zoomed_width, zoomed_height, false), video_config_geometry);
      else
         fprintf(stdout, "* Update center  : %dx%d, zoomed: %dx%d, aspect:%0.3f, video_config:%d\n", retrow, retroh, zoomed_width, zoomed_height, retro_get_aspect_ratio(zoomed_width, zoomed_height, false), video_config_geometry);
   }

   /* Triggers check_prefs_changed_gfx() in vsync_handle_check() */
   prefs_changed = 1;

   /* Changing any drawing/offset parameters requires
    * a drawing reset - it is safest to just do this
    * whenever retro_update_av_info() is called */
   request_reset_drawing = true;

   return true;
}

void retro_run(void)
{
   /* Core options */
   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();

   /* Soft reset requested */
   if (request_reset_soft)
      retro_reset_soft();

   /* Handle statusbar text, audio filter type & video geometry + resolution */
   update_audiovideo();

   /* AV info change is requested */
   if (request_update_av_info)
      retro_update_av_info();

   /* Poll inputs */
   retro_poll_event();

   /* If any drawing parameters/offsets have been modified,
    * must call reset_drawing() to ensure that the changes
    * are 'registered' by center_image() in drawing.c
    * > If we don't do this, the wrong parameters may be
    *   used on the next frame, which can lead to out of
    *   bounds video buffer access (memory corruption)
    * > This check must come *after* horizontal/vertical
    *   offset calculation, retro_update_av_info() and
    *   retro_poll_event() */
   if (request_reset_drawing)
   {
      request_reset_drawing = false;
      reset_drawing();
   }

   /* Dynamic resolution changing requires a frame breather after reset_drawing() */
   if (request_init_custom_timer > 0)
   {
      if (request_init_custom_timer == 2)
         request_reset_drawing = true;
      request_init_custom_timer--;
      if (request_init_custom_timer == 0)
         init_custom();
   }

   /* Refresh CPU prefs */
   if (request_check_prefs_timer > 0)
   {
      request_check_prefs_timer--;
      if (request_check_prefs_timer == 0)
      {
         update_variables();
         config_changed = 1;
         check_prefs_changed_audio();
         check_prefs_changed_custom();
         check_prefs_changed_cpu();
         config_changed = 0;
      }
   }

   /* Check if a restart is required */
   if (restart_pending)
   {
      restart_pending = 0;
      libretro_do_restart(sizeof(uae_argv)/sizeof(*uae_argv), uae_argv);
      /* Re-run emulation first pass */
      restart_pending = m68k_go(1, 0);
      video_cb(retro_bmp, zoomed_width, zoomed_height, retrow << (pix_bytes / 2));
      return;
   }

   /* Resume emulation for 1 frame */
   restart_pending = m68k_go(1, 1);

   /* Warning messages */
   if (retro_message)
   {
      struct retro_message msg;
      msg.msg = retro_message_msg;
      msg.frames = 500;
      environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
      retro_message = false;
   }

   /* Virtual keyboard */
   if (retro_vkbd)
   {
      /* VKBD requires a graceful redraw, blunt reset_drawing() interferes with zoom */
      frame_redraw_necessary = 2;
      print_vkbd(retro_bmp);
   }
   /* Maximum 288p/576p PAL shenanigans:
    * Mask the last line(s), since UAE does not refresh the last line, and even internal OSD will leave trails */
   if (video_config & PUAE_VIDEO_PAL)
   {
      if (video_config & PUAE_VIDEO_DOUBLELINE)
      {
         if (interlace_seen)
         {
            DrawHline(retro_bmp, 0, 572, zoomed_width, 0, 0);
            DrawHline(retro_bmp, 0, 573, zoomed_width, 0, 0);
            DrawHline(retro_bmp, 0, 574, zoomed_width, 0, 0);
            DrawHline(retro_bmp, 0, 575, zoomed_width, 0, 0);
         }
         else
         {
            DrawHline(retro_bmp, 0, 574, zoomed_width, 0, 0);
            DrawHline(retro_bmp, 0, 575, zoomed_width, 0, 0);
         }
      }
      else
      {
         DrawHline(retro_bmp, 0, 287, zoomed_width, 0, 0);
      }
   }
   video_cb(retro_bmp, zoomed_width, zoomed_height, retrow << (pix_bytes / 2));
}

bool retro_load_game(const struct retro_game_info *info)
{
   /* Content */
   char *local_path;
   if (info)
   {
      /* Special unicode chars won't work without conversion */
      local_path = utf8_to_local_string_alloc(info->path);
      if (local_path)
      {
         strcpy(full_path, local_path);
         free(local_path);
         local_path = NULL;
      }
      else
         return false;
   }

   /* UAE config */
   static bool retro_return;
   retro_return = retro_create_config();
   if (!retro_return)
      return false;

   /* Initialise emulation */
   umain(sizeof(uae_argv)/sizeof(*uae_argv), uae_argv);

   /* Run emulation first pass */
   restart_pending = m68k_go(1, 0);
   /* > We are now ready to enter the run loop */
   libretro_runloop_active = 1;

   /* Save states
    * > Ensure that save state file path is empty,
    *   since we use memory based save states */
   savestate_fname[0] = '\0';
   /* > Get save state size
    *   Here we use initial size + 5%
    *   Should be sufficient in all cases
    * NOTE: It would be better to calculate the
    * state size based on current config parameters,
    * but while
    *   - currprefs.chipmem_size
    *   - currprefs.bogomem_size
    *   - currprefs.fastmem_size
    * account for *most* of the size, there are
    * simply too many other factors to rely on this
    * alone (i.e. mem size + 5% is fine in most cases,
    * but if the user supplies a custom uae config file
    * then this is not adequate at all). Untangling the
    * full set of values that are recorded is beyond
    * my patience... */
   struct zfile *state_file = save_state("libretro", 0);

   if (state_file)
   {
      save_state_file_size  = (size_t)zfile_size(state_file);
      save_state_file_size += (size_t)(((float)save_state_file_size * 0.05f) + 0.5f);
      zfile_fclose(state_file);
   }

   return true;
}

void retro_unload_game(void)
{
   /* Ensure save state de-serialization file
    * is closed/NULL
    * Note: Have to do this here (not in retro_deinit())
    * since leave_program() calls zfile_exit() */
   if (retro_deserialize_file)
   {
      zfile_fclose(retro_deserialize_file);
      retro_deserialize_file = NULL;
   }

   leave_program();

   libretro_runloop_active = 0;
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
   return save_state_file_size;
}

bool retro_serialize(void *data_, size_t size)
{
   struct zfile *state_file = save_state("libretro", (uae_u64)save_state_file_size);
   bool success = false;

   if (state_file)
   {
      uae_s64 state_file_size = zfile_size(state_file);

      if (size >= state_file_size)
      {
         size_t len = zfile_fread(data_, 1, state_file_size, state_file);

         if (len == state_file_size)
            success = true;
      }

      zfile_fclose(state_file);
   }

   return success;
}

bool retro_unserialize(const void *data_, size_t size)
{
   /* TODO: When attempting to use runahead, CD32
    * and WHDLoad content will hang on boot. It seems
    * we cannot restore a state until the system has
    * passed some level of initialisation - but the
    * point at which a restore becomes 'safe' is
    * unknown (for CD32 content, for example, we have
    * to wait ~300 frames before runahead can be enabled) */
   bool success = false;

   /* Cannot restore state while any 'savestate'
    * operation is underway
    * > Actual restore is deferred until m68k_go(),
    *   so we have to use a shared shared state file
    *   object - this cannot be modified until the
    *   restore is complete
    * > Note that this condition should never be
    *   true - if a save state operation is underway
    *   at this point then we are dealing with an
    *   unknown error */
   if (!savestate_state)
   {
#if 0
      /* Savestates also save CPU prefs, therefore force core options, but skip it for now */
      request_check_prefs_timer = 4;
#endif

      if (retro_deserialize_file)
      {
         zfile_fclose(retro_deserialize_file);
         retro_deserialize_file = NULL;
      }

      retro_deserialize_file = zfile_fopen_empty(NULL, "libretro", size);

      if (retro_deserialize_file)
      {
         size_t len = zfile_fwrite(data_, 1, size, retro_deserialize_file);

         if (len == size)
         {
            unsigned frame_counter = 0;
            unsigned max_frames    = 50;

            zfile_fseek(retro_deserialize_file, 0, SEEK_SET);
            savestate_state = STATE_DORESTORE;

            /* For correct operation of the frontend,
             * the save state restore must be completed
             * by the time this function returns.
             * Since PUAE requires several (2) frames to get
             * itself in order during a restore event, we
             * have to keep emulating frames until the
             * restore is complete...
             * > Note that we set a 'timeout' of 50 frames
             *   here (1s of emulated time at 50Hz) to
             *   prevent lock-ups in the event of unexpected
             *   errors
             * > Temporarily 'deactivate' runloop - this lets
             *   us call m68k_go() without accessing frontend
             *   features - specifically, it disables the audio
             *   callback functionality */
            libretro_runloop_active = 0;
            while (savestate_state && (frame_counter < max_frames))
            {
               /* Note that retro_deserialize_file will be
                * closed inside m68k_go() upon successful
                * completion of the restore event */
               restart_pending = m68k_go(1, 1);
               frame_counter++;
            }
            libretro_runloop_active = 1;

            /* If the above while loop times out, then
             * everything is completely broken. We cannot
             * handle this here, so just assume the restore
             * completed successfully... */
            request_reset_drawing = true;
            success               = true;
         }
         else
         {
            zfile_fclose(retro_deserialize_file);
            retro_deserialize_file = NULL;
         }
      }
   }

   return success;
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

#if defined(ANDROID) || defined(__SWITCH__) || defined(WIIU) || defined(__CELLOS_LV2__)
#ifndef __CELLOS_LV2__
#include <sys/timeb.h>
#else
#include "ps3_headers.h"
#undef timezone
#endif

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
#ifndef __CELLOS_LV2__    
    tb->timezone = tz.tz_minuteswest;
    tb->dstflag  = tz.tz_dsttime;
#endif

    return 0;
}
#endif
