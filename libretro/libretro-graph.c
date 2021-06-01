#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "libretro-core.h"
#include "libretro-graph.h"

#include "font.i"

static unsigned short int *linesurf = NULL;
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



void draw_fbox(int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha)
{
   if (pix_bytes == 4)
      draw_fbox_bmp32((uint32_t *)retro_bmp, x, y, dx, dy, color, alpha);
   else
      draw_fbox_bmp(retro_bmp, x, y, dx, dy, color, alpha);
}

void draw_fbox_bmp(unsigned short *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha)
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

void draw_fbox_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha)
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



void draw_box_bmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
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

void draw_box_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
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



void draw_point_bmp(unsigned short *buffer, int x, int y, unsigned short color)
{
   int idx;

   idx = x + y * retrow;
   buffer[idx] = color;
}



void draw_hline(int x, int y, int dx, int dy, uint32_t color)
{
   if (pix_bytes == 4)
      draw_hline_bmp32((uint32_t *)retro_bmp, x, y, dx, dy, color);
   else
      draw_hline_bmp(retro_bmp, x, y, dx, dy, color);
}

void draw_hline_bmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
{
   int i, j, idx;

   (void)j;

   for (i=x; i<x+dx; i++)
   {
      idx = i+y*retrow;
      buffer[idx] = color;
   }
}

void draw_hline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
{
   int i, j, idx;

   (void)j;

   for (i=x; i<x+dx; i++)
   {
      idx = i+y*retrow;
      buffer[idx] = color;
   }
}

void draw_vline(int x, int y, int dx, int dy, uint32_t color)
{
   if (pix_bytes == 4)
      draw_vline_bmp32((uint32_t *)retro_bmp, x, y, dx, dy, color);
   else
      draw_vline_bmp(retro_bmp, x, y, dx, dy, color);
}

void draw_vline_bmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color)
{
   int i, j, idx;

   (void)i;

   for (j=y; j<y+dy; j++)
   {
      idx = x+j*retrow;
      buffer[idx] = color;
   }
}

void draw_vline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
{
   int i, j, idx;

   (void)i;

   for (j=y; j<y+dy; j++)
   {
      idx = x+j*retrow;
      buffer[idx] = color;
   }
}

