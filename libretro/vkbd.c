#include "libretro-glue.h"
#include "vkbd_def.h"
#include "graph.h"

extern int NPAGE;
extern int SHOWKEYPOS;
extern int SHOWKEYTRANS;
extern int SHIFTON;
extern int vkflag[9];
extern unsigned int video_config_geometry;
extern unsigned int opt_vkbd_theme;
extern unsigned int opt_vkbd_alpha;

int RGBc(int r, int g, int b)
{
   if (pix_bytes == 4)
      return ARGB888(255, r, g, b);
   else
      return RGB565(r, g, b);
}

void print_virtual_kbd(unsigned short int *pixels)
{
   int x, y;
   bool shifted;
   int page             = (NPAGE == -1) ? 0 : NPLGN * NLIGN;
   uint16_t *pix        = &pixels[0];

   int XKEY             = 0;
   int YKEY             = 0;
   int XTEXT            = 0;
   int YTEXT            = 0;
   int XOFFSET          = 0;
   int XPADDING         = 0;
   int YOFFSET          = 0;
   int YPADDING         = 0;
   int XKEYSPACING      = 1;
   int YKEYSPACING      = 1;
   int ALPHA            = opt_vkbd_alpha;
   int BKG_ALPHA        = ALPHA;
   int BKG_PADDING_X    = 0;
   int BKG_PADDING_Y    = 4;
   int BKG_COLOR        = 0;
   int BKG_COLOR_NORMAL = 0;
   int BKG_COLOR_ALT    = 0;
   int BKG_COLOR_EXTRA  = 0;
   int BKG_COLOR_SEL    = 0;
   int BKG_COLOR_ACTIVE = 0;
   int BKG_COLOR_BORDER = 0;
   int FONT_MAX         = 4;
   int FONT_WIDTH       = 1;
   int FONT_HEIGHT      = 1;
   int FONT_COLOR       = 0;
   int FONT_COLOR_NORMAL= 0;
   int FONT_COLOR_SEL   = 0;

   switch (opt_vkbd_theme)
   {
      default:
      case 0: // Classic
         BKG_COLOR_NORMAL  = RGBc(216, 209, 201);
         BKG_COLOR_ALT     = RGBc(159, 154, 150);
         BKG_COLOR_EXTRA   = RGBc(143, 140, 129);
         BKG_COLOR_SEL     = RGBc( 60,  60,  60);
         BKG_COLOR_ACTIVE  = RGBc(250, 250, 250);
         FONT_COLOR_NORMAL = RGBc( 10,  10,  10);
         FONT_COLOR_SEL    = RGBc(250, 250, 250);
         break;

      case 1: // CD32
         BKG_COLOR_NORMAL  = RGBc( 64,  64,  64);
         BKG_COLOR_ALT     = RGBc( 32,  32,  32);
         BKG_COLOR_EXTRA   = RGBc( 16,  16,  16);
         BKG_COLOR_SEL     = RGBc(140, 140, 140);
         BKG_COLOR_ACTIVE  = RGBc( 10,  10,  10);
         FONT_COLOR_NORMAL = RGBc(250, 250, 250);
         FONT_COLOR_SEL    = RGBc( 10,  10,  10);
         break;

      case 2: // Dark
         BKG_COLOR_NORMAL  = RGBc( 32,  32,  32);
         BKG_COLOR_ALT     = RGBc( 48,  48,  48);
         BKG_COLOR_EXTRA   = RGBc( 16,  16,  16);
         BKG_COLOR_SEL     = RGBc(140, 140, 140);
         BKG_COLOR_ACTIVE  = RGBc( 10,  10,  10);
         FONT_COLOR_NORMAL = RGBc(250, 250, 250);
         FONT_COLOR_SEL    = RGBc( 10,  10,  10);
         break;

      case 3: // Light
         BKG_COLOR_NORMAL  = RGBc(220, 220, 220);
         BKG_COLOR_ALT     = RGBc(188, 188, 188);
         BKG_COLOR_EXTRA   = RGBc(172, 172, 172);
         BKG_COLOR_SEL     = RGBc( 60,  60,  60);
         BKG_COLOR_ACTIVE  = RGBc(250, 250, 250);
         FONT_COLOR_NORMAL = RGBc( 10,  10,  10);
         FONT_COLOR_SEL    = RGBc(250, 250, 250);
         break;
   }

   if (video_config_geometry & PUAE_VIDEO_HIRES || video_config_geometry & PUAE_VIDEO_SUPERHIRES)
   {
      if(video_config_geometry & PUAE_VIDEO_DOUBLELINE)
      {
         // PUAE_VIDEO_HIRES_DOUBLELINE
         FONT_WIDTH        = 2;
         FONT_HEIGHT       = 2;
         XKEYSPACING       = 2;
         YKEYSPACING       = 2;
         XOFFSET           = 2;
         YPADDING          = 10;
         YOFFSET           = 0;
         if (SHOWKEYPOS == 1 && zoomed_height > (retroh + (YPADDING * 3) - (zoomed_height / 2)))
            YOFFSET = -zoomed_height + retroh - (zoomed_height / 2) + 5;
         else
            YOFFSET = -(YPADDING);
      }
      else
      {
         // PUAE_VIDEO_HIRES
         FONT_WIDTH        = 2;
         XKEYSPACING       = 2;
         XOFFSET           = 2;
         YPADDING          = 6;
         BKG_PADDING_Y     = -1;
         YOFFSET           = 0;
         if (SHOWKEYPOS == 1 && zoomed_height > (retroh + (YPADDING * 3) - (zoomed_height / 2)))
            YOFFSET = -zoomed_height + retroh - (zoomed_height / 2) + 5;
         else
            YOFFSET = -(YPADDING * 2);
      }

      // PUAE_VIDEO_SUPERHIRES
      if (video_config_geometry & PUAE_VIDEO_SUPERHIRES)
      {
         FONT_WIDTH *= 2;
         XOFFSET *= 2;
         XKEYSPACING *= 2;
      }
   }
   else
   {
      // PUAE_VIDEO_LORES
      BKG_PADDING_X     = -2;
      BKG_PADDING_Y     = -1;

      XOFFSET           = 3;
      YPADDING          = 6;
      YOFFSET           = 0;
      if (SHOWKEYPOS == 1 && zoomed_height > (retroh + (YPADDING * 3) - (zoomed_height / 2)))
         YOFFSET = -zoomed_height + retroh - (zoomed_height / 2) + 5;
      else
         YOFFSET = -(YPADDING * 2);
   }

   int XSIDE = (retrow - XPADDING) / NPLGN;
   int YSIDE = (retroh - (zoomed_height / 2) - 15) / NLIGN;

   int XBASEKEY = (XPADDING > 0) ? (XPADDING / 2) : 0;
   int YBASEKEY = (zoomed_height - (NLIGN * YSIDE)) - (YPADDING / 2);

   int XBASETEXT = ((XPADDING > 0) ? (XPADDING / 2) : 0) + (video_config_geometry & PUAE_VIDEO_SUPERHIRES) ? 12 : (video_config_geometry & PUAE_VIDEO_HIRES) ? 6 : 5;
   int YBASETEXT = YBASEKEY + ((video_config_geometry & PUAE_VIDEO_NTSC) ? 4 : 5);

   /* Coordinates */
   vkbd_x_min = XBASEKEY + XKEYSPACING;
   vkbd_x_max = retrow - XBASEKEY - XKEYSPACING;
   vkbd_y_min = YOFFSET + YBASEKEY + YKEYSPACING;
   vkbd_y_max = YOFFSET + YBASEKEY + (YSIDE * NLIGN);

   /* Opacity */
   BKG_ALPHA = (SHOWKEYTRANS == -1) ? 255 : ALPHA;

   /* Alternate color keys */
   int alt_keys[] =
   {
      AK_F1, AK_F2, AK_F3, AK_F4, AK_F5, AK_F6, AK_F7, AK_F8, AK_F9, AK_F10,
      AK_LSH, AK_RSH, AK_LALT, AK_RALT, AK_LAMI, AK_RAMI, -15 /*LMB*/, -16 /*RMB*/,
      AK_ESC, AK_BACKQUOTE, AK_TAB, AK_CTRL, AK_CAPSLOCK, AK_RET, AK_ENT, AK_BS, AK_DEL, AK_NPDEL, AK_HELP,
   };
   int alt_keys_len = sizeof(alt_keys) / sizeof(alt_keys[0]);

   /* Extra color keys */
   int extra_keys[] =
   {
       -2 /*NUMPAD*/, -18 /*J/M*/, -19 /*TRBF*/, -22 /*ASPR*/, -21 /*STBR*/,
   };
   int extra_keys_len = sizeof(extra_keys) / sizeof(extra_keys[0]);

   /* Key label shifted */
   shifted = false;
   if (SHIFTON == 1 || vkey_sticky1 == AK_LSH || vkey_sticky2 == AK_LSH || vkey_sticky1 == AK_RSH || vkey_sticky2 == AK_RSH)
      shifted = true;
   if (vkflag[4] == 1 && (vkey_pressed == AK_LSH || vkey_pressed == AK_RSH))
      shifted = true;

   /* Key layout */
   for (x = 0; x < NPLGN; x++)
   {
      for (y = 0; y < NLIGN; y++)
      {
         /* Default key color */
         BKG_COLOR = BKG_COLOR_NORMAL;

         /* Reset key color */
         if (MVk[(y * NPLGN) + x].val == -20)
            BKG_COLOR = RGBc(128, 0, 0);
         else
         {
            /* Alternate key color */
            for (int alt_key = 0; alt_key < alt_keys_len; ++alt_key)
                if (alt_keys[alt_key] == MVk[(y * NPLGN) + x + page].val)
                    BKG_COLOR = BKG_COLOR_ALT;

            /* Extra key color */
            for (int extra_key = 0; extra_key < extra_keys_len; ++extra_key)
                if (extra_keys[extra_key] == MVk[(y * NPLGN) + x + page].val)
                    BKG_COLOR = BKG_COLOR_EXTRA;
         }

         /* Key positions */
         XKEY  = XBASEKEY + (x * XSIDE) + XOFFSET;
         XTEXT = XBASETEXT + BKG_PADDING_X + (x * XSIDE) + XOFFSET;
         YKEY  = YOFFSET + YBASEKEY + (y * YSIDE);
         YTEXT = YOFFSET + YBASETEXT + BKG_PADDING_Y + (y * YSIDE);

         /* Default font color */
         FONT_COLOR = FONT_COLOR_NORMAL;

         /* Sticky + CapsLock + pressed colors */
         if ((vkey_sticky1 == MVk[(y * NPLGN) + x + page].val
          ||  vkey_sticky2 == MVk[(y * NPLGN) + x + page].val
          ||   (SHIFTON==1 && MVk[(y * NPLGN) + x + page].val==AK_CAPSLOCK)
          || (vkflag[8]==1 && MVk[(y * NPLGN) + x + page].val==AK_RET))
          && BKG_COLOR != BKG_COLOR_EXTRA && MVk[(y * NPLGN) + x + page].val != -20)
         {
            FONT_COLOR = FONT_COLOR_NORMAL;
            BKG_COLOR = BKG_COLOR_ACTIVE;
         }

         /* Key background */
         if (pix_bytes == 4)
            DrawFBoxBmp32((uint32_t *)pix, XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING, BKG_COLOR, BKG_ALPHA);
         else
            DrawFBoxBmp(pix, XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING, BKG_COLOR, BKG_ALPHA);

         /* Key text shadow */
         if (pix_bytes == 4)
            Draw_text32((uint32_t *)pix, (FONT_COLOR_SEL == RGBc(250, 250, 250) ? XTEXT+FONT_WIDTH : XTEXT-FONT_WIDTH), (FONT_COLOR_SEL == RGBc(250, 250, 250) ? YTEXT+FONT_HEIGHT : YTEXT-FONT_HEIGHT), (FONT_COLOR_SEL == RGBc(250, 250, 250) ? RGBc(80, 80, 80) : RGBc(50, 50, 50)), BKG_COLOR, 100, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (!shifted) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
         else
            Draw_text(pix, (FONT_COLOR_SEL == RGBc(250, 250, 250) ? XTEXT+FONT_WIDTH : XTEXT-FONT_WIDTH), (FONT_COLOR_SEL == RGBc(250, 250, 250) ? YTEXT+FONT_HEIGHT : YTEXT-FONT_HEIGHT), (FONT_COLOR_SEL == RGBc(250, 250, 250) ? RGBc(80, 80, 80) : RGBc(50, 50, 50)), BKG_COLOR, 100, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (!shifted) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);

         /* Key text */
         if (pix_bytes == 4)
         {
            Draw_text32((uint32_t *)pix, XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, 220, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (!shifted) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
         }
         else
         {
            Draw_text(pix, XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, 220, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (!shifted) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
         }
      }
   }

   /* Selected key position */
   XKEY  = XBASEKEY + (vkey_pos_x * XSIDE) + XOFFSET;
   XTEXT = XBASETEXT + BKG_PADDING_X + (vkey_pos_x * XSIDE) + XOFFSET;
   YKEY  = YOFFSET + YBASEKEY + (vkey_pos_y * YSIDE);
   YTEXT = YOFFSET + YBASETEXT + BKG_PADDING_Y + (vkey_pos_y * YSIDE);

   /* Opacity */
   BKG_ALPHA = (SHOWKEYTRANS == -1) ? 250 : 220;

   /* Pressed key color */
   if (vkflag[4] == 1 && (MVk[(vkey_pos_y * NPLGN) + vkey_pos_x + page].val == vkey_sticky1 || MVk[(vkey_pos_y * NPLGN) + vkey_pos_x + page].val == vkey_sticky2))
      ; // no-op
   else if (vkflag[4] == 1)
      BKG_COLOR_SEL = BKG_COLOR_ACTIVE;
   else
      FONT_COLOR = FONT_COLOR_SEL;

   /* Selected key background */
   if (pix_bytes == 4)
      DrawFBoxBmp32((uint32_t *)pix, XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING, BKG_COLOR_SEL, BKG_ALPHA);
   else
      DrawFBoxBmp(pix, XKEY+XKEYSPACING, YKEY+YKEYSPACING, XSIDE-XKEYSPACING, YSIDE-YKEYSPACING, BKG_COLOR_SEL, BKG_ALPHA);

   /* Selected key text */
   if (pix_bytes == 4)
   {
      Draw_text32((uint32_t *)pix, XTEXT, YTEXT, FONT_COLOR, 0, BKG_ALPHA, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
         (!shifted) ? MVk[(vkey_pos_y * NPLGN) + vkey_pos_x + page].norml : MVk[(vkey_pos_y * NPLGN) + vkey_pos_x + page].shift);
   }
   else
   {
      Draw_text(pix, XTEXT, YTEXT, FONT_COLOR, 0, BKG_ALPHA, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
         (!shifted) ? MVk[(vkey_pos_y * NPLGN) + vkey_pos_x + page].norml : MVk[(vkey_pos_y * NPLGN) + vkey_pos_x + page].shift);
   }

#ifdef POINTER_DEBUG
   if (pix_bytes == 4)
      DrawHlineBmp32((uint32_t *)retro_bmp, pointer_x, pointer_y, 1, 1, RGBc(255, 0, 255));
   else
      DrawHlineBmp(retro_bmp, pointer_x, pointer_y, 1, 1, RGBc(255, 0, 255));
#endif
}

int check_vkey(int x, int y)
{
   /* Check which key is pressed */
   int page = (NPAGE == -1) ? 0 : NPLGN * NLIGN;
   return MVk[(y * NPLGN) + x + page].val;
}
