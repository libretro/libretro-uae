#include "keyboard.h"
#include "libretro-core.h"
#include "libretro-graph.h"
#include "libretro-vkbd.h"
#include "libretro-mapper.h"

#include "options.h"

bool retro_vkbd = false;
static bool retro_vkbd_page = false;
static bool retro_vkbd_transparent = true;
signed char retro_vkbd_ready = 0;
int vkflag[10] = {0};

#ifdef POINTER_DEBUG
static int pointer_x = 0;
static int pointer_y = 0;
#endif
static int last_pointer_x = 0;
static int last_pointer_y = 0;

static int vkey_pos_x = 0;
static int vkey_pos_y = 0;
static int vkbd_x_min = 0;
static int vkbd_x_max = 0;
static int vkbd_y_min = 0;
static int vkbd_y_max = 0;

/* VKBD_MIN_HOLDING_TIME: Hold a direction longer than this and automatic movement sets in */
/* VKBD_MOVE_DELAY: Delay between automatic movement from button to button */
#define VKBD_MIN_HOLDING_TIME 200
#define VKBD_MOVE_DELAY 50
static bool let_go_of_direction = true;
static long last_move_time = 0;
static long last_press_time = 0;

/* VKBD_STICKY_HOLDING_TIME: Button press longer than this triggers sticky key */
#define VKBD_STICKY_HOLDING_TIME 1000
static bool let_go_of_button = true;
static long last_press_time_button = 0;
static int vkey_pressed = -1;
static int vkey_sticky = -1;
static int vkey_sticky1 = -1;
static int vkey_sticky2 = -1;

static long last_vkey_pressed_time = 0;
static int last_vkey_pressed = -1;
static int vkey_sticky1_release = 0;
static int vkey_sticky2_release = 0;

typedef struct
{
   char normal[10];
   char shift[10];
   int value;
} retro_vkeys;

