#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "libretro-core.h"
#include "libretro-graph.h"

#include "font.i"

static unsigned char *linesurf = NULL;
static int linesurf_w          = 0;
static int linesurf_h          = 0;

static uint32_t *linesurf32    = NULL;
static int linesurf32_w        = 0;
static int linesurf32_h        = 0;

#define BLEND_ALPHA25(fg, bg, out)                                   \
{                                                                    \
   unsigned short color_50 = ((fg + bg + ((fg ^ bg) & 0x821)) >> 1); \
   (*(out)) = ((color_50 + bg + ((color_50 ^ bg) & 0x821)) >> 1);    \
}

#define BLEND_ALPHA50(fg, bg, out)                    \
{                                                     \
   (*(out)) = ((fg + bg + ((fg ^ bg) & 0x821)) >> 1); \
}

#define BLEND_ALPHA75(fg, bg, out)                                   \
{                                                                    \
   unsigned short color_50 = ((fg + bg + ((fg ^ bg) & 0x821)) >> 1); \
   (*(out)) = ((fg + color_50 + ((fg ^ color_50) & 0x821)) >> 1);    \
}

#define BLEND32_ALPHA25(fg, bg, out)                                \
{                                                                   \
   uint32_t color_50 = ((fg + bg + ((fg ^ bg) & 0x10101)) >> 1);    \
   (*(out)) = ((color_50 + bg + ((color_50 ^ bg) & 0x10101)) >> 1); \
}

#define BLEND32_ALPHA50(fg, bg, out)                    \
{                                                       \
   (*(out)) = ((fg + bg + ((fg ^ bg) & 0x10101)) >> 1); \
}

#define BLEND32_ALPHA75(fg, bg, out)                                \
{                                                                   \
   uint32_t color_50 = ((fg + bg + ((fg ^ bg) & 0x10101)) >> 1);    \
   (*(out)) = ((fg + color_50 + ((fg ^ color_50) & 0x10101)) >> 1); \
}

void DrawFBoxBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color, libretro_graph_alpha_t alpha)
{
   int i, j;

   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         /* Do nothing - buffer is already the
          * correct colour */
         break;
      case GRAPH_ALPHA_25:
         for (j=y; j<y+dy; j++)
         {
            unsigned short *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               BLEND_ALPHA25(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_50:
         for (j=y; j<y+dy; j++)
         {
            unsigned short *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               BLEND_ALPHA50(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_75:
         for (j=y; j<y+dy; j++)
         {
            unsigned short *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               BLEND_ALPHA75(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_100:
      default:
         for (j=y; j<y+dy; j++)
         {
            unsigned short *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               *buf_ptr = color;
               buf_ptr++;
            }
         }
         break;
   }
}

void DrawFBoxBmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha)
{
   int i, j;

   color = color & 0xFFFFFF;

   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         /* Do nothing - buffer is already the
          * correct colour */
         break;
      case GRAPH_ALPHA_25:
         for (j=y; j<y+dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               BLEND32_ALPHA25(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_50:
         for (j=y; j<y+dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               BLEND32_ALPHA50(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_75:
         for (j=y; j<y+dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               BLEND32_ALPHA75(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_100:
      default:
         for (j=y; j<y+dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i=x; i<x+dx; i++)
            {
               *buf_ptr = color;
               buf_ptr++;
            }
         }
         break;
   }
}

void DrawBoxBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
{
   int i, j, idx;

   for (i=x; i<x+dx; i++)
   {
      idx = i+y*retrow;
      buffer[idx] = color;
      idx = i+(y+dy)*retrow;
      buffer[idx] = color;
   }

   for (j=y; j<y+dy; j++)
   {
      idx = x+j*retrow;
      buffer[idx] = color;
      idx = (x+dx)+j*retrow;
      buffer[idx] = color;
   }
}

void DrawBoxBmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
{
   int i, j, idx;

   for (i=x; i<x+dx; i++)
   {
      idx = i+y*retrow;
      buffer[idx] = color;
      idx = i+(y+dy)*retrow;
      buffer[idx] = color;
   }

   for (j=y; j<y+dy; j++)
   {
      idx = x+j*retrow;
      buffer[idx] = color;
      idx = (x+dx)+j*retrow;
      buffer[idx] = color;
   }
}

void DrawPointBmp(unsigned short *buffer, int x, int y, unsigned short color)
{
   int idx;

   idx = x + y * retrow;
   buffer[idx] = color;
}

void DrawHlineBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
{
   int i, j, idx;

   (void)j;

   for (i=x; i<x+dx; i++)
   {
      idx = i+y*retrow;
      buffer[idx] = color;
   }
}

void DrawHlineBmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
{
   int i, j, idx;

   (void)j;

   for (i=x; i<x+dx; i++)
   {
      idx = i+y*retrow;
      buffer[idx] = color;
   }
}

void DrawHline(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
{
   if (pix_bytes == 4)
      DrawHlineBmp32((uint32_t *)buffer, x, y, dx, dy, (uint32_t)color);
   else
      DrawHlineBmp(buffer, x, y, dx, dy, color);
}

void DrawVlineBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
{
   int i, j, idx;

   (void)i;

   for (j=y; j<y+dy; j++)
   {
      idx = x+j*retrow;
      buffer[idx] = color;
   }	
}

void DrawlineBmp(unsigned short *buffer, int x1, int y1, int x2, int y2, unsigned short color)
{
   int pixx, pixy, x, y, dx, dy, sx, sy, swaptmp, idx;

   dx = x2 - x1;
   dy = y2 - y1;
   sx = (dx >= 0) ? 1 : -1;
   sy = (dy >= 0) ? 1 : -1;

   if (dx == 0)
   {
      if (dy > 0)
      {
         DrawVlineBmp(buffer, x1, y1, 0, dy, color);
         return;
      }
      else if (dy < 0)
      {
         DrawVlineBmp(buffer, x1, y2, 0, -dy, color);
         return;
      }
      else
      {
         idx = x1+y1*retrow;
         buffer[idx] = color;
         return;
      }
   }

   if (dy == 0)
   {
      if (dx > 0)
      {
         DrawHlineBmp(buffer, x1, y1, dx, 0, color);
         return;
      }
      else if (dx < 0)
      {
         DrawHlineBmp(buffer, x2, y1, -dx,0, color);
         return;
      }
   }

   dx = sx * dx + 1;
   dy = sy * dy + 1;

   pixx = 1;
   pixy = retrow;

   pixx *= sx;
   pixy *= sy;

   if (dx < dy)
   {
      swaptmp = dx;
      dx = dy;
      dy = swaptmp;
      swaptmp = pixx;
      pixx = pixy;
      pixy = swaptmp;
   }

   x = 0;
   y = 0;

   idx = x1+y1*retrow;

   for (; x < dx; x++, idx +=pixx)
   {
      buffer[idx] = color;
      y += dy;
      if (y >= dx)
      {
         y -= dx;
         idx += pixy;
      }
   }
}

void Draw_string(unsigned short *surf, signed short int x, signed short int y,
      const char *string, unsigned short maxstrlen,
      unsigned short xscale, unsigned short yscale,
      unsigned short fg, unsigned short bg, libretro_graph_alpha_t alpha, bool draw_bg)
{
   int k, strlen;
   int xrepeat, yrepeat;
   signed short int ypixel;
   unsigned short *yptr; 
   int col, bit;
   unsigned char b;

   (void)k;

   if (!string)
      return;

   /* Pseudo transparency for now */
   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         fg = ((bg == 0) ? 0xFFFF : bg);
         bg = 0;
         break;
      case GRAPH_ALPHA_25:
         BLEND_ALPHA25(fg, ((bg == 0) ? 0xFFFF : bg), &fg);
         bg = 0;
         break;
      case GRAPH_ALPHA_50:
         BLEND_ALPHA50(fg, ((bg == 0) ? 0xFFFF : bg), &fg);
         bg = 0;
         break;
      case GRAPH_ALPHA_75:
         BLEND_ALPHA75(fg, ((bg == 0) ? 0xFFFF : bg), &fg);
         bg = 0;
         break;
      case GRAPH_ALPHA_100:
      default:
         bg = draw_bg ? bg : 0;
         break;
   }

   for (strlen = 0; strlen < maxstrlen && string[strlen]; strlen++) {}

   int surfw = strlen * 7 * xscale;
   int surfh = 8 * yscale;

   /* No horizontal wrap */
   if ((surfw + x) > retrow)
      return;

   if ((linesurf_w < surfw) || (linesurf_h < surfh))
   {
      if (linesurf)
         free(linesurf);

      linesurf   = (unsigned char*)malloc(sizeof(unsigned short)*surfw*surfh);
      linesurf_w = surfw;
      linesurf_h = surfh;
   }

   yptr = (unsigned short *)&linesurf[0];

   /* Skip the 8th row */
   surfh -= 1;

   for (ypixel = 0; ypixel < 8; ypixel++)
   {
      for (col = 0; col < strlen; col++)
      {
         b = font_array[(string[col]^0x80)*8 + ypixel];
         for (bit = 0; bit < 7; bit++, yptr++)
         {
            *yptr = (b & (1<<(7-bit))) ? fg : bg;
            for (xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
               yptr[1] = *yptr;
         }
      }

      for (yrepeat = 1; yrepeat < yscale; yrepeat++)
         for (xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
            *yptr = yptr[-surfw];
   }

   yptr = (unsigned short*)&linesurf[0];

   for (yrepeat = y; yrepeat < y+surfh; yrepeat++)
   {
      unsigned short *surf_ptr = surf + (yrepeat * retrow) + x;
      for (xrepeat = x; xrepeat < x+surfw; xrepeat++, yptr++)
      {
         if (*yptr != 0) *surf_ptr = *yptr;
         surf_ptr++;
      }
   }
}

void Draw_string32(uint32_t *surf, signed short int x, signed short int y,
      const char *string, unsigned short maxstrlen,
      unsigned short xscale, unsigned short yscale,
      uint32_t fg, uint32_t bg, libretro_graph_alpha_t alpha, bool draw_bg)
{
   int k, strlen;
   int xrepeat, yrepeat;
   signed short int ypixel;
   uint32_t *yptr;
   int col, bit;
   unsigned char b;

   (void)k;

   if (!string)
      return;

   /* Pseudo transparency for now */
   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         fg = ((bg == 0) ? 0xFFFFFFFF : bg);
         bg = 0;
         break;
      case GRAPH_ALPHA_25:
         fg = fg & 0xFFFFFF;
         BLEND32_ALPHA25(fg, ((bg == 0) ? 0xFFFFFF : bg & 0xFFFFFF), &fg);
         bg = 0;
         break;
      case GRAPH_ALPHA_50:
         fg = fg & 0xFFFFFF;
         BLEND32_ALPHA50(fg, ((bg == 0) ? 0xFFFFFF : bg & 0xFFFFFF), &fg);
         bg = 0;
         break;
      case GRAPH_ALPHA_75:
         fg = fg & 0xFFFFFF;
         BLEND32_ALPHA75(fg, ((bg == 0) ? 0xFFFFFF : bg & 0xFFFFFF), &fg);
         bg = 0;
         break;
      case GRAPH_ALPHA_100:
      default:
         bg = draw_bg ? bg : 0;
         break;
   }

   for (strlen = 0; strlen < maxstrlen && string[strlen]; strlen++) {}

   int surfw = strlen * 7 * xscale;
   int surfh = 8 * yscale;

   /* No horizontal wrap */
   if ((surfw + x) > retrow)
      return;

   if ((linesurf32_w < surfw) || (linesurf32_h < surfh))
   {
      if (linesurf32)
         free(linesurf32);

      linesurf32   = (uint32_t *)malloc(sizeof(uint32_t)*surfw*surfh);
      linesurf32_w = surfw;
      linesurf32_h = surfh;
   }

   yptr = (uint32_t *)&linesurf32[0];

   /* Skip the 8th row */
   surfh -= 1;

   for (ypixel = 0; ypixel < 8; ypixel++)
   {
      for (col = 0; col < strlen; col++)
      {
         b = font_array[(string[col]^0x80)*8 + ypixel];
         for (bit = 0; bit < 7; bit++, yptr++)
         {
            *yptr = (b & (1<<(7-bit))) ? fg : bg;
            for (xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
               yptr[1] = *yptr;
         }
      }

      for (yrepeat = 1; yrepeat < yscale; yrepeat++)
         for (xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
            *yptr = yptr[-surfw];
   }

   yptr = (uint32_t *)&linesurf32[0];

   for (yrepeat = y; yrepeat < y+surfh; yrepeat++)
   {
      uint32_t *surf_ptr = surf + (yrepeat * retrow) + x;
      for (xrepeat = x; xrepeat < x+surfw; xrepeat++, yptr++)
      {
         if (*yptr != 0) *surf_ptr = *yptr;
         surf_ptr++;
      }
   }
}

void Draw_text(unsigned short *buffer, int x, int y,
      unsigned short fgcol, unsigned short bgcol, libretro_graph_alpha_t alpha, bool draw_bg,
      int scalex, int scaley, int max, unsigned char *string)
{
   if (string == NULL)
      return;

#if 0
   Draw_string(buffer, x, y, text, max, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
#else
   unsigned char c;
   char s[2] = {0};
   int charwidth = 6;
   int cmax;
   cmax = strlen(string);
   cmax = (cmax > max) ? max : cmax;
   for (int i = 0; i < cmax; i++)
   {
      c = string[i];
      if (c == 0)
         break;
      if (c & 0x80)
      {
         snprintf(s, sizeof(s), "%c", c - 0x80);
         Draw_string(buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, bgcol, fgcol, alpha, draw_bg);
      }
      else
      {
         snprintf(s, sizeof(s), "%c", c);
         Draw_string(buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
      }
   }
#endif
}

void Draw_text32(uint32_t *buffer, int x, int y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, bool draw_bg,
      int scalex, int scaley, int max, unsigned char *string)
{
   if (string == NULL)
      return;

#if 0
   Draw_string32(buffer, x, y, text, max, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
#else
   unsigned char c;
   char s[2] = {0};
   int charwidth = 6;
   int cmax;
   cmax = strlen(string);
   cmax = (cmax > max) ? max : cmax;
   for (int i = 0; i < cmax; i++)
   {
      c = string[i];
      if (c == 0)
         break;
      if (c & 0x80)
      {
         snprintf(s, sizeof(s), "%c", c - 0x80);
         Draw_string32(buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, bgcol, fgcol, alpha, draw_bg);
      }
      else
      {
         snprintf(s, sizeof(s), "%c", c);
         Draw_string32(buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
      }
   }
#endif
}

void LibretroGraphFree(void)
{
   if (linesurf)
      free(linesurf);
   linesurf = NULL;

   if (linesurf32)
      free(linesurf32);
   linesurf32 = NULL;

   linesurf_w   = 0;
   linesurf_h   = 0;
   linesurf32_w = 0;
   linesurf32_h = 0;
}
