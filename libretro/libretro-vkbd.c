#include "libretro-core.h"
#include "libretro-graph.h"
#include "keyboard.h"
#include "libretro-vkbd.h"

#include "options.h"

bool retro_vkbd = false;
bool retro_vkbd_page = false;
bool retro_vkbd_position = false;
bool retro_vkbd_transparent = true;
extern bool retro_capslock;
extern int vkflag[10];
extern unsigned int video_config_geometry;
extern unsigned int opt_vkbd_theme;
extern libretro_graph_alpha_t opt_vkbd_alpha;

int RGBc(int r, int g, int b)
{
   if (pix_bytes == 4)
      return ARGB888(255, r, g, b);
   else
      return RGB565(r, g, b);
}

void print_vkbd(unsigned short int *pixels)
{
   uint16_t *pix                     = &pixels[0];
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
   int YPADDING                      = 6;
   int XKEYSPACING                   = 1;
   int YKEYSPACING                   = 1;
   libretro_graph_alpha_t ALPHA      = opt_vkbd_alpha;
   libretro_graph_alpha_t BKG_ALPHA  = ALPHA;
   int BKG_PADDING_X                 = 0;
   int BKG_PADDING_Y                 = 0;
   int BKG_COLOR                     = 0;
   int BKG_COLOR_NORMAL              = 0;
   int BKG_COLOR_ALT                 = 0;
   int BKG_COLOR_EXTRA               = 0;
   int BKG_COLOR_SEL                 = 0;
   int BKG_COLOR_ACTIVE              = 0;
   int FONT_MAX                      = 4;
   int FONT_WIDTH                    = 1;
   int FONT_HEIGHT                   = 1;
   int FONT_COLOR                    = 0;
   int FONT_COLOR_NORMAL             = 0;
   int FONT_COLOR_SEL                = 0;

   unsigned COLOR_BLACK              = RGBc(  5,   5,   5);
   unsigned COLOR_GRAYBLACK          = RGBc( 25,  25,  25);
   unsigned COLOR_GRAYWHITE          = RGBc(125, 125, 125);
   unsigned COLOR_WHITE              = RGBc(250, 250, 250);

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
         BKG_COLOR_NORMAL  = RGBc(216, 209, 201);
         BKG_COLOR_ALT     = RGBc(159, 154, 150);
         BKG_COLOR_EXTRA   = RGBc(143, 140, 129);
         BKG_COLOR_SEL     = RGBc( 60,  60,  60);
         BKG_COLOR_ACTIVE  = RGBc(250, 250, 250);
         FONT_COLOR_NORMAL = COLOR_BLACK;
         FONT_COLOR_SEL    = COLOR_WHITE;
         break;

      case 2: /* CD32 */
         BKG_COLOR_NORMAL  = RGBc( 64,  64,  64);
         BKG_COLOR_ALT     = RGBc( 32,  32,  32);
         BKG_COLOR_EXTRA   = RGBc( 16,  16,  16);
         BKG_COLOR_SEL     = RGBc(140, 140, 140);
         BKG_COLOR_ACTIVE  = RGBc( 10,  10,  10);
         FONT_COLOR_NORMAL = COLOR_WHITE;
         FONT_COLOR_SEL    = COLOR_BLACK;
         break;

      case 3: /* Dark */
         BKG_COLOR_NORMAL  = RGBc( 32,  32,  32);
         BKG_COLOR_ALT     = RGBc( 70,  70,  70);
         BKG_COLOR_EXTRA   = RGBc( 14,  14,  14);
         BKG_COLOR_SEL     = RGBc(140, 140, 140);
         BKG_COLOR_ACTIVE  = RGBc( 16,  16,  16);
         FONT_COLOR_NORMAL = COLOR_WHITE;
         FONT_COLOR_SEL    = COLOR_BLACK;
         break;

      case 4: /* Light */
         BKG_COLOR_NORMAL  = RGBc(210, 210, 210);
         BKG_COLOR_ALT     = RGBc(180, 180, 180);
         BKG_COLOR_EXTRA   = RGBc(150, 150, 150);
         BKG_COLOR_SEL     = RGBc( 60,  60,  60);
         BKG_COLOR_ACTIVE  = RGBc(250, 250, 250);
         FONT_COLOR_NORMAL = COLOR_BLACK;
         FONT_COLOR_SEL    = COLOR_WHITE;
         break;
   }

   if (video_config_geometry & PUAE_VIDEO_HIRES || video_config_geometry & PUAE_VIDEO_SUPERHIRES)
   {
      FONT_WIDTH         = 2;
      XKEYSPACING        = 2;

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
      }
   }

   XPADDING      = zoomed_width - (320 * FONT_WIDTH);

   int XSIDE     = (zoomed_width - XPADDING) / VKBDX;
   int YSIDE     = (zoomed_height - YPADDING) / VKBDY;
   int YSIDE_MAX = 21 * FONT_HEIGHT;
   YSIDE         = (YSIDE > YSIDE_MAX) ? YSIDE_MAX : YSIDE;

   /* Position toggle */
   if (retro_vkbd_position && zoomed_height > (YSIDE * VKBDY) + (YPADDING * 6))
      YOFFSET = -zoomed_height + ((YSIDE * VKBDY) + (YPADDING * 3));
   else
      YOFFSET = -(YPADDING * 2);

   int XBASEKEY  = (XPADDING > 0) ? (XPADDING / 2) : 0;
   int YBASEKEY  = (zoomed_height - (YSIDE * VKBDY)) - (YPADDING / 2);

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
          ||(vkflag[RETRO_DEVICE_ID_JOYPAD_START] && vkeys[(y * VKBDX) + x + page].value == AK_RET))
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
         if (pix_bytes == 4)
            DrawFBoxBmp32((uint32_t *)pix,
                          XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
                          BKG_COLOR, BKG_ALPHA);
         else
            DrawFBoxBmp(pix,
                          XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
                          BKG_COLOR, BKG_ALPHA);

         /* Key text shadow */
         if (text_outline)
         {
            for (int sx = -1; sx < 2; sx++)
            {
               for (int sy = -1; sy < 2; sy++)
               {
                  if (sx == 0 && sy == 0)
                     continue;
                  if (pix_bytes == 4)
                     Draw_text32((uint32_t *)pix,
                                 XTEXT+(sx*FONT_WIDTH),
                                 YTEXT+(sy*FONT_HEIGHT),
                                 BKG_COLOR,
                                 (FONT_COLOR == COLOR_WHITE ? COLOR_GRAYBLACK : COLOR_GRAYWHITE),
                                 GRAPH_ALPHA_75+(-sx-sy), false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                                 (!shifted) ? vkeys[(y * VKBDX) + x + page].normal : vkeys[(y * VKBDX) + x + page].shift);
                  else
                     Draw_text(pix,
                                 XTEXT+(sx*FONT_WIDTH),
                                 YTEXT+(sy*FONT_HEIGHT),
                                 BKG_COLOR,
                                 (FONT_COLOR == COLOR_WHITE ? COLOR_GRAYBLACK : COLOR_GRAYWHITE),
                                 GRAPH_ALPHA_75+(-sx-sy), false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                                 (!shifted) ? vkeys[(y * VKBDX) + x + page].normal : vkeys[(y * VKBDX) + x + page].shift);
               }
            }
         }
         else
         {
            if (pix_bytes == 4)
               Draw_text32((uint32_t *)pix,
                           XTEXT+1,
                           YTEXT+1,
                           BKG_COLOR,
                           (FONT_COLOR == COLOR_WHITE ? COLOR_GRAYBLACK : COLOR_GRAYWHITE),
                           GRAPH_ALPHA_75, false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                           (!shifted) ? vkeys[(y * VKBDX) + x + page].normal : vkeys[(y * VKBDX) + x + page].shift);
            else
               Draw_text(pix,
                           XTEXT+1,
                           YTEXT+1,
                           BKG_COLOR,
                           (FONT_COLOR == COLOR_WHITE ? COLOR_GRAYBLACK : COLOR_GRAYWHITE),
                           GRAPH_ALPHA_75, false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                           (!shifted) ? vkeys[(y * VKBDX) + x + page].normal : vkeys[(y * VKBDX) + x + page].shift);
         }

         /* Key text */
         if (pix_bytes == 4)
            Draw_text32((uint32_t *)pix,
                        XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, GRAPH_ALPHA_100, false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                        (!shifted) ? vkeys[(y * VKBDX) + x + page].normal : vkeys[(y * VKBDX) + x + page].shift);
         else
            Draw_text(pix,
                        XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, GRAPH_ALPHA_100, false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
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
   if (pix_bytes == 4)
      DrawFBoxBmp32((uint32_t *)pix,
                    XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
                    BKG_COLOR_SEL, BKG_ALPHA);
   else
      DrawFBoxBmp(pix,
                    XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING,
                    BKG_COLOR_SEL, BKG_ALPHA);

   /* Selected key text */
   if (pix_bytes == 4)
      Draw_text32((uint32_t *)pix,
                  XTEXT, YTEXT, FONT_COLOR, 0, GRAPH_ALPHA_100, false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                  (!shifted) ? vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].normal
                             : vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].shift);
   else
      Draw_text(pix,
                  XTEXT, YTEXT, FONT_COLOR, 0, GRAPH_ALPHA_100, false, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
                  (!shifted) ? vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].normal
                             : vkeys[(vkey_pos_y * VKBDX) + vkey_pos_x + page].shift);

#ifdef POINTER_DEBUG
   if (pix_bytes == 4)
      DrawHlineBmp32((uint32_t *)retro_bmp,
                     pointer_x, pointer_y, 1, 1,
                     RGBc(255, 0, 255));
   else
      DrawHlineBmp(retro_bmp,
                     pointer_x, pointer_y, 1, 1,
                     RGBc(255, 0, 255));
#endif
}

int check_vkey(int x, int y)
{
   /* Check which key is pressed */
   int page = (retro_vkbd_page) ? VKBDX * VKBDY : 0;
   return vkeys[(y * VKBDX) + x + page].value;
}