static retro_vkeys vkeys[VKBDX * VKBDY * 2] =
{
   /* -11 */
   { {10,'L'},{10,'L'},-15 },
   { {10,'R'},{10,'R'},-16 },
   { {10,30},{10,30},-11 },
   { {10,28},{10,28},-12 },
   { {10,27},{10,27},-13 },
   { {10,29},{10,29},-14 },
   { "J/M" ,"J/M" ,-18 },
   { "TRBF","TRBF",-19 },
   { "ASPR","ZOOM",-22 },
   { "STBR","SVDS",-21 },
   { {17}  ,{17}  ,-20 }, /* Reset */

   /* 0 PG1 */
   { "Esc" ,"Esc" ,RETROK_ESCAPE },
   { "F1"  ,"F1"  ,RETROK_F1 },
   { "F2"  ,"F2"  ,RETROK_F2 },
   { "F3"  ,"F3"  ,RETROK_F3 },
   { "F4"  ,"F4"  ,RETROK_F4 },
   { "F5"  ,"F5"  ,RETROK_F5 },
   { "F6"  ,"F6"  ,RETROK_F6 },
   { "F7"  ,"F7"  ,RETROK_F7 },
   { "F8"  ,"F8"  ,RETROK_F8 },
   { "F9"  ,"F9"  ,RETROK_F9 },
   { "F10" ,"F10" ,RETROK_F10 },

   /* 11 */
   { "`"   ,"~"   ,RETROK_BACKQUOTE },
   { "1"   ,"!"   ,RETROK_1 },
   { "2"   ,"@"   ,RETROK_2 },
   { "3"   ,"#"   ,RETROK_3 },
   { "4"   ,"$"   ,RETROK_4 },
   { "5"   ,"%"   ,RETROK_5 },
   { "6"   ,"^"   ,RETROK_6 },
   { "7"   ,"&"   ,RETROK_7 },
   { "8"   ,"*"   ,RETROK_8 },
   { "9"   ,"("   ,RETROK_9 },
   { "0"   ,")"   ,RETROK_0 },

   /* 22 */
   { {11}  ,{11}  ,RETROK_TAB },
   { "Q"   ,"Q"   ,RETROK_q },
   { "W"   ,"W"   ,RETROK_w },
   { "E"   ,"E"   ,RETROK_e },
   { "R"   ,"R"   ,RETROK_r },
   { "T"   ,"T"   ,RETROK_t },
   { "Y"   ,"Y"   ,RETROK_y },
   { "U"   ,"U"   ,RETROK_u },
   { "I"   ,"I"   ,RETROK_i },
   { "O"   ,"O"   ,RETROK_o },
   { "P"   ,"P"   ,RETROK_p },

   /* 33 */
   { "Ctrl","Ctrl",RETROK_LCTRL },
   { "A"   ,"A"   ,RETROK_a },
   { "S"   ,"S"   ,RETROK_s },
   { "D"   ,"D"   ,RETROK_d },
   { "F"   ,"F"   ,RETROK_f },
   { "G"   ,"G"   ,RETROK_g },
   { "H"   ,"H"   ,RETROK_h },
   { "J"   ,"J"   ,RETROK_j },
   { "K"   ,"K"   ,RETROK_k },
   { "L"   ,"L"   ,RETROK_l },
   { ";"   ,":"   ,RETROK_SEMICOLON },

   /* 44 */
   { "Caps\1Lock","Caps\1Lock",RETROK_CAPSLOCK },
   { "Z"   ,"Z"   ,RETROK_z },
   { "X"   ,"X"   ,RETROK_x },
   { "C"   ,"C"   ,RETROK_c },
   { "V"   ,"V"   ,RETROK_v },
   { "B"   ,"B"   ,RETROK_b },
   { "N"   ,"N"   ,RETROK_n },
   { "M"   ,"M"   ,RETROK_m },
   { ","   ,"<"   ,RETROK_COMMA },
   { "."   ,">"   ,RETROK_PERIOD },
   { "/"   ,"?"   ,RETROK_SLASH },

   /* 55 */
   { {12}  ,{12}  ,RETROK_LSHIFT },
   { "-"   ,"_"   ,RETROK_MINUS },
   { "="   ,"+"   ,RETROK_EQUALS },
   { "\\"  ,"|"   ,RETROK_BACKSLASH },
   { "["   ,"{"   ,RETROK_LEFTBRACKET },
   { "]"   ,"}"   ,RETROK_RIGHTBRACKET },
   { "'"   ,"\""  ,RETROK_QUOTE },
   { {12}  ,{12}  ,RETROK_RSHIFT },
   { "Del" ,"Del" ,RETROK_DELETE },
   { {30}  ,{30}  ,RETROK_UP },
   { "Help","Help",RETROK_INSERT },

   /* 66 */
   { {15}  ,{15}  ,-2 }, /* Numpad */
   { "Alt" ,"Alt" ,RETROK_LALT },
   { {14}  ,{14}  ,RETROK_LSUPER },
   { {18}  ,{18}  ,RETROK_SPACE },
   { {13}  ,{13}  ,RETROK_RSUPER },
   { "Alt" ,"Alt" ,RETROK_RALT },
   { {25}  ,{25}  ,RETROK_BACKSPACE },
   { {16}  ,{16}  ,RETROK_RETURN },
   { {27}  ,{27}  ,RETROK_LEFT },
   { {28}  ,{28}  ,RETROK_DOWN },
   { {29}  ,{29}  ,RETROK_RIGHT },



   /* -11 */
   { {10,'L'},{10,'L'},-15 },
   { {10,'R'},{10,'R'},-16 },
   { {10,30},{10,30},-11 },
   { {10,28},{10,28},-12 },
   { {10,27},{10,27},-13 },
   { {10,29},{10,29},-14 },
   { "J/M" ,"J/M" ,-18 },
   { "TRBF","TRBF",-19 },
   { "ASPR","ZOOM",-22 },
   { "STBR","SVDS",-21 },
   { {17}  ,{17}  ,-20 }, /* Reset */

   /* 0 PG2 */
   { "Esc" ,"Esc" ,RETROK_ESCAPE },
   { "F1"  ,"F1"  ,RETROK_F1 },
   { "F2"  ,"F2"  ,RETROK_F2 },
   { "F3"  ,"F3"  ,RETROK_F3 },
   { "F4"  ,"F4"  ,RETROK_F4 },
   { "F5"  ,"F5"  ,RETROK_F5 },
   { "F6"  ,"F6"  ,RETROK_F6 },
   { "F7"  ,"F7"  ,RETROK_F7 },
   { "F8"  ,"F8"  ,RETROK_F8 },
   { "F9"  ,"F9"  ,RETROK_F9 },
   { "F10" ,"F10" ,RETROK_F10 },

   /* 11 */
   { "`"     ,"~"    ,RETROK_BACKQUOTE },
   { {15,'1'},"End"  ,RETROK_KP1 },
   { {15,'2'},{15,28},RETROK_KP2 },
   { {15,'3'},"PgDn" ,RETROK_KP3 },
   { {15,'4'},{15,27},RETROK_KP4 },
   { {15,'5'}," "    ,RETROK_KP5 },
   { {15,'6'},{15,29},RETROK_KP6 },
   { {15,'7'},"Home" ,RETROK_KP7 },
   { {15,'8'},{15,30},RETROK_KP8 },
   { {15,'9'},"PgUp" ,RETROK_KP9 },
   { {15,'0'},"Ins"  ,RETROK_KP0 },

   /* 22 */
   { {11}  ,{11}  ,RETROK_TAB },
   { "Q"   ,"Q"   ,RETROK_q },
   { "W"   ,"W"   ,RETROK_w },
   { "E"   ,"E"   ,RETROK_e },
   { "R"   ,"R"   ,RETROK_r },
   { "T"   ,"T"   ,RETROK_t },
   { "Y"   ,"Y"   ,RETROK_y },
   { "U"   ,"U"   ,RETROK_u },
   { "I"   ,"I"   ,RETROK_i },
   { "O"   ,"O"   ,RETROK_o },
   { "P"   ,"P"   ,RETROK_p },

   /* 33 */
   { "Ctrl","Ctrl",RETROK_LCTRL },
   { "A"   ,"A"   ,RETROK_a },
   { "S"   ,"S"   ,RETROK_s },
   { "D"   ,"D"   ,RETROK_d },
   { "F"   ,"F"   ,RETROK_f },
   { "G"   ,"G"   ,RETROK_g },
   { "H"   ,"H"   ,RETROK_h },
   { "J"   ,"J"   ,RETROK_j },
   { "K"   ,"K"   ,RETROK_k },
   { "L"   ,"L"   ,RETROK_l },
   { ";"   ,":"   ,RETROK_SEMICOLON },

   /* 44 */
   { "Caps\1Lock","Caps\1Lock",RETROK_CAPSLOCK },
   { "Z"   ,"Z"   ,RETROK_z },
   { "X"   ,"X"   ,RETROK_x },
   { "C"   ,"C"   ,RETROK_c },
   { "V"   ,"V"   ,RETROK_v },
   { "B"   ,"B"   ,RETROK_b },
   { "N"   ,"N"   ,RETROK_n },
   { "M"   ,"M"   ,RETROK_m },
   { ","   ,"<"   ,RETROK_COMMA },
   { "."   ,">"   ,RETROK_PERIOD },
   { "/"   ,"?"   ,RETROK_SLASH },

   /* 55 */
   { {12}  ,{12}  ,RETROK_LSHIFT },
   { {15,'-'},{15,'-'},RETROK_KP_MINUS },
   { {15,'+'},{15,'+'},RETROK_KP_PLUS },
   { {15,'/'},{15,'/'},RETROK_KP_DIVIDE },
   { {15,'('},{15,'('},RETROK_HOME },
   { {15,')'},{15,')'},RETROK_END },
   { {15,'*'},{15,'*'},RETROK_KP_MULTIPLY },
   { {12}  ,{12}  ,RETROK_RSHIFT },
   { {15,'.'},{15,'D','e','l'},RETROK_KP_PERIOD },
   { {15,30} ,{15,30},RETROK_KP8 },
   { "Help","Help",RETROK_INSERT },

   /* 66 */
   { {15}  ,{15}  ,-2 }, /* Numpad */
   { "Alt" ,"Alt" ,RETROK_LALT },
   { {14}  ,{14}  ,RETROK_LSUPER },
   { {18}  ,{18}  ,RETROK_SPACE },
   { {13}  ,{13}  ,RETROK_RSUPER },
   { "Alt" ,"Alt" ,RETROK_RALT },
   { {25}  ,{25}  ,RETROK_BACKSPACE },
   { {15,16},{15,16},RETROK_KP_ENTER },
   { {15,27},{15,27},RETROK_KP4 },
   { {15,28},{15,28},RETROK_KP2 },
   { {15,29},{15,29},RETROK_KP6 },
};

