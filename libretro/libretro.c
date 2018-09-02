#include "libretro.h"
#include "gui-retro/dialog.h"
#include "retrodep/retroglue.h"
#include "libretro-mapper.h"
#include "libretro-glue.h"
#include "retro_files.h"
#include "retro_strings.h"
#include "sources/src/include/uae_types.h"

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
bool opt_analog = false;
static int firstps = 0;

#if defined(NATMEM_OFFSET)
extern uae_u8 *natmem_offset;
extern uae_u32 natmem_size;
#endif
extern unsigned short int  bmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern unsigned short int  savebmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern int pauseg;
extern int SND;
extern int SHIFTON;
extern int snd_sampler;
extern short signed int SNDBUF[1024*2];
extern char RPATH[512];
int ledtype;

extern void update_input(void);

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

static struct retro_input_descriptor input_descriptors[] = {
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Fire" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Enter GUI" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Mouse mode toggle" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Keyboard overlay" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "Toggle m/k status" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "Joystick number" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "Mouse speed" },
   // Terminate
   { 255, 255, 255, 255, NULL }
};

const char *retro_save_directory;
const char *retro_system_directory;
const char *retro_content_directory;

// Amiga default models

#define A500 "\
cpu_type=68000\n\
chipmem_size=2\n\
bogomem_size=7\n\
chipset=ocs\n"

#define A600 "\
cpu_type=68000\n\
chipmem_size=2\n\
fastmem_size=4\n\
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

static char uae_machine[256];
static char uae_kickstart[16];
static char uae_config[1024];

void retro_set_environment(retro_environment_t cb)
{
	// Max resolution is 640x512 for PAL and 640x400 for NTSC (they works fine when doubled)
	// PUAE recommanded resolution is 720x568 for PAL with overscan and 720x480 for NTSC with overscan
	
   struct retro_variable variables[] = {
     { "puae_model", "Model; A600|A1200|A500", },
     { "puae_resolution", "Internal resolution; 640x512|640x400|720x568|720x480|640x432|640x480|640x540|704x480|704x540|720x540|800x600|1024x768", },
     { "puae_analog","Use Analog; OFF|ON", },
     { "puae_leds","Leds; Standard|Simplified|None", },
     { "puae_cpu_speed", "CPU speed; real|max", },
     { "puae_cpu_compatible", "CPU compatible; true|false", },
     { "puae_sound_output", "Sound output; normal|exact|interrupts|none", },
     { "puae_sound_frequency", "Sound frequency; 44100|22050|11025", },
     { "puae_sound_channels", "Sound channels; stereo|mixed|mono", },
     { "puae_sound_interpol", "Sound interpolation; none|rh|crux|sync", },
     { "puae_floppy_speed", "Floppy speed; 100|200|300|400|500|600|700|800", },
     { "puae_immediate_blits", "Immediate blit; false|true", },
     { "puae_ntsc", "NTSC Mode; false|true", },
     { "puae_gfx_linemode", "Line mode; double|none", }, // Removed scanline, we have shaders for this
     { "puae_gfx_correct_aspect", "Correct aspect ratio; true|false", },
     { "puae_gfx_center_vertical", "Vertical centering; simple|smart|none", },
     { "puae_gfx_center_horizontal", "Horizontal centering; simple|smart|none", },
     { NULL, NULL },
   };
  /*
    bool no_rom = true;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);
  */
   
   environ_cb = cb;
   cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
}

