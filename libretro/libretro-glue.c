#include "libretro-core.h"
#include "libretro-glue.h"
#include "libretro-graph.h"
#include "libretro-mapper.h"
#include "encodings/utf.h"
#include "streams/file_stream.h"

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"
#include "gui.h"
#include "memory.h"
#include "xwin.h"
#include "custom.h"
#include "drawing.h"
#include "hotkeys.h"
#include "disk.h"
#include "parser.h"
#include "inputdevice.h"
#include "newcpu.h"
extern int mouse_port[NORMAL_JPORTS];

int log_scsi;
int log_net;
int tablet_log;
int uaelib_debug;
bool beamracer_debug;
float vsync_vblank, vsync_hblank;
int busywait;
int vsync_activeheight, vsync_totalheight;
int max_uae_width = EMULATOR_MAX_WIDTH;
int max_uae_height = EMULATOR_MAX_HEIGHT;
int pause_emulation;
bool gfx_hdr;
addrbank *gfxmem_banks[MAX_RTG_BOARDS];
struct AmigaMonitor AMonitors[MAX_AMIGAMONITORS];
static int display_change_requested;
struct vidbuf_description *gfxvidinfo = &adisplays[0].gfxvidinfo;
struct serparportinfo *comports[MAX_SERPAR_PORTS];

extern unsigned int retro_devices[RETRO_DEVICES];
bool inputdevice_finalized = false;
extern int retro_ui_get_pointer_state(uint8_t port, int *px, int *py, uint8_t *pb);

extern unsigned short int defaultw;
extern unsigned short int defaulth;
extern unsigned char width_multiplier;
extern uint8_t libretro_frame_end;

unsigned short int* pixbuf = NULL;
extern unsigned short int retro_bmp[RETRO_BMP_SIZE];
extern char retro_temp_directory[RETRO_PATH_MAX];

int retro_thisframe_first_drawn_line;
int retro_thisframe_last_drawn_line;
int retro_min_diwstart;
int retro_max_diwstop;
extern int min_diwstart;
extern int max_diwstop;

extern int opt_statusbar;
extern int opt_statusbar_position;

unsigned int statusbar_message_timer = 0;
unsigned char statusbar_text[RETRO_PATH_MAX] = {0};
extern float retro_refresh;
extern char full_path[RETRO_PATH_MAX];

static bool flag_empty(int val[16])
{
   for (int x = 0; x < 16; x++)
   {
      if (val[x])
         return false;
   }
   return true;
}