void print_vkbd(void)
{
   libretro_graph_alpha_t ALPHA      = opt_vkbd_alpha;
   libretro_graph_alpha_t BKG_ALPHA  = ALPHA;
   long now                          = retro_ticks() / 1000;
   bool shifted                      = false;
   bool text_outline                 = false;
   int page                          = (retro_vkbd_page) ? VKBDX * VKBDY : 0;
   int x                             = 0;
   int y                             = 0;

   int XKEY                          = 0;
   int YKEY                          = 0;
   int XTEXT                         = 0;
   int YTEXT                         = 0;
   int XOFFSET                       = 0;
   int XPADDING                      = 0;
   int YOFFSET                       = 0;
   int YPADDING                      = 0;
   int XKEYSPACING                   = 1;
   int YKEYSPACING                   = 1;

   int BKG_PADDING_X                 = 0;
   int BKG_PADDING_Y                 = 0;
   int BKG_COLOR                     = 0;
   int BKG_COLOR_NORMAL              = 0;
   int BKG_COLOR_ALT                 = 0;
   int BKG_COLOR_EXTRA               = 0;
   int BKG_COLOR_SEL                 = 0;
   int BKG_COLOR_ACTIVE              = 0;

   int FONT_MAX                      = 10;
   int FONT_WIDTH                    = 1;
   int FONT_HEIGHT                   = 1;
   int FONT_COLOR                    = 0;
   int FONT_COLOR_NORMAL             = 0;
   int FONT_COLOR_SEL                = 0;

   char string[11]                   = {0};
   unsigned theme                    = opt_vkbd_theme;
   if (theme & 0x80)
   {
      text_outline = true;
      theme &= ~0x80;
   }

   if (!theme)
   {
      switch (currprefs.cs_compatible)
      {
         case CP_CD32:
            theme = 2;
            break;
         case CP_CDTV:
            theme = 3;
            break;
         default:
            theme = 1;
            break;
      }
   }

   switch (theme)
   {
      default:
      case 1: /* Classic */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_BEIGE_32 : COLOR_BEIGE_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_BEIGEDARK_32 : COLOR_BEIGEDARK_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_100_32 : COLOR_100_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_40_32 : COLOR_40_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_250_32 : COLOR_250_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         break;

      case 4: /* Light */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_220_32 : COLOR_220_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_180_32 : COLOR_180_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_100_32 : COLOR_100_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_40_32 : COLOR_40_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_250_32 : COLOR_250_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         break;

      case 2: /* CD32 */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_64_32 : COLOR_64_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_32_32 : COLOR_32_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_16_32 : COLOR_16_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_180_32 : COLOR_180_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_10_32 : COLOR_10_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         break;

      case 3: /* Dark */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_32_32 : COLOR_32_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_64_32 : COLOR_64_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_16_32 : COLOR_16_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_180_32 : COLOR_180_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_10_32 : COLOR_10_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         break;
   }

   if (video_config_geometry & PUAE_VIDEO_HIRES || video_config_geometry & PUAE_VIDEO_SUPERHIRES)
   {
      FONT_WIDTH         = 2;
      XKEYSPACING        = 2;
      XOFFSET            = -1;

      if (video_config_geometry & PUAE_VIDEO_DOUBLELINE)
      {
         /* PUAE_VIDEO_HIRES_DOUBLELINE */
         FONT_HEIGHT    *= 2;
         YKEYSPACING    *= 2;
         YPADDING       *= 2;
      }

      /* PUAE_VIDEO_SUPERHIRES */
      if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
      {
         FONT_WIDTH     *= 2;
         XKEYSPACING    *= 2;
         XOFFSET        *= 2;
      }
   }

   int XSIDE     = (320 * FONT_WIDTH) / VKBDX;
   int YSIDE     = 21 * FONT_HEIGHT;

   XPADDING      = zoomed_width - (XSIDE * VKBDX);
   YPADDING      = zoomed_height - (YSIDE * VKBDY);

   int XBASEKEY  = (XPADDING > 0) ? (XPADDING / 2) : 0;
   int YBASEKEY  = (YPADDING > 0) ? (YPADDING / 2) : 0;

   int XBASETEXT = XBASEKEY + (3 * FONT_WIDTH);
   int YBASETEXT = YBASEKEY + (3 * FONT_HEIGHT);

   /* Coordinates */
   vkbd_x_min = XOFFSET + XBASEKEY + XKEYSPACING;
   vkbd_x_max = XOFFSET - XBASEKEY - XKEYSPACING + zoomed_width;
   vkbd_y_min = YOFFSET + YBASEKEY + YKEYSPACING;
   vkbd_y_max = YOFFSET + YBASEKEY + (YSIDE * VKBDY);

   /* Opacity */
   BKG_ALPHA = (retro_vkbd_transparent) ? ALPHA : GRAPH_ALPHA_100;

   /* Alternate color keys */
   int alt_keys[] =
   {
      RETROK_F1, RETROK_F2, RETROK_F3, RETROK_F4, RETROK_F5, RETROK_F6, RETROK_F7, RETROK_F8, RETROK_F9, RETROK_F10,
      RETROK_LSHIFT, RETROK_RSHIFT, RETROK_LALT, RETROK_RALT, RETROK_LSUPER, RETROK_RSUPER, -15 /*LMB*/, -16 /*RMB*/,
      RETROK_BACKSPACE, RETROK_DELETE, RETROK_INSERT, RETROK_HOME, RETROK_END,
      RETROK_ESCAPE, RETROK_BACKQUOTE, RETROK_TAB, RETROK_LCTRL, RETROK_CAPSLOCK, RETROK_RETURN, RETROK_KP_ENTER,
      RETROK_KP_MINUS, RETROK_KP_PLUS, RETROK_KP_DIVIDE, RETROK_KP_MULTIPLY, RETROK_KP_PERIOD,
   };
   int alt_keys_len = sizeof(alt_keys) / sizeof(alt_keys[0]);

   /* Extra color keys */
   int extra_keys[] =
   {
       -2 /*NUMPAD*/, -18 /*J/M*/, -19 /*TRBF*/, -22 /*ASPR/ZOOM*/, -21 /*STBR*/,
   };
   int extra_keys_len = sizeof(extra_keys) / sizeof(extra_keys[0]);

   /* Key label shifted */
   shifted = false;
   if (retro_capslock || vkey_sticky1 == RETROK_LSHIFT || vkey_sticky2 == RETROK_LSHIFT || vkey_sticky1 == RETROK_RSHIFT || vkey_sticky2 == RETROK_RSHIFT)
      shifted = true;
   if (vkflag[RETRO_DEVICE_ID_JOYPAD_B] == 1 && (vkey_pressed == RETROK_LSHIFT || vkey_pressed == RETROK_RSHIFT))
      shifted = true;

   /* Key layout */
   for (x = 0; x < VKBDX; x++)
   {
      for (y = 0; y < VKBDY; y++)
      {
         /* Skip selected key */
         if (((vkey_pos_y * VKBDX) + vkey_pos_x + page) == ((y * VKBDX) + x + page))
            continue;

         /* Default key color */
         BKG_COLOR = BKG_COLOR_NORMAL;
         BKG_ALPHA = (retro_vkbd_transparent) ? ALPHA : GRAPH_ALPHA_100;

         /* Reset key color */
         if (vkeys[(y * VKBDX) + x].value == -20)
            BKG_COLOR = RGBc(128, 0, 0);
         else
         {
            /* Alternate key color */
            for (int alt_key = 0; alt_key < alt_keys_len; ++alt_key)
                if (alt_keys[alt_key] == vkeys[(y * VKBDX) + x + page].value)
                    BKG_COLOR = BKG_COLOR_ALT;

            /* Extra key color */
            for (int extra_key = 0; extra_key < extra_keys_len; ++extra_key)
                if (extra_keys[extra_key] == vkeys[(y * VKBDX) + x + page].value)
                    BKG_COLOR = BKG_COLOR_EXTRA;
         }

         /* Key positions */
         XKEY  = XOFFSET + XBASEKEY + (x * XSIDE);
         XTEXT = XOFFSET + XBASETEXT + BKG_PADDING_X + (x * XSIDE);
         YKEY  = YOFFSET + YBASEKEY + (y * YSIDE);
         YTEXT = YOFFSET + YBASETEXT + BKG_PADDING_Y + (y * YSIDE);

         /* Default font color */
         FONT_COLOR = FONT_COLOR_NORMAL;

         /* Pressed keys via mapper */
         int current_key = vkeys[(y * VKBDX) + x + page].value;
         current_key = (current_key > 0) ? current_key : 0;

         /* Sticky + CapsLock + pressed colors */
         if ( (vkey_sticky1 == vkeys[(y * VKBDX) + x + page].value
          ||   vkey_sticky2 == vkeys[(y * VKBDX) + x + page].value
          ||(retro_capslock && vkeys[(y * VKBDX) + x + page].value == RETROK_CAPSLOCK)
          ||(retro_key_state_internal[current_key])
          ||(vkflag[RETRO_DEVICE_ID_JOYPAD_START] && vkeys[(y * VKBDX) + x + page].value == RETROK_RETURN)
          ||(vkflag[RETRO_DEVICE_ID_JOYPAD_X]     && vkeys[(y * VKBDX) + x + page].value == RETROK_SPACE))
          && BKG_COLOR != BKG_COLOR_EXTRA && vkeys[(y * VKBDX) + x + page].value != -20)
         {
            FONT_COLOR = FONT_COLOR_NORMAL;
            BKG_COLOR  = BKG_COLOR_ACTIVE;

            switch (BKG_ALPHA)
            {
               case GRAPH_ALPHA_0:
               case GRAPH_ALPHA_25:
               case GRAPH_ALPHA_50:
                  BKG_ALPHA = GRAPH_ALPHA_75;
                  break;
               case GRAPH_ALPHA_75:
               case GRAPH_ALPHA_100:
               default:
                  /* Do nothing */
                  break;
            }
         }

         /* Key background */
         draw_fbox(XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
                   BKG_COLOR, BKG_ALPHA);

         /* Key text */
         draw_text(XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, GRAPH_ALPHA_25,
                   (text_outline) ? GRAPH_BG_OUTLINE : GRAPH_BG_SHADOW, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                   (!shifted) ? vkeys[(y * VKBDX) + x + page].normal : vkeys[(y * VKBDX) + x + page].shift);
      }
   }

   /* Selected key position */
   XKEY  = XOFFSET + XBASEKEY + (vkey_pos_x * XSIDE);
   XTEXT = XOFFSET + XBASETEXT + BKG_PADDING_X + (vkey_pos_x * XSIDE);
   YKEY  = YOFFSET + YBASEKEY + (vkey_pos_y * YSIDE);
   YTEXT = YOFFSET + YBASETEXT + BKG_PADDING_Y + (vkey_pos_y * YSIDE);

   /* Opacity */
   BKG_ALPHA = (retro_vkbd_transparent) ?
         ((BKG_ALPHA == GRAPH_ALPHA_100) ? GRAPH_ALPHA_100 : GRAPH_ALPHA_75) : GRAPH_ALPHA_100;

   /* Pressed key color */
   if (vkflag[RETRO_DEVICE_ID_JOYPAD_B] && (vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].value == vkey_sticky1 ||
                                            vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].value == vkey_sticky2))
      ; /* no-op */
   else if (vkflag[RETRO_DEVICE_ID_JOYPAD_B])
      BKG_COLOR_SEL = BKG_COLOR_ACTIVE;
   else
      FONT_COLOR = FONT_COLOR_SEL;

   if (vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].value == -20) /* Reset */
   {
      signed char reset_counter = 0;
      if (last_vkey_pressed_time < now && last_vkey_pressed != -1)
         reset_counter = (VKBD_STICKY_HOLDING_TIME - (now - last_vkey_pressed_time)) / 100;
      if (reset_counter < 0)
         reset_counter = 0;

      if (last_vkey_pressed != -1 && reset_counter == 0)
      {
         FONT_COLOR = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         BKG_COLOR_SEL = RGBc(128, 0, 0);
      }

      if (reset_counter > 0)
         snprintf(string, sizeof(string), "%1d", reset_counter);
      else
         snprintf(string, sizeof(string), "%s",
               (!shifted) ? vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].normal
                          : vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].shift);
   }
   else
      snprintf(string, sizeof(string), "%s",
            (!shifted) ? vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].normal
                       : vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].shift);

   /* Selected key background */
   draw_fbox(XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
             BKG_COLOR_SEL, BKG_ALPHA);

   /* Selected key text */
   draw_text(XTEXT, YTEXT, FONT_COLOR, 0, GRAPH_ALPHA_100,
             GRAPH_BG_NONE, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
             string);

