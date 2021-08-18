#include "keyboard.h"
#include "libretro-core.h"
#include "libretro-graph.h"
#include "libretro-vkbd.h"
#include "libretro-mapper.h"

#include "options.h"

bool retro_vkbd = false;
bool retro_vkbd_page = false;
static bool retro_vkbd_transparent = true;
short int retro_vkbd_ready = 0;
int vkflag[10] = {0};

#ifdef POINTER_DEBUG
int pointer_x = 0;
int pointer_y = 0;
#endif
int last_pointer_x = 0;
int last_pointer_y = 0;

int vkey_pos_x = 0;
int vkey_pos_y = 0;
int vkbd_x_min = 0;
int vkbd_x_max = 0;
int vkbd_y_min = 0;
int vkbd_y_max = 0;

/* VKBD_MIN_HOLDING_TIME: Hold a direction longer than this and automatic movement sets in */
/* VKBD_MOVE_DELAY: Delay between automatic movement from button to button */
#define VKBD_MIN_HOLDING_TIME 200
#define VKBD_MOVE_DELAY 50
bool let_go_of_direction = true;
long last_move_time = 0;
long last_press_time = 0;

/* VKBD_STICKY_HOLDING_TIME: Button press longer than this triggers sticky key */
#define VKBD_STICKY_HOLDING_TIME 1000
bool let_go_of_button = true;
long last_press_time_button = 0;
int vkey_pressed = -1;
int vkey_sticky = -1;
int vkey_sticky1 = -1;
int vkey_sticky2 = -1;

long last_vkey_pressed_time = 0;
int last_vkey_pressed = -1;
int vkey_sticky1_release = 0;
int vkey_sticky2_release = 0;



void print_vkbd(void)
{
   libretro_graph_alpha_t ALPHA      = opt_vkbd_alpha;
   libretro_graph_alpha_t BKG_ALPHA  = ALPHA;
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
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_132_32 : COLOR_132_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_40_32 : COLOR_40_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_250_32 : COLOR_250_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         break;

      case 2: /* CD32 */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_64_32 : COLOR_64_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_32_32 : COLOR_32_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_16_32 : COLOR_16_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_140_32 : COLOR_140_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_10_32 : COLOR_10_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         break;

      case 3: /* Dark */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_32_32 : COLOR_32_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_64_32 : COLOR_64_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_10_32 : COLOR_10_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_140_32 : COLOR_140_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_16_32 : COLOR_16_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         break;

      case 4: /* Light */
         BKG_COLOR_NORMAL  = (pix_bytes == 4) ? COLOR_200_32 : COLOR_200_16;
         BKG_COLOR_ALT     = (pix_bytes == 4) ? COLOR_160_32 : COLOR_160_16;
         BKG_COLOR_EXTRA   = (pix_bytes == 4) ? COLOR_132_32 : COLOR_132_16;
         BKG_COLOR_SEL     = (pix_bytes == 4) ? COLOR_40_32 : COLOR_40_16;
         BKG_COLOR_ACTIVE  = (pix_bytes == 4) ? COLOR_250_32 : COLOR_250_16;
         FONT_COLOR_NORMAL = (pix_bytes == 4) ? COLOR_BLACK_32 : COLOR_BLACK_16;
         FONT_COLOR_SEL    = (pix_bytes == 4) ? COLOR_WHITE_32 : COLOR_WHITE_16;
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
      AK_F1, AK_F2, AK_F3, AK_F4, AK_F5, AK_F6, AK_F7, AK_F8, AK_F9, AK_F10,
      AK_LSH, AK_RSH, AK_LALT, AK_RALT, AK_LAMI, AK_RAMI, -15 /*LMB*/, -16 /*RMB*/,
      AK_ESC, AK_BACKQUOTE, AK_TAB, AK_CTRL, AK_CAPSLOCK, AK_RET, AK_ENT, AK_BS, AK_DEL, AK_NPDEL, AK_HELP,
      AK_NPSUB, AK_NPADD, AK_NPDIV, AK_NPLPAREN, AK_NPRPAREN, AK_NPMUL
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
   if (retro_capslock || vkey_sticky1 == AK_LSH || vkey_sticky2 == AK_LSH || vkey_sticky1 == AK_RSH || vkey_sticky2 == AK_RSH)
      shifted = true;
   if (vkflag[RETRO_DEVICE_ID_JOYPAD_B] == 1 && (vkey_pressed == AK_LSH || vkey_pressed == AK_RSH))
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

         /* Sticky + CapsLock + pressed colors */
         if ( (vkey_sticky1 == vkeys[(y * VKBDX) + x + page].value
          ||   vkey_sticky2 == vkeys[(y * VKBDX) + x + page].value
          ||(retro_capslock && vkeys[(y * VKBDX) + x + page].value == AK_CAPSLOCK)
          ||(vkflag[RETRO_DEVICE_ID_JOYPAD_START] && vkeys[(y * VKBDX) + x + page].value == AK_RET)
          ||(vkflag[RETRO_DEVICE_ID_JOYPAD_X]     && vkeys[(y * VKBDX) + x + page].value == AK_SPC))
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

   /* Selected key background */
   draw_fbox(XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
             BKG_COLOR_SEL, BKG_ALPHA);

   /* Selected key text */
   draw_text(XTEXT, YTEXT, FONT_COLOR, 0, GRAPH_ALPHA_100,
             GRAPH_BG_NONE, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
             (!shifted) ? vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].normal
                        : vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].shift);

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
            retro_key_up(keyboard_translation[RETROK_LSHIFT]);
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
   const int mspeed_default = 3;
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
      retro_key_down(AK_RET);
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 0;
      retro_key_up(AK_RET);
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
      retro_key_down(AK_SPC);
   }
   else
   if (vkflag[i] && (!(joypad_bits[0] & (1 << i)) &&
                     !(joypad_bits[1] & (1 << i))))
   {
      vkflag[i] = 0;
      retro_key_up(AK_SPC);
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
         if (vkey_pressed == AK_CAPSLOCK)
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
               retro_key_down(keyboard_translation[RETROK_LSHIFT]);

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
         if (now - last_vkey_pressed_time > 500)
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
         if (now - last_vkey_pressed_time > 100)
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
