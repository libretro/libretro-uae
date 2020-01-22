#include "libretro-glue.h"

#include "vkbd_def.h"
#include "graph.h"

extern int NPAGE;
extern int SHOWKEYPOS;
extern int SHOWKEYTRANS;
extern int SHIFTON;
extern int pix_bytes;
extern int vkflag[8];
extern unsigned int video_config_geometry;
extern unsigned int opt_vkbd_alpha;

void virtual_kbd(unsigned short int *pixels, int vx, int vy)
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
   int KEYSPACING       = 1;
   int ALPHA            = opt_vkbd_alpha;
   int BKG_ALPHA        = ALPHA;
   int BKG_PADDING_X    = 0;
   int BKG_PADDING_Y    = 4;
   int BKG_COLOR        = 0;
   int BKG_COLOR_NORMAL = 0;
   int BKG_COLOR_ALT    = 0;
   int BKG_COLOR_EXTRA  = 0;
   int BKG_COLOR_SEL    = 0;
   int BKG_COLOR_DARK   = 0;
   int BKG_COLOR_BORDER = 0;
   int FONT_MAX         = 8;
   int FONT_WIDTH       = 1;
   int FONT_HEIGHT      = 1;
   int FONT_COLOR       = 0;
   int FONT_COLOR_NORMAL= 0;
   int FONT_COLOR_SEL   = 0;

   if (pix_bytes == 4)
   {
      BKG_COLOR_NORMAL  = RGB888(24, 24, 24);
      BKG_COLOR_ALT     = RGB888(20, 20, 20);
      BKG_COLOR_EXTRA   = RGB888(14, 14, 14);
      BKG_COLOR_SEL     = RGB888(10, 10, 10);
      BKG_COLOR_DARK    = RGB888(5, 5, 5);
      BKG_COLOR_BORDER  = RGB888(24, 24, 24);
      FONT_COLOR_NORMAL = RGB888(3, 3, 3);
      FONT_COLOR_SEL    = 0xffffff; //RGB888(255, 255, 255);
   }
   else
   {
      BKG_COLOR_NORMAL  = RGB565(24, 24, 24);
      BKG_COLOR_ALT     = RGB565(20, 20, 20);
      BKG_COLOR_EXTRA   = RGB565(14, 14, 14);
      BKG_COLOR_SEL     = RGB565(10, 10, 10);
      BKG_COLOR_DARK    = RGB565(5, 5, 5);
      BKG_COLOR_BORDER  = RGB565(24, 24, 24);
      FONT_COLOR_NORMAL = RGB565(3, 3, 3);
      FONT_COLOR_SEL    = 0xffff; //RGB565(255, 255, 255);
   }

   if (video_config_geometry & 0x04)
   {
      // PUAE_VIDEO_HIRES
      FONT_WIDTH        = 2;
      FONT_HEIGHT       = 2;
      FONT_MAX          = 4;
      XOFFSET           = 2;
      YPADDING          = 10;
      YOFFSET           = 0;
      if (SHOWKEYPOS == 1 && zoomed_height > (retroh + (YPADDING * 3) - (zoomed_height / 2)))
         YOFFSET = -zoomed_height + retroh - (zoomed_height / 2) + 5;
      else
         YOFFSET = -(YPADDING);
   }
   else if (video_config_geometry & 0x08)
   {
      // PUAE_VIDEO_HIRES_SINGLE
      FONT_WIDTH        = 2;
      FONT_MAX          = 4;
      XOFFSET           = 2;
      YPADDING          = 6;
      BKG_PADDING_Y     = -1;
      YOFFSET           = 0;
      if (SHOWKEYPOS == 1 && zoomed_height > (retroh + (YPADDING * 3) - (zoomed_height / 2)))
         YOFFSET = -zoomed_height + retroh - (zoomed_height / 2) + 5;
      else
         YOFFSET = -(YPADDING * 2);
   }
   else
   {
      // PUAE_VIDEO_LORES
      BKG_PADDING_X     = -2;
      BKG_PADDING_Y     = -1;
      FONT_MAX          = 4;

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

   int XBASEKEY = (XPADDING / 2);
   int YBASEKEY = (zoomed_height - (NLIGN * YSIDE)) - (YPADDING / 2);

   int XBASETEXT = (XPADDING / 2) + 5;
   int YBASETEXT = YBASEKEY + ((video_config_geometry & 0x02) ? 4 : 6);

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
            BKG_COLOR = (pix_bytes == 4) ? RGB888(110,0,0) : RGB565(110,0,0);
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
          || (vkflag[7]==1 && MVk[(y * NPLGN) + x + page].val==AK_RET))
          && BKG_COLOR != BKG_COLOR_EXTRA && MVk[(y * NPLGN) + x + page].val != -20)
         {
            FONT_COLOR = FONT_COLOR_SEL;
            BKG_COLOR = BKG_COLOR_DARK;
         }

         /* Key background */
         if (pix_bytes == 4)
            DrawFBoxBmp32((uint32_t *)pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR, BKG_ALPHA);
         else
            DrawFBoxBmp(pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR, BKG_ALPHA);

         /* Key text shadow */
         if (pix_bytes == 4)
            Draw_text32((uint32_t *)pix, XTEXT+1, YTEXT, BKG_COLOR, FONT_COLOR, 200, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (!shifted) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
         else
            Draw_text(pix, XTEXT+1, YTEXT, BKG_COLOR, FONT_COLOR, 200, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
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
   XKEY  = XBASEKEY + (vx * XSIDE) + XOFFSET;
   XTEXT = XBASETEXT + BKG_PADDING_X + (vx * XSIDE) + XOFFSET;
   YKEY  = YOFFSET + YBASEKEY + (vy * YSIDE);
   YTEXT = YOFFSET + YBASETEXT + BKG_PADDING_Y + (vy * YSIDE);

   /* Pressed key background color */
   if (vkflag[4] == 1)
      BKG_COLOR_SEL = BKG_COLOR_DARK;

   /* Opacity */
   BKG_ALPHA = (SHOWKEYTRANS == -1) ? 255 : 230;

   /* Selected key background */
   if (pix_bytes == 4)
      DrawFBoxBmp32((uint32_t *)pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR_SEL, BKG_ALPHA);
   else
      DrawFBoxBmp(pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR_SEL, BKG_ALPHA);

   /* Selected key text */
   if (pix_bytes == 4)
   {
      Draw_text32((uint32_t *)pix, XTEXT, YTEXT, FONT_COLOR_SEL, 0, BKG_ALPHA, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
         (!shifted) ? MVk[(vy * NPLGN) + vx + page].norml : MVk[(vy * NPLGN) + vx + page].shift);
   }
   else
   {
      Draw_text(pix, XTEXT, YTEXT, FONT_COLOR_SEL, 0, BKG_ALPHA, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
         (!shifted) ? MVk[(vy * NPLGN) + vx + page].norml : MVk[(vy * NPLGN) + vx + page].shift);
   }
}

int check_vkey(int x, int y)
{
   /* Check which key is pressed */
   int page = (NPAGE == -1) ? 0 : NPLGN * NLIGN;
   return MVk[(y * NPLGN) + x + page].val;
}
