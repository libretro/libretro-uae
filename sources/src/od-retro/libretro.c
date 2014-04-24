#include "libretro.h"
#include "libretro-euae.h"

cothread_t mainThread;
cothread_t emuThread;

int retrow=640; 
int retroh=480;
int CROP_WIDTH;
int CROP_HEIGHT;
int VIRTUAL_WIDTH;
int sndbufpos=0;
char Key_Sate[512];
char Key_Sate2[512];
bool opt_analog=false;
static int firstps=0;

extern unsigned short int  bmp[1024*1024];
extern unsigned short int  savebmp[1024*1024];
extern int pauseg;
extern int SND;
extern int SHIFTON;
extern int snd_sampler;
extern short signed int SNDBUF[1024*2];
extern char RPATH[512];

extern void update_input(void);

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   struct retro_variable variables[] = {
      { "resolution","Internal resolution; 640x480|720x480|800x600|1024x768", },
      { "analog","Use Analog; OFF|ON", },
      { NULL, NULL },
   };

   bool no_rom = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);
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
         retrow = strtoul(pch, NULL, 0);
      pch = strtok(NULL, "x");
      if (pch)
         retroh = strtoul(pch, NULL, 0);

      fprintf(stderr, "[libretro-test]: Got size: %u x %u.\n", retrow, retroh);

      CROP_WIDTH =retrow;
      CROP_HEIGHT= (retroh-80);
      VIRTUAL_WIDTH = retrow;
      memset(bmp, 0, sizeof(bmp));
      Screen_SetFullUpdate();
   }

   var.key = "analog";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "OFF") == 0)
         opt_analog = false;
      if (strcmp(var.value, "ON") == 0)
         opt_analog = true;

      fprintf(stderr, "[libretro-test]: Analog: %s.\n",opt_analog?"ON":"OFF");
   }
}

static void retro_wrap_emulator(void)
{
   char **argv2 = (char *[]){"puae\0", "\0"};
   umain(1,argv2);

   pauseg=-1;

   environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, 0); 

   // Were done here
   co_switch(mainThread);

   // Dead emulator, but libco says not to return
   while(true)
   {
      LOGI("Running a dead emulator.");
      co_switch(mainThread);
   }
}

void retro_init(void)
{

   memset(Key_Sate,0,512);
   memset(Key_Sate2,0,512);

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      fprintf(stderr, "RGB565 is not supported.\n");
      exit(0);//return false;
   }
   InitOSGLU();
   memset(bmp, 0, sizeof(bmp));

   update_variables();

   retrow=640; 
   retroh=480;
   CROP_WIDTH =retrow;
   CROP_HEIGHT= (retroh-80);
   VIRTUAL_WIDTH = retrow;

   if(!emuThread && !mainThread)
   {
      mainThread = co_active();
      emuThread = co_create(/*1048576*sizeof(void*)*/65536*sizeof(void*), retro_wrap_emulator);
   }
}

void retro_deinit(void)
{	

   UnInitOSGLU();	

   if(emuThread)
      co_delete(emuThread);
   emuThread = 0;

   LOGI("Retro DeInit\n");
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
   info->block_extract = false;	
   info->valid_extensions = "adf|dms|zip";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   struct retro_game_geometry geom = { 640,480,1024,1024,4.0 / 3.0 };
   struct retro_system_timing timing = { 60.0, 22050.0 };

   info->geometry = geom;
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

static firstpass=1;

int save_bkg(void)
{
   memcpy(savebmp, bmp,sizeof(bmp));
}
int restore_bkg(void)
{
   memcpy(bmp,savebmp,sizeof(bmp));
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
void enter_gui0(void)
{
   save_bkg();

   Dialog_DoProperty();
   pauseg=0;

   restore_bkg();
}

void retro_run(void)
{
   int x;

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
   if (*info->path)
   {
      const char *full_path = (const char*)info->path;
      strcpy(RPATH,full_path);
   }

   return true;
}

void retro_unload_game(void)
{
   pauseg=0;
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
