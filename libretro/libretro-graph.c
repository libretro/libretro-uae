#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "libretro-core.h"
#include "libretro-graph.h"
#include "libretro-font.i"

unsigned short int graphed[RETRO_BMP_SIZE];

static uint16_t *linesurf16 = NULL;
static int linesurf16_w     = 0;
static int linesurf16_h     = 0;

static uint32_t *linesurf32 = NULL;
static int linesurf32_w     = 0;
static int linesurf32_h     = 0;

int RGBc(int r, int g, int b)
{
   if (pix_bytes == 4)
      return ARGB888(255, r, g, b);
   else
      return RGB565(r, g, b);
}

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
      draw_fbox_bmp16((uint16_t *)retro_bmp, x, y, dx, dy, color, alpha);
}

void draw_fbox_bmp16(unsigned short *buffer, int x, int y, int dx, int dy, uint16_t color, libretro_graph_alpha_t alpha)
{
   int i, j;

   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         /* Do nothing - buffer is already the
          * correct colour */
         break;
      case GRAPH_ALPHA_25:
         for (j = y; j < y + dy; j++)
         {
            uint16_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               BLEND_ALPHA25(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_50:
         for (j = y; j < y + dy; j++)
         {
            uint16_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               BLEND_ALPHA50(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_75:
         for (j = y; j < y + dy; j++)
         {
            uint16_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               BLEND_ALPHA75(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_100:
      default:
         for (j = y; j < y + dy; j++)
         {
            uint16_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
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
         for (j = y; j < y + dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               BLEND32_ALPHA25(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_50:
         for (j = y; j < y + dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               BLEND32_ALPHA50(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_75:
         for (j = y; j < y + dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               BLEND32_ALPHA75(color, *buf_ptr, buf_ptr);
               buf_ptr++;
            }
         }
         break;
      case GRAPH_ALPHA_100:
      default:
         for (j = y; j < y + dy; j++)
         {
            uint32_t *buf_ptr = buffer + (j * retrow) + x;
            for (i = x; i < x + dx; i++)
            {
               *buf_ptr = color;
               buf_ptr++;
            }
         }
         break;
   }
}


void draw_box(int x, int y, int dx, int dy, int width, int height, uint32_t color, libretro_graph_alpha_t alpha)
{
   if (pix_bytes == 4)
      draw_box_bmp32((uint32_t *)retro_bmp, x, y, dx, dy, width, height, color, alpha);
   else
      draw_box_bmp16((uint16_t *)retro_bmp, x, y, dx, dy, width, height, color, alpha);
}

void draw_box_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, int width, int height, uint16_t color, libretro_graph_alpha_t alpha)
{
   int i, j, k, idx;
   uint16_t *buf_ptr;

   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         /* Do nothing - buffer is already the
          * correct colour */
         break;
      case GRAPH_ALPHA_25:
         for (i = x; i < x + dx + width; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = i + (y * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y + height; j < y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }
         break;
      case GRAPH_ALPHA_50:
         for (i = x; i < x + dx + width; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = i + (y * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y + height; j < y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }
         break;
      case GRAPH_ALPHA_75:
         for (i = x; i < x + dx + width; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = i + (y * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y + height; j < y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }
         break;
      case GRAPH_ALPHA_100:
      default:
         for (i = x; i <= x + dx; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = + i + (y * retrow) + (k * retrow);
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y; j <= y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
            }
         }
         break;
   }
}

void draw_box_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, int width, int height, uint32_t color, libretro_graph_alpha_t alpha)
{
   int i, j, k, idx;
   uint32_t *buf_ptr;

   color = color & 0xFFFFFF;

   switch (alpha)
   {
      case GRAPH_ALPHA_0:
         /* Do nothing - buffer is already the
          * correct colour */
         break;
      case GRAPH_ALPHA_25:
         for (i = x; i < x + dx + width; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = i + (y * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y + height; j < y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA25(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }
         break;
      case GRAPH_ALPHA_50:
         for (i = x; i < x + dx + width; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = i + (y * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y + height; j < y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA50(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }
         break;
      case GRAPH_ALPHA_75:
         for (i = x; i < x + dx + width; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = i + (y * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y + height; j < y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               buf_ptr = &buffer[idx];
               if (!graphed[idx])
               {
                  BLEND32_ALPHA75(color, *buf_ptr, buf_ptr);
                  graphed[idx] = 1;
               }
            }
         }
         break;
      case GRAPH_ALPHA_100:
      default:
         for (i = x; i <= x + dx; i++)
         {
            for (k = 0; k < height; k++)
            {
               idx = + i + (y * retrow) + (k * retrow);
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
               idx = i + ((y + dy) * retrow) + (k * retrow);
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
            }
         }

         for (j = y; j <= y + dy; j++)
         {
            for (k = 0; k < width; k++)
            {
               idx = x + (j * retrow) + k;
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
               idx = (x + dx) + (j * retrow) + k;
               if (!graphed[idx])
               {
                  buffer[idx] = color;
                  graphed[idx] = 1;
               }
            }
         }
         break;
   }
}



void draw_hline(int x, int y, int dx, int dy, uint32_t color)
{
   if (pix_bytes == 4)
      draw_hline_bmp32((uint32_t *)retro_bmp, x, y, dx, dy, color);
   else
      draw_hline_bmp16((uint16_t *)retro_bmp, x, y, dx, dy, color);
}

void draw_hline_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, uint16_t color)
{
   int i, j, idx;

   (void)j;

   for (i = x; i < x + dx; i++)
   {
      idx = i + (y * retrow);
      if (idx < 0)
         continue;
      buffer[idx] = color;
   }
}

void draw_hline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
{
   int i, j, idx;

   (void)j;

   for (i = x; i < x + dx; i++)
   {
      idx = i + (y * retrow);
      if (idx < 0)
         continue;
      buffer[idx] = color;
   }
}

void draw_vline(int x, int y, int dx, int dy, uint32_t color)
{
   if (pix_bytes == 4)
      draw_vline_bmp32((uint32_t *)retro_bmp, x, y, dx, dy, color);
   else
      draw_vline_bmp16((uint16_t *)retro_bmp, x, y, dx, dy, color);
}

void draw_vline_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, uint16_t color)
{
   int i, j, idx;

   (void)i;

   for (j = y; j < y + dy; j++)
   {
      idx = x + (j * retrow);
      if (idx < 0)
         continue;
      buffer[idx] = color;
   }
}

void draw_vline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color)
{
   int i, j, idx;

   (void)i;

   for (j = y; j < y + dy; j++)
   {
      idx = x + (j * retrow);
      if (idx < 0)
         continue;
      buffer[idx] = color;
   }
}



static void draw_char_1pass16(const char *string, uint16_t strlen,
      uint8_t charw, uint8_t charh,
      uint8_t xscale, uint8_t yscale,
      uint16_t fg, uint16_t bg)
{
   unsigned char b = 0;
   unsigned short int col = 0;
   unsigned short int bit = 0;
   unsigned short int surfw = 0;
   short int ypixel = 0;
   short int xrepeat = 0;
   short int yrepeat = 0;

   uint16_t *yptr;

   if (!linesurf16)
      return;

   surfw = linesurf16_w;
   yptr  = &linesurf16[0];

   for (ypixel = 0; ypixel < charh + 1; ypixel++)
   {
      /* Fill */
      for (col = 0; col < strlen; col++)
      {
         b = font_array[(string[col])*charw + ypixel - 1];
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

static void draw_char_1pass32(const char *string, uint16_t strlen,
      uint8_t charw, uint8_t charh,
      uint8_t xscale, uint8_t yscale,
      uint32_t fg, uint32_t bg)
{
   unsigned char b = 0;
   unsigned short int col = 0;
   unsigned short int bit = 0;
   unsigned short int surfw = 0;
   short int ypixel = 0;
   short int xrepeat = 0;
   short int yrepeat = 0;

   uint32_t *yptr;

   if (!linesurf32)
      return;

   surfw = linesurf32_w;
   yptr  = &linesurf32[0];

   for (ypixel = 0; ypixel < charh + 1; ypixel++)
   {
      /* Fill */
      for (col = 0; col < strlen; col++)
      {
         b = font_array[(string[col])*charw + ypixel - 1];
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

static void draw_char_2pass16(uint16_t *surf,
      uint16_t x, uint16_t y,
      uint8_t xscale, uint8_t yscale,
      uint16_t fg, uint16_t bg,
      libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg)
{
   short int xrepeat = 0;
   short int yrepeat = 0;
   short int pcount  = 0;
   unsigned short int surfw = 0;
   unsigned short int surfh = 0;
   unsigned short int surfhxscale = 0;

   uint16_t *yptr;
   uint16_t *surf_ptr;

   if (!linesurf16)
      return;

   surfw = linesurf16_w;
   surfh = linesurf16_h;
   yptr  = &linesurf16[0];

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
                  {
                     switch (alpha)
                     {
                        case GRAPH_ALPHA_0:
                           *yptr = *surf_ptr;
                           break;
                        case GRAPH_ALPHA_25:
                           BLEND_ALPHA25(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_50:
                           BLEND_ALPHA50(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_75:
                           BLEND_ALPHA75(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_100:
                        default:
                           break;
                     }
                  }
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
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++, pcount++)
            {
               if (*yptr == bg)
               {
                  /* Right + Bottom right + Bottom */
                  if (
                        (pcount >= xscale             && yptr[-xscale] == fg)             ||
                        (pcount >= surfhxscale+xscale && yptr[-surfhxscale-xscale] == fg) ||
                        (pcount >= surfhxscale        && yptr[-surfhxscale] == fg)
                  )
                  {
                     switch (alpha)
                     {
                        case GRAPH_ALPHA_0:
                           *yptr = *surf_ptr;
                           break;
                        case GRAPH_ALPHA_25:
                           BLEND_ALPHA25(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_50:
                           BLEND_ALPHA50(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_75:
                           BLEND_ALPHA75(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_100:
                        default:
                           break;
                     }
                  }
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
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++, pcount++)
            {
               if (*yptr == bg)
               {
                  if (
                      /* Diagonals */
                      (pcount >= surfhxscale+xscale && yptr[-surfhxscale-xscale] == fg) ||
                      (pcount >= xscale             && yptr[+surfhxscale-xscale] == fg) ||
                      (pcount >= surfhxscale        && yptr[-surfhxscale+xscale] == fg) ||
                      (pcount >= 0                  && yptr[+surfhxscale+xscale] == fg) ||
                      /* Verticals */
                      (pcount >= surfhxscale        && yptr[-surfhxscale] == fg)        ||
                      (pcount >= 0                  && yptr[+surfhxscale] == fg)        ||
                      /* Horizontals */
                      (pcount >= xscale             && yptr[-xscale] == fg)             ||
                      (pcount >= 0                  && yptr[+xscale] == fg)
                  )
                  {
                     switch (alpha)
                     {
                        case GRAPH_ALPHA_0:
                           *yptr = *surf_ptr;
                           break;
                        case GRAPH_ALPHA_25:
                           BLEND_ALPHA25(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_50:
                           BLEND_ALPHA50(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_75:
                           BLEND_ALPHA75(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_100:
                        default:
                           break;
                     }
                  }
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
      uint16_t x, uint16_t y,
      uint8_t xscale, uint8_t yscale,
      uint32_t fg, uint32_t bg,
      libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg)
{
   short int xrepeat = 0;
   short int yrepeat = 0;
   short int pcount  = 0;
   unsigned short int surfw = 0;
   unsigned short int surfh = 0;
   unsigned short int surfhxscale = 0;

   uint32_t *yptr;
   uint32_t *surf_ptr;

   if (!linesurf32)
      return;

   surfw = linesurf32_w;
   surfh = linesurf32_h;
   yptr  = &linesurf32[0];

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
                  {
                     switch (alpha)
                     {
                        case GRAPH_ALPHA_0:
                           *yptr = *surf_ptr;
                           break;
                        case GRAPH_ALPHA_25:
                           BLEND32_ALPHA25(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_50:
                           BLEND32_ALPHA50(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_75:
                           BLEND32_ALPHA75(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_100:
                        default:
                           break;
                     }
                  }
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
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++, pcount++)
            {
               if (*yptr == bg)
               {
                  /* Right + Bottom right + Bottom */
                  if (
                        (pcount >= xscale             && yptr[-xscale] == fg)             ||
                        (pcount >= surfhxscale+xscale && yptr[-surfhxscale-xscale] == fg) ||
                        (pcount >= surfhxscale        && yptr[-surfhxscale] == fg)
                  )
                  {
                     switch (alpha)
                     {
                        case GRAPH_ALPHA_0:
                           *yptr = *surf_ptr;
                           break;
                        case GRAPH_ALPHA_25:
                           BLEND32_ALPHA25(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_50:
                           BLEND32_ALPHA50(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_75:
                           BLEND32_ALPHA75(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_100:
                        default:
                           break;
                     }
                  }
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
            for (xrepeat = x; xrepeat < surfw + x; xrepeat++, yptr++, pcount++)
            {
               if (*yptr == bg)
               {
                  if (
                      /* Diagonals */
                      (pcount >= surfhxscale+xscale && yptr[-surfhxscale-xscale] == fg) ||
                      (pcount >= xscale             && yptr[+surfhxscale-xscale] == fg) ||
                      (pcount >= surfhxscale        && yptr[-surfhxscale+xscale] == fg) ||
                      (pcount >= 0                  && yptr[+surfhxscale+xscale] == fg) ||
                      /* Verticals */
                      (pcount >= surfhxscale        && yptr[-surfhxscale] == fg)        ||
                      (pcount >= 0                  && yptr[+surfhxscale] == fg)        ||
                      /* Horizontals */
                      (pcount >= xscale             && yptr[-xscale] == fg)             ||
                      (pcount >= 0                  && yptr[+xscale] == fg)
                  )
                  {
                     switch (alpha)
                     {
                        case GRAPH_ALPHA_0:
                           *yptr = *surf_ptr;
                           break;
                        case GRAPH_ALPHA_25:
                           BLEND32_ALPHA25(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_50:
                           BLEND32_ALPHA50(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_75:
                           BLEND32_ALPHA75(*surf_ptr, bg, yptr);
                           break;
                        case GRAPH_ALPHA_100:
                        default:
                           break;
                     }
                  }
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

void draw_string_bmp16(uint16_t *surf, uint16_t x, uint16_t y,
      const char *string, uint16_t maxstrlen,
      uint16_t xscale, uint16_t yscale,
      uint16_t fg, uint16_t bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg)
{
   unsigned char strlen;
   unsigned char surfw;
   unsigned char surfh;
   unsigned char charw = 8;
   unsigned char charh = 8;
   uint16_t fg_blend = COLOR_BLACK_16;

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

   if ((linesurf16_w != surfw) || (linesurf16_h != surfh))
   {
      unsigned size = (surfw + xscale) * (surfh + yscale);
      if (linesurf16)
         free(linesurf16);

      linesurf16   = (uint16_t *)malloc(sizeof(uint16_t)*size);
      linesurf16_w = surfw;
      linesurf16_h = surfh;
   }

   draw_char_1pass16(string, strlen, charw, charh, xscale, yscale, fg, bg);
   draw_char_2pass16(surf, x, y, xscale, yscale, fg, bg, alpha, draw_bg);
}

void draw_string_bmp32(uint32_t *surf, uint16_t x, uint16_t y,
      const char *string, uint16_t maxstrlen,
      uint16_t xscale, uint16_t yscale,
      uint32_t fg, uint32_t bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg)
{
   unsigned char strlen;
   unsigned char surfw;
   unsigned char surfh;
   unsigned char charw = 8;
   unsigned char charh = 8;
   uint32_t fg_blend = COLOR_BLACK_32 & 0xFFFFFF;

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
      unsigned size = (surfw + xscale) * (surfh + yscale);
      if (linesurf32)
         free(linesurf32);

      linesurf32   = (uint32_t *)malloc(sizeof(uint32_t)*size);
      linesurf32_w = surfw;
      linesurf32_h = surfh;
   }

   draw_char_1pass32(string, strlen, charw, charh, xscale, yscale, fg, bg);
   draw_char_2pass32(surf, x, y, xscale, yscale, fg, bg, alpha, draw_bg);
}

void draw_text(uint16_t x, uint16_t y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      uint8_t scalex, uint8_t scaley, uint16_t max, const unsigned char *string)
{
   if (pix_bytes == 4)
      draw_text_bmp32((uint32_t *)retro_bmp, x, y, fgcol, bgcol, alpha, draw_bg, scalex, scaley, max, string);
   else
      draw_text_bmp16((uint16_t *)retro_bmp, x, y, fgcol, bgcol, alpha, draw_bg, scalex, scaley, max, string);
}

void draw_text_bmp16(uint16_t *buffer, uint16_t x, uint16_t y,
      uint16_t fgcol, uint16_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      uint8_t scalex, uint8_t scaley, uint16_t max, const unsigned char *string)
{
#if 0
   draw_string_bmp16(buffer, x, y, text, max, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
#else
   unsigned int i = 0;
   unsigned int xpos = 0;
   unsigned char c;
   unsigned char s[2] = {0};
   unsigned char charwidth_default = 6;
   unsigned char charwidth = charwidth_default;
   unsigned char cmax;

   if (string == NULL)
      return;

   cmax = strlen(string);
   cmax = (cmax > max) ? max : cmax;

   for (i = 0; i < cmax; i++)
   {
      bool narrow = false;

      c = string[i];
      if (c == 0)
         break;

      /* Linebreak hack */
      if (c == '\1')
      {
         xpos = 0;
         y += charwidth_default * scaley;
         continue;
      }

      /* Very narrow letters */
      if (c == 'l' || c == 'i')
      {
         narrow = true;
         xpos  -= scalex;
      }

      if (c & 0x80)
      {
         snprintf(s, sizeof(s), "%c", c - 0x80);
         draw_string_bmp16((uint16_t *)buffer, x + xpos, y, s, 1, scalex, scaley, bgcol, fgcol, alpha, draw_bg);
      }
      else
      {
         snprintf(s, sizeof(s), "%c", c);
         draw_string_bmp16((uint16_t *)buffer, x + xpos, y, s, 1, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
      }

      if (narrow)
      {
         charwidth = 3;
      }
      else
      {
         charwidth = charwidth_default;
         /* Narrower lower case */
         if (c >= 'a' && c <= 'z' && c != 'm' && c != 'w')
            charwidth = 4;
      }

      xpos += (charwidth * scalex);
   }
#endif
}

void draw_text_bmp32(uint32_t *buffer, uint16_t x, uint16_t y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      uint8_t scalex, uint8_t scaley, uint16_t max, const unsigned char *string)
{
#if 0
   draw_string_bmp32(buffer, x, y, text, max, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
#else
   unsigned int i = 0;
   unsigned int xpos = 0;
   unsigned char c;
   unsigned char s[2] = {0};
   unsigned char charwidth_default = 6;
   unsigned char charwidth = charwidth_default;
   unsigned char cmax;

   if (string == NULL)
      return;

   cmax = strlen(string);
   cmax = (cmax > max) ? max : cmax;

   for (i = 0; i < cmax; i++)
   {
      bool narrow = false;

      c = string[i];
      if (c == 0)
         break;

      /* Linebreak hack */
      if (c == '\1')
      {
         xpos = 0;
         y += charwidth_default * scaley;
         continue;
      }

      /* Very narrow letters */
      if (c == 'l' || c == 'i')
      {
         narrow = true;
         xpos  -= scalex;
      }

      if (c & 0x80)
      {
         snprintf(s, sizeof(s), "%c", c - 0x80);
         draw_string_bmp32(buffer, x + xpos, y, s, 1, scalex, scaley, bgcol, fgcol, alpha, draw_bg);
      }
      else
      {
         snprintf(s, sizeof(s), "%c", c);
         draw_string_bmp32(buffer, x + xpos, y, s, 1, scalex, scaley, fgcol, bgcol, alpha, draw_bg);
      }

      if (narrow)
      {
         charwidth = 3;
      }
      else
      {
         charwidth = charwidth_default;
         /* Narrower lower case */
         if (c >= 'a' && c <= 'z' && c != 'm' && c != 'w')
            charwidth = 4;
      }

      xpos += (charwidth * scalex);
   }
#endif
}

void libretro_graph_free(void)
{
   if (linesurf16)
      free(linesurf16);
   linesurf16 = NULL;

   if (linesurf32)
      free(linesurf32);
   linesurf32 = NULL;

   linesurf16_w = 0;
   linesurf16_h = 0;
   linesurf32_w = 0;
   linesurf32_h = 0;
}