static void update_variables(void)
{
	uae_machine[0] = '\0';
	uae_config[0] = '\0';
   struct retro_variable var = {0};

   var.key = "puae_resolution";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      char *pch;
      char str[100];
      snprintf(str, sizeof(str), var.value);

      pch = strtok(str, "x");
      if (pch)
         defaultw = strtoul(pch, NULL, 0);
      pch = strtok(NULL, "x");
      if (pch)
         defaulth = strtoul(pch, NULL, 0);
   }

   var.key = "puae_analog";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "OFF") == 0)
        opt_analog = false;
      if (strcmp(var.value, "ON") == 0)
        opt_analog = true;
      ledtype = 1;
   }
   
   var.key = "puae_leds";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "Standard") == 0)
      {
        ledtype = 0;        
      }
      if (strcmp(var.value, "Simplified") == 0)
      {
        ledtype = 1;  
      }
      if (strcmp(var.value, "None") == 0)
      {
        ledtype = 2;  
      }
   }

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
   }

   var.key = "puae_sound_frequency";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "sound_frequency=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_sound_channels";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "sound_channels=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_sound_interpol";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "sound_interpol=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_floppy_speed";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "floppy_speed=");
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

   var.key = "puae_ntsc";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "ntsc=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_gfx_linemode";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "gfx_linemode=");
		strcat(uae_config, var.value);
		strcat(uae_config, "\n");
   }

   var.key = "puae_gfx_correct_aspect";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
		strcat(uae_config, "gfx_correct_aspect=");
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
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   (void)port;
   (void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "PUAE";
   info->library_version  = "v2.6.1";
   info->need_fullpath    = true;
   info->block_extract    = false;	
   info->valid_extensions = "adf|dms|fdi|ipf|zip|hdf|hdz|uae";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   static struct retro_game_geometry geom480   = { 640, 480, EMULATOR_MAX_WIDTH, EMULATOR_MAX_HEIGHT, 4.0 / 3.0 };
   static struct retro_game_geometry geom540   = { 720, 540, EMULATOR_MAX_WIDTH, EMULATOR_MAX_HEIGHT, 4.0 / 3.0 };
   struct retro_system_timing timing = { 50.0, 44100.0 };

   if      (retrow == 640 && retroh == 400) info->geometry = geom480;
   else if (retrow == 640 && retroh == 400) info->geometry = geom480;
   else if (retrow == 720 && retroh == 540) info->geometry = geom540;
   else { static struct retro_game_geometry geom; geom.base_width=retrow; geom.base_height=retroh; geom.max_width=EMULATOR_MAX_WIDTH; geom.max_height=EMULATOR_MAX_HEIGHT; geom.aspect_ratio=(float)retrow/(float)retroh; info->geometry = geom; }
   info->timing   = timing;
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

void retro_reset(void){}

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
      update_input();	
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
#define LIBRETRO_PUAE_CONF "puae_libretro.uae"
#define WHDLOAD_HDF "WHDLoad.hdf"

bool retro_load_game(const struct retro_game_info *info)
{
  int w = 0, h = 0;

  environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_descriptors);

  RPATH[0] = '\0';
  
  if (*info->path)
  {
	  const char *full_path = (const char*)info->path;
			
	  // If argument is a disk or hard drive image file file
	  if(		strendswith(full_path, ADF_FILE_EXT)
			||	strendswith(full_path, FDI_FILE_EXT)
			||	strendswith(full_path, DMS_FILE_EXT)
			||	strendswith(full_path, IPF_FILE_EXT)
			||	strendswith(full_path, ZIP_FILE_EXT)
			||	strendswith(full_path, HDF_FILE_EXT)
			||	strendswith(full_path, HDZ_FILE_EXT))
	  {
			printf("Game '%s' is a disk or hard drive image file.\n", full_path);
			
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
					// No machine specified we will use the configured one
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
					fprintf(stderr, "You must have a correct kickstart file ('%s') in your RetroArch system directory to launch an disk or hard drive image file.\n", kickstart);
					fclose(configfile);
					return false;	  
				}

				fprintf(configfile, "kickstart_rom_file=%s\n", (const char*)&kickstart);

				// If argument is a hard drive image file
				if(		strendswith(full_path, HDF_FILE_EXT)
					||	strendswith(full_path, HDZ_FILE_EXT))
				{
					// Init WHDLoad
					char whdload[RETRO_PATH_MAX];
					path_join((char*)&whdload, retro_system_directory, WHDLOAD_HDF);
					
					// Verifiy WHDLoad
					if(!file_exists(whdload))
					{
						// WHDLoad image not found
						fprintf(stderr, "WHDLoad image file '%s' not found.\n", (const char*)&whdload);
						fprintf(stderr, "You must have a WHDLoad image file ('WHDLoad.hdf') in your RetroArch system directory to launch a hard drive image file.\n");
						fclose(configfile);
						return false;	  
					}
					
					// Write hard drives informations					
					fprintf(configfile, "hardfile=read-write,32,1,2,512,%s\n", (const char*)&whdload);
					fprintf(configfile, "hardfile=read-write,32,1,2,512,%s\n", full_path);
				}
				else
				{
					// Write floppy information
					fprintf(configfile, "floppy0=%s\n", full_path);
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
   return RETRO_REGION_NTSC;
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