#ifdef POINTER_DEBUG
   draw_hline(pointer_x, pointer_y, 1, 1, RGBc(255, 0, 255));
#endif
}

static void input_vkbd_sticky(void)
{
   if (vkey_sticky && last_vkey_pressed != -1 && last_vkey_pressed > 0)
   {
      if (vkey_sticky1 > -1 && vkey_sticky1 != last_vkey_pressed)
      {
         if (vkey_sticky2 > -1 && vkey_sticky2 != last_vkey_pressed)
            retro_key_up(vkey_sticky2);
         vkey_sticky2 = last_vkey_pressed;
      }
      else
         vkey_sticky1 = last_vkey_pressed;
   }

   /* Keyup only after button is up */
   if (last_vkey_pressed != -1 && !vkflag[RETRO_DEVICE_ID_JOYPAD_B])
   {
      if (last_vkey_pressed < -10)
      {
         if (last_vkey_pressed == -15)
         {
            retro_mouse_button(0, 0, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_B] = 0;
         }
         else if (last_vkey_pressed == -16)
         {
            retro_mouse_button(0, 1, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_A] = 0;
         }
         else if (last_vkey_pressed == -17)
         {
            retro_mouse_button(0, 2, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_Y] = 0;
         }
      }
      else
      {
         if (vkey_pressed == -1 && last_vkey_pressed >= 0 && last_vkey_pressed != vkey_sticky1 && last_vkey_pressed != vkey_sticky2)
            retro_key_up(last_vkey_pressed);

         if (retro_capslock)
            retro_key_up(RETROK_LSHIFT);
      }

      last_vkey_pressed = -1;
   }

   if (vkey_sticky1_release)
   {
      vkey_sticky1_release = 0;
      vkey_sticky1 = -1;
      retro_key_up(vkey_sticky1);
   }
   if (vkey_sticky2_release)
   {
      vkey_sticky2_release = 0;
      vkey_sticky2 = -1;
      retro_key_up(vkey_sticky2);
   }
}