void draw_line_bmp(unsigned short *buffer, int x1, int y1, int x2, int y2, unsigned short color)
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
         draw_vline_bmp(buffer, x1, y1, 0, dy, color);
         return;
      }
      else if (dy < 0)
      {
         draw_vline_bmp(buffer, x1, y2, 0, -dy, color);
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
         draw_hline_bmp(buffer, x1, y1, dx, 0, color);
         return;
      }
      else if (dx < 0)
      {
         draw_hline_bmp(buffer, x2, y1, -dx,0, color);
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


static void draw_char_1pass(const char *string, unsigned int strlen,
      int charw, int charh,
      unsigned short int xscale, unsigned short int yscale,
      unsigned short int fg, unsigned short int bg)
{
   unsigned char b = 0;
   unsigned short int col = 0;
   unsigned short int bit = 0;
   unsigned short int surfw = 0;
   short int ypixel = 0;
   short int xrepeat = 0;
   short int yrepeat = 0;

   unsigned short int *yptr;

   if (!linesurf)
      return;

   surfw = linesurf_w;
   yptr  = &linesurf[0];

   for (ypixel = 0; ypixel < charh + 1; ypixel++)
   {
      /* Fill */
      for (col = 0; col < strlen; col++)
      {
         b = font_array[(string[col] ^ 0x80)*charw + ypixel - 1];
         for (bit = 0; bit < charw + 1; bit++, yptr++)
         {
            *yptr = (b & (1 << (charw - 1 - bit) + 1)) ? fg : bg;
            for (xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
               yptr[1] = *yptr;
         }
      }

      /* Scale */
      for (yrepeat = 1; yrepeat < yscale; yrepeat++)
         for (xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
            *yptr = yptr[-surfw];
   }
}

static void draw_char_1pass32(const char *string, unsigned int strlen,
      int charw, int charh,
      unsigned short int xscale, unsigned short int yscale,
      uint32_t fg, uint32_t bg)
{
   unsigned char b = 0;
   unsigned short int col = 0;
   unsigned short int bit = 0;
   unsigned short int surfw = 0;
   short int ypixel = 0;
   short int xrepeat = 0;
   short int yrepeat = 0;

   uint32_t *yptr32;

   if (!linesurf32)
      return;

   surfw  = linesurf32_w;
   yptr32 = &linesurf32[0];

   for (ypixel = 0; ypixel < charh + 1; ypixel++)
   {
      /* Fill */
      for (col = 0; col < strlen; col++)
      {
         b = font_array[(string[col] ^ 0x80)*charw + ypixel - 1];
         for (bit = 0; bit < charw + 1; bit++, yptr32++)
         {
            *yptr32 = (b & (1 << (charw - 1 - bit) + 1)) ? fg : bg;
            for (xrepeat = 1; xrepeat < xscale; xrepeat++, yptr32++)
               yptr32[1] = *yptr32;
         }
      }

      /* Scale */
      for (yrepeat = 1; yrepeat < yscale; yrepeat++)
         for (xrepeat = 0; xrepeat < surfw; xrepeat++, yptr32++)
            *yptr32 = yptr32[-surfw];
   }
}

static void draw_char_2pass(unsigned short *surf,
      unsigned short int x, unsigned short int y,
      unsigned short int xscale, unsigned short int yscale,
      unsigned short int fg, unsigned short int bg, libretro_graph_bg_t draw_bg)
{
   short int xrepeat = 0;
   short int yrepeat = 0;
   unsigned short int surfw = 0;
   unsigned short int surfh = 0;
   unsigned short int surfhxscale = 0;

   unsigned short int *yptr;
   unsigned short *surf_ptr;

   if (!linesurf)
      return;

   surfw = linesurf_w;
   surfh = linesurf_h;
   yptr  = &linesurf[0];

   surfhxscale = surfh * xscale;

   switch (draw_bg)
   {
      case GRAPH_BG_ALL:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++)
            {
               if (*yptr == bg)
               {
                  if (yrepeat > y - yscale && yrepeat < surfh + y - (yscale * 2) &&
                      xrepeat >= x + xscale && xrepeat < surfw + x - xscale)
                     ; /* no-op */
                  else
                     *yptr = 0;
               }

               if (*yptr != 0)
                  *surf_ptr = *yptr;
               surf_ptr++;
            }
         }
         break;

      case GRAPH_BG_SHADOW:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++)
            {
               if (*yptr == bg)
               {
                  /* Bottom right */
                  if (yptr[-surfhxscale-xscale] == fg)
                     ; /* no-op */
                  else
                     *yptr = 0;
               }

               if (*yptr != 0)
                  *surf_ptr = *yptr;
               surf_ptr++;
            }
         }
         break;

      case GRAPH_BG_OUTLINE:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++)
            {
               if (*yptr == bg)
               {
                  if (
                      /* Diagonals */
                      yptr[-surfhxscale-xscale] == fg || yptr[+surfhxscale-xscale] == fg ||
                      yptr[-surfhxscale+xscale] == fg || yptr[+surfhxscale+xscale] == fg ||
                      /* Verticals */
                      yptr[-surfhxscale] == fg        || yptr[+surfhxscale] == fg        ||
                      /* Horizontals */
                      yptr[-xscale] == fg             || yptr[+xscale] == fg
                  )
                     ; /* no-op */
                  else
                     *yptr = 0;
               }

               if (*yptr != 0)
                  *surf_ptr = *yptr;
               surf_ptr++;
            }
         }
         break;

      case GRAPH_BG_NONE:
      default:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++)
            {
               if (*yptr != 0)
                  *surf_ptr = *yptr;
               surf_ptr++;
            }
         }
         break;
   }
}