static unsigned char* joystick_value_human(int val[16], int uae_device)
{
   /*
    * uae_device:
    * 0 = Single button joystick (Keyrah + Parallel)
    * 1 = RetroPad
    * 2 = CD32 Pad
    * 3 = Mouse
    * 4 = Analog joystick
    */

   unsigned str_len = 4;
   unsigned char *str = malloc(sizeof(char)*str_len);
   snprintf(str, sizeof(unsigned char)*str_len, "%3s", "   ");

   if (val[RETRO_DEVICE_ID_JOYPAD_UP] || val[RETRO_DEVICE_ID_JOYPAD_SELECT]) /* Unused SELECT acts as a jump button */
      str[1] = 30;

   else if (val[RETRO_DEVICE_ID_JOYPAD_DOWN])
      str[1] = 28;

   if (val[RETRO_DEVICE_ID_JOYPAD_LEFT])
      str[0] = 27;

   else if (val[RETRO_DEVICE_ID_JOYPAD_RIGHT])
      str[2] = 29;

   if (val[RETRO_DEVICE_ID_JOYPAD_B])
   {
      switch (uae_device)
      {
         case 1:
            if (opt_retropad_options == RETROPAD_OPTIONS_ROTATE || opt_retropad_options == RETROPAD_OPTIONS_ROTATE_JUMP)
               str[1] = ('2' | 0x80);
            else
               str[1] = (str[1] | 0x80);
            break;
         case 3:
            str[1] = ('L' | 0x80);
            break;
         case 4:
            str[1] = ('1' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_A])
   {
      switch (uae_device)
      {
         case 1:
            if (opt_retropad_options == RETROPAD_OPTIONS_ROTATE || opt_retropad_options == RETROPAD_OPTIONS_ROTATE_JUMP)
               ; /* no-op */
            else
               str[1] = ('2' | 0x80);
            break;
         case 3:
            str[1] = ('R' | 0x80);
            break;
         case 4:
            str[1] = ('2' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_Y])
   {
      switch (uae_device)
      {
         case 1:
            if (opt_retropad_options == RETROPAD_OPTIONS_ROTATE || opt_retropad_options == RETROPAD_OPTIONS_ROTATE_JUMP)
               str[1] = (str[1] | 0x80);
            break;
         case 3:
            str[1] = ('M' | 0x80);
            break;
         case 4:
            str[1] = ('3' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_X])
   {
      switch (uae_device)
      {
         case 1:
         case 3:
            break;
         case 4:
            str[1] = ('4' | 0x80);
            break;
         default:
            str[1] = (str[1] | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_START])
   {
      switch (uae_device)
      {
         case 2:
            str[1] = ('P' | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_L])
   {
      switch (uae_device)
      {
         case 2:
            str[0] = ('R' | 0x80);
            break;
      }
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_R])
   {
      switch (uae_device)
      {
         case 2:
            str[2] = ('F' | 0x80);
            break;
      }
   }

   return str;
}

static unsigned int joystick_color(int val[16])
{
   unsigned color = 0;

   if (opt_cd32pad_options == RETROPAD_OPTIONS_ROTATE || opt_cd32pad_options == RETROPAD_OPTIONS_ROTATE_JUMP)
   {
      if (val[RETRO_DEVICE_ID_JOYPAD_Y])
         color |= (pix_bytes == 4) ? RGB888(248,0,0) : RGB565(255,0,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_B])
         color |= (pix_bytes == 4) ? RGB888(0,0,248) : RGB565(0,0,255);

      if (val[RETRO_DEVICE_ID_JOYPAD_X])
         color |= (pix_bytes == 4) ? RGB888(0,248,0) : RGB565(0,255,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_A])
         color |= (pix_bytes == 4) ? RGB888(248,248,0) : RGB565(255,255,0);
   }
   else
   {
      if (val[RETRO_DEVICE_ID_JOYPAD_B])
         color |= (pix_bytes == 4) ? RGB888(248,0,0) : RGB565(255,0,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_A])
         color |= (pix_bytes == 4) ? RGB888(0,0,248) : RGB565(0,0,255);

      if (val[RETRO_DEVICE_ID_JOYPAD_Y])
         color |= (pix_bytes == 4) ? RGB888(0,248,0) : RGB565(0,255,0);

      if (val[RETRO_DEVICE_ID_JOYPAD_X])
         color |= (pix_bytes == 4) ? RGB888(248,248,0) : RGB565(255,255,0);
   }

   if (val[RETRO_DEVICE_ID_JOYPAD_L])
      color |= (pix_bytes == 4) ? RGB888(170,170,170) : RGB565(110,110,110);

   if (val[RETRO_DEVICE_ID_JOYPAD_R])
      color |= (pix_bytes == 4) ? RGB888(170,170,170) : RGB565(110,110,110);

   if (val[RETRO_DEVICE_ID_JOYPAD_START])
      color |= (pix_bytes == 4) ? RGB888(164,164,164) : RGB565(72,72,72);

   if (color == 0)
      color = (pix_bytes == 4) ? 0xffffff : 0xffff;

   return color;
}

void display_current_image(const char *image, bool inserted)
{
   static char imagename[RETRO_PATH_MAX] = {0};
   static char imagename_prev[RETRO_PATH_MAX] = {0};
   unsigned char* imagename_local;

   /* Skip the initial insert message with forced message mode */
   if (libretro_runloop_active || (!libretro_runloop_active && !(opt_statusbar & STATUSBAR_MESSAGES)))
      statusbar_message_timer = 2 * retro_refresh;

   if (strcmp(image, ""))
   {
      snprintf(imagename, sizeof(imagename), "%s%.98s", "  ", path_basename(image));
      snprintf(imagename_prev, sizeof(imagename_prev), "%.100s", imagename);
   }
   else
      snprintf(imagename, sizeof(imagename), "%.100s", imagename_prev);

   imagename_local = utf8_to_local_string_alloc(imagename);
   snprintf(&statusbar_text[0], sizeof(statusbar_text), "%-100s", imagename_local);

   if (inserted)
      statusbar_text[0] = (7 | 0x80);
   else if (!strcmp(image, ""))
      statusbar_text[0] = (8 | 0x80);

   free(imagename_local);
   imagename_local = NULL;
}

#include "statusline.h"
extern void draw_status_line(int monid, int line, int statusy);
static void retro_draw_frame_extras(void)
{
   struct amigadisplay *ad = &adisplays[0];
   struct vidbuf_description *vidinfo = &ad->gfxvidinfo;
   struct vidbuffer *vb = &vidinfo->drawbuffer;

   int slx, sly;
   int mult = 1;
   statusline_getpos(vb->monitor_id, &slx, &sly, vb->outwidth, vb->outheight);
   for (int i = 0; i < TD_TOTAL_HEIGHT * mult; i++) {
      int line = sly + i;
      draw_status_line(vb->monitor_id, line, i);
   }
}

void print_statusbar(void)
{
   if (opt_statusbar & STATUSBAR_BASIC && !statusbar_message_timer)
      goto end;

   int BOX_Y                = 0;
   int BOX_WIDTH            = 0;
   int BOX_HEIGHT           = 11;
   int BOX_PADDING          = 2;

   int FONT_WIDTH           = 1;
   if (video_config & PUAE_VIDEO_HIRES)
   {
      if (video_config & PUAE_VIDEO_DOUBLELINE)
         FONT_WIDTH         = 1;
      else
         FONT_WIDTH         = 2;
   }
   else if (video_config & PUAE_VIDEO_SUPERHIRES)
   {
      if (video_config & PUAE_VIDEO_DOUBLELINE)
         FONT_WIDTH         = 2;
      else
         FONT_WIDTH         = 4;
   }
   int FONT_HEIGHT          = 1;
   int FONT_COLOR           = (pix_bytes == 4) ? 0xffffff : 0xffff;;
   int FONT_SLOT            = 34 * FONT_WIDTH;

   int TEXT_X               = 1 * FONT_WIDTH;
   int TEXT_Y               = 0;
   int TEXT_LENGTH          = (video_config & PUAE_VIDEO_DOUBLELINE) ? 128 : 64;

   /* Statusbar location */
   /* Top */
   if (opt_statusbar_position < 0)
      TEXT_Y = BOX_PADDING;
   /* Bottom */
   else
      TEXT_Y = gfxvidinfo->drawbuffer.outheight - opt_statusbar_position - BOX_HEIGHT + BOX_PADDING;
   BOX_Y = TEXT_Y - BOX_PADDING;

   /* Statusbar size */
   BOX_WIDTH = retrow_crop;
   int CROP_WIDTH_OFFSET = retrow - retrow_crop;

   /* Video resolution */
   int TEXT_X_RESOLUTION = TEXT_X + (FONT_SLOT*4) + (FONT_WIDTH*16) - (CROP_WIDTH_OFFSET/2);
   unsigned char RESOLUTION[10] = {0};
   snprintf(RESOLUTION, sizeof(RESOLUTION), "%4dx%3d", retrow_crop, retroh_crop);

   /* Model & memory */
   int TEXT_X_MODEL  = TEXT_X + (FONT_SLOT*6) + (FONT_WIDTH*35) - CROP_WIDTH_OFFSET;
   int TEXT_X_MEMORY = TEXT_X + (FONT_SLOT*6) + (FONT_WIDTH*3) - CROP_WIDTH_OFFSET;
   /* Sacrifice memory slot if there is not enough width */
   if (!(video_config & PUAE_VIDEO_DOUBLELINE))
   {
      if (TEXT_X_MEMORY < (TEXT_X_RESOLUTION + FONT_SLOT + (FONT_WIDTH*14)))
         TEXT_X_MEMORY = -1;
   }

   unsigned char MODEL[10] = {0};
   unsigned char MEMORY[5] = {0};
   float mem_size = 0;
   mem_size  = (float)(currprefs.chipmem.size / 0x80000) / 2;
   mem_size += (float)(currprefs.bogomem.size / 0x40000) / 4;
   mem_size += (float)(currprefs.fastmem[0].size / 0x100000);
   mem_size += (float)(currprefs.z3fastmem[0].size / 0x100000);
   if (TEXT_X_MEMORY > 0)
      snprintf(MEMORY, sizeof(MEMORY), (mem_size < 10) ? "%3.1fM" : "%3.0fM", mem_size);

   switch (currprefs.cs_compatible)
   {
      case CP_A500:
         snprintf(MODEL, sizeof(MODEL), "%s", "A500");
         break;
      case CP_A500P:
         snprintf(MODEL, sizeof(MODEL), "%s", "A500+");
         break;
      case CP_A600:
         snprintf(MODEL, sizeof(MODEL), "%s", "A600");
         break;
      case CP_A1200:
         snprintf(MODEL, sizeof(MODEL), "%s", "A1200");
         break;
      case CP_A2000:
         snprintf(MODEL, sizeof(MODEL), "%s", "A2000");
         break;
      case CP_A4000:
         snprintf(MODEL, sizeof(MODEL), "%s", "A4000");
         break;
      case CP_CDTV:
         snprintf(MODEL, sizeof(MODEL), "%s", "CDTV");
         break;
      case CP_CD32:
         snprintf(MODEL, sizeof(MODEL), "%s", "CD32");
         break;
   }

   /* Double line positions */
   if (video_config & PUAE_VIDEO_DOUBLELINE)
   {
      TEXT_X_RESOLUTION = TEXT_X + (FONT_SLOT*9)  + (FONT_WIDTH*25) - (CROP_WIDTH_OFFSET/2);
      TEXT_X_MODEL      = TEXT_X + (FONT_SLOT*17) + (FONT_WIDTH*20) - CROP_WIDTH_OFFSET;
      TEXT_X_MEMORY     = TEXT_X + (FONT_SLOT*16) + (FONT_WIDTH*15) - CROP_WIDTH_OFFSET;
   }

   /* Joy port indicators */
   unsigned char JOYMODE1[5] = {0};
   unsigned char JOYMODE2[5] = {0};
   unsigned char JOYMODE3[5] = {0};
   unsigned char JOYMODE4[5] = {0};

   unsigned char JOYPORT1[5] = {0};
   unsigned char JOYPORT2[5] = {0};
   unsigned char JOYPORT3[5] = {0};
   unsigned char JOYPORT4[5] = {0};

   /* Joy port positions */
   int TEXT_X_JOYMODE1 = TEXT_X;
   int TEXT_X_JOYPORT1 = TEXT_X_JOYMODE1 + (FONT_WIDTH*13);

   int TEXT_X_JOYMODE2 = TEXT_X + FONT_SLOT;
   int TEXT_X_JOYPORT2 = TEXT_X_JOYMODE2 + (FONT_WIDTH*13);

   int TEXT_X_JOYMODE3 = TEXT_X + (FONT_SLOT*2);
   int TEXT_X_JOYPORT3 = TEXT_X_JOYMODE3 + (FONT_WIDTH*13);

   int TEXT_X_JOYMODE4 = TEXT_X + (FONT_SLOT*3);
   int TEXT_X_JOYPORT4 = TEXT_X_JOYMODE4 + (FONT_WIDTH*13);

   /* Regular joyflags */
   if (!retro_mousemode)
   {
      if (is_cd32pad(0))
         snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "C1");
      else if (retro_devices[0] == RETRO_DEVICE_PUAE_ANALOG)
         snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "A1");
      else if (retro_devices[0] == RETRO_DEVICE_PUAE_LIGHTGUN
            || retro_devices[0] == RETRO_DEVICE_PUAE_LIGHTPEN)
         snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "L1");
      else
         snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "J1");

      if (is_cd32pad(1))
         snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "C2");
      else if (retro_devices[1] == RETRO_DEVICE_PUAE_ANALOG)
         snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "A2");
      else if (retro_devices[1] == RETRO_DEVICE_PUAE_LIGHTGUN
            || retro_devices[1] == RETRO_DEVICE_PUAE_LIGHTPEN)
         snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "L2");
      else
         snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "J2");
   }
   else
   {
      snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "M1");
      snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "M2");
   }

   /* Regular ports */
   if (is_cd32pad(0))
      snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(jflag[0], 2));
   else if (retro_devices[0] == RETRO_DEVICE_PUAE_ANALOG)
      snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(aflag[0], 4));
   else if (retro_devices[0] == RETRO_DEVICE_PUAE_LIGHTGUN
         || retro_devices[0] == RETRO_DEVICE_PUAE_LIGHTPEN)
      snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(mflag[0], 3));
   else
      snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(jflag[0], 1));

   if (is_cd32pad(1))
      snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(jflag[1], 2));
   else if (retro_devices[1] == RETRO_DEVICE_PUAE_ANALOG)
      snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(aflag[1], 4));
   else if (retro_devices[1] == RETRO_DEVICE_PUAE_LIGHTGUN
         || retro_devices[1] == RETRO_DEVICE_PUAE_LIGHTPEN)
      snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(mflag[1], 3));
   else
      snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(jflag[1], 1));

   /* Parallel ports, hidden if not connected */
   if (retro_devices[2])
   {
      snprintf(JOYMODE3, sizeof(JOYMODE3), "%2s", "J3");
      snprintf(JOYPORT3, sizeof(JOYPORT3), "%3s", joystick_value_human(jflag[2], 0));
   }
   if (retro_devices[3])
   {
      snprintf(JOYMODE4, sizeof(JOYMODE4), "%2s", "J4");
      snprintf(JOYPORT4, sizeof(JOYPORT4), "%3s", joystick_value_human(jflag[3], 0));
   }

   /* Mouse flags */
   if (!flag_empty(mflag[1])
         && (  retro_devices[1] != RETRO_DEVICE_PUAE_LIGHTGUN
            && retro_devices[1] != RETRO_DEVICE_PUAE_LIGHTPEN))
   {
      snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "M1");
      snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(mflag[1], 3));
   }
   if (!flag_empty(mflag[0])
      && (  retro_devices[0] != RETRO_DEVICE_PUAE_LIGHTGUN
         && retro_devices[0] != RETRO_DEVICE_PUAE_LIGHTPEN))
   {
      snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "M2");
      snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(mflag[0], 3));
   }

   /* Keyrah flags */
   if (opt_keyrah_keypad)
   {
      if (!flag_empty(kjflag[0]))
      {
         snprintf(JOYMODE1, sizeof(JOYMODE1), "%2s", "K1");
         snprintf(JOYPORT1, sizeof(JOYPORT1), "%3s", joystick_value_human(kjflag[0], 0));
      }
      if (!flag_empty(kjflag[1]))
      {
         snprintf(JOYMODE2, sizeof(JOYMODE2), "%2s", "K2");
         snprintf(JOYPORT2, sizeof(JOYPORT2), "%3s", joystick_value_human(kjflag[1], 0));
      }
   }

   /* Button colorize CD32 Pad */
   int JOY1_COLOR = FONT_COLOR;
   int JOY2_COLOR = FONT_COLOR;
   if (is_cd32pad(0))
      JOY1_COLOR = joystick_color(jflag[0]);
   if (is_cd32pad(1))
      JOY2_COLOR = joystick_color(jflag[1]);

   /* Statusbar output */
   draw_fbox(0, BOX_Y, BOX_WIDTH, BOX_HEIGHT, 0, GRAPH_ALPHA_100);

   if (statusbar_message_timer)
   {
      draw_text(TEXT_X, TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, TEXT_LENGTH, statusbar_text);
      goto end;
   }

   draw_text(TEXT_X_JOYMODE1,   TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE1);
   draw_text(TEXT_X_JOYPORT1,   TEXT_Y, JOY1_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT1);

   draw_text(TEXT_X_JOYMODE2,   TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE2);
   draw_text(TEXT_X_JOYPORT2,   TEXT_Y, JOY2_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT2);

   draw_text(TEXT_X_JOYMODE3,   TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE3);
   draw_text(TEXT_X_JOYPORT3,   TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT3);

   draw_text(TEXT_X_JOYMODE4,   TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYMODE4);
   draw_text(TEXT_X_JOYPORT4,   TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, JOYPORT4);

   draw_text(TEXT_X_RESOLUTION, TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, RESOLUTION);
   draw_text(TEXT_X_MEMORY,     TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, MEMORY);
   draw_text(TEXT_X_MODEL,      TEXT_Y, FONT_COLOR, 0, GRAPH_ALPHA_100, GRAPH_BG_ALL, FONT_WIDTH, FONT_HEIGHT, 10, MODEL);

