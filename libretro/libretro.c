#include "libretro.h"
#include "retrodep/retroglue.h"
#include "libretro-mapper.h"
#include "libretro-glue.h"
#include "retro_files.h"
#include "retro_strings.h"
#include "retro_disk_control.h"
#include "uae_types.h"

#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "inputdevice.h"
#include "savestate.h"

#define EMULATOR_DEF_WIDTH 720
#define EMULATOR_DEF_HEIGHT 568
#define EMULATOR_MAX_WIDTH 1024
#define EMULATOR_MAX_HEIGHT 1024

#if EMULATOR_DEF_WIDTH < 0 || EMULATOR_DEF_WIDTH > EMULATOR_MAX_WIDTH || EMULATOR_DEF_HEIGHT < 0 || EMULATOR_DEF_HEIGHT > EMULATOR_MAX_HEIGHT
#error EMULATOR_DEF_WIDTH || EMULATOR_DEF_HEIGHT
#endif

cothread_t mainThread;
cothread_t emuThread;

int defaultw = EMULATOR_DEF_WIDTH;
int defaulth = EMULATOR_DEF_HEIGHT;
int retrow = 0;
int retroh = 0;
char key_state[512];
char key_state2[512];
bool opt_use_whdload_hdf = true;
bool opt_enhanced_statusbar = true;
int opt_statusbar_position = 0;
int opt_statusbar_position_old = 0;
unsigned int opt_keyrahkeypad = 0;
unsigned int opt_analogmouse = 0;
int analog_deadzone = 6144;
unsigned int analog_sensitivity = 2048;
extern int turbo_fire_button;
extern unsigned int turbo_pulse;
int pix_bytes = 2;
static bool pix_bytes_initialized = false;
bool fake_ntsc = false;
bool real_ntsc = false;
bool request_update_av_info = false;
int zoom_mode_id = 0;
int zoomed_height;
bool fast_forward_is_on = false;

#if defined(NATMEM_OFFSET)
extern uae_u8 *natmem_offset;
extern uae_u32 natmem_size;
#endif
extern unsigned short int  bmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern unsigned short int  savebmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern int SHIFTON;
extern int STATUSON;
extern char RPATH[512];
extern void Print_Status(void);
extern unsigned short * sndbuffer;
extern int sndbufsize;
static int firstpass = 1;
extern int prefs_changed;
int opt_vertical_offset = 0;
extern int minfirstline;
static int minfirstline_update_frame_timer = 3;
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

int mapper_keys[31]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static char buf[64][4096] = { 0 };

#ifdef WIN32
#define DIR_SEP_STR "\\"
#else
#define DIR_SEP_STR "/"
#endif

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

const char *retro_save_directory;
const char *retro_system_directory;
const char *retro_content_directory;

// Disk control context
static dc_storage* dc;

// Amiga default models
// chipmem_size 1 = 0.5MB, 2 = 1MB, 4 = 2MB
// bogomem_size 2 = 0.5MB, 4 = 1MB, 6 = 1.5MB, 7 = 1.8MB

#define A500 "\
cpu_type=68000\n\
chipmem_size=1\n\
bogomem_size=2\n\
chipset_compatible=A500\n\
chipset=ocs\n"

#define A500OG "\
cpu_type=68000\n\
chipmem_size=1\n\
bogomem_size=0\n\
chipset_compatible=A500\n\
chipset=ocs\n"

#define A500PLUS "\
cpu_type=68000\n\
chipmem_size=2\n\
bogomem_size=4\n\
chipset_compatible=A500+\n\
chipset=ecs\n"

#define A600 "\
cpu_type=68000\n\
chipmem_size=4\n\
fastmem_size=8\n\
chipset_compatible=A600\n\
chipset=ecs\n"

#define A1200 "\
cpu_type=68ec020\n\
chipmem_size=4\n\
fastmem_size=8\n\
chipset_compatible=A1200\n\
chipset=aga\n"

#define A1200OG "\
cpu_type=68ec020\n\
chipmem_size=4\n\
fastmem_size=0\n\
chipset_compatible=A1200\n\
chipset=aga\n"


// Amiga default kickstarts

#define A500_ROM    "kick34005.A500"
#define A500KS2_ROM "kick37175.A500"
#define A600_ROM    "kick40063.A600"
#define A1200_ROM   "kick40068.A1200"

#define PUAE_VIDEO_PAL 		0x01
#define PUAE_VIDEO_NTSC 	0x02
#define PUAE_VIDEO_HIRES 	0x04
#define PUAE_VIDEO_CROP 	0x08

#define PUAE_VIDEO_PAL_OV_LO 	PUAE_VIDEO_PAL
#define PUAE_VIDEO_PAL_CR_LO 	PUAE_VIDEO_PAL|PUAE_VIDEO_CROP
#define PUAE_VIDEO_NTSC_OV_LO 	PUAE_VIDEO_NTSC
#define PUAE_VIDEO_NTSC_CR_LO 	PUAE_VIDEO_NTSC|PUAE_VIDEO_CROP
#define PUAE_VIDEO_PAL_OV_HI 	PUAE_VIDEO_PAL|PUAE_VIDEO_HIRES
#define PUAE_VIDEO_PAL_CR_HI 	PUAE_VIDEO_PAL|PUAE_VIDEO_CROP|PUAE_VIDEO_HIRES
#define PUAE_VIDEO_NTSC_OV_HI 	PUAE_VIDEO_NTSC|PUAE_VIDEO_HIRES
#define PUAE_VIDEO_NTSC_CR_HI 	PUAE_VIDEO_NTSC|PUAE_VIDEO_CROP|PUAE_VIDEO_HIRES

