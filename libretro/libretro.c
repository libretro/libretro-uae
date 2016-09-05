#include "libretro.h"
#include "gui-retro/dialog.h"
#include "retrodep/retroglue.h"
#include "libretro-mapper.h"
#include "libretro-glue.h"


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


void retro_set_environment(retro_environment_t cb)
{
   struct retro_variable variables[] = {
     { "resolution", "Internal resolution; 640x400|640x432|640x480|640x540|704x480|704x540|720x480|720x540|800x600|1024x768", },
     { "analog","Use Analog; OFF|ON", },
     { "leds","Leds; Standard|Simplified|None", },
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
   struct retro_variable var = {0};

   var.key = "resolution";
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

   var.key = "analog";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "OFF") == 0)
        opt_analog = false;
      if (strcmp(var.value, "ON") == 0)
        opt_analog = true;
      ledtype = 1;
   }
   
   var.key = "leds";
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
   info->valid_extensions = "adf|dms|fdi|ipf|zip|uae";
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

bool retro_load_game(const struct retro_game_info *info)
{
  int w = 0, h = 0;

  environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_descriptors);

  RPATH[0] = '\0';

  if (*info->path)
    {
      const char *full_path = (const char*)info->path;
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

  if (w<=0 || h<=0 || w>EMULATOR_MAX_WIDTH || h>EMULATOR_MAX_HEIGHT) {
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
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void) {}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}
