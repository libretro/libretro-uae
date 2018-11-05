#include "libretro-glue.h"

#include "vkbd_def.h"
#include "graph.h"

extern int NPAGE;
extern int KCOL;
extern int BKGCOLOR;
extern int SHIFTON;

void virtual_kdb(unsigned short int *pixels,int vx,int vy)
{
   int y,x,page   = (NPAGE== -1) ? 0 : 50;
   uint16_t  *pix = &pixels[0];
   uint16_t coul  = RGB565(28, 28, 31);
   BKGCOLOR       = (KCOL>0?0x8080:0);

   for(x=0;x<NPLGN;x++)
   {
      for(y=0;y<NLIGN;y++)
      {
         DrawBoxBmp(pix,XBASE3+x*XSIDE,YBASE3+y*YSIDE, XSIDE,YSIDE, RGB565(7, 2, 1));
         Draw_text(pix,XBASE0-2+x*XSIDE ,YBASE0+YSIDE*y,coul, BKGCOLOR ,2, 4,20,
               SHIFTON==-1?MVk[(y*NPLGN)+x+page].norml:MVk[(y*NPLGN)+x+page].shift);	
      }
   }

   DrawBoxBmp(pix,XBASE3+vx*XSIDE,YBASE3+vy*YSIDE, XSIDE,YSIDE, RGB565(31, 2, 1));
   Draw_text(pix,XBASE0-2+vx*XSIDE ,YBASE0+YSIDE*vy,RGB565(2,31,1), BKGCOLOR ,2, 4,20,
         (SHIFTON == -1) ? MVk[(vy*NPLGN)+vx+page].norml : MVk[(vy*NPLGN)+vx+page].shift);	
}

int check_vkey2(int x,int y)
{
   /*check which key is press */
   int page = (NPAGE == -1) ? 0 : 50;
   return   MVk[y*NPLGN+x+page].val;
}
