#include "libretro.h"
#include "gui-retro/dialog.h"
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

#define EMULATOR_DEF_WIDTH 640
#define EMULATOR_DEF_HEIGHT 400
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
int CROP_WIDTH;
int CROP_HEIGHT;
int sndbufpos=0;
char key_state[512];
char key_state2[512];
bool opt_use_whdload_hdf = true;
static int firstps = 0;

#if defined(NATMEM_OFFSET)
extern uae_u8 *natmem_offset;
extern uae_u32 natmem_size;
#endif
extern unsigned short int  bmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern unsigned short int  savebmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern int pauseg;
extern int SHIFTON;
extern int STATUSON;
extern int snd_sampler;
extern short signed int SNDBUF[1024*2];
extern char RPATH[512];
extern void Print_Status(void);
int ledtype=1;
unsigned int video_config = 0;

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
unsigned uae_devices[4];
extern int cd32_pad_enabled[NORMAL_JPORTS];

int mapper_keys[30]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static char buf[64][4096] = { 0 };

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

#define A500 "\
cpu_type=68000\n\
chipmem_size=1\n\
bogomem_size=2\n\
chipset=ocs\n"

#define A600 "\
cpu_type=68000\n\
chipmem_size=4\n\
fastmem_size=8\n\
chipset=ecs\n"

#define A1200 "\
cpu_type=68ec020\n\
chipmem_size=4\n\
fastmem_size=8\n\
chipset=aga\n"

// Amiga default kickstarts

#define A500_ROM 	"kick34005.A500"
#define A600_ROM 	"kick40063.A600"
#define A1200_ROM 	"kick40068.A1200"

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
            { "A500", NULL },
            { "A600", NULL },
            { "A1200", NULL },
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
         "puae_video_hires",
         "High resolution",
         "Needs restart",
         {
            { "true", NULL },
            { "false", NULL },
            { NULL, NULL },
         },
         "true"
      },
      {
         "puae_video_crop_overscan",
         "Crop overscan",
         "Needs restart",
         {
            { "false", NULL },
            { "true", NULL },
            { NULL, NULL },
         },
         "false"
      },
      {
         "puae_cpu_speed",
         "CPU speed",
         "Needs restart",
         {
            { "real", NULL },
            { "max", NULL },
            { NULL, NULL },
         },
         "real"
      },
      {
         "puae_cpu_compatible",
         "CPU compatible",
         "Needs restart",
         {
            { "true", NULL },
            { "false", NULL },
            { NULL, NULL },
         },
         "true"
      },
      {
         "puae_cycle_exact",
         "CPU cycle exact",
         "Needs restart, A500 only",
         {
            { "false", NULL },
            { "true", NULL },
            { NULL, NULL },
         },
         "false"
      },
      {
         "puae_sound_output",
         "Sound output",
         "",
         {
            { "normal", NULL },
            { "exact", NULL },
            { "interrupts", NULL },
            { "none", NULL },
            { NULL, NULL },
         },
         "normal"
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
            { "none", NULL },
            { "anti", NULL },
            { "sinc", NULL },
            { "rh", NULL },
            { "crux", NULL },
            { NULL, NULL },
         },
         "none"
      },
      {
         "puae_sound_filter",
         "Sound filter",
         "",
         {
            { "emulated", NULL },
            { "off", NULL },
            { "on", NULL },
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
         "Needs restart",
         {
            { "100", "1x" },
            { "200", "2x" },
            { "400", "4x" },
            { "800", "8x" },
            { "0", "turbo" },
            { NULL, NULL },
         },
         "100"
      },
      {
         "puae_floppy_sound",
         "Floppy sound emulation",
         "Needs restart and external files in system/uae_data/",
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
         "puae_immediate_blits",
         "Immediate blits",
         "Needs restart",
         {
            { "false", NULL },
            { "true", NULL },
            { NULL, NULL },
         },
         "false"
      },
      {
         "puae_gfx_center_vertical",
         "Vertical centering",
         "Needs restart",
         {
            { "simple", NULL },
            { "smart", NULL },
            { "none", NULL },
            { NULL, NULL },
         },
         "simple"
      },
      {
         "puae_gfx_center_horizontal",
         "Horizontal centering",
         "Needs restart",
         {
            { "simple", NULL },
            { "smart", NULL },
            { "none", NULL },
            { NULL, NULL },
         },
         "simple"
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
         "puae_mapper_b",
         "RetroPad B",
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
         "RETROK_END"
      },
      {
         "puae_mapper_mouse_speed",
         "Hotkey: Change mouse speed",
         "Pressing a button mapped to this key alters the mouse speed",
         {{ NULL, NULL }},
         "---"
      },
      {
         "puae_mapper_gui",
         "Hotkey: Enter GUI",
         "",
         {{ NULL, NULL }},
         "RETROK_HOME"
      },

      { NULL, NULL, NULL, {{0}}, NULL },
   };

   /* fill in the values for all the mappers */
   int i = 0;
   int j = 0;
   while(core_options[i].key)
   {
      if (strstr(core_options[i].key, "puae_mapper_"))
      {
         j = 0;
         while(keyDesc[j] && j < RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
         {
            core_options[i].values[j].value = keyDesc[j];
            core_options[i].values[j].label = NULL;
            ++j;
         };
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
   }

   var.key = "puae_video_standard";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if(strcmp(var.value, "PAL") == 0)
      {
         video_config |= PUAE_VIDEO_PAL;
         strcat(uae_config, "ntsc=false\n");
      }
      else
      {
         video_config |= PUAE_VIDEO_NTSC;
         strcat(uae_config, "ntsc=true\n");
      }
   }

   var.key = "puae_video_hires";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
	   if(strcmp(var.value, "true") == 0)
		   video_config |= PUAE_VIDEO_HIRES;
   }

   var.key = "puae_video_crop_overscan";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
	   if(strcmp(var.value, "true") == 0)
		   video_config |= PUAE_VIDEO_CROP;
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
		char buf[50];
		snprintf(buf, 50, "%d", val);
		strcat(uae_config, "sound_stereo_separation=");
		strcat(uae_config, buf);
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
   }

   var.key = "puae_immediate_blits";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "immediate_blits=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }


   var.key = "puae_gfx_autoscale";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "gfx_autoscale=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_gfx_center_vertical";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "gfx_center_vertical=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_gfx_center_horizontal";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "gfx_center_horizontal=");
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

   var.key = "puae_use_whdload";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
        opt_use_whdload_hdf = true;
      if (strcmp(var.value, "disabled") == 0)
        opt_use_whdload_hdf = false;
   }

   var.key = "puae_mapper_select";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[2] = keyId(var.value);
   }

   var.key = "puae_mapper_start";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[3] = keyId(var.value);
   }

   var.key = "puae_mapper_y";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[1] = keyId(var.value);
   }

   var.key = "puae_mapper_x";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[9] = keyId(var.value);
   }

   var.key = "puae_mapper_b";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[0] = keyId(var.value);
   }

   var.key = "puae_mapper_l";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[10] = keyId(var.value);
   }

   var.key = "puae_mapper_r";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[11] = keyId(var.value);
   }

   var.key = "puae_mapper_l2";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[12] = keyId(var.value);
   }

   var.key = "puae_mapper_r2";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[13] = keyId(var.value);
   }

   var.key = "puae_mapper_l3";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[14] = keyId(var.value);
   }

   var.key = "puae_mapper_r3";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[15] = keyId(var.value);
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

   var.key = "puae_mapper_gui";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      mapper_keys[28] = keyId(var.value);
   }



    /* Always trigger audio change */
    config_changed = 1;
    check_prefs_changed_audio();

    /* OSD defaults to top right */
    strcat(uae_config, "osd_position=0:-1\n");
   
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
}