end:
   /* UAE internal LED has to come after statusbar */
   retro_draw_frame_extras();
}

/*
 * Handle target-specific cfgfile options
 */
void target_save_options (struct zfile* f, struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const char *option, const char *value, int type)
{
   return 0;
}

void target_default_options (struct uae_prefs *p, int type)
{
   p->start_gui = false;
   p->use_serial = true;
   p->sound_auto = false;
   p->leds_on_screen = 1;
   p->bogomem.size = 0x00000000;
   p->floppy_auto_ext2 = 2;
   p->nr_floppies = 1;
   p->floppyslots[1].dfxtype = DRV_NONE;
   p->lightpen_crosshair = false;

   p->jports[0].jd[0].id = JSEM_MICE;
   p->jports[1].jd[0].id = JSEM_JOYS;

   p->gfx_monitor[0].gfx_size_fs.width  = EMULATOR_MAX_WIDTH;
   p->gfx_monitor[0].gfx_size_fs.height = EMULATOR_MAX_HEIGHT;

   /* Required for SCSI CD image mounts */
   p->win32_automount_cddrives = true;

   /* Required for parallel port joysticks */
   p->win32_samplersoundcard = -1;
}

void target_fixup_options (struct uae_prefs *p)
{
   p->gfx_iscanlines = 1;
   p->gfx_pscanlines = 0;
}

/*** Input ***/

/* Mouse */
void retro_mouse(int port, int dx, int dy)
{
   mouse_port[port] = 1;
   cd32_pad_enabled[port] = 0;
   setmousestate(port, 0, dx, 0);
   setmousestate(port, 1, dy, 0);
}

void retro_mouse_button(int port, int button, int state)
{
   mouse_port[port] = 1;
   cd32_pad_enabled[port] = 0;
   setmousebuttonstate(port, button, state);
}

static void retro_cd32pad_enable(int port)
{
   if (     (retro_devices[port] == RETRO_DEVICE_PUAE_CD32PAD)
         || (  (retro_devices[port] == RETRO_DEVICE_JOYPAD)
               && (strstr(full_path, "CD32") || currprefs.cs_compatible == CP_CD32)
            )
      )
      cd32_pad_enabled[port] = 1;
}

/* Joystick */
void retro_joystick(int port, int axis, int state)
{
   /* Disable mouse in normal ports, joystick/mouse inverted */
   if (port < 2)
   {
      int m_port = (port == 0) ? 1 : 0;
      mouse_port[m_port] = 0;

      if (!cd32_pad_enabled[m_port])
         retro_cd32pad_enable(m_port);
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
      cd32_pad_enabled[m_port] = 0;
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

      if (!cd32_pad_enabled[m_port])
         retro_cd32pad_enable(m_port);
   }
   setjoybuttonstate(port, button, state);
}

void retro_arcadia_button(int port, int button, int state)
{
   if (port < 2)
   {
      switch (button)
      {
         case 6:
            if (state)
               retro_key_down((port == 0) ? RETROK_F2 : RETROK_F1);
            else
               retro_key_up((port == 0) ? RETROK_F2 : RETROK_F1);
            break;
         case 5:
            if (state)
               retro_key_down(RETROK_F5);
            else
               retro_key_up(RETROK_F5);
            break;
         case 2:
         case 3:
            if (state)
               retro_key_down((port == 0) ? RETROK_F4 : RETROK_F3);
            else
               retro_key_up((port == 0) ? RETROK_F4 : RETROK_F3);
            break;
         case 1:
            if (state)
               retro_key_down((port == 0) ? RETROK_RSHIFT : RETROK_LSHIFT);
            else
               retro_key_up((port == 0) ? RETROK_RSHIFT : RETROK_LSHIFT);
            break;
         case 0:
            retro_joystick_button(port, button, state);
            break;
         default:
            break;
      }
   }
}

