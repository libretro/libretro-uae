#include "libretro-glue.h"

#include "vkbd_def.h"
#include "graph.h"

extern int NPAGE;
extern int SHOWKEYPOS;
extern int SHOWKEYTRANS;
extern int SHIFTON;
extern int pix_bytes;
extern int vkflag[7];
extern unsigned int video_config_geometry;
extern unsigned int opt_vkbd_alpha;

void virtual_kbd(unsigned short int *pixels, int vx, int vy)
{
   int x, y;
   bool sticky;
   int page = (NPAGE == -1) ? 0 : NPLGN * NLIGN;
   uint16_t *pix = &pixels[0];

   int XKEY, YKEY, XTEXT, YTEXT;
   int XOFFSET          = 0;
   int XPADDING         = 0;
   int YOFFSET          = 0;
   int YPADDING         = 0;
   int KEYSPACING       = 1;
   int ALPHA            = opt_vkbd_alpha;
   int BKG_ALPHA        = ALPHA;
   int BKG_PADDING_X    = 0;
   int BKG_PADDING_Y    = 4;
   int BKG_COLOR;
   int BKG_COLOR_NORMAL;
   int BKG_COLOR_ALT;
   int BKG_COLOR_EXTRA;
   int BKG_COLOR_SEL;
   int BKG_COLOR_DARK;
   int BKG_COLOR_BORDER;
   int FONT_MAX         = 8;
   int FONT_WIDTH       = 1;
   int FONT_HEIGHT      = 1;
   int FONT_COLOR;
   int FONT_COLOR_NORMAL;
   int FONT_COLOR_SEL;

   if (pix_bytes == 4)
   {
      BKG_COLOR_NORMAL  = RGB888(24, 24, 24);
      BKG_COLOR_ALT     = RGB888(20, 20, 20);
      BKG_COLOR_EXTRA   = RGB888(14, 14, 14);
      BKG_COLOR_SEL     = RGB888(10, 10, 10);
      BKG_COLOR_DARK    = RGB888(5, 5, 5);
      BKG_COLOR_BORDER  = RGB888(24, 24, 24);
      FONT_COLOR_NORMAL = RGB888(2, 2, 2);
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
      FONT_COLOR_NORMAL = RGB565(2, 2, 2);
      FONT_COLOR_SEL    = 0xffff; //RGB565(255, 255, 255);
   }

   if (video_config_geometry & 0x04)    // PUAE_VIDEO_HIRES
   {
      YPADDING          = 10;
      YOFFSET           = (SHOWKEYPOS == 1) ? (-zoomed_height + (YPADDING / 2) + (retroh / 2)) : -(YPADDING);
      XOFFSET           = 2;
   }
   else if (video_config_geometry & 0x08)    // PUAE_VIDEO_HIRES_SINGLE
   {
      YPADDING          = 6;
      BKG_PADDING_Y     = -1;
      YOFFSET           = (SHOWKEYPOS == 1) ? (-zoomed_height + YPADDING + (retroh / 2)) : -(YPADDING * 2);
      XOFFSET           = 2;
   }
   else                                 // PUAE_VIDEO_LORES
   {
      BKG_PADDING_X     = -2;
      BKG_PADDING_Y     = -1;
      FONT_MAX          = 4;

      YPADDING          = 6;
      YOFFSET           = (SHOWKEYPOS == 1) ? (-zoomed_height + YPADDING + (retroh / 2)) : -(YPADDING * 2);
      XOFFSET           = 3;
   }

   int XSIDE = (retrow - XPADDING) / NPLGN;
   int YSIDE = ((retroh / 2) - YPADDING) / NLIGN;

   int XBASEKEY = (XPADDING / 2);
   int YBASEKEY = (zoomed_height - (NLIGN * YSIDE)) - (YPADDING / 2);

   int XBASETEXT = (XPADDING / 2) + 5;
   int YBASETEXT = YBASEKEY + 5;

   /* Alternate color keys */
   char *alt_keys[] =
   {
      "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
      "LShift", "LAlt", "LAmiga", "RAmiga", "RAlt", "RShift",
      "Esc", "`", "Tab", "Ctrl", "CapsLock", "Return", "<-", "Del", "Help"
   };
   int alt_keys_len = sizeof(alt_keys) / sizeof(alt_keys[0]);

   /* Key layout */
   for (x = 0; x < NPLGN; x++)
   {
      for (y = 0; y < NLIGN; y++)
      {
         /* Sticky reset */
         sticky = false;

         /* Default key color */
         BKG_COLOR = BKG_COLOR_NORMAL;

         /* Extra key color */
         if (!strcmp("NUMPAD", MVk[(y * NPLGN) + x].norml))
            BKG_COLOR = BKG_COLOR_EXTRA;
         else
         /* Alternate key color */
            for (int alt_key = 0; alt_key < alt_keys_len; ++alt_key)
                if (!strcmp(alt_keys[alt_key], MVk[(y * NPLGN) + x].norml))
                    BKG_COLOR = BKG_COLOR_ALT;

         /* Key positions */
         XKEY  = XBASEKEY + (x * XSIDE) + XOFFSET;
         XTEXT = XBASETEXT + BKG_PADDING_X + (x * XSIDE) + XOFFSET;
         YKEY  = YOFFSET + YBASEKEY + (y * YSIDE);
         YTEXT = YOFFSET + YBASETEXT + BKG_PADDING_Y + (y * YSIDE);

         /* Default font color */
         FONT_COLOR = FONT_COLOR_NORMAL;

         /* Sticky key + CapsLock colors */
         if (vkey_sticky1 == MVk[(y * NPLGN) + x + page].val
          || vkey_sticky2 == MVk[(y * NPLGN) + x + page].val
          || (SHIFTON==1 && MVk[(y * NPLGN) + x + page].val==AK_CAPSLOCK))
         {
            sticky = true;
            FONT_COLOR = FONT_COLOR_SEL;
            BKG_COLOR = BKG_COLOR_DARK;
         }

         /* Opacity */
         BKG_ALPHA = (SHOWKEYTRANS == -1) ? 255 : ALPHA;

         /* Key background */
         if (pix_bytes == 4)
            DrawFBoxBmp32((uint32_t *)pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR, BKG_ALPHA);
         else
            DrawFBoxBmp(pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR, BKG_ALPHA);

         /* Key text shadow */
         if (pix_bytes == 4)
            Draw_text32((uint32_t *)pix, XTEXT+1, YTEXT+1, BKG_COLOR, BKG_COLOR, 64, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (SHIFTON == -1) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
         else
            Draw_text(pix, XTEXT+1, YTEXT+1, BKG_COLOR, BKG_COLOR, 64, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (SHIFTON == -1) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);

         /* Key text */
         if (pix_bytes == 4)
            Draw_text32((uint32_t *)pix, XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, 255, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (SHIFTON == -1) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
         else
            Draw_text(pix, XTEXT, YTEXT, FONT_COLOR, BKG_COLOR, 255, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
               (SHIFTON == -1) ? MVk[(y * NPLGN) + x + page].norml : MVk[(y * NPLGN) + x + page].shift);
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
   BKG_ALPHA = (SHOWKEYTRANS == -1) ? 255 : 200;

   /* Selected key background */
   if (pix_bytes == 4)
      DrawFBoxBmp32((uint32_t *)pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR_SEL, BKG_ALPHA);
   else
      DrawFBoxBmp(pix, XKEY+KEYSPACING, YKEY+KEYSPACING, XSIDE-KEYSPACING, YSIDE-KEYSPACING, BKG_COLOR_SEL, BKG_ALPHA);

   /* Selected key text */
   if (pix_bytes == 4)
      Draw_text32((uint32_t *)pix, XTEXT, YTEXT, FONT_COLOR_SEL, 0, 255, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
         (SHIFTON == -1) ? MVk[(vy * NPLGN) + vx + page].norml : MVk[(vy * NPLGN) + vx + page].shift);
   else
      Draw_text(pix, XTEXT, YTEXT, FONT_COLOR_SEL, 0, 255, FONT_WIDTH, FONT_HEIGHT, FONT_MAX,
         (SHIFTON == -1) ? MVk[(vy * NPLGN) + vx + page].norml : MVk[(vy * NPLGN) + vx + page].shift);
}

int check_vkey(int x, int y)
{
   /* Check which key is pressed */
   int page = (NPAGE == -1) ? 0 : NPLGN * NLIGN;
   return MVk[(y * NPLGN) + x + page].val;
}