static void retro_wrap_emulator(void)
{
   static char *argv[] = { "puae", RPATH };
   umain(sizeof(argv)/sizeof(*argv), argv);

   pauseg = -1;

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
extern const char* Floppy_SetDiskFileNameNone(int Drive);
extern const char* Floppy_SetDiskFileName(int Drive, const char *pszFileName, const char *pszZipPath);
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
			Floppy_SetDiskFileNameNone(0);
			changed_prefs.floppyslots[0].df[0] = 0;
			DISK_check_change();
			disk_eject(0);
		}
		else
		{
			if (strlen(dc->files[dc->index]) > 0)
			{
				if (File_Exists(dc->files[dc->index]))
				{
					// This sets szDiskFileName[0]
					Floppy_SetDiskFileName(0, dc->files[dc->index], 0);

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

	// Inputs
#define RETRO_DESCRIPTOR_BLOCK( _user )                                            \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "A" },               \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "B" },               \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "X" },               \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Y" },               \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },     \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },       \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },       \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },         \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },             \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },         \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "R" },               \
   { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "L" },               \
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

   InitOSGLU();
   memset(bmp, 0, sizeof(bmp));

   update_variables();

   CROP_WIDTH    = retrow;
   CROP_HEIGHT   = (retroh - 80);

   if(!emuThread && !mainThread)
   {
      mainThread = co_active();
      emuThread = co_create(65536 * sizeof(void*), retro_wrap_emulator);
   }
}

void retro_deinit(void)
{	
   UnInitOSGLU();	

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

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   static struct retro_game_geometry geom480   = { 640, 480, EMULATOR_MAX_WIDTH, EMULATOR_MAX_HEIGHT, 4.0 / 3.0 };
   static struct retro_game_geometry geom540   = { 720, 540, EMULATOR_MAX_WIDTH, EMULATOR_MAX_HEIGHT, 4.0 / 3.0 };

   if      (retrow == 640 && retroh == 400) info->geometry = geom480;
   else if (retrow == 640 && retroh == 400) info->geometry = geom480;
   else if (retrow == 720 && retroh == 540) info->geometry = geom540;
   else { static struct retro_game_geometry geom; geom.base_width=retrow; geom.base_height=retroh; geom.max_width=EMULATOR_MAX_WIDTH; geom.max_height=EMULATOR_MAX_HEIGHT; geom.aspect_ratio=(float)retrow/(float)retroh; info->geometry = geom; }

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
}

void retro_audio_cb( short l, short r)
{
   audio_cb(l,r);
}

extern unsigned short * sndbuffer;
extern int sndbufsize;
signed short rsnd=0;

static int firstpass = 1;

void save_bkg(void)
{
   memcpy(savebmp, bmp,sizeof(bmp));
}

void restore_bkg(void)
{
   memcpy(bmp,savebmp,sizeof(bmp));
}

void enter_gui0(void)
{
   save_bkg();

   Dialog_DoProperty();
   pauseg = 0;

   restore_bkg();
}

void pause_select(void)
{
   if(pauseg==1 && firstps==0)
   {
      firstps=1;
      enter_gui0();
      firstps=0;
   }
}

void retro_run(void)
{
   bool updated = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();

   if(pauseg==0)
   {
      if(firstpass)
      {
         firstpass=0;
         goto sortie;
      }
      retro_poll_event();
      if(STATUSON==1) Print_Status();
   }

sortie:

   video_cb(bmp,retrow,retroh , retrow << 1);

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
				if((strstr(full_path, "(A500)") != NULL) || (strstr(full_path, "(OCS)") != NULL))
				{
					// Use A500
					printf("Found '(A500)' or '(OCS)' in filename '%s'. We will use a A500 with kickstart 1.3 r34.005 rom configuration.\n", full_path);
					fprintf(configfile, A500);
					path_join((char*)&kickstart, retro_system_directory, A500_ROM);
				}
				else if((strstr(full_path, "(A600)") != NULL) || (strstr(full_path, "(ECS)") != NULL))
				{
					// Use A600
					printf("Found '(A600)' or '(ECS)' in filename '%s'. We will use a A600 with kickstart 3.1 r40.063 rom configuration.\n", full_path);
					fprintf(configfile, A600);
					path_join((char*)&kickstart, retro_system_directory, A600_ROM);
				}
				else if((strstr(full_path, "(A1200)") != NULL) || (strstr(full_path, "(AGA)") != NULL))
				{
					// Use A1200
					printf("Found '(A1200)' or '(AGA)' in filename '%s'. We will use a A1200 with kickstart 3.1 r40.068 rom configuration.\n", full_path);
					fprintf(configfile, A1200);
					path_join((char*)&kickstart, retro_system_directory, A1200_ROM);
				}
				else
				{
					// No machine specified, we will use the configured one
					printf("No machine specified in filename '%s'. We will use the default configuration.\n", full_path);
					fprintf(configfile, uae_machine);
					path_join((char*)&kickstart, retro_system_directory, uae_kickstart);
				}

				// Write common config
				fprintf(configfile, uae_config);
				
				// FIXME : Bug if show_leds is set to false
				// If this parameter is set to false or not specified (default false) Rick Dangerous 2 (adf or whd version) hang when selecting level.
				// But no impact since the parameter in RetroArch configuration overload this setting : If Leds is set to none, the led won't be drawn on screen and it works...
				fprintf(configfile, "show_leds=true\n");
								
				// Verifiy kickstart
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

						// Verifiy WHDLoad
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

					            // By default only 2 drives are enabled, so floppyXtype needs to be set on the extra drives
					            if(i > 1)
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
		  
			strncpy(RPATH, full_path, sizeof(RPATH));

			// checking parsed file for custom resolution
			FILE * configfile;

			char filebuf[4096];
			if((configfile = fopen (RPATH, "r")))
			{
				while(fgets(filebuf, sizeof(filebuf), configfile))
				{
				  sscanf(filebuf,"gfx_width = %d",&w);
				  sscanf(filebuf,"gfx_height = %d",&h);
				}
				fclose(configfile);
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
             printf("No machine specified. We will use the default configuration.\n");
             fprintf(configfile, uae_machine);
             path_join((char*)&kickstart, retro_system_directory, uae_kickstart);

             // Write common config
             fprintf(configfile, uae_config);

             // FIXME : Bug if show_leds is set to false
             // If this parameter is set to false or not specified (default false) Rick Dangerous 2 (adf or whd version) hang when selecting level.
             // But no impact since the parameter in RetroArch configuration overload this setting : If Leds is set to none, the led won't be drawn on screen and it works...
             fprintf(configfile, "show_leds=true\n");

             // Verifiy kickstart
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
  CROP_WIDTH = retrow;
  CROP_HEIGHT = (retroh-80);
  memset(bmp, 0, sizeof(bmp));
  Screen_SetFullUpdate();
  return true;
}

void retro_unload_game(void)
{
   pauseg = 0;
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
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
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