/* Keyboard */
void retro_key_down(int key)
{
   retro_key_state_internal[key] = 1;
   inputdevice_do_keyboard(keyboard_translation[key], 1);
}

void retro_key_up(int key)
{
   retro_key_state_internal[key] = 0;
   inputdevice_do_keyboard(keyboard_translation[key], 0);
}

/* Pointer / lightgun */
void retro_lightpen_update(void)
{
   uint8_t i;
   uint8_t buttons = 0;
   int x = 0, y = 0;

   if (!lightpen_enabled)
      return;

   for (i = 0; i < 2; i++)
   {
      uint8_t uae_port = (i == 0) ? 1 : 0;

      x = y = buttons = 0;

      if (retro_devices[i] != RETRO_DEVICE_PUAE_LIGHTGUN
       && retro_devices[i] != RETRO_DEVICE_PUAE_LIGHTPEN)
         continue;

      retro_ui_get_pointer_state(i, &x, &y, &buttons);

#if 0
      printf("%s * port=%d uae_port=%d x=%i y=%i b=%02x\n", __func__, i, uae_port, x, y, buttons);
#endif

      /* These are post-corrections to UAE internal
       * mouse-to-lightpen translation, which must
       * be active in order to lightpen be active.
       * Buttons are set where mouse movement is set. */
      lightpen_x[i] = x;
      lightpen_y[i] = y;
   }
}


/* Graphics */
int lockscr(struct vidbuffer *vb, bool fullupdate, bool first, bool skip)
{
   return 1;
}

void unlockscr(struct vidbuffer *vb, int y_start, int y_end)
{
   /* These values must be cached here, since the
    * source variables will be reset before the frame
    * ends and control is returned to the frontend */
   retro_thisframe_first_drawn_line = thisframe_first_drawn_line;
   retro_thisframe_last_drawn_line  = thisframe_last_drawn_line;
   retro_min_diwstart               = min_diwstart;
   retro_max_diwstop                = max_diwstop;

   /* Align the resulting Automatic Crop screen height to even number */
   if (!retro_av_info_is_lace && (retro_thisframe_last_drawn_line - retro_thisframe_first_drawn_line + 1) % 2)
      retro_thisframe_last_drawn_line++;

   if (retro_thisframe_first_drawn_line > 0)
      retro_thisframe_first_drawn_line--;

   if (retro_thisframe_last_drawn_line > 0)
      retro_thisframe_last_drawn_line--;
   if (!retro_thisframe_last_drawn_line)
      retro_thisframe_last_drawn_line = -1;

   /* Flag that we should end the frame, return out of retro_run */
   libretro_frame_end = 1;
   set_special(SPCFLAG_CHECK);

   if (lightpen_enabled)
      retro_lightpen_update();

#ifdef WITH_MPEG2
   /* CD32 FMV exceptions */
   if (!changed_prefs.cartfile[0] && gfxvidinfo->drawbuffer.tempbufferinuse)
      gfxvidinfo->drawbuffer.tempbufferinuse = false;

   if (gfxvidinfo->drawbuffer.tempbufferinuse)
   {
      retro_thisframe_first_drawn_line = thisframe_first_drawn_line = minfirstline;
      retro_thisframe_last_drawn_line  = thisframe_last_drawn_line  = minfirstline + (retroh / 2);
      retro_max_diwstop                = max_diwstop = (min_diwstart + (352 * width_multiplier));
      gui_flicker_led(LED_CD, 0, 1);
   }
#endif
}

int graphics_init(bool mousecapture)
{
   if (pixbuf != NULL)
      return 1;

   pixbuf = (unsigned short int*) &retro_bmp[0];
   if (pixbuf == NULL)
   {
      log_cb(RETRO_LOG_ERROR, "Error: not enough memory to initialize screen buffer!\n");
      return -1;
   }

   gfxvidinfo->drawbuffer.width_allocated    = defaultw;
   gfxvidinfo->drawbuffer.height_allocated   = defaulth;
   gfxvidinfo->drawbuffer.pixbytes           = pix_bytes;
   gfxvidinfo->drawbuffer.rowbytes           = gfxvidinfo->drawbuffer.width_allocated * gfxvidinfo->drawbuffer.pixbytes;
   gfxvidinfo->drawbuffer.bufmem             = (unsigned char*)pixbuf;
   gfxvidinfo->drawbuffer.linemem            = 0;
   gfxvidinfo->drawbuffer.emergmem           = 0;

   reset_drawing();
   graphics_setup();

#ifdef WITH_MPEG2
   allocvidbuffer(0, &gfxvidinfo->tempbuffer, defaultw, defaulth, pix_bytes * 8);
   gfxvidinfo->tempbuffer.bufmem             = (unsigned char*)pixbuf;
#endif

#if 0
   printf("%s: %dx%dx%d bufmem_alloc=%d\n", __func__,
         gfxvidinfo->drawbuffer.width_allocated,
         gfxvidinfo->drawbuffer.height_allocated,
         gfxvidinfo->drawbuffer.pixbytes,
         gfxvidinfo->drawbuffer.bufmem_allocated);
#endif
   return 1;
}

void graphics_reset(bool forced)
{
}

int is_fullscreen (void)
{
   return 1;
}

int is_vsync (void)
{
   return 0;
}