static char uae_machine[256];
static char uae_kickstart[16];
static char uae_config[1024];

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_controller_description p1_controllers[] ={
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
         "Needs restart",
         {
            { "A500", "A500 (512KB Chip + 512KB Slow)" },
            { "A500OG", "A500 (512KB Chip)" },
            { "A500PLUS", "A500+ (1MB Chip + 1MB Slow)" },
            { "A600", "A600 (2MB Chip + 8MB Fast)" },
            { "A1200", "A1200 (2MB Chip + 8MB Fast)" },
            { "A1200OG", "A1200 (2MB Chip)" },
            { NULL, NULL },
         },
         "A500"
      },
      {
         "puae_video_standard",
         "Video standard",
         "Needs restart",
         {
            { "PAL", NULL },
            { "NTSC", NULL },
            { NULL, NULL },
         },
         "PAL"
      },
      {
         "puae_video_aspect",
         "Aspect ratio",
         "",
         {
            { "Auto", "Automatic" },
            { "PAL", NULL },
            { "NTSC", NULL },
            { NULL, NULL },
         },
         "Auto"
      },
      {
         "puae_video_allow_hz_change",
         "Allow PAL/NTSC Hz change",
         "",
         {
            { "enabled", NULL },
            { "disabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      {
         "puae_video_hires",
         "High resolution",
         "Needs restart",
         {
            { "true", "enabled" },
            { "false", "disabled" },
            { NULL, NULL },
         },
         "true"
      },
      {
         "puae_video_crop_overscan",
         "Crop overscan",
         "Needs restart",
         {
            { "false", "disabled" },
            { "true", "enabled" },
            { NULL, NULL },
         },
         "false"
      },
      {
         "puae_statusbar_position",
         "Statusbar position",
         "",
         {
            { "bottom6", "Bottom - 6" },
            { "bottom5", "Bottom - 5" },
            { "bottom4", "Bottom - 4" },
            { "bottom3", "Bottom - 3" },
            { "bottom2", "Bottom - 2" },
            { "bottom1", "Bottom - 1" },
            { "bottom", "Bottom" },
            { "top", "Top" },
            { "top1", "Top + 1" },
            { "top2", "Top + 2" },
            { "top3", "Top + 3" },
            { "top4", "Top + 4" },
            { "top5", "Top + 5" },
            { "top6", "Top + 6" },
            { NULL, NULL },
         },
         "bottom"
      },
      {
         "puae_enhanced_statusbar",
         "Enhanced statusbar",
         "Displays additional information in statusbar",
         {
            { "enabled", NULL },
            { "disabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      {
         "puae_cpu_speed",
         "CPU speed",
         "Needs restart. Max needs cycle exact off",
         {
            { "real", "Real" },
            { "max", "Max" },
            { NULL, NULL },
         },
         "real"
      },
      {
         "puae_cpu_compatible",
         "CPU compatible",
         "Needs restart",
         {
            { "true", "enabled" },
            { "false", "disabled" },
            { NULL, NULL },
         },
         "true"
      },
      {
         "puae_cycle_exact",
         "Cycle exact",
         "Combined CPU + Blitter cycle exact. Needs restart",
         {
            { "false", "disabled" },
            { "true", "enabled" },
            { NULL, NULL },
         },
         "false"
      },
      {
         "puae_sound_output",
         "Sound output",
         "",
         {
            { "exact", "Exact" },
            { "normal", "Normal" },
            { "interrupts", "Interrupts" },
            { "none", "None" },
            { NULL, NULL },
         },
         "exact"
      },
      {
         "puae_sound_stereo_separation",
         "Sound stereo separation",
         "",
         {
            { "100\%", NULL },
            { "90\%", NULL },
            { "80\%", NULL },
            { "70\%", NULL },
            { "60\%", NULL },
            { "50\%", NULL },
            { "40\%", NULL },
            { "30\%", NULL },
            { "20\%", NULL },
            { "10\%", NULL },
            { "0\%", NULL },
            { NULL, NULL },
         },
         "100\%"
      },
      {
         "puae_sound_interpol",
         "Sound interpolation",
         "",
         {
            { "none", "None" },
            { "anti", "Anti" },
            { "sinc", "Sinc" },
            { "rh", "RH" },
            { "crux", "Crux" },
            { NULL, NULL },
         },
         "none"
      },
      {
         "puae_sound_filter",
         "Sound filter",
         "",
         {
            { "emulated", "Emulated" },
            { "off", "Always off" },
            { "on", "Always on" },
         },
         "emulated"
      },
      {
         "puae_sound_filter_type",
         "Sound filter type",
         "",
         {
            { "standard", "A500" },
            { "enhanced", "A1200" },
         },
         "standard",
      },
      {
         "puae_floppy_speed",
         "Floppy speed",
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
         "puae_floppy_sound",
         "Floppy sound emulation",
         "Needs external files in system/uae_data/",
         {
            { "100", "disabled" },
            { "90", "10\% volume" },
            { "80", "20\% volume" },
            { "70", "30\% volume" },
            { "60", "40\% volume" },
            { "50", "50\% volume" },
            { "40", "60\% volume" },
            { "30", "70\% volume" },
            { "20", "80\% volume" },
            { "10", "90\% volume" },
            { "0", "100\% volume" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_gfx_framerate",
         "Frameskip",
         "Cycle exact needs to be off for this to come into effect at startup",
         {
            { "disabled", NULL },
            { "1", NULL },
            { "2", NULL },
            { NULL, NULL },
         },
         "disabled"
      },
      {
         "puae_gfx_colors",
         "Color depth",
         "24bit is slower and not available on all platforms. Need restart",
         {
            { "16bit", "Thousands (16bit)" },
            { "24bit", "Millions (24bit)" },
            { NULL, NULL },
         },
         "16bit"
      },
      {
         "puae_collision_level",
         "Collision level",
         "Playfields is recommended default",
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
         "Immediate blits",
         "",
         {
            { "false", "disabled" },
            { "true", "enabled" },
            { NULL, NULL },
         },
         "false"
      },
      {
         "puae_gfx_center_horizontal",
         "Horizontal centering",
         "Essential with crop overscan enabled",
         {
            { "none", "disabled" },
            { "simple", "Simple" },
            { "smart", "Smart" },
            { NULL, NULL },
         },
         "none"
      },
      {
         "puae_gfx_center_vertical",
         "Vertical centering",
         "Essential with crop overscan enabled",
         {
            { "none", "disabled" },
            { "simple", "Simple" },
            { "smart", "Smart" },
            { NULL, NULL },
         },
         "none"
      },
      {
         "puae_zoom_mode",
         "Zoom mode",
         "Zoom requires vertical centering: off,  integer scaling: off, and aspect ratio: core provided for best results",
         {
            { "None", "disabled" },
            { "Small", NULL },
            { "Medium", NULL },
            { "Large", NULL },
            { "Larger", NULL },
            { "Maximum", NULL },
            { NULL, NULL },
         },
         "None"
      },
      {
         "puae_vertical_pos",
         "Vertical position",
         "Allows to shift the screen up and down, useful to manually center the screen",
         {
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
            { "-2", NULL },
            { "-4", NULL },
            { "-6", NULL },
            { "-8", NULL },
            { "-10", NULL },
            { "-12", NULL },
            { "-14", NULL },
            { "-16", NULL },
            { "-18", NULL },
            { "-20", NULL },
            { NULL, NULL },
         },
         "0"
      },
      {
         "puae_use_whdload",
         "Use WHDLoad.hdf",
         "Needs restart",
         {
            { "enabled", NULL },
            { "disabled", NULL },
            { NULL, NULL },
         },
         "enabled"
      },
      {
         "puae_analogmouse",
         "Analog mouse",
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
         "Analog deadzone",
         "",
         {
            { "15", "15\%" },
            { "20", "20\%" },
            { "25", "25\%" },
            { "30", "30\%" },
            { "35", "35\%" },
            { "0", "0\%" },
            { "5", "5\%" },
            { "10", "10\%" },
            { NULL, NULL },
         },
         "15"
      },
      {
         "puae_analogmouse_sensitivity",
         "Analog sensitivity",
         "",
         {
            { "1.0", "100\%" },
            { "1.1", "110\%" },
            { "1.2", "120\%" },
            { "1.3", "130\%" },
            { "1.4", "140\%" },
            { "1.5", "150\%" },
            { "0.5", "50\%" },
            { "0.6", "60\%" },
            { "0.7", "70\%" },
            { "0.8", "80\%" },
            { "0.9", "90\%" },
            { NULL, NULL },
         },
         "1.0"
      },
      {
         "puae_keyrah_keypad_mappings",
         "Keyrah keypad mappings",
         "Hardcoded keypad to joy mappings for Keyrah hardware",
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
         "Hotkey: Toggle virtual keyboard",
         "Pressing a button mapped to this key opens the keyboard",
         {{ NULL, NULL }},
         "RETROK_F11"
      },
      {
         "puae_mapper_statusbar",
         "Hotkey: Toggle statusbar",
         "Pressing a button mapped to this key toggles statusbar",
         {{ NULL, NULL }},
         "RETROK_F10"
      },
      {
         "puae_mapper_mouse_toggle",
         "Hotkey: Toggle mouse",
         "Pressing a button mapped to this key toggles between joystick and mouse",
         {{ NULL, NULL }},
         "RETROK_RCTRL"
      },
      {
         "puae_mapper_mouse_speed",
         "Hotkey: Change mouse speed",
         "Pressing a button mapped to this key alters the mouse speed",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_reset",
         "Hotkey: Reset",
         "Ctrl-Amiga-Amiga",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_aspect_ratio_toggle",
         "Hotkey: Toggle aspect ratio",
         "Only usable with PAL video",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_zoom_mode_toggle",
         "Hotkey: Toggle zoom mode",
         "",
         {{ NULL, NULL }},
         "---"
      },
      /* Button mappings */
      {
         "puae_mapper_select",
         "RetroPad Select",
         "",
         {{ NULL, NULL }},
         "---"
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
         "RETROK_SPACE"
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
         "RETROK_F11"
      },
      {
         "puae_mapper_r",
         "RetroPad R",
         "",
         {{ NULL, NULL }},
         "RETROK_F10"
      },
      {
         "puae_mapper_l2",
         "RetroPad L2",
         "",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_r2",
         "RetroPad R2",
         "",
         {{ NULL, NULL }},
         "---"
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
         "Mapping for left analog stick up",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_ld",
         "RetroPad L-Down",
         "Mapping for left analog stick down",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_ll",
         "RetroPad L-Left",
         "Mapping for left analog stick left",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_lr",
         "RetroPad L-Right",
         "Mapping for left analog stick right",
         {{ NULL, NULL }},
         "---"
      },

      /* Right Stick */
      {
         "puae_mapper_ru",
         "RetroPad R-Up",
         "Mapping for right analog stick up",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rd",
         "RetroPad R-Down",
         "Mapping for right analog stick down",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rl",
         "RetroPad R-Left",
         "Mapping for right analog stick left",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_rr",
         "RetroPad R-Right",
         "Mapping for right analog stick right",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_turbo_fire_button",
         "RetroPad turbo fire",
         "Replaces mapped key with a turbo fire button",
         {
            { "disabled", NULL },
            { "A", "RetroPad A" },
            { "Y", "RetroPad Y" },
            { "X", "RetroPad X" },
            { "L", "RetroPad L" },
            { "R", "RetroPad R" },
            { "L2", "RetroPad L2" },
            { "R2", "RetroPad R2" },
         },
         "disabled"
      },
      {
         "puae_turbo_pulse",
         "RetroPad turbo pulse",
         "Frames in a button cycle. 2 equals button press and release on every other frame",
         {
            { "2", NULL },
            { "4", NULL },
            { "6", NULL },
            { "8", NULL },
            { "10", NULL },
            { "12", NULL },
         },
         "4"
      },

      { NULL, NULL, NULL, {{0}}, NULL },
   };

   /* fill in the values for all the mappers */
   int i = 0;
   int j = 0;
   int hotkey = 0;
   while(core_options[i].key)
   {
      if (strstr(core_options[i].key, "puae_mapper_"))
      {
         /* Show different key list for hotkeys (special negatives removed) */
         if(strstr(core_options[i].key, "puae_mapper_vkbd")
         || strstr(core_options[i].key, "puae_mapper_statusbar")
         || strstr(core_options[i].key, "puae_mapper_mouse_toggle")
         || strstr(core_options[i].key, "puae_mapper_mouse_speed")
         || strstr(core_options[i].key, "puae_mapper_reset")
         || strstr(core_options[i].key, "puae_mapper_aspect_ratio_toggle")
         || strstr(core_options[i].key, "puae_mapper_zoom_mode_toggle")
         )
            hotkey = 1;
         else
            hotkey = 0;

         j = 0;
         if(hotkey)
         {
             while(keyDescHotkeys[j] && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
             {
                core_options[i].values[j].value = keyDescHotkeys[j];
                core_options[i].values[j].label = NULL;
                ++j;
             };
         }
         else
         {
             while(keyDesc[j] && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
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
   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);

   unsigned version = 0;
   if (cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && (version == 1))
      cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, core_options);
   else
   {
      /* Fallback for older API */
      static struct retro_variable variables[64] = { 0 };
      i = 0;
      while(core_options[i].key)
      {
         buf[i][0] = 0;
         variables[i].key = core_options[i].key;
         strcpy(buf[i], core_options[i].desc);
         strcat(buf[i], "; ");
         strcat(buf[i], core_options[i].default_value);
         j = 0;
         while(core_options[i].values[j].value && j < RETRO_NUM_CORE_OPTION_VALUES_MAX)
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
      cb( RETRO_ENVIRONMENT_SET_VARIABLES, variables);
   }

   static bool allowNoGameMode;
   allowNoGameMode = true;
   environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &allowNoGameMode);
}

static void update_variables(void)
{
   uae_machine[0] = '\0';
   uae_config[0] = '\0';

   struct retro_variable var = {0};

   var.key = "puae_model";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "A500") == 0)
      {
          strcat(uae_machine, A500);
          strcpy(uae_kickstart, A500_ROM);
      }
      if (strcmp(var.value, "A500OG") == 0)
      {
          strcat(uae_machine, A500OG);
          strcpy(uae_kickstart, A500_ROM);
      }
      if (strcmp(var.value, "A500PLUS") == 0)
      {
          strcat(uae_machine, A500PLUS);
          strcpy(uae_kickstart, A500KS2_ROM);
      }
      if (strcmp(var.value, "A600") == 0)
      {
          strcat(uae_machine, A600);
          strcpy(uae_kickstart, A600_ROM);
      }
      if (strcmp(var.value, "A1200") == 0)
      {
          strcat(uae_machine, A1200);
          strcpy(uae_kickstart, A1200_ROM);
      }
      if (strcmp(var.value, "A1200OG") == 0)
      {
          strcat(uae_machine, A1200OG);
          strcpy(uae_kickstart, A1200_ROM);
      }
   }

   var.key = "puae_video_standard";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* video_config change only at start */
      if(video_config_old == 0)
         if(strcmp(var.value, "PAL") == 0)
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
   }

   var.key = "puae_video_aspect";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "Auto") == 0) video_config_aspect = 0;
      else if(strcmp(var.value, "PAL") == 0) video_config_aspect = PUAE_VIDEO_PAL;
      else if(strcmp(var.value, "NTSC") == 0) video_config_aspect = PUAE_VIDEO_NTSC;
   }

   var.key = "puae_video_allow_hz_change";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "enabled") == 0) video_config_allow_hz_change = 1;
      else if(strcmp(var.value, "disabled") == 0) video_config_allow_hz_change = 0;
   }

   var.key = "puae_video_hires";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "true") == 0)
         video_config |= PUAE_VIDEO_HIRES;
      else
         video_config &= ~PUAE_VIDEO_HIRES;
   }

   var.key = "puae_video_crop_overscan";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "true") == 0)
         video_config |= PUAE_VIDEO_CROP;
      else
         video_config &= ~PUAE_VIDEO_CROP;
   }

   var.key = "puae_enhanced_statusbar";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "enabled") == 0)
         opt_enhanced_statusbar = true;
      else
      {
         opt_enhanced_statusbar = false;

         /* Screen refresh required */
         Screen_SetFullUpdate();
      }
   }

   var.key = "puae_statusbar_position";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "top") == 0) opt_statusbar_position = -1;
      else if(strcmp(var.value, "top1") == 0) opt_statusbar_position = -10;
      else if(strcmp(var.value, "top2") == 0) opt_statusbar_position = -20;
      else if(strcmp(var.value, "top3") == 0) opt_statusbar_position = -30;
      else if(strcmp(var.value, "top4") == 0) opt_statusbar_position = -40;
      else if(strcmp(var.value, "top5") == 0) opt_statusbar_position = -50;
      else if(strcmp(var.value, "top6") == 0) opt_statusbar_position = -60;
      else if(strcmp(var.value, "bottom") == 0) opt_statusbar_position = 0;
      else if(strcmp(var.value, "bottom1") == 0) opt_statusbar_position = 10;
      else if(strcmp(var.value, "bottom2") == 0) opt_statusbar_position = 20;
      else if(strcmp(var.value, "bottom3") == 0) opt_statusbar_position = 30;
      else if(strcmp(var.value, "bottom4") == 0) opt_statusbar_position = 40;
      else if(strcmp(var.value, "bottom5") == 0) opt_statusbar_position = 50;
      else if(strcmp(var.value, "bottom6") == 0) opt_statusbar_position = 60;

      /* Exceptions for lo-res and enhanced statusbar */
      if(video_config & 0x04) // PUAE_VIDEO_HIRES
         ;
      else
         if(opt_enhanced_statusbar)
            opt_statusbar_position = (opt_statusbar_position < 0) ? ((opt_statusbar_position==-1) ? -10 : opt_statusbar_position-10) : opt_statusbar_position+10;
         else
            opt_statusbar_position = (opt_statusbar_position < 0) ? ((opt_statusbar_position==-1) ? -1 : opt_statusbar_position-2) : opt_statusbar_position;

      /* Screen refresh required */
      if(opt_statusbar_position_old != opt_statusbar_position)
         Screen_SetFullUpdate();

      opt_statusbar_position_old = opt_statusbar_position;
   }

   var.key = "puae_cpu_speed";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cpu_speed=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");
   }

   var.key = "puae_cpu_compatible";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cpu_compatible=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");
   }

   var.key = "puae_cycle_exact";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "cycle_exact=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");
   }

   var.key = "puae_sound_output";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_output=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (strcmp(var.value, "none") == 0) changed_prefs.produce_sound=0;
      else if (strcmp(var.value, "interrupts") == 0) changed_prefs.produce_sound=1;
      else if (strcmp(var.value, "normal") == 0) changed_prefs.produce_sound=2;
      else if (strcmp(var.value, "exact") == 0) changed_prefs.produce_sound=3;
   }

   var.key = "puae_sound_stereo_separation";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int val = atoi(var.value) / 10;
      changed_prefs.sound_stereo_separation=val;

      char valbuf[50];
      snprintf(valbuf, 50, "%d", val);
      strcat(uae_config, "sound_stereo_separation=");
      strcat(uae_config, valbuf);
      strcat(uae_config, "\n");
   }

   var.key = "puae_sound_interpol";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_interpol=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (strcmp(var.value, "none") == 0) changed_prefs.sound_interpol=0;
      else if (strcmp(var.value, "anti") == 0) changed_prefs.sound_interpol=1;
      else if (strcmp(var.value, "sinc") == 0) changed_prefs.sound_interpol=2;
      else if (strcmp(var.value, "rh") == 0) changed_prefs.sound_interpol=3;
      else if (strcmp(var.value, "crux") == 0) changed_prefs.sound_interpol=4;
   }

   var.key = "puae_sound_filter";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_filter=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");
      
      if (strcmp(var.value, "emulated") == 0) changed_prefs.sound_filter=FILTER_SOUND_EMUL; 
      else if (strcmp(var.value, "off") == 0) changed_prefs.sound_filter=FILTER_SOUND_OFF;
      else if (strcmp(var.value, "on") == 0) changed_prefs.sound_filter=FILTER_SOUND_ON;
   }

   var.key = "puae_sound_filter_type";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "sound_filter_type=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if (strcmp(var.value, "standard") == 0) changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A500;
      else if (strcmp(var.value, "enhanced") == 0) changed_prefs.sound_filter_type=FILTER_SOUND_TYPE_A1200;
   }

   var.key = "puae_floppy_speed";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "floppy_speed=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if(firstpass != 1)
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
      if(firstpass != 1)
         changed_prefs.dfxclickvolume=atoi(var.value);
   }

   var.key = "puae_immediate_blits";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "immediate_blits=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if(firstpass != 1)
      {
         if (strcmp(var.value, "false") == 0) changed_prefs.immediate_blits=0;
         else if (strcmp(var.value, "true") == 0) changed_prefs.immediate_blits=1;
      }
   }

   var.key = "puae_collision_level";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "collision_level=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if(firstpass != 1)
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
      changed_prefs.gfx_framerate=val;

      if(val>1)
      {
         char valbuf[50];
         snprintf(valbuf, 50, "%d", val);
         strcat(uae_config, "gfx_framerate=");
         strcat(uae_config, valbuf);
         strcat(uae_config, "\n");
      }
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
         pix_bytes_initialized = true;
      }
   }

   var.key = "puae_gfx_center_horizontal";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "gfx_center_horizontal=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if(firstpass != 1)
      {
         int val;
         if (strcmp(var.value, "none") == 0) val=0;
         else if (strcmp(var.value, "simple") == 0) val=1;
         else if (strcmp(var.value, "smart") == 0) val=2;
         changed_prefs.gfx_xcenter=val;
      }
   }

   var.key = "puae_gfx_center_vertical";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      strcat(uae_config, "gfx_center_vertical=");
      strcat(uae_config, var.value);
      strcat(uae_config, "\n");

      if(firstpass != 1)
      {
         int val;
         if (strcmp(var.value, "none") == 0) val=0;
         else if (strcmp(var.value, "simple") == 0) val=1;
         else if (strcmp(var.value, "smart") == 0) val=2;
         changed_prefs.gfx_ycenter=val;
      }
   }

   var.key = "puae_zoom_mode";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "None") == 0) zoom_mode_id=0;
      else if (strcmp(var.value, "Small") == 0) zoom_mode_id=1;
      else if (strcmp(var.value, "Medium") == 0) zoom_mode_id=2;
      else if (strcmp(var.value, "Large") == 0) zoom_mode_id=3;
      else if (strcmp(var.value, "Larger") == 0) zoom_mode_id=4;
      else if (strcmp(var.value, "Maximum") == 0) zoom_mode_id=5;
   }

   var.key = "puae_vertical_pos";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int new_vertical_offset = atoi(var.value);
      if (new_vertical_offset >= -20 && new_vertical_offset <= 50)
      {
         /* this offset is used whenever minfirstline is reset on gfx mode changes in the init_hz() function */
         opt_vertical_offset = new_vertical_offset;
         /* we need to update the currently used minfirstline, too, for immediate effect */
         /* 26 is the PUAE default for minfirstline */
         minfirstline = 26 + opt_vertical_offset;
      }
   }

   var.key = "puae_use_whdload";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
        opt_use_whdload_hdf = true;
      if (strcmp(var.value, "disabled") == 0)
        opt_use_whdload_hdf = false;
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
      analog_deadzone = 328 * atoi(var.value);
   }

   var.key = "puae_analogmouse_sensitivity";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      analog_sensitivity = 2048 / atof(var.value);
   }

   var.key = "puae_keyrah_keypad_mappings";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0) opt_keyrahkeypad=0;
      else if (strcmp(var.value, "enabled") == 0) opt_keyrahkeypad=1;
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

   var.key = "puae_mapper_mouse_speed";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[27] = keyId(var.value);
   }

   var.key = "puae_mapper_reset";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[28] = keyId(var.value);
   }

   var.key = "puae_mapper_aspect_ratio_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[29] = keyId(var.value);
   }

   var.key = "puae_mapper_zoom_mode_toggle";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[30] = keyId(var.value);
   }




   // Setting resolution
   // According to PUAE configuration.txt :
   //
   // To emulate a high-resolution, fully overscanned PAL screen - either
   // non-interlaced with line-doubling, or interlaced - you need to use a
   // display of at least 720 by 568 pixels. If you specify a smaller size,
   // E-UAE's display will be clipped to fit (and you can use the gfx_center_*
   // options - see below - to centre the clipped region of the display).
   // Similarly, to fully display an over-scanned lo-res PAL screen, you need a
   // display of 360 by 284 pixels.
   //
   // So, here are the standard resolutions :
   // - **360x284**: PAL Low resolution with overscan
   // - **320x256**: PAL Low resolution cropped/clipped (without the "borders")
   // - **360x240**: NTSC Low resolution with overscan
   // - **320×200**: NTSC Low resolution cropped/clipped (without the "borders")
   // - **720x568**: PAL High resolution with overscan
   // - **640×512**: PAL High resolution cropped/clipped (without the "borders")
   // - **720x480**: NTSC High resolution with overscan
   // - **640×400**: NTSC High resolution cropped/clipped (without the "borders")
   switch(video_config)
   {
		case PUAE_VIDEO_PAL_OV_LO:
			defaultw = 360;
			defaulth = 284;
			strcat(uae_config, "gfx_lores=true\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_PAL_CR_LO:
			defaultw = 320;
			defaulth = 256;
			strcat(uae_config, "gfx_lores=true\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_NTSC_OV_LO:
			defaultw = 360;
			defaulth = 240;
			strcat(uae_config, "gfx_lores=true\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_NTSC_CR_LO:
			defaultw = 320;
			defaulth = 200;
			strcat(uae_config, "gfx_lores=true\n");
			strcat(uae_config, "gfx_linemode=none\n");
			break;
		case PUAE_VIDEO_PAL_OV_HI:
			defaultw = 720;
			defaulth = 568;
			strcat(uae_config, "gfx_lores=false\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
		case PUAE_VIDEO_PAL_CR_HI:
			defaultw = 640;
			defaulth = 512;
			strcat(uae_config, "gfx_lores=false\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
		case PUAE_VIDEO_NTSC_OV_HI:
			defaultw = 720;
			defaulth = 480;
			strcat(uae_config, "gfx_lores=false\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
		case PUAE_VIDEO_NTSC_CR_HI:
			defaultw = 640;
			defaulth = 400;
			strcat(uae_config, "gfx_lores=false\n");
			strcat(uae_config, "gfx_linemode=double\n");
			break;
   }

   /* Always update av_info geometry */
   request_update_av_info = true;

   /* Always trigger audio and custom change */
   config_changed = 1;
   check_prefs_changed_audio();
   check_prefs_changed_custom();
   config_changed = 0;
}

static void retro_wrap_emulator(void)
{
   static char *argv[] = { "puae", RPATH };
   umain(sizeof(argv)/sizeof(*argv), argv);

   environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, 0); 

   /* We're done here */
   co_switch(mainThread);

   /* Dead emulator, 
    * but libco says not to return. */
   while(true)
   {
      LOGI("Running a dead emulator.");
      co_switch(mainThread);
   }
}

//*****************************************************************************
//*****************************************************************************
// Disk control
extern void DISK_check_change(void);
extern void disk_eject (int num);

static bool disk_set_eject_state(bool ejected)
{
	if (dc)
	{
		if(dc->eject_state == ejected)
			return true;
		else
			dc->eject_state = ejected;
		
		if (dc->eject_state)
		{
			changed_prefs.floppyslots[0].df[0] = 0;
			DISK_check_change();
			disk_eject(0);
		}
		else
		{
			if (strlen(dc->files[dc->index]) > 0)
			{
				if (file_exists(dc->files[dc->index]))
				{
					if (currprefs.nr_floppies-1 < 0 )
						currprefs.nr_floppies = 1;

					//check whether drive is enabled
					if (currprefs.floppyslots[0].dfxtype < 0)
					{
						changed_prefs.floppyslots[0].dfxtype = 0;
						DISK_check_change();
					}
					changed_prefs = currprefs;
					strcpy (changed_prefs.floppyslots[0].df,dc->files[dc->index]);
					DISK_check_change();
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
		if(index == dc->index)
			return true;

		if ((index < dc->count) && (dc->files[index]))
		{
			dc->index = index;
			printf("Disk (%d) inserted into drive DF0: %s\n", dc->index+1, dc->files[dc->index]);
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

		if(dc->files[index])
		{
			free(dc->files[index]);
			dc->files[index] = NULL;
		}

		// TODO : Handling removing of a disk image when info = NULL

		if(info != NULL)
			dc->files[index] = strdup(info->path);
	}

    return false;
}

static bool disk_add_image_index(void)
{
	if (dc)
	{
		if(dc->count <= DC_MAX_SIZE)
		{
			dc->files[dc->count] = NULL;
			dc->count++;
			return true;
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

//*****************************************************************************
//*****************************************************************************
// Init
void retro_init(void)
{
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;

	const char *system_dir = NULL;

	if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
	{
	  // if defined, use the system directory			
	  retro_system_directory=system_dir;		
	}		   

	const char *content_dir = NULL;

	if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
	{
	  // if defined, use the system directory			
	  retro_content_directory=content_dir;		
	}			

	const char *save_dir = NULL;

	if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
	{
	  // If save directory is defined use it, otherwise use system directory
	  retro_save_directory = *save_dir ? save_dir : retro_system_directory;      
	}
	else
	{
	  // make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend
	  retro_save_directory=retro_system_directory;
	}

   printf("Retro SYSTEM_DIRECTORY %s\n",retro_system_directory);
   printf("Retro SAVE_DIRECTORY %s\n",retro_save_directory);
   printf("Retro CONTENT_DIRECTORY %s\n",retro_content_directory);

 	// Disk control interface
	dc = dc_create();
	environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE, &disk_interface);

   // Savestates
   static uint32_t quirks = RETRO_SERIALIZATION_QUIRK_INCOMPLETE | RETRO_SERIALIZATION_QUIRK_MUST_INITIALIZE | RETRO_SERIALIZATION_QUIRK_CORE_VARIABLE_SIZE;
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

   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      fprintf(stderr, "[libretro-uae]: RGB565 is not supported.\n");
      exit(0);//return false;
   }

   memset(bmp, 0, sizeof(bmp));

   update_variables();

   if(!emuThread && !mainThread)
   {
      mainThread = co_active();
      emuThread = co_create(65536 * sizeof(void*), retro_wrap_emulator);
   }
}

void retro_deinit(void)
{	
   if(emuThread)
      co_delete(emuThread);
   emuThread = 0;

	// Clean the m3u storage
	if(dc)
		dc_free(dc);
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   if(port<4)
   {
      uae_devices[port]=device;
      int uae_port;
      uae_port = (port==0) ? 1 : 0;
      cd32_pad_enabled[uae_port]=0;
      switch(device)
      {
         case RETRO_DEVICE_JOYPAD:
            printf("Controller %u: RetroPad\n", (port+1));
            break;

         case RETRO_DEVICE_UAE_CD32PAD:
            printf("Controller %u: CD32 Pad\n", (port+1));
            cd32_pad_enabled[uae_port]=1;
            break;

         case RETRO_DEVICE_UAE_JOYSTICK:
            printf("Controller %u: Joystick\n", (port+1));
            break;

         case RETRO_DEVICE_UAE_KEYBOARD:
            printf("Controller %u: Keyboard\n", (port+1));
            break;

         case RETRO_DEVICE_NONE:
            printf("Controller %u: Unplugged\n", (port+1));
            break;
      }

      /* After startup input_get_default_joystick will need to be refreshed for cd32<>joystick change to work.
         Doing updateconfig straight from boot will crash, hence inputdevice_finalized */
      if(inputdevice_finalized) {
         inputdevice_updateconfig(&currprefs, &currprefs);
      }
   }
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "PUAE";
   info->library_version  = "2.6.1" GIT_VERSION;
   info->need_fullpath    = true;
   info->block_extract    = false;	
   info->valid_extensions = "adf|dms|fdi|ipf|zip|hdf|hdz|uae|m3u";
}

bool retro_update_av_info(bool change_geometry, bool change_timing, bool isntsc)
{
   request_update_av_info = false;
   float hz = currprefs.chipset_refreshrate;
   fprintf(stderr, "[libretro-uae]: Trying to update AV geometry:%d timing:%d, to: ntsc:%d hz:%2.2f, from video_config:%d, video_aspect:%d\n", change_geometry, change_timing, isntsc, hz, video_config, video_config_aspect);

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
   if(change_timing)
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
   if(video_config_old == video_config && change_timing)
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
      if(video_config_old == video_config)
      {
         fprintf(stderr, "[libretro-uae]: Already at wanted AV\n");
         return false;
      }
   }

   /* Geometry dimensions */
   switch(video_config_geometry)
   {
      case PUAE_VIDEO_PAL_OV_LO:
         retrow = 360;
         retroh = 284;
         break;
      case PUAE_VIDEO_PAL_CR_LO:
         retrow = 320;
         retroh = 256;
         break;
      case PUAE_VIDEO_NTSC_OV_LO:
         retrow = 360;
         retroh = 240;
         break;
      case PUAE_VIDEO_NTSC_CR_LO:
         retrow = 320;
         retroh = 200;
         break;
      case PUAE_VIDEO_PAL_OV_HI:
         retrow = 720;
         retroh = 568;
         break;
      case PUAE_VIDEO_PAL_CR_HI:
         retrow = 640;
         retroh = 512;
         break;
      case PUAE_VIDEO_NTSC_OV_HI:
         retrow = 720;
         retroh = 480;
         break;
      case PUAE_VIDEO_NTSC_CR_HI:
         retrow = 640;
         retroh = 400;
         break;
   }

   /* Special cropped height for Dyna Blaster (uncropped will leave some statusbar trails, but who cares since they will be cleaned up by itself */
   if(fake_ntsc && video_config & PUAE_VIDEO_CROP)
   {
      retroh = ((video_config & PUAE_VIDEO_HIRES) ? 460 : 230);
   }

   /* When the actual dimensions change and not just the view */
   if(change_timing || fake_ntsc)
   {
      defaultw = retrow;
      defaulth = retroh;
   }

   static struct retro_system_av_info new_av_info;
   new_av_info.geometry.base_width = retrow;
   new_av_info.geometry.base_height = retroh;

   if(video_config_geometry & PUAE_VIDEO_NTSC || video_config_aspect == PUAE_VIDEO_NTSC) {
      new_av_info.geometry.aspect_ratio=(float)retrow/(float)retroh * 44.0/52.0;
      hz = 60;
   } else {
      new_av_info.geometry.aspect_ratio=(float)retrow/(float)retroh;
      hz = 50;
   }

   /* Disable Hz change if not allowed */
   if(!video_config_allow_hz_change)
      change_timing = 0;

   /* Logging */
   if(change_geometry && change_timing) {
      fprintf(stderr, "[libretro-uae]: Update av_info: %dx%d %2.2fHz, video_config:%d\n", retrow, retroh, hz, video_config_geometry);
   } else if(change_geometry && !change_timing) {
      fprintf(stderr, "[libretro-uae]: Update geometry: %dx%d, video_config:%d\n", retrow, retroh, video_config_geometry);
   } else if(!change_geometry && change_timing) {
      fprintf(stderr, "[libretro-uae]: Update timing: %2.2fHz, video_config:%d\n", hz, video_config_geometry);
   }

   if(change_timing) {
      struct retro_system_av_info new_timing;
      retro_get_system_av_info(&new_timing);
      new_timing.timing.fps = hz;
      environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &new_timing);
   }

   if(change_geometry) {
      environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &new_av_info);

      /* ensure statusbar stays visible when it is at bottom */
      opt_statusbar_position = opt_statusbar_position_old;
      if(!change_timing)
         if(retroh < defaulth)
            if (opt_statusbar_position >= 0 && (defaulth - retroh) > opt_statusbar_position)
               opt_statusbar_position = defaulth - retroh;

      //printf("statusbar:%d old:%d, retroh:%d defaulth:%d\n", opt_statusbar_position, opt_statusbar_position_old, retroh, defaulth);
   }

   /* apply zoom mode if necessary */
   zoomed_height = retroh;
   switch (zoom_mode_id)
   {
      case 1:
         if (video_config & PUAE_VIDEO_HIRES)
            zoomed_height = 512;
         else
            zoomed_height = 256;
         break;
      case 2:
         if (video_config & PUAE_VIDEO_HIRES)
            zoomed_height = 480;
         else
            zoomed_height = 240;
         break;
      case 3:
         if (video_config & PUAE_VIDEO_HIRES)
            zoomed_height = 448;
         else
            zoomed_height = 224;
         break;
      case 4:
         if (video_config & PUAE_VIDEO_HIRES)
            zoomed_height = 432;
         else
            zoomed_height = 216;
         break;
      case 5:
         if (video_config & PUAE_VIDEO_HIRES)
            zoomed_height = 400;
         else
            zoomed_height = 200;
         break;
      default:
         break;
   }

   if (zoomed_height > retroh)
      zoomed_height = retroh;

   if (zoomed_height != retroh)
   {
      new_av_info.geometry.base_height = zoomed_height;
      if(video_config_geometry & PUAE_VIDEO_NTSC || video_config_aspect == PUAE_VIDEO_NTSC) {
         new_av_info.geometry.aspect_ratio=(float)retrow/(float)zoomed_height * 44.0/52.0;
      } else {
         new_av_info.geometry.aspect_ratio=(float)retrow/(float)zoomed_height;
      }
      environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &new_av_info);

      /* ensure statusbar stays visible when it is at bottom */
      if (opt_statusbar_position >= 0 && (retroh - zoomed_height) > opt_statusbar_position)
         opt_statusbar_position = retroh - zoomed_height;
   }

   /* No need to check changed gfx at startup */
   if(firstpass != 1) {
      prefs_changed = 1; // check_prefs_changed_gfx() will be called automatically in vsync_handle_check()
   }

   return true;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   /* need to do this here because core option values are not available in retro_init */
   if (pix_bytes == 4)
   {
      enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
      if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
      {
         fprintf(stderr, "[libretro-uae]: XRGB8888 is not supported. Trying RGB565\n");
         fmt = RETRO_PIXEL_FORMAT_RGB565;
         pix_bytes = 2;
         if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
         {
            fprintf(stderr, "[libretro-uae]: RGB565 is not supported.\n");
            exit(0);//return false;
         }
      }
   }

   static struct retro_game_geometry geom;
   geom.base_width=retrow;
   geom.base_height=retroh;
   geom.max_width=EMULATOR_MAX_WIDTH;
   geom.max_height=EMULATOR_MAX_HEIGHT;

   if (retro_get_region() == RETRO_REGION_NTSC)
      geom.aspect_ratio=(float)retrow/(float)retroh * 44.0/52.0;
   else
      geom.aspect_ratio=(float)retrow/(float)retroh;
   info->geometry = geom;

   info->timing.sample_rate = 44100.0;
   info->timing.fps = (retro_get_region() == RETRO_REGION_NTSC) ? 60 : 50;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_shutdown_uae(void)
{
   environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
}

void retro_reset(void)
{
   uae_reset(1, 1); /* hardreset, keyboardreset */
   fake_ntsc=false;
}

void retro_audio_cb( short l, short r)
{
   audio_cb(l,r);
}

void retro_run(void)
{
   bool updated = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();

   bool fast_forward = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_FASTFORWARDING, &fast_forward) && fast_forward)
   {
      fast_forward_is_on = true;
      if(currprefs.sound_auto == 0)
      {
         changed_prefs.sound_auto = 1;
         config_changed = 1;
         check_prefs_changed_audio();
         config_changed = 0;
      }
   }
   else
   {
      fast_forward_is_on = false;
      if(currprefs.sound_auto == 1)
      {
         changed_prefs.sound_auto = 0;
         config_changed = 1;
         check_prefs_changed_audio();
         config_changed = 0;
      }
   }

   if (request_update_av_info)
      retro_update_av_info(1, 0, 0);

   if (minfirstline_update_frame_timer > 0 && opt_vertical_offset != 0)
   {
      minfirstline_update_frame_timer--;
      //printf("minfirstline frame:%d\n", minfirstline_update_frame_timer);
      if (minfirstline_update_frame_timer == 0)
         minfirstline = 26 + opt_vertical_offset;
   }

   if (firstpass)
   {
      firstpass=0;
      goto sortie;
   }



   retro_poll_event();
   if (STATUSON==1) Print_Status();

sortie:

   video_cb(bmp,retrow,zoomed_height , retrow << (pix_bytes / 2));

   co_switch(emuThread);
}

#define ADF_FILE_EXT "adf"
#define FDI_FILE_EXT "fdi"
#define DMS_FILE_EXT "dms"
#define IPF_FILE_EXT "ipf"
#define ZIP_FILE_EXT "zip"
#define HDF_FILE_EXT "hdf"
#define HDZ_FILE_EXT "hdz"
#define UAE_FILE_EXT "uae"
#define M3U_FILE_EXT "m3u"
#define LIBRETRO_PUAE_CONF "puae_libretro.uae"
#define WHDLOAD_HDF "WHDLoad.hdf"

bool retro_load_game(const struct retro_game_info *info)
{
  int w = 0, h = 0;

  RPATH[0] = '\0';
  
  if (info)
  {
	  const char *full_path = (const char*)info->path;
			
	  // If argument is a disk or hard drive image file
	  if(		strendswith(full_path, ADF_FILE_EXT)
			||	strendswith(full_path, FDI_FILE_EXT)
			||	strendswith(full_path, DMS_FILE_EXT)
			||	strendswith(full_path, IPF_FILE_EXT)
			||	strendswith(full_path, ZIP_FILE_EXT)
			||	strendswith(full_path, HDF_FILE_EXT)
			||	strendswith(full_path, HDZ_FILE_EXT)
			||	strendswith(full_path, M3U_FILE_EXT))
	  {
			printf("Game '%s' is a disk, a hard drive image or a m3u file.\n", full_path);
			
			path_join((char*)&RPATH, retro_save_directory, LIBRETRO_PUAE_CONF);
			printf("Generating temporary uae config file '%s'.\n", (const char*)&RPATH);

			// Open tmp config file
			FILE * configfile;
			if((configfile = fopen(RPATH, "w")))
			{
				char kickstart[RETRO_PATH_MAX];

				// If a machine was specified in the name of the game
				if((strstr(full_path, "(A1200OG)") != NULL) || (strstr(full_path, "(A1200NF)") != NULL))
				{
					// Use A1200 barebone
					printf("Found '(A1200OG)' or '(A1200NF)' in filename '%s'. Booting A1200 NoFast with Kickstart 3.1 r40.068 rom.\n", full_path);
					fprintf(configfile, A1200OG);
					path_join((char*)&kickstart, retro_system_directory, A1200_ROM);
				}
				else if((strstr(full_path, "(A1200)") != NULL) || (strstr(full_path, "(AGA)") != NULL))
				{
					// Use A1200
					printf("Found '(A1200)' or '(AGA)' in filename '%s'. Booting A1200 with Kickstart 3.1 r40.068 rom.\n", full_path);
					fprintf(configfile, A1200);
					path_join((char*)&kickstart, retro_system_directory, A1200_ROM);
				}
				else if((strstr(full_path, "(A600)") != NULL) || (strstr(full_path, "(ECS)") != NULL))
				{
					// Use A600
					printf("Found '(A600)' or '(ECS)' in filename '%s'. Booting A600 with Kickstart 3.1 r40.063 rom.\n", full_path);
					fprintf(configfile, A600);
					path_join((char*)&kickstart, retro_system_directory, A600_ROM);
				}
				else if((strstr(full_path, "(A500+)") != NULL) || (strstr(full_path, "(A500PLUS)") != NULL))
				{
					// Use A500+
					printf("Found '(A500+)' or '(A500PLUS)' in filename '%s'. Booting A500+ with Kickstart 2.04 r37.175.\n", full_path);
					fprintf(configfile, A500PLUS);
					path_join((char*)&kickstart, retro_system_directory, A500KS2_ROM);
				}
				else if((strstr(full_path, "(A500OG)") != NULL) || (strstr(full_path, "(512K)") != NULL))
				{
					// Use A500 barebone
					printf("Found '(A500OG)' or '(512K)' in filename '%s'. Booting A500 512K with Kickstart 1.3 r34.005.\n", full_path);
					fprintf(configfile, A500OG);
					path_join((char*)&kickstart, retro_system_directory, A500_ROM);
				}
				else if((strstr(full_path, "(A500)") != NULL) || (strstr(full_path, "(OCS)") != NULL))
				{
					// Use A500
					printf("Found '(A500)' or '(OCS)' in filename '%s'. Booting A500 with Kickstart 1.3 r34.005.\n", full_path);
					fprintf(configfile, A500);
					path_join((char*)&kickstart, retro_system_directory, A500_ROM);
				}
				else
				{
					// No machine specified, we will use the configured one
					printf("No machine specified in filename '%s'. Booting default configuration.\n", full_path);
					fprintf(configfile, uae_machine);
					path_join((char*)&kickstart, retro_system_directory, uae_kickstart);
				}

				// Write common config
				fprintf(configfile, uae_config);

				// If region was specified in the name of the game
				if(strstr(full_path, "(NTSC)") != NULL)
				{
				   printf("Found '(NTSC)' in filename '%s'\n", full_path);
				   fprintf(configfile, "ntsc=true\n");
				}
				else if(strstr(full_path, "(PAL)") != NULL)
				{
				   printf("Found '(PAL)' in filename '%s'\n", full_path);
				   fprintf(configfile, "ntsc=false\n");
				}
				
				// Verify kickstart
				if(!file_exists(kickstart))
				{
					// Kickstart rom not found
					fprintf(stderr, "Kickstart rom '%s' not found.\n", (const char*)&kickstart);
					fprintf(stderr, "You must have a correct kickstart file ('%s') in your RetroArch system directory.\n", kickstart);
					fclose(configfile);
					return false;	  
				}

				fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

				// If argument is a hard drive image file
				if(		strendswith(full_path, HDF_FILE_EXT)
					||	strendswith(full_path, HDZ_FILE_EXT))
				{
					if (opt_use_whdload_hdf)
					{
						// Init WHDLoad
						char whdload[RETRO_PATH_MAX];
						path_join((char*)&whdload, retro_system_directory, WHDLOAD_HDF);

						// Verify WHDLoad
						if(file_exists(whdload))
							fprintf(configfile, "hardfile=read-write,32,1,2,512,%s\n", (const char*)&whdload);
						else
							fprintf(stderr, "WHDLoad image file '%s' not found.\n", (const char*)&whdload);
					}
					fprintf(configfile, "hardfile=read-write,32,1,2,512,%s\n", full_path);
				}
				else
				{
					// If argument is a m3u playlist
					if(strendswith(full_path, M3U_FILE_EXT))
					{
						// Parse the m3u file
						dc_parse_m3u(dc, full_path);

						// Some debugging
						printf("m3u file parsed, %d file(s) found\n", dc->count);
						for(unsigned i = 0; i < dc->count; i++)
						{
							printf("file %d: %s\n", i+1, dc->files[i]);
						}						
					}
					else
					{
						// Add the file to disk control context
						// Maybe, in a later version of retroarch, we could add disk on the fly (didn't find how to do this)
						dc_add_file(dc, full_path);
					}

					// Init first disk
					dc->index = 0;
					dc->eject_state = false;
					printf("Disk (%d) inserted into drive DF0: %s\n", dc->index+1, dc->files[dc->index]);
					fprintf(configfile, "floppy0=%s\n", dc->files[0]);

					// Append rest of the disks to the config if m3u is a MultiDrive-m3u
					if(strstr(full_path, "(MD)") != NULL)
					{
					    for(unsigned i = 1; i < dc->count; i++)
					    {
					        dc->index = i;
					        if(i <= 3)
					        {
					            printf("Disk (%d) inserted into drive DF%d: %s\n", dc->index+1, i, dc->files[dc->index]);
					            fprintf(configfile, "floppy%d=%s\n", i, dc->files[i]);

					            // By default only DF0: is enabled, so floppyXtype needs to be set on the extra drives
					            if(i > 0)
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
				fprintf(stderr, "Error while writing '%s' file.\n", (const char*)&RPATH);
				return false;	  
			}	  
	  }
	  // If argument is an uae file
	  else if(strendswith(full_path, UAE_FILE_EXT))
	  {
			printf("Game '%s' is an UAE config file.\n", full_path);

			// Prepend default config
			path_join((char*)&RPATH, retro_save_directory, LIBRETRO_PUAE_CONF);
			printf("Generating temporary uae config file '%s'.\n", (const char*)&RPATH);

			// Open tmp config file
			FILE * configfile;

			if((configfile = fopen(RPATH, "w")))
			{
                char kickstart[RETRO_PATH_MAX];

                // No machine specified, we will use the configured one
                //printf("No machine specified in filename '%s'. Booting default configuration.\n", full_path);
                fprintf(configfile, uae_machine);
                path_join((char*)&kickstart, retro_system_directory, uae_kickstart);

                // Write common config
                fprintf(configfile, uae_config);

				// Verify kickstart
				//if(!file_exists(kickstart))
				//{
				//	// Kickstart rom not found
				//	fprintf(stderr, "Kickstart rom '%s' not found.\n", (const char*)&kickstart);
				//	fprintf(stderr, "You must have a correct kickstart file ('%s') in your RetroArch system directory.\n", kickstart);
				//	fclose(configfile);
				//	return false;
				//}

				fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

                //strncpy(RPATH, full_path, sizeof(RPATH));

                // Iterate parsed file and append all rows to the temporary config
                FILE * configfile_custom;

                char filebuf[4096];
                if((configfile_custom = fopen (full_path, "r")))
                {
                    while(fgets(filebuf, sizeof(filebuf), configfile_custom))
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
				fprintf(stderr, "Error while writing '%s' file.\n", (const char*)&RPATH);
				return false;
			}
	  }
	  // Other extensions
	  else
	  {
			// Unsupported file format
			fprintf(stderr, "Content '%s'. Unsupported file format.\n", full_path);
			return false;	  
	  }
   }
   // Empty content
   else
   {
         path_join((char*)&RPATH, retro_save_directory, LIBRETRO_PUAE_CONF);
         printf("Generating temporary uae config file '%s'.\n", (const char*)&RPATH);

         // Open tmp config file
         FILE * configfile;
         if((configfile = fopen(RPATH, "w")))
         {
             char kickstart[RETRO_PATH_MAX];

             // No machine specified we will use the configured one
             printf("No machine specified. Booting default configuration.\n");
             fprintf(configfile, uae_machine);
             path_join((char*)&kickstart, retro_system_directory, uae_kickstart);

             // Write common config
             fprintf(configfile, uae_config);

             // Verify kickstart
             if(!file_exists(kickstart))
             {
                 // Kickstart rom not found
                 fprintf(stderr, "Kickstart rom '%s' not found.\n", (const char*)&kickstart);
                 fprintf(stderr, "You must have a correct kickstart file ('%s') in your RetroArch system directory.\n", kickstart);
                 fclose(configfile);
                 return false;
             }

             fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);
             fclose(configfile);
         }
  }
  
  if (w<=0 || h<=0 || w>EMULATOR_MAX_WIDTH || h>EMULATOR_MAX_HEIGHT) 
  {
    w = defaultw;
    h = defaulth;
  }

  fprintf(stderr, "[libretro-uae]: resolution selected: %dx%d (default: %dx%d)\n", w, h, defaultw, defaulth);

  retrow = w;
  retroh = h;
  memset(bmp, 0, sizeof(bmp));
  Screen_SetFullUpdate();
  return true;
}

void retro_unload_game(void)
{
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
   if (firstpass != 1)
   {
      snprintf(savestate_fname, sizeof(savestate_fname), "%s%suae_tempsave.uss", retro_save_directory, DIR_SEP_STR);
      if (save_state(savestate_fname, "retro") >= 0)
      {
         FILE *file = fopen(savestate_fname, "rb");
         if (file)
         {
            size_t size = 0;
            fseek(file, 0L, SEEK_END);
            size = ftell(file);
            fclose(file);
            return size;
         }
      }
   }
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   if (firstpass != 1)
   {
      snprintf(savestate_fname, sizeof(savestate_fname), "%s%suae_tempsave.uss", retro_save_directory, DIR_SEP_STR);
      if (save_state(savestate_fname, "retro") >= 0)
      {
         FILE *file = fopen(savestate_fname, "rb");
         if (file)
         {
            if (fread(data_, size, 1, file) == 1)
            {
               fclose(file);
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
   if (firstpass != 1)
   {
      snprintf(savestate_fname, sizeof(savestate_fname), "%s%suae_tempsave.uss", retro_save_directory, DIR_SEP_STR);
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

#ifdef ANDROID
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
