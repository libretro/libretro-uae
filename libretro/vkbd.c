#include "libretro-glue.h"

#include "vkbd_def.h"
#include "graph.h"

extern int NPAGE;
extern int SHOWKEYPOS;
extern int SHOWKEYTRANS;
extern int SHIFTON;
extern int vkflag[6];
extern int video_config;
extern int pix_bytes;

void virtual_kbd(unsigned short int *pixels,int vx,int vy)
{
   int y,x,page   = (NPAGE== -1) ? 0 : NPLGN*NLIGN;
   uint16_t  *pix = &pixels[0];

   int maxchr			= 8;
   int XPADDING         = 20;
   int XKEY, YKEY, XTEXT, YTEXT, YOFFSET, YPADDING;
   int BKG_COLOR;
   int BKG_COLOR_NORMAL;
   int BKG_COLOR_ALT;
   int BKG_COLOR_EXTRA;
   int BKG_COLOR_SEL;
   int BKG_COLOR_DARK;
   int BKG_COLOR_BORDER;
   int FONT_COLOR_NORMAL;
   int FONT_COLOR_SEL;
   if (pix_bytes == 4)
   {
      BKG_COLOR_NORMAL = RGB888(24 * 255 / 31, 24 * 255 / 31, 24 * 255 / 31);
      BKG_COLOR_ALT	= RGB888(20 * 255 / 31, 20 * 255 / 31, 20 * 255 / 31);
      BKG_COLOR_EXTRA  = RGB888(14 * 255 / 31, 14 * 255 / 31, 14 * 255 / 31);
      BKG_COLOR_SEL	= RGB888(10 * 255 / 31, 10 * 255 / 31, 10 * 255 / 31);
      BKG_COLOR_DARK   = RGB888(4 * 255 / 31, 4 * 255 / 31, 4 * 255 / 31);
      BKG_COLOR_BORDER	= RGB888(1 * 255 / 31, 1 * 255 / 31, 1 * 255 / 31);
      FONT_COLOR_NORMAL= RGB888(1 * 255 / 31,1 * 255 / 31,1 * 255 / 31);
      FONT_COLOR_SEL	= RGB888(254,254,254);
   }
   else
   {
      BKG_COLOR_NORMAL = RGB565(24, 24, 24);
      BKG_COLOR_ALT	= RGB565(20, 20, 20);
      BKG_COLOR_EXTRA  = RGB565(14, 14, 14);
      BKG_COLOR_SEL	= RGB565(10, 10, 10);
      BKG_COLOR_DARK   = RGB565(4, 4, 4);
      BKG_COLOR_BORDER	= RGB565(1, 1, 1);
      FONT_COLOR_NORMAL= RGB565(1,1,1);
      FONT_COLOR_SEL	= RGB565(254,254,254);
   }
   int BKG_PADDING_X	= 0;
   int BKG_PADDING_Y	= 4;
   int FONT_WIDTH		= 1;
   int FONT_HEIGHT		= 1;
   int FONT_COLOR;

   if(video_config & 0x04)      // PUAE_VIDEO_HIRES
   {
      if (video_config & 0x02)  // PUAE_VIDEO_NTSC
         YPADDING       = 280;
      else                      // PUAE_VIDEO_PAL
         YPADDING       = 320;

      if (video_config & 0x08)  // PUAE_VIDEO_CROP
         YPADDING       = YPADDING - 40;

      YOFFSET = (SHOWKEYPOS==1) ? (YPADDING/2) : 0;
   }
   else                         // PUAE_VIDEO_LORES
   {
      if (video_config & 0x02)  // PUAE_VIDEO_NTSC
         YPADDING       = 120;
      else                      // PUAE_VIDEO_PAL
         YPADDING       = 160;

      XPADDING          = 6;

      if (video_config & 0x08)  // PUAE_VIDEO_CROP
      {
         YPADDING       = YPADDING - 30;
         XPADDING       = 0;
      }

      BKG_PADDING_X     = -2;
      BKG_PADDING_Y     = 1;
      maxchr            = 4;

      YOFFSET = (SHOWKEYPOS==1) ? (YPADDING/2) : 0;
   }

   int XSIDE = (retrow - XPADDING) / NPLGN;
   int YSIDE = (retroh - YPADDING) / NLIGN;

   int XBASEKEY = (XPADDING/2);
   int YBASEKEY = (retroh - NLIGN*YSIDE) - (YPADDING/2);

   int XBASETEXT = (XPADDING/2) + 4;
   int YBASETEXT = YBASEKEY + 4;

   if ((video_config & ~0x04) && (video_config & 0x08)) // PUAE_VIDEO_LORES & PUAE_VIDEO_CROP
      XBASETEXT = XBASETEXT - 1;
   else
   if ((video_config & ~0x04) && (video_config & ~0x08)) // PUAE_VIDEO_LORES
      YOFFSET = YOFFSET - 4;

   if ((video_config & 0x04) && (video_config & 0x08)) // PUAE_VIDEO_HIRES & PUAE_VIDEO_CROP
      YOFFSET = YOFFSET - 10;
   else
   if ((video_config & 0x04) && (video_config & ~0x08)) // PUAE_VIDEO_HIRES
      YOFFSET = YOFFSET - 10;

   /* Alternate color keys */
   char *alt_keys[] = {
      "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
      "LShift", "LAlt", "LAmiga", "RAmiga", "RAlt", "RShift",
      "Esc", "`", "Tab", "Ctrl", "CapsLock", "Return", "<-", "Del", "Help"
   };
   int alt_keys_len = sizeof(alt_keys)/sizeof(alt_keys[0]);

   /* Border color in transparency mode */
   if(SHOWKEYTRANS==1)
   {
       if (pix_bytes == 4)
         BKG_COLOR_BORDER = RGB565(8, 8, 8);
       else
         BKG_COLOR_BORDER = RGB888(8 * 255 / 31, 8 * 255 / 31, 8 * 255 / 31);
   }

   /* Key layout */
   for(x=0;x<NPLGN;x++)
   {
      for(y=0;y<NLIGN;y++)
      {
         /* Default key color */
         BKG_COLOR = BKG_COLOR_NORMAL;

         /* Extra key color */
         if(!strcmp("NUMPAD", MVk[(y*NPLGN)+x].norml))
            BKG_COLOR = BKG_COLOR_EXTRA;
         else
         /* Alternate key color */
            for(int alt_key = 0; alt_key < alt_keys_len; ++alt_key)
                if(!strcmp(alt_keys[alt_key], MVk[(y*NPLGN)+x].norml))
                    BKG_COLOR = BKG_COLOR_ALT;

         /* Key positions */
         XKEY  = XBASEKEY+(x*XSIDE);
         XTEXT = XBASETEXT+BKG_PADDING_X+(x*XSIDE);
         YKEY  = YOFFSET+YBASEKEY+(y*YSIDE);
         YTEXT = YOFFSET+YBASETEXT+BKG_PADDING_Y+(y*YSIDE);

         /* Key background */
         if(SHOWKEYTRANS==1)
         {
            if (pix_bytes == 4)
               DrawBoxBmp32((uint32_t *)pix, XKEY,YKEY, XSIDE,YSIDE, 0);
            else
               DrawBoxBmp(pix, XKEY,YKEY, XSIDE,YSIDE, 0);
         }
         else
         {
            if (pix_bytes == 4)
               DrawFBoxBmp32((uint32_t *)pix, XKEY,YKEY, XSIDE,YSIDE, BKG_COLOR);
            else
               DrawFBoxBmp(pix, XKEY,YKEY, XSIDE,YSIDE, BKG_COLOR);
         }
         /* Default font color */
         FONT_COLOR = FONT_COLOR_NORMAL;

         /* Better readability with transparency */
         if(SHOWKEYTRANS==1) {
             BKG_COLOR = BKG_COLOR_SEL;
             BKG_COLOR_BORDER = BKG_COLOR_NORMAL;
             FONT_COLOR = FONT_COLOR_SEL;
         }

         /* Key border */
         if (pix_bytes == 4)
            DrawBoxBmp32((uint32_t *)pix, XKEY,YKEY, XSIDE,YSIDE, BKG_COLOR_BORDER);
         else
            DrawBoxBmp(pix, XKEY,YKEY, XSIDE,YSIDE, BKG_COLOR_BORDER);

         /* Key text */
         if (pix_bytes == 4)
            Draw_text32((uint32_t *)pix, XTEXT, YTEXT, FONT_COLOR,BKG_COLOR, FONT_WIDTH,FONT_HEIGHT, maxchr,
               SHIFTON==-1?MVk[(y*NPLGN)+x+page].norml:MVk[(y*NPLGN)+x+page].shift);	
         else
            Draw_text(pix, XTEXT, YTEXT, FONT_COLOR,BKG_COLOR, FONT_WIDTH,FONT_HEIGHT, maxchr,
               SHIFTON==-1?MVk[(y*NPLGN)+x+page].norml:MVk[(y*NPLGN)+x+page].shift);	
      }
   }

   /* Key positions */
   XKEY  = XBASEKEY+1+(vx*XSIDE);
   XTEXT = XBASETEXT+BKG_PADDING_X+(vx*XSIDE);
   YKEY  = YOFFSET+YBASEKEY+1+(vy*YSIDE);
   YTEXT = YOFFSET+YBASETEXT+BKG_PADDING_Y+(vy*YSIDE);

   /* Pressed key background */
   if(vkflag[4]==1) {
      BKG_COLOR_SEL = BKG_COLOR_DARK;
   }

   /* Selected key background */
   if (pix_bytes == 4)
      DrawFBoxBmp32((uint32_t *)pix, XKEY,YKEY, XSIDE-1,YSIDE-1, BKG_COLOR_SEL);
   else
      DrawFBoxBmp(pix, XKEY,YKEY, XSIDE-1,YSIDE-1, BKG_COLOR_SEL);

   /* Selected key border, NO */
   //DrawBoxBmp(pix, XBASEKEY+vx*XSIDE,YBASEKEY+vy*YSIDE, XSIDE,YSIDE, BKG_COLOR_DARK);

   /* Selected key text */
   if (pix_bytes == 4)
      Draw_text32((uint32_t *)pix, XTEXT,YTEXT, FONT_COLOR_SEL,BKG_COLOR_SEL, FONT_WIDTH,FONT_HEIGHT, maxchr,
         (SHIFTON == -1) ? MVk[(vy*NPLGN)+vx+page].norml : MVk[(vy*NPLGN)+vx+page].shift);	
   else
      Draw_text(pix, XTEXT,YTEXT, FONT_COLOR_SEL,BKG_COLOR_SEL, FONT_WIDTH,FONT_HEIGHT, maxchr,
         (SHIFTON == -1) ? MVk[(vy*NPLGN)+vx+page].norml : MVk[(vy*NPLGN)+vx+page].shift);	
}

int check_vkey2(int x,int y)
{
   /*check which key is press */
   int page = (NPAGE == -1) ? 0 : NPLGN*NLIGN;
   return   MVk[y*NPLGN+x+page].val;
}