int graphics_setup(void)
{
   /* monid, Rw,Gw,Bw, Rs,Gs,Bs, Aw,As,Alpha, swap, yuv */
   if (pix_bytes == 2)
      alloc_colors64k (0, 5, 6, 5, 11, 5, 0, 0, 0, 0, 0, false);
   else
      alloc_colors64k (0, 8, 8, 8, 16, 8, 0, 0, 0, 0, 0, false);

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

int mousehack_allowed (void)
{
   return 0;
}

int debuggable (void)
{
   return 0;
}

void screenshot (int monid, int type, int f)
{
}

void toggle_fullscreen(int monid, int mode)
{
}

int check_prefs_changed_gfx (void)
{
   int changed = 0;

   if (!config_changed && !display_change_requested)
      return 0;

   if (gfxvidinfo->drawbuffer.width_allocated  != defaultw ||
       gfxvidinfo->drawbuffer.height_allocated != defaulth)
   {
      changed = 1;
      changed_prefs.gfx_monitor[0].gfx_size_win.width  = defaultw;
      changed_prefs.gfx_monitor[0].gfx_size_win.height = defaulth;

      gfxvidinfo->drawbuffer.width_allocated  = defaultw;
      gfxvidinfo->drawbuffer.height_allocated = defaulth;
      gfxvidinfo->drawbuffer.rowbytes         = gfxvidinfo->drawbuffer.width_allocated * gfxvidinfo->drawbuffer.pixbytes;

#if 0
   printf("%s: %dx%d, res=%d vres=%d\n", __func__,
         changed_prefs.gfx_monitor[0].gfx_size_win.width,
         changed_prefs.gfx_monitor[0].gfx_size_win.height,
         changed_prefs.gfx_resolution,
         changed_prefs.gfx_vresolution);
#endif
   }

   if (currprefs.gfx_monitor[0].gfx_size_win.width   != changed_prefs.gfx_monitor[0].gfx_size_win.width)
       currprefs.gfx_monitor[0].gfx_size_win.width    = changed_prefs.gfx_monitor[0].gfx_size_win.width;
   if (currprefs.gfx_monitor[0].gfx_size_win.height  != changed_prefs.gfx_monitor[0].gfx_size_win.height)
       currprefs.gfx_monitor[0].gfx_size_win.height   = changed_prefs.gfx_monitor[0].gfx_size_win.height;

   if (currprefs.gfx_resolution       != changed_prefs.gfx_resolution)
       currprefs.gfx_resolution        = changed_prefs.gfx_resolution;
   if (currprefs.gfx_vresolution      != changed_prefs.gfx_vresolution)
       currprefs.gfx_vresolution       = changed_prefs.gfx_vresolution;

   if (currprefs.gfx_scandoubler      != changed_prefs.gfx_scandoubler)
       currprefs.gfx_scandoubler       = changed_prefs.gfx_scandoubler;

   if (currprefs.gfx_luminance        != changed_prefs.gfx_luminance ||
       currprefs.gfx_contrast         != changed_prefs.gfx_contrast  ||
       currprefs.gfx_gamma            != changed_prefs.gfx_gamma)
   {
       currprefs.gfx_luminance         = changed_prefs.gfx_luminance;
       currprefs.gfx_contrast          = changed_prefs.gfx_contrast;
       currprefs.gfx_gamma             = changed_prefs.gfx_gamma;

       changed = 1;
       graphics_setup();
   }

   /* 2 = 16bit, 5 = 32bit */
   currprefs.color_mode = (pix_bytes == 2) ? 2 : 5;

   return changed;
}

static int target_get_display_scanline2(int displayindex)
{
#if 0
	if (pD3DKMTGetScanLine) {
		D3DKMT_GETSCANLINE sl = { 0 };
		struct MultiDisplay *md = displayindex < 0 ? getdisplay(&currprefs, 0) : &Displays[displayindex];
		if (!md->HasAdapterData)
			return -11;
		sl.VidPnSourceId = md->VidPnSourceId;
		sl.hAdapter = md->AdapterHandle;
		NTSTATUS status = pD3DKMTGetScanLine(&sl);
		if (status == STATUS_SUCCESS) {
			if (sl.InVerticalBlank)
				return -1;
			return sl.ScanLine;
		} else {
			if ((int)status > 0)
				return -(int)status;
			return status;
		}
		return -12;
	} else if (D3D_getscanline) {
		int scanline;
		bool invblank;
		if (D3D_getscanline(&scanline, &invblank)) {
			if (invblank)
				return -1;
			return scanline;
		}
		return -14;
	}
	return -13;
#endif
}

extern uae_u64 spincount;
bool calculated_scanline = 1;

int target_get_display_scanline(int displayindex)
{
#if 0
	if (!scanlinecalibrating && calculated_scanline) {
		static int lastline;
		float diff = read_processor_time() - wait_vblank_timestamp;
		if (diff < 0)
			return -1;
		int sl = (int)(diff * (vsync_activeheight + (vsync_totalheight - vsync_activeheight) / 10) * vsync_vblank / syncbase);
		if (sl < 0)
			sl = -1;
		return sl;
	} else {
		static uae_u64 lastrdtsc;
		static int lastvpos;
		if (spincount == 0 || currprefs.m68k_speed >= 0) {
			lastrdtsc = 0;
			lastvpos = target_get_display_scanline2(displayindex);
			return lastvpos;
		}
		uae_u64 v = __rdtsc();
		if (lastrdtsc > v)
			return lastvpos;
		lastvpos = target_get_display_scanline2(displayindex);
		lastrdtsc = __rdtsc() + spincount * 4;
		return lastvpos;
	}
#endif
}

void vsync_clear(void)
{
#if 0
	vsync_active = false;
	if (waitvblankevent)
		ResetEvent(waitvblankevent);
#endif
}

int vsync_isdone(frame_time_t *dt)
{
#if 0
	if (isvsync() == 0)
		return -1;
	if (waitvblankthread_mode <= 0)
		return -2;
	if (dt)
		*dt = wait_vblank_timestamp;
	return vsync_active ? 1 : 0;
#else
   return 1;
#endif
}

bool target_graphics_buffer_update(int monid)
{
    return true;
}

float target_getcurrentvblankrate(int monid)
{
    return retro_refresh;
}

void target_reset (void)
{
	clipboard_reset ();
}

void target_paste_to_keyboard (void) {}
bool target_can_autoswitchdevice(void) { return false; }

struct netdriverdata **target_ethernet_enumerate (void)
{
   return NULL;
}

static int deskhz;
float target_adjust_vblank_hz(int monid, float hz)
{
#if 1
	return hz;
#else
	struct AmigaMonitor *mon = &AMonitors[monid];
	int maxrate;
	if (!currprefs.lightboost_strobo)
		return hz;
	if (isfullscreen() > 0) {
		maxrate = mon->currentmode.freq;
	} else {
		maxrate = deskhz;
	}
	double nhz = hz * 2.0;
	if (nhz >= maxrate - 1 && nhz < maxrate + 1)
		hz -= 0.5;
	return hz;
#endif
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
      default:
      case 0:
         return "RetroPad0";
         break;
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
      default:
      case 0:
         return "RetroPad0";
         break;
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

int input_get_default_joystick (struct uae_input_device *uid, int num, int port, int af, int mode, bool gp, bool joymouseswap)
{
   if (is_cd32pad(0))
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
      uid[0].eventid[ID_BUTTON_OFFSET + 4][0] = -1;
      uid[0].eventid[ID_BUTTON_OFFSET + 5][0] = -1;
      uid[0].eventid[ID_BUTTON_OFFSET + 6][0] = -1;
   }
   else
   {
      uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY2_HORIZ;
      uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY2_VERT;
      uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY2_FIRE_BUTTON;
      uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY2_2ND_BUTTON;
      uid[0].eventid[ID_BUTTON_OFFSET + 2][0] = -1;
      uid[0].eventid[ID_BUTTON_OFFSET + 3][0] = -1;
      uid[0].eventid[ID_BUTTON_OFFSET + 4][0] = -1;
      uid[0].eventid[ID_BUTTON_OFFSET + 5][0] = -1;
      uid[0].eventid[ID_BUTTON_OFFSET + 6][0] = -1;
   }

   if (is_cd32pad(1))
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
      uid[1].eventid[ID_BUTTON_OFFSET + 4][0] = -1;
      uid[1].eventid[ID_BUTTON_OFFSET + 5][0] = -1;
      uid[1].eventid[ID_BUTTON_OFFSET + 6][0] = -1;
   }
   else
   {
      uid[1].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_JOY1_HORIZ;
      uid[1].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_JOY1_VERT;
      uid[1].eventid[ID_AXIS_OFFSET + 2][0]   = INPUTEVENT_JOY1_HORIZ_POT;
      uid[1].eventid[ID_AXIS_OFFSET + 3][0]   = INPUTEVENT_JOY1_VERT_POT;
      uid[1].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_FIRE_BUTTON;
      uid[1].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_2ND_BUTTON;
      uid[1].eventid[ID_BUTTON_OFFSET + 2][0] = -1;
      uid[1].eventid[ID_BUTTON_OFFSET + 3][0] = -1;
      uid[1].eventid[ID_BUTTON_OFFSET + 4][0] = -1;
      uid[1].eventid[ID_BUTTON_OFFSET + 5][0] = -1;
      uid[1].eventid[ID_BUTTON_OFFSET + 6][0] = -1;
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

int input_get_default_joystick_analog (struct uae_input_device *uid, int num, int port, int af, bool gp, bool joymouseswap)
{
   uid[num].eventid[ID_AXIS_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_HORIZ_POT : INPUTEVENT_JOY1_HORIZ_POT;
   uid[num].eventid[ID_AXIS_OFFSET + 1][0] = port ? INPUTEVENT_JOY2_VERT_POT : INPUTEVENT_JOY1_VERT_POT;
   uid[num].eventid[ID_BUTTON_OFFSET + 0][0] = port ? INPUTEVENT_JOY2_LEFT : INPUTEVENT_JOY1_LEFT;
   uid[num].eventid[ID_BUTTON_OFFSET + 1][0] = port ? INPUTEVENT_JOY2_RIGHT : INPUTEVENT_JOY1_RIGHT;
   uid[num].eventid[ID_BUTTON_OFFSET + 2][0] = port ? INPUTEVENT_JOY2_UP : INPUTEVENT_JOY1_UP;
   uid[num].eventid[ID_BUTTON_OFFSET + 3][0] = port ? INPUTEVENT_JOY2_DOWN : INPUTEVENT_JOY1_DOWN;
   uid[num].eventid[ID_BUTTON_OFFSET + 4][0] = -1;
   uid[num].eventid[ID_BUTTON_OFFSET + 5][0] = -1;
   uid[num].eventid[ID_BUTTON_OFFSET + 6][0] = -1;

   return 0;
}

void target_inputdevice_unacquire(void) {}
void target_inputdevice_acquire(void) {}

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

int input_get_default_mouse (struct uae_input_device *uid, int num, int port, int af, bool gp, bool wheel, bool joymouseswap)
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

int input_get_default_lightpen (struct uae_input_device *uid, int num, int port, int af, bool gp, bool joymouseswap, int submode)
{
   uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_LIGHTPEN_HORIZ;
   uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_LIGHTPEN_VERT;
   uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = (submode) ? INPUTEVENT_JOY1_LEFT : INPUTEVENT_JOY1_3RD_BUTTON;

   uid[1].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_LIGHTPEN_HORIZ2;
   uid[1].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_LIGHTPEN_VERT2;
   uid[1].eventid[ID_BUTTON_OFFSET + 0][0] = (submode) ? INPUTEVENT_JOY2_LEFT : INPUTEVENT_JOY2_3RD_BUTTON;

   uid[0].enabled = 1;
   uid[1].enabled = 1;
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

bool isguiactive(void)
{
   return 0;
}



/* Libretro misc */
int sensible_strcmp(char *a, char *b)
{
   int i;

   /* No-Intro has randomly "Program" and "Data" disks,
    * which means regular sorting will be wrong */
   if (strstr(a, "(Program"))
      return -1;

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

int retro_remove(const char *path)
{
#if defined(_WIN32) && !defined(LEGACY_WIN32)
   wchar_t *pathW = utf8_to_utf16_string_alloc(path);

   if (pathW)
   {
      if (DeleteFileW(pathW))
      {
         free(pathW);
         return 0;
      }
      free(pathW);
      return -1;
   }

   return DeleteFile(path);
#else
   return remove(path);
#endif
}

int retro_rmdir(const char *path)
{
#if defined(_WIN32) && !defined(LEGACY_WIN32)
   wchar_t *pathW = utf8_to_utf16_string_alloc(path);

   if (pathW)
   {
      if (RemoveDirectoryW(pathW))
      {
         free(pathW);
         return 0;
      }
      free(pathW);
      return -1;
   }

   return RemoveDirectory(path);
#else
   return rmdir(path);
#endif
}

int remove_recurse(const char *path)
{
   struct dirent *dirp;
   char filename[RETRO_PATH_MAX];
   int ret   = 0;
   RDIR *dir = retro_opendir(path);
   if (dir == NULL)
      return -1;

   while (retro_readdir(dir))
   {
      const char *name = retro_dirent_get_name(dir);

      if (name[0] == '.')
         continue;

      snprintf(filename, sizeof(filename), "%s%s%s", path, DIR_SEP_STR, name);

      if (path_is_directory(filename))
         ret = remove_recurse(filename);
      else
         ret = retro_remove(filename);

      if (!ret)
         log_cb(RETRO_LOG_INFO, "Clean: %s\n", filename);
      else
         log_cb(RETRO_LOG_INFO, "Clean fail: %s\n", filename);
   }

   retro_closedir(dir);

   /* Leave the root directory for RAM disk usage */
   if (strcmp(retro_temp_directory, path))
      retro_rmdir(path);

   return ret;
}

int fcopy(const char *src, const char *dst)
{
   char buf[256] = {0};
   size_t n      = 0;
   int ret       = 0;

   char path_dst[RETRO_PATH_MAX] = {0};
   snprintf(path_dst, sizeof(path_dst), "%s", dst);
   path_basedir(path_dst);

   if (!path_is_directory(path_dst))
   {
      log_cb(RETRO_LOG_INFO, "Mkdir: %s\n", path_dst);
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
   char buf_src[256] = {0};
   char buf_dst[256] = {0};
   size_t n          = 0;
   int ret           = 0;

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
      ret = fread(buf_dst, sizeof(char), sizeof(buf_dst), fp_dst);
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

uint64_t fsize(const char *path)
{
   FILE *handle  = NULL;
   uint64_t size = 0;

   handle = fopen(path, "rb");
   if (handle)
   {
      if (!fseeko(handle, 0, SEEK_END))
         size = ftello(handle);
      fclose(handle);
   }
   return size;
}

void path_join(char* out, const char* basedir, const char* filename)
{
   snprintf(out, RETRO_PATH_MAX, "%s%s%s", basedir, DIR_SEP_STR, filename);
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
#define BUFLEN 16384

void gz_compress(const char *in, const char *out)
{
   char buf[BUFLEN];
   size_t len;
   int err;
   FILE *in_fp;
   gzFile out_fp;

   out_fp = gzopen(out, "wb");
   if (out_fp == NULL)
      return;

   in_fp = fopen(in, "rb");
   if (in_fp == NULL)
      return;

   for (;;)
   {
      len = fread(buf, 1, sizeof(buf), in_fp);
      int buflen;

      if (len <= 0)
      {
         if (len < 0)
            log_cb(RETRO_LOG_ERROR, "GZip: Read error\n");
         break;
      }

      buflen = gzwrite(out_fp, buf, len);
      if (buflen != len)
         log_cb(RETRO_LOG_ERROR, "GZip: %s\n", gzerror(out_fp, &err));
   }
   fclose(in_fp);

   if (gzclose(out_fp) == Z_OK)
      log_cb(RETRO_LOG_INFO, "GZip: %s\n", out);
}

void gz_uncompress(const char *in, const char *out)
{
   char gzbuf[BUFLEN];
   int len;
   int err;

   struct gzFile_s *in_fp;
   if ((in_fp = gzopen(in, "r")))
   {
      FILE *out_fp;
      if ((out_fp = fopen(out, "wb")))
      {
         for (;;)
         {
            len = gzread(in_fp, gzbuf, sizeof(gzbuf));
            if (len <= 0)
            {
               if (len < 0)
                  log_cb(RETRO_LOG_ERROR, "GUnzip: %s\n", gzerror(in_fp, &err));
               break;
            }

            if (fwrite(gzbuf, 1, len, out_fp) != len)
               log_cb(RETRO_LOG_ERROR, "GUnzip: Write error\n");
         }
         fclose(out_fp);

         if (!len)
            log_cb(RETRO_LOG_INFO, "GUnzip: %s\n", out);
      }
      gzclose(in_fp);
   }
}

void zip_uncompress(const char *in, const char *out, char *lastfile)
{
   uLong i;
   unz_global_info gi;

   unzFile uf           = NULL;
   char *in_local       = NULL;
   const char* password = NULL;
   int size_buf         = 8192;
   int err;

   in_local             = utf8_to_local_string_alloc(in);
   uf                   = unzOpen(in_local);

   free(in_local);
   in_local = NULL;

   err = unzGetGlobalInfo (uf, &gi);

   for (i = 0; i < gi.number_entry; i++)
   {
      char filename_inzip[256];
      char filename_withpath[512];
      char* filename_withoutpath;
      char* p;
      unz_file_info file_info;
      FILE *fout = NULL;
      void* buf;

      filename_inzip[0]    = '\0';
      filename_withpath[0] = '\0';

      buf = (void*)malloc(size_buf);
      if (buf == NULL)
      {
         log_cb(RETRO_LOG_ERROR, "Unzip: Error allocating memory\n");
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
            filename_withoutpath = p + 1;
         p++;
      }

      if ((*filename_withoutpath) == '\0')
      {
         log_cb(RETRO_LOG_INFO, "Mkdir: %s\n", filename_withpath);
         path_mkdir(filename_withpath);
      }
      else if (!path_is_valid(filename_withpath))
      {
         char* write_filename;
         unsigned skip = 0;
         unsigned x    = 0;

#ifdef USE_LIBRETRO_VFS
         write_filename = strdup(filename_withpath);
#else
         write_filename = local_to_utf8_string_alloc(filename_withpath);
#endif

#if 0
         /* Replace non-ascii chars with underscore */
         for (x = 128; x < 256; x++)
            string_replace_all_chars(write_filename, x, '_');
#endif

         err = unzOpenCurrentFilePassword(uf, password);
         if (err != UNZ_OK)
            log_cb(RETRO_LOG_ERROR, "Unzip: Error %d with zipfile in unzOpenCurrentFilePassword: %s\n", err, write_filename);

         if ((skip == 0) && (err == UNZ_OK))
         {
            fout = fopen(write_filename, "wb");
            if (fout == NULL)
               log_cb(RETRO_LOG_ERROR, "Unzip: Error opening %s\n", write_filename);
         }

         if (fout != NULL)
         {
            log_cb(RETRO_LOG_INFO, "Unzip: %s\n", write_filename);

            do
            {
               err = unzReadCurrentFile(uf, buf, size_buf);
               if (err < 0)
               {
                  log_cb(RETRO_LOG_ERROR, "Unzip: Error %d with zipfile in unzReadCurrentFile\n", err);
                  break;
               }
               if (err > 0)
               {
                  if (!fwrite(buf, err, 1, fout))
                  {
                     log_cb(RETRO_LOG_ERROR, "Unzip: Error in writing extracted file\n");
                     err = UNZ_ERRNO;
                     break;
                  }
               }
            }
            while (err > 0);
            if (fout)
               fclose(fout);
         }

         free(write_filename);
         write_filename = NULL;

         if (err == UNZ_OK)
         {
            err = unzCloseCurrentFile(uf);
            if (err != UNZ_OK)
               log_cb(RETRO_LOG_ERROR, "Unzip: Error %d with zipfile in unzCloseCurrentFile\n", err);
         }
         else
            unzCloseCurrentFile(uf);
      }

      free(buf);

      if ((i + 1) < gi.number_entry)
      {
         err = unzGoToNextFile(uf);
         if (err != UNZ_OK)
         {
            log_cb(RETRO_LOG_ERROR, "Unzip: Error %d with zipfile in unzGoToNextFile\n", err);
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
#define SEVENZIP_LOOKTOREAD_BUF_SIZE (1 << 14)
struct sevenzip_context_t
{
   uint8_t *output;
   CFileInStream archiveStream;
   CLookToRead2 lookStream;
   ISzAlloc allocImp;
   ISzAlloc allocTempImp;
   CSzArEx db;
   size_t temp_size;
   uint32_t block_index;
   uint32_t parse_index;
   uint32_t decompress_index;
   uint32_t packIndex;
};

static void *sevenzip_stream_alloc_impl(ISzAllocPtr p, size_t size)
{
   if (size == 0)
      return 0;
   return malloc(size);
}

static void sevenzip_stream_free_impl(ISzAllocPtr p, void *address)
{
   (void)p;

   if (address)
      free(address);
}

static void *sevenzip_stream_alloc_tmp_impl(ISzAllocPtr p, size_t size)
{
   (void)p;
   if (size == 0)
      return 0;
   return malloc(size);
}

void sevenzip_uncompress(const char *in, const char *out, char *lastfile)
{
   CFileInStream archiveStream;
   CLookToRead2 lookStream;
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

   lookStream.bufSize   = SEVENZIP_LOOKTOREAD_BUF_SIZE * sizeof(Byte);
   lookStream.buf       = (Byte*)malloc(lookStream.bufSize);

   if (!lookStream.buf)
      lookStream.bufSize = 0;

#if defined(_WIN32) && defined(USE_WINDOWS_FILE) && !defined(LEGACY_WIN32)
   if (!string_is_empty(in))
   {
      wchar_t *pathW = utf8_to_utf16_string_alloc(in);

      if (pathW)
      {
         /* Could not open 7zip archive? */
         if (InFile_OpenW(&archiveStream.file, pathW))
         {
            free(pathW);
            return;
         }

         free(pathW);
      }
   }
#else
   /* Could not open 7zip archive? */
   if (InFile_Open(&archiveStream.file, in))
      return;
#endif

   FileInStream_CreateVTable(&archiveStream);
   LookToRead2_CreateVTable(&lookStream, false);
   lookStream.realStream = &archiveStream.vt;
   LookToRead2_Init(&lookStream);
   CrcGenerateTable();

   memset(&db, 0, sizeof(db));

   SzArEx_Init(&db);

   if (SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp) == SZ_OK)
   {
      uint32_t i;
      uint16_t *temp       = NULL;
      size_t temp_size     = 0;
      uint32_t block_index = 0xFFFFFFFF;
      SRes res             = SZ_OK;
      size_t output_size   = 0;

      for (i = 0; i < db.NumFiles; i++)
      {
         size_t j;
         size_t len;
         char infile[RETRO_PATH_MAX];
         size_t offset                = 0;
         size_t outSizeProcessed      = 0;
         unsigned x;

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
         res = SzArEx_Extract(&db, &lookStream.vt, i, &block_index,
               &output, &output_size, &offset, &outSizeProcessed,
               &allocImp, &allocTempImp);

         if (res != SZ_OK)
            break; /* This goes to the error section. */

         outsize = (int64_t)outSizeProcessed;

         char output_path[RETRO_PATH_MAX] = {0};
         snprintf(output_path, RETRO_PATH_MAX, "%s%s%s", out, DIR_SEP_STR, infile);
         if (dc_get_image_type(output_path) == DC_IMAGE_TYPE_FLOPPY && lastfile != NULL)
            snprintf(lastfile, RETRO_PATH_MAX, "%s", path_basename(output_path));

#if 0
         /* Replace non-ascii chars with underscore */
         for (x = 128; x < 256; x++)
            string_replace_all_chars(output_path, x, '_');
#endif

         for (j = 0; output_path[j] != 0; j++)
         {
            if (output_path[j] == '/')
            {
               output_path[j] = 0;
               path_mkdir((const char *)output_path);
               output_path[j] = DIR_SEP_CHR;
            }
         }

         const void *ptr = (const void*)(output + offset);

         if (path_is_valid(output_path))
            continue;
         else if (SzArEx_IsDir(&db, i))
         {
            path_mkdir((const char *)temp);
            log_cb(RETRO_LOG_INFO, "Mkdir: %s\n", output_path);
            continue;
         }

         if (filestream_write_file(output_path, ptr, outsize))
         {
            res = SZ_OK;
            log_cb(RETRO_LOG_INFO, "Un7ip: %s\n", output_path);
         }
         else
         {
            res = SZ_ERROR_FAIL;
            log_cb(RETRO_LOG_ERROR, "Un7ip: Error writing extracted file %s\n", output_path);
         }
      }

      if (temp)
         free(temp);
      IAlloc_Free(&allocImp, output);

      if (res == SZ_ERROR_UNSUPPORTED)
         log_cb(RETRO_LOG_ERROR, "Un7ip: Decoder doesn't support this archive\n");
      else if (res == SZ_ERROR_MEM)
         log_cb(RETRO_LOG_ERROR, "Un7ip: Can not allocate memory\n");
      else if (res == SZ_ERROR_CRC)
         log_cb(RETRO_LOG_ERROR, "Un7ip: CRC error\n");
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
        printf ("Invalid size\n");
        exit (EXIT_FAILURE);
    }

    if ((size >= (1LL << 31)) && (sizeof (off_t) < sizeof (uae_u64))) {
        printf ("Specified size too large (2GB file size is maximum).\n");
        exit (EXIT_FAILURE);
    }

    num_blocks = size / block_size;

    /* We don't want more than (2^32)-1 blocks */
    if (num_blocks >= (1LL << 32)) {
        printf ("Specified size too large (too many blocks).\n");
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
        printf ("Specified size is too small.\n");
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

UINT32 get_bigendian_uint32(const UINT8 *base)
{
	return (base[0] << 24) | (base[1] << 16) | (base[2] << 8) | base[3];
}

/*-------------------------------------------------
    get_bigendian_uint64 - fetch a UINT64 from
    the data stream in bigendian order
-------------------------------------------------*/

UINT64 get_bigendian_uint64(const UINT8 *base)
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
			/* convert to physical and proceed */
			physlba = file->cdtoc.tracks[track].physframeofs + (loglba - file->cdtoc.tracks[track].logframeofs);
			chdlba = physlba - file->cdtoc.tracks[track].physframeofs + file->cdtoc.tracks[track].chdframeofs;
			*tracknum = track;
			return chdlba;
		}
	}

	return loglba;
}

/*-------------------------------------------------
    constructor - "open" a CD-ROM file from an
    already-opened CHD file
-------------------------------------------------*/

/**
 * @fn  cdrom_file *cdrom_open(chd_file *chd)
 *
 * @brief   Queries if a given cdrom open.
 *
 * @param [in,out]  chd If non-null, the chd.
 *
 * @return  null if it fails, else a cdrom_file*.
 */

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
		file->cdtoc.tracks[i].logframeofs = 0;

		if (file->cdtoc.tracks[i].pgdatasize == 0)
		{
			// Anything that isn't cue.
			// toc (cdrdao): Pregap data seems to be included at the end of previous track.
			// START/PREGAP is only issued in special cases, for instance alongside ZERO commands.
			// ZERO and SILENCE commands are supposed to generate additional data that's not included
			// in the image directly, so the total logofs value must be offset to point to index 1.
			logofs += file->cdtoc.tracks[i].pregap;
		}
		else
		{
			// cues: Pregap is the difference between index 0 and index 1 unless PREGAP is specified.
			// The data is assumed to be in the bin and not generated separately, so the pregap should
			// only be added to the current track's lba to offset it to index 1.
			file->cdtoc.tracks[i].logframeofs = file->cdtoc.tracks[i].pregap;
		}

		file->cdtoc.tracks[i].physframeofs = physofs;
		file->cdtoc.tracks[i].chdframeofs = chdofs;
		file->cdtoc.tracks[i].logframeofs += logofs;
		file->cdtoc.tracks[i].logframes = file->cdtoc.tracks[i].frames - file->cdtoc.tracks[i].pregap;

		// postgap counts against the next track
		logofs += file->cdtoc.tracks[i].postgap;

		physofs += file->cdtoc.tracks[i].frames;
		chdofs  += file->cdtoc.tracks[i].frames;
		chdofs  += file->cdtoc.tracks[i].extraframes;
		logofs  += file->cdtoc.tracks[i].frames;

#if 0
        printf("Track %02d is format %d subtype %d datasize %d subsize %d frames %d extraframes %d pregap %d pgmode %d presize %d postgap %d logofs %d physofs %d chdofs %d logframes %d\n", i+1,
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
            file->cdtoc.tracks[i].chdframeofs,
            file->cdtoc.tracks[i].logframes);
#endif
	}

	/* fill out dummy entries for the last track to help our search */
	file->cdtoc.tracks[i].physframeofs = physofs;
	file->cdtoc.tracks[i].logframeofs = logofs;
	file->cdtoc.tracks[i].chdframeofs = chdofs;
	file->cdtoc.tracks[i].logframes = 0;

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


UINT8 m_cache[CD_FRAME_SIZE*CD_FRAMES_PER_HUNK*2] = {0};
UINT32 m_cachehunk = 0;

/**
 * @fn  std::error_condition chd_file::read_bytes(uint64_t offset, void *buffer, uint32_t bytes)
 *
 * @brief   -------------------------------------------------
 *            read_bytes - read from the CHD at a byte level, using the cache to handle partial
 *            hunks
 *          -------------------------------------------------.
 *
 * @param   offset          The offset.
 * @param [in,out]  buffer  If non-null, the buffer.
 * @param   bytes           The bytes.
 *
 * @return  The bytes.
 */

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

/***************************************************************************
    CORE READ ACCESS
***************************************************************************/

/**
 * @fn  std::error_condition read_partial_sector(void *dest, uint32_t lbasector, uint32_t chdsector, uint32_t tracknum, uint32_t startoffs, uint32_t length, bool phys)
 *
 * @brief   Reads partial sector.
 *
 * @param [in,out]  dest    If non-null, destination for the.
 * @param   lbasector       The lbasector.
 * @param   chdsector       The chdsector.
 * @param   tracknum        The tracknum.
 * @param   startoffs       The startoffs.
 * @param   length          The length.
 * @param   phys            true to physical.
 *
 * @return  The partial sector.
 */

chd_error read_partial_sector(cdrom_file *file, void *dest, UINT32 lbasector, UINT32 chdsector, UINT32 tracknum, UINT32 startoffs, UINT32 length, bool phys)
{
	chd_error result = CHDERR_NONE;
	bool needswap = false;

	// if this is pregap info that isn't actually in the file, just return blank data
	if (!phys)
	{
		if ((file->cdtoc.tracks[tracknum].pgdatasize == 0) && (lbasector < file->cdtoc.tracks[tracknum].logframeofs))
		{
			write_log("PG missing sector: LBA %d, trklog %d\n", lbasector, file->cdtoc.tracks[tracknum].logframeofs);
			memset(dest, 0, length);
			return result;
		}
	}

	// if a CHD, just read
	if (file->chd != NULL)
	{
		if (!phys && file->cdtoc.tracks[tracknum].pgdatasize != 0)
		{
			// chdman (phys=true) relies on chdframeofs to point to index 0 instead of index 1 for extractcd.
			// Actually playing CDs requires it to point to index 1 instead of index 0, so adjust the offset when phys=false.
			chdsector += file->cdtoc.tracks[tracknum].pregap;
		}

		result = chd_read_bytes(file->chd, (UINT64)chdsector * (UINT64)CD_FRAME_SIZE + startoffs, dest, length);

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

UINT32 cdrom_read_data(cdrom_file *file, UINT32 lbasector, void *buffer, UINT32 datatype, bool phys)
{
	// compute CHD sector and tracknumber
	UINT32 tracknum = 0;
	UINT32 chdsector;

	if (file == NULL)
		return 0;

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
		return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 0, file->cdtoc.tracks[tracknum].datasize, phys) == CHDERR_NONE);
	}
	else
	{
		/* return 2048 bytes of mode 1 data from a 2352 byte mode 1 raw sector */
		if ((datatype == CD_TRACK_MODE1) && (tracktype == CD_TRACK_MODE1_RAW))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 16, 2048, phys) == CHDERR_NONE);
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
			return (read_partial_sector(file, bufptr+16, lbasector, chdsector, tracknum, 0, 2048, phys) == CHDERR_NONE);
		}

		/* return 2048 bytes of mode 1 data from a mode2 form1 or raw sector */
		if ((datatype == CD_TRACK_MODE1) && ((tracktype == CD_TRACK_MODE2_FORM1)||(tracktype == CD_TRACK_MODE2_RAW)))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 24, 2048, phys) == CHDERR_NONE);
		}

		/* return 2048 bytes of mode 1 data from a mode2 form2 or XA sector */
		if ((datatype == CD_TRACK_MODE1) && (tracktype == CD_TRACK_MODE2_FORM_MIX))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 8, 2048, phys) == CHDERR_NONE);
		}

		/* return mode 2 2336 byte data from a 2352 byte mode 1 or 2 raw sector (skip the header) */
		if ((datatype == CD_TRACK_MODE2) && ((tracktype == CD_TRACK_MODE1_RAW) || (tracktype == CD_TRACK_MODE2_RAW)))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 16, 2336, phys) == CHDERR_NONE);
		}

		/* return 2352 byte mode 1 raw sector from a 2352 byte mode 2 raw sector */
		if ((datatype == CD_TRACK_MODE1_RAW) && (tracktype == CD_TRACK_MODE2_RAW))
		{
			return (read_partial_sector(file, buffer, lbasector, chdsector, tracknum, 0, 2352, phys) == CHDERR_NONE);
		}

		write_log("CDROM: Conversion from type %d to type %d not supported!\n", tracktype, datatype);
		return 0;
	}
}