static void draw_char_2pass32(uint32_t *surf,
      unsigned short int x, unsigned short int y,
      unsigned short int xscale, unsigned short int yscale,
      uint32_t fg, uint32_t bg, libretro_graph_bg_t draw_bg)
{
   short int xrepeat = 0;
   short int yrepeat = 0;
   unsigned short int surfw = 0;
   unsigned short int surfh = 0;
   unsigned short int surfhxscale = 0;

   uint32_t *yptr32;
   uint32_t *surf_ptr32;

   if (!linesurf32)
      return;

   surfw  = linesurf32_w;
   surfh  = linesurf32_h;
   yptr32 = &linesurf32[0];

   surfhxscale = surfh * xscale;

   switch (draw_bg)
   {
      case GRAPH_BG_ALL:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr32 = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr32++)
            {
               if (*yptr32 == bg)
               {
                  if (yrepeat > y - yscale && yrepeat < surfh + y - (yscale * 2) &&
                      xrepeat >= x + xscale && xrepeat < surfw + x - xscale)
                     ; /* no-op */
                  else
                     *yptr32 = 0;
               }

               if (*yptr32 != 0)
                  *surf_ptr32 = *yptr32;
               surf_ptr32++;
            }
         }
         break;

      case GRAPH_BG_SHADOW:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr32 = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr32++)
            {
               if (*yptr32 == bg)
               {
                  /* Bottom right */
                  if (yptr32[-surfhxscale-xscale] == fg)
                     ; /* no-op */
                  else
                     *yptr32 = 0;
               }

               if (*yptr32 != 0)
                  *surf_ptr32 = *yptr32;
               surf_ptr32++;
            }
         }
         break;

      case GRAPH_BG_OUTLINE:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr32 = surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr32++)
            {
               if (*yptr32 == bg)
               {
                  if (
                      /* Diagonals */
                      yptr32[-surfhxscale-xscale] == fg || yptr32[+surfhxscale-xscale] == fg ||
                      yptr32[-surfhxscale+xscale] == fg || yptr32[+surfhxscale+xscale] == fg ||
                      /* Verticals */
                      yptr32[-surfhxscale] == fg        || yptr32[+surfhxscale] == fg        ||
                      /* Horizontals */
                      yptr32[-xscale] == fg             || yptr32[+xscale] == fg
                  )
                     ; /* no-op */
                  else
                     *yptr32 = 0;
               }

               if (*yptr32 != 0)
                  *surf_ptr32 = *yptr32;
               surf_ptr32++;
            }
         }
         break;

      case GRAPH_BG_NONE:
      default:
         for (yrepeat = y - yscale; yrepeat < surfh + y - yscale; yrepeat++)
         {
            surf_ptr32 = (uint32_t *)surf + (yrepeat * retrow) + x - xscale;
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr32++)
            {
               if (*yptr32 != 0)
                  *surf_ptr32 = *yptr32;
               surf_ptr32++;
            }
         }
         break;
   }
}

void draw_string_bmp(unsigned short *surf, unsigned short int x, unsigned short int y,
      const char *string, unsigned short int maxstrlen,
      unsigned short int xscale, unsigned short int yscale,
      unsigned short int fg, unsigned short int bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg)
{
   short int strlen;
   short int surfw;
   short int surfh;
   unsigned short int charw = 8;
   unsigned short int charh = 8;
   unsigned short int fg_blend = (fg == COLOR_BLACK_16) ? COLOR_GRAY_16 : COLOR_BLACK_16;

   if (!string)
      return;

   for (strlen = 0; strlen < maxstrlen && string[strlen]; strlen++) {}

   surfw = xscale * charw * strlen;
   surfh = yscale * charh;
   /* Background breather */
   surfw += xscale;
   surfh += yscale;

   /* No horizontal wrap */
   if ((surfw + x - xscale) > retrow)
      return;

   /* Background transparency */
   bg = draw_bg ? bg : 0;
   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         fg = ((bg == 0) ? 0xFFFF : bg);
         bg = 0;
         break;
      case GRAPH_ALPHA_25:
         BLEND_ALPHA25(fg_blend, ((bg == 0) ? 0xFFFF : bg), &bg);
         break;
      case GRAPH_ALPHA_50:
         BLEND_ALPHA50(fg_blend, ((bg == 0) ? 0xFFFF : bg), &bg);
         break;
      case GRAPH_ALPHA_75:
         BLEND_ALPHA75(fg_blend, ((bg == 0) ? 0xFFFF : bg), &bg);
         break;
      case GRAPH_ALPHA_100:
      default:
         break;
   }

   if ((linesurf_w != surfw) || (linesurf_h != surfh))
   {
      if (linesurf)
         free(linesurf);

      linesurf   = (unsigned short*)malloc(sizeof(unsigned short)*surfw*surfh);
      linesurf_w = surfw;
      linesurf_h = surfh;
   }

   draw_char_1pass(string, strlen, charw, charh, xscale, yscale, fg, bg);
   draw_char_2pass(surf, x, y, xscale, yscale, fg, bg, draw_bg);
}

