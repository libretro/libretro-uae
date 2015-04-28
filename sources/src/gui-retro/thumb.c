/*
	modded for libretro-uae
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

extern unsigned short int bmp[1024*1024];
extern unsigned short int savebmp[1024*1024];

typedef unsigned  short int  PIXEL;

#define AVERAGE(a, b)   ( (((a) & 0xfefe) + ((b) & 0xfefe)) >> 1 )

void ScaleLine(PIXEL *Target, PIXEL *Source, int SrcWidth, int TgtWidth)
{
   int NumPixels = TgtWidth;
   int IntPart = SrcWidth / TgtWidth;
   int FractPart = SrcWidth % TgtWidth;
   int E = 0;

   while (NumPixels-- > 0)
   {
      *Target++ = *Source;
      Source += IntPart;
      E += FractPart;
      if (E >= TgtWidth)
      {
         E -= TgtWidth;
         Source++;
      } /* if */
   } /* while */
}

void ScaleRect(PIXEL *Target, PIXEL *Source, int SrcWidth, int SrcHeight,
      int TgtWidth, int TgtHeight)
{
   int NumPixels = TgtHeight;
   int IntPart = (SrcHeight / TgtHeight) * SrcWidth;
   int FractPart = SrcHeight % TgtHeight;
   int E = 0;
   PIXEL *PrevSource = NULL;

   while (NumPixels-- > 0)
   {
      if (Source == PrevSource)
         memcpy(Target, Target-TgtWidth, TgtWidth*sizeof(*Target));
      else
      {
         ScaleLine(Target, Source, SrcWidth, TgtWidth);
         PrevSource = Source;
      } /* if */
      Target += TgtWidth;
      Source += IntPart;
      E += FractPart;
      if (E >= TgtHeight)
      {
         E -= TgtHeight;
         Source += SrcWidth;
      } /* if */
   } /* while */
}

void dothumb(char *name,int sw,int sh,int tw,int th)
{
   unsigned short int target[th*tw];
   unsigned short int buffer[2]={tw,th};

   memset(target,0,2*tw*th);

   PIXEL *Source=&savebmp[0];
   PIXEL *Target=&target[0];

   ScaleRect(Target, Source, sw, sh,tw,th);

   FILE * pFile;
   pFile = fopen (name, "wb");
   if (!pFile)
      return;
   fwrite (buffer , 1,2*2    , pFile);
   fwrite (target , 1,tw*th*2, pFile);
   fclose (pFile);
}

void loadthumb(char *name,int x,int y)
{
   unsigned short int buffer[2];
   FILE * pFile;
   pFile = fopen (name, "rb");
   if (pFile==NULL)return;

   int i,j,idx;
   int dx,dy;

   i = fread (buffer,1,2*2,pFile);
   dx=buffer[0];
   dy=buffer[1];

   unsigned short int target[dx*dy];

   i = fread (target,1,dx*dy*2,pFile);
   fclose (pFile);

   idx=0;
   for(j=y;j<y+dy;j++)
   {
      for(i=x;i<x+dx;i++)
      {
         DrawPointBmp(bmp,i,j,target[idx]);
         idx++;			
      }
   }
}


void ScaleMinifyByTwo(PIXEL *Target, PIXEL *Source, int SrcWidth, int SrcHeight)
{
   int x, y, x2, y2;
   int TgtWidth, TgtHeight;
   PIXEL p, q;

   TgtWidth = SrcWidth / 2;
   TgtHeight = SrcHeight / 2;
   for (y = 0; y < TgtHeight; y++)
   {
      y2 = 2 * y;
      for (x = 0; x < TgtWidth; x++)
      {
         x2 = 2 * x;
         p = AVERAGE(Source[y2*SrcWidth + x2], Source[y2*SrcWidth + x2 + 1]);
         q = AVERAGE(Source[(y2+1)*SrcWidth + x2], Source[(y2+1)*SrcWidth + x2 + 1]);
         Target[y*TgtWidth + x] = AVERAGE(p, q);
      } /* for */
   } /* for */
}