void toggle_vkbd(void)
{
   /* No toggling while key is pressed */
   if (vkflag[RETRO_DEVICE_ID_JOYPAD_B])
      return;
   retro_vkbd = !retro_vkbd;
   /* Reset VKBD input readiness */
   retro_vkbd_ready = -2;
   /* Release VKBD controllable joypads */
   memset(joypad_bits, 0, 2*sizeof(joypad_bits[0]));
}

void input_vkbd(void)
{
   long now = 0;
   unsigned int i = 0;

   input_vkbd_sticky();

   if (!retro_vkbd)
      return;

   /* Wait for all inputs to be released */
   if (retro_vkbd_ready < 1)
   {
      if ((retro_vkbd_ready == 0 && !joypad_bits[0] && !joypad_bits[1]) ||
           retro_vkbd_ready < 0)
         retro_vkbd_ready++;
      return;
   }

   now = retro_ticks() / 1000;

   /* VKBD Mouse acceleration */
   const int mspeed_default = 2;
   static int mspeed;
   if (!vkflag[RETRO_DEVICE_ID_JOYPAD_B])
      mspeed = mspeed_default;

   /* Absolute pointer */
   int p_x = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
   int p_y = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);

   if (p_x != 0 && p_y != 0 && (p_x != last_pointer_x || p_y != last_pointer_y))
   {
      int px = (int)((p_x + 0x7fff) * zoomed_width / 0xffff);
      int py = (int)((p_y + 0x7fff) * zoomed_height / 0xffff);
      last_pointer_x = p_x;
      last_pointer_y = p_y;
#ifdef POINTER_DEBUG
      pointer_x = px;
      pointer_y = py;
#endif
      if (px >= vkbd_x_min && px <= vkbd_x_max && py >= vkbd_y_min && py <= vkbd_y_max)
      {
         float vkey_width = (float)(vkbd_x_max - vkbd_x_min) / VKBDX;
         vkey_pos_x = ((px - vkbd_x_min) / vkey_width);

         float vkey_height = (float)(vkbd_y_max - vkbd_y_min) / VKBDY;
         vkey_pos_y = ((py - vkbd_y_min) / vkey_height);

         vkey_pos_x = (vkey_pos_x < 0) ? 0 : vkey_pos_x;
         vkey_pos_x = (vkey_pos_x > VKBDX - 1) ? VKBDX - 1 : vkey_pos_x;
         vkey_pos_y = (vkey_pos_y < 0) ? 0 : vkey_pos_y;
         vkey_pos_y = (vkey_pos_y > VKBDY - 1) ? VKBDY - 1 : vkey_pos_y;

#ifdef POINTER_DEBUG
         printf("px:%d py:%d (%d,%d) vkey:%dx%d\n", p_x, p_y, px, py, vkey_pos_x, vkey_pos_y);
#endif
      }
   }

   /* Press Return, RetroPad Start */
   i = RETRO_DEVICE_ID_JOYPAD_START;
   if (!vkflag[i] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                             (joypad_bits[1] & (1 << i)))
                                         && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN))
   {
      vkflag[i] = 1;
      retro_key_down(RETROK_RETURN);
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 0;
      retro_key_up(RETROK_RETURN);
   }

   /* Toggle CapsLock, RetroPad Y */
   i = RETRO_DEVICE_ID_JOYPAD_Y;
   if (!vkflag[i] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                             (joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 1;
      retro_capslock = !retro_capslock;
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 0;
   }

   /* Press Space, RetroPad X */
   i = RETRO_DEVICE_ID_JOYPAD_X;
   if (!vkflag[i] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                             (joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 1;
      retro_key_down(RETROK_SPACE);
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 0;
      retro_key_up(RETROK_SPACE);
   }

   /* Toggle transparency, RetroPad A */
   i = RETRO_DEVICE_ID_JOYPAD_A;
   if (!vkflag[i] && mapper_keys[i] >= 0 && ((joypad_bits[0] & (1 << i)) ||
                                             (joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 1;
      retro_vkbd_transparent = !retro_vkbd_transparent;
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 0;
   }

   /* Key press, RetroPad B joyports 1+2 / Keyboard Enter / Pointer */
   i = RETRO_DEVICE_ID_JOYPAD_B;
   if (!vkflag[i] && ((joypad_bits[0] & (1 << i)) ||
                      (joypad_bits[1] & (1 << i)) ||
                      input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN) ||
                      input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)))
   {
      vkflag[i] = 1;
      vkey_pressed = vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + ((retro_vkbd_page) ? VKBDX * VKBDY : 0)].value;

      if (vkey_pressed == -1)
         last_vkey_pressed = -1;
      else if (vkey_pressed == -2)
      {
         last_vkey_pressed = -1;
         retro_vkbd_page = !retro_vkbd_page;
      }
      else if (vkey_pressed < -10)
      {
         last_vkey_pressed = vkey_pressed;
         last_vkey_pressed_time = now;

         if (vkey_pressed == -11) /* Mouse up */
         {
            retro_mouse(0, 0, -3);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_UP] = 1;
         }
         else if (vkey_pressed == -12) /* Mouse down */
         {
            retro_mouse(0, 0, 3);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
         }
         else if (vkey_pressed == -13) /* Mouse left */
         {
            retro_mouse(0, -3, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
         }
         else if (vkey_pressed == -14) /* Mouse right */
         {
            retro_mouse(0, 3, 0);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
         }
         else if (vkey_pressed == -15) /* LMB */
         {
            retro_mouse_button(0, 0, 1);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_B] = 1;
         }
         else if (vkey_pressed == -16) /* RMB */
         {
            retro_mouse_button(0, 1, 1);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_A] = 1;
         }
         else if (vkey_pressed == -17) /* MMB */
         {
            retro_mouse_button(0, 2, 1);
            mflag[0][RETRO_DEVICE_ID_JOYPAD_Y] = 1;
         }
         else if (vkey_pressed == -18) /* Toggle joystick/mouse */
         {
            emu_function(EMU_JOYMOUSE);
         }
         else if (vkey_pressed == -19) /* Toggle turbo fire */
         {
            emu_function(EMU_TURBO_FIRE);
         }
         else if (vkey_pressed == -20) /* Reset */
         {
            /* Reset on release */
         }
         else if (vkey_pressed == -21) /* Toggle statusbar / save disk */
         {
            if (retro_capslock)
               emu_function(EMU_SAVE_DISK);
            else
               emu_function(EMU_STATUSBAR);
         }
         else if (vkey_pressed == -22) /* Toggle aspect ratio / zoom mode */
         {
            if (retro_capslock)
               emu_function(EMU_ZOOM_MODE);
            else
               emu_function(EMU_ASPECT_RATIO);
         }
      }
      else
      {
         if (vkey_pressed == RETROK_CAPSLOCK)
         {
            retro_key_down(vkey_pressed);
            retro_key_up(vkey_pressed);
            retro_capslock = !retro_capslock;
            last_vkey_pressed = -1;
         }
         else
         {
            last_vkey_pressed = vkey_pressed;
            if (retro_capslock)
               retro_key_down(RETROK_LSHIFT);

            if (!vkey_sticky)
            {
               if (vkey_pressed == vkey_sticky1)
                  vkey_sticky1_release = 1;
               if (vkey_pressed == vkey_sticky2)
                  vkey_sticky2_release = 1;
            }
            retro_key_down(vkey_pressed);
         }
      }
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i)) &&
                     !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN) &&
                     !input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)))
   {
      vkey_pressed = -1;
      vkflag[i] = 0;

      if (last_vkey_pressed > -10 && last_vkey_pressed < -15)
      {
         mspeed = mspeed_default;

         if (vkey_pressed == -11) /* Mouse up */
            mflag[0][RETRO_DEVICE_ID_JOYPAD_UP] = 0;
         else if (vkey_pressed == -12) /* Mouse down */
            mflag[0][RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;
         else if (vkey_pressed == -13) /* Mouse left */
            mflag[0][RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;
         else if (vkey_pressed == -14) /* Mouse right */
            mflag[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
      }
      else if (last_vkey_pressed == -20)
      {
         /* Reset on long press */
         if (now - last_vkey_pressed_time > VKBD_STICKY_HOLDING_TIME)
            emu_function(EMU_RESET);
      }
   }
   else
   if (vkflag[i] && ((joypad_bits[0] & (1 << i)) ||
                     (joypad_bits[1] & (1 << i)) ||
                     input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN) ||
                     input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)))
   {
      if (last_vkey_pressed != -20)
      {
         if (now - last_vkey_pressed_time > 200)
         {
            mspeed++;
            last_vkey_pressed_time = now;
         }

         if (vkey_pressed == -11) /* Mouse up */
            retro_mouse(0, 0, -mspeed);
         else if (vkey_pressed == -12) /* Mouse down */
            retro_mouse(0, 0, mspeed);
         else if (vkey_pressed == -13) /* Mouse left */
            retro_mouse(0, -mspeed, 0);
         else if (vkey_pressed == -14) /* Mouse right */
            retro_mouse(0, mspeed, 0);
      }
   }

   if (vkflag[i] && vkey_pressed > 0)
   {
      if (let_go_of_button)
         last_press_time_button = now;
      if (now - last_press_time_button > VKBD_STICKY_HOLDING_TIME)
         vkey_sticky = 1;
      let_go_of_button = false;
   }
   else
   {
      let_go_of_button = true;
      vkey_sticky = 0;
   }

   /* Allow directions when key is not pressed */
   if (!vkflag[RETRO_DEVICE_ID_JOYPAD_B])
   {
      if (!vkflag[RETRO_DEVICE_ID_JOYPAD_UP] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
                                                 (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) ||
                                                 input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_UP)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_UP] = 1;
      else
      if (vkflag[RETRO_DEVICE_ID_JOYPAD_UP] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) &&
                                                !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) &&
                                                !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_UP)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_UP] = 0;

      if (!vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
                                                   (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) ||
                                                   input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_DOWN)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN] = 1;
      else
      if (vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) &&
                                                  !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) &&
                                                  !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_DOWN)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN] = 0;

      if (!vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
                                                   (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) ||
                                                   input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LEFT)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT] = 1;
      else
      if (vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) &&
                                                  !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) &&
                                                  !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LEFT)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT] = 0;

      if (!vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT] && ((joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) ||
                                                    (joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) ||
                                                    input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RIGHT)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT] = 1;
      else
      if (vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT] && (!(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) &&
                                                   !(joypad_bits[1] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) &&
                                                   !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RIGHT)))
         vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
   }
   else /* Release all directions when key is pressed */
   {
      vkflag[RETRO_DEVICE_ID_JOYPAD_UP]    = 0;
      vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN]  = 0;
      vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT]  = 0;
      vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT] = 0;
   }

   if (vkflag[RETRO_DEVICE_ID_JOYPAD_UP] ||
       vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN] ||
       vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT] ||
       vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT])
   {
      if (let_go_of_direction)
         /* just pressing down */
         last_press_time = now;

      if ((now - last_press_time > VKBD_MIN_HOLDING_TIME
        && now - last_move_time > VKBD_MOVE_DELAY)
        || let_go_of_direction)
      {
         last_move_time = now;

         if (vkflag[RETRO_DEVICE_ID_JOYPAD_UP])
            vkey_pos_y -= 1;
         else if (vkflag[RETRO_DEVICE_ID_JOYPAD_DOWN])
            vkey_pos_y += 1;

         if (vkflag[RETRO_DEVICE_ID_JOYPAD_LEFT])
            vkey_pos_x -= 1;
         else if (vkflag[RETRO_DEVICE_ID_JOYPAD_RIGHT])
            vkey_pos_x += 1;
      }
      let_go_of_direction = false;
   }
   else
      let_go_of_direction = true;

   if (vkey_pos_x < 0)
      vkey_pos_x = VKBDX - 1;
   else if (vkey_pos_x > VKBDX - 1)
      vkey_pos_x = 0;
   if (vkey_pos_y < 0)
      vkey_pos_y = VKBDY - 1;
   else if (vkey_pos_y > VKBDY - 1)
      vkey_pos_y = 0;

#if 0
   printf("vkey:%d sticky:%d sticky1:%d sticky2:%d, now:%d last:%d\n", vkey_pressed, vkey_sticky, vkey_sticky1, vkey_sticky2, now, last_press_time_button);
#endif
}