void draw_string_bmp32(uint32_t *surf, unsigned short int x, unsigned short int y,
      const char *string, unsigned short int maxstrlen,
      unsigned short int xscale, unsigned short int yscale,
      uint32_t fg, uint32_t bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg)
{
   short int strlen;
   short int surfw;
   short int surfh;
   unsigned short int charw = 8;
   unsigned short int charh = 8;
   uint32_t fg_blend = (fg == COLOR_BLACK_32) ? COLOR_GRAY_32 & 0xFFFFFF : COLOR_BLACK_32 & 0xFFFFFF;

   if (!string)
      return;

   for (strlen = 0; strlen < maxstrlen && string[strlen]; strlen++) {}

   surfw = xscale * charw * strlen;
   surfh = yscale * charh;
   /* Background breather */
   surfw += xscale;
   surfh += yscale;

   /* No horizontal wrap */
   if ((surfw + x - xscale) > retrow)
      return;

   /* Background transparency */
   bg = draw_bg ? bg : 0;
   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         fg = ((bg == 0) ? 0xFFFFFFFF : bg);
         bg = 0;
         break;
      case GRAPH_ALPHA_25:
         fg = fg & 0xFFFFFF;
         BLEND32_ALPHA25(fg_blend, ((bg == 0) ? 0xFFFFFF : bg & 0xFFFFFF), &bg);
         break;
      case GRAPH_ALPHA_50:
         fg = fg & 0xFFFFFF;
         BLEND32_ALPHA50(fg_blend, ((bg == 0) ? 0xFFFFFF : bg & 0xFFFFFF), &bg);
         break;
      case GRAPH_ALPHA_75:
         fg = fg & 0xFFFFFF;
         BLEND32_ALPHA75(fg_blend, ((bg == 0) ? 0xFFFFFF : bg & 0xFFFFFF), &bg);
         break;
      case GRAPH_ALPHA_100:
      default:
         break;
   }

   if ((linesurf32_w != surfw) || (linesurf32_h != surfh))
   {
      if (linesurf32)
         free(linesurf32);

      linesurf32   = (uint32_t *)malloc(sizeof(uint32_t)*surfw*surfh);
      linesurf32_w = surfw;
      linesurf32_h = surfh;
   }

   draw_char_1pass32(string, strlen, charw, charh, xscale, yscale, fg, bg);
   draw_char_2pass32(surf, x, y, xscale, yscale, fg, bg, draw_bg);
}

void draw_text(unsigned short int x, unsigned short int y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      unsigned short int scalex, unsigned short int scaley, unsigned short int max, unsigned char *string)
{
   if (pix_bytes == 4)
      draw_text_bmp32((uint32_t *)retro_bmp, x, y, fgcol, bgcol, alpha, draw_bg, scalex, scaley, max, string);
   else
      draw_text_bmp(retro_bmp, x, y, fgcol, bgcol, alpha, draw_bg, scalex, scaley, max, string);
}

void draw_text_bmp(unsigned short *buffer, unsigned short int x, unsigned short int y,
      unsigned short int fgcol, unsigned short int bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      unsigned short int scalex, unsigned short int scaley, unsigned short int max, unsigned char *string)
{
   if (string == NULL)
      return;

#if 0
   draw_string_bmp(buffer, x, y, text, max, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
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
         draw_string_bmp((unsigned short *)buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, bgcol, fgcol, alpha, draw_bg);
      }
      else
      {
         snprintf(s, sizeof(s), "%c", c);
         draw_string_bmp((unsigned short *)buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
      }
   }
#endif
}

void draw_text_bmp32(uint32_t *buffer, unsigned short int x, unsigned short int y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      unsigned short int scalex, unsigned short int scaley, unsigned short int max, unsigned char *string)
{
   if (string == NULL)
      return;

#if 0
   draw_string_bmp32(buffer, x, y, text, max, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
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
         draw_string_bmp32(buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, bgcol, fgcol, alpha, draw_bg);
      }
      else
      {
         snprintf(s, sizeof(s), "%c", c);
         draw_string_bmp32(buffer, x+(i*charwidth*scalex), y, s, 1, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
      }
   }
#endif
}

void libretro_graph_free(void)
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