UINT32 cdrom_read_subcode(cdrom_file *file, UINT32 lbasector, void *buffer, bool phys)
{
	// compute CHD sector and tracknumber
	UINT32 tracknum = 0;
	UINT32 chdsector;

	if (file == NULL)
		return 0;

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
	chd_error err = read_partial_sector(file, buffer, lbasector, chdsector, tracknum, file->cdtoc.tracks[tracknum].datasize, file->cdtoc.tracks[tracknum].subsize, phys);
	return (err == CHDERR_NONE);
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




//**************************************************************************
//  CONSTANTS
//**************************************************************************

static const UINT8 V34_MAP_ENTRY_FLAG_TYPE_MASK = 0x0f;     // what type of hunk
static const UINT8 V34_MAP_ENTRY_FLAG_NO_CRC = 0x10;        // no CRC is present

/**
 * @fn  std::error_condition chd_file::hunk_info(uint32_t hunknum, chd_codec_type &compressor, uint32_t &compbytes)
 *
 * @brief   -------------------------------------------------
 *            hunk_info - return information about this hunk
 *          -------------------------------------------------.
 *
 * @param   hunknum             The hunknum.
 * @param [in,out]  compressor  The compressor.
 * @param [in,out]  compbytes   The compbytes.
 *
 * @return  A std::error_condition.
 */

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


#endif /* WITH_CHD */
