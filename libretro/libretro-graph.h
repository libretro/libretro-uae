#ifndef LIBRETRO_GRAPH_H
#define LIBRETRO_GRAPH_H

#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))
#define RGB888(r, g, b) (((r * 255 / 31) << 16) | ((g * 255 / 31) << 8) | (b * 255 / 31))
#define ARGB888(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)

#define COLOR_RED_16             RGB565(128,   0,   0)
#define COLOR_RED_32       ARGB888(255, 128,   0,   0)
#define COLOR_GREEN_16           RGB565(  0, 128,   0)
#define COLOR_GREEN_32       ARGB888(255, 0, 128,   0)

#define COLOR_BLACK_16           RGB565(  5,   5,   5)
#define COLOR_BLACK_32     ARGB888(255,   5,   5,   5)
#define COLOR_WHITE_16           RGB565(255, 255, 255)
#define COLOR_WHITE_32     ARGB888(255, 255, 255, 255)

#define COLOR_10_16              RGB565( 10,  10,  10)
#define COLOR_10_32        ARGB888(255,  10,  10,  10)
#define COLOR_16_16              RGB565( 16,  16,  16)
#define COLOR_16_32        ARGB888(255,  16,  16,  16)
#define COLOR_32_16              RGB565( 32,  32,  32)
#define COLOR_32_32        ARGB888(255,  32,  32,  32)
#define COLOR_40_16              RGB565( 40,  40,  40)
#define COLOR_40_32        ARGB888(255,  40,  40,  40)
#define COLOR_64_16              RGB565( 64,  64,  64)
#define COLOR_64_32        ARGB888(255,  64,  64,  64)
#define COLOR_100_16             RGB565(100, 100, 100)
#define COLOR_100_32       ARGB888(255, 100, 100, 100)
#define COLOR_160_16             RGB565(160, 160, 160)
#define COLOR_160_32       ARGB888(255, 160, 160, 160)
#define COLOR_180_16             RGB565(180, 180, 180)
#define COLOR_180_32       ARGB888(255, 180, 180, 180)
#define COLOR_220_16             RGB565(220, 220, 220)
#define COLOR_220_32       ARGB888(255, 220, 220, 220)
#define COLOR_250_16             RGB565(250, 250, 250)
#define COLOR_250_32       ARGB888(255, 250, 250, 250)

#define COLOR_BEIGE_16           RGB565(208, 208, 202)
#define COLOR_BEIGE_32     ARGB888(255, 208, 208, 202)
#define COLOR_BEIGEDARK_16       RGB565(154, 154, 150)
#define COLOR_BEIGEDARK_32 ARGB888(255, 154, 154, 150)

extern unsigned short int graphed[RETRO_BMP_SIZE];

typedef enum {
   GRAPH_ALPHA_0 = 0,
   GRAPH_ALPHA_25,
   GRAPH_ALPHA_50,
   GRAPH_ALPHA_75,
   GRAPH_ALPHA_100
} libretro_graph_alpha_t;

typedef enum {
   GRAPH_BG_NONE = 0,
   GRAPH_BG_ALL,
   GRAPH_BG_SHADOW,
   GRAPH_BG_OUTLINE
} libretro_graph_bg_t;

void draw_fbox(int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha);
void draw_fbox_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, uint16_t color, libretro_graph_alpha_t alpha);
void draw_fbox_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha);

void draw_box(int x, int y, int dx, int dy, int width, int height, uint32_t color, libretro_graph_alpha_t alpha);
void draw_box_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, int width, int height, uint16_t color, libretro_graph_alpha_t alpha);
void draw_box_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, int width, int height, uint32_t color, libretro_graph_alpha_t alpha);

void draw_hline(int x, int y, int dx, int dy, uint32_t color);
void draw_hline_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, uint16_t color);
void draw_hline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color);

void draw_vline(int x, int y, int dx, int dy, uint32_t color);
void draw_vline_bmp16(uint16_t *buffer, int x, int y, int dx, int dy, uint16_t color);
void draw_vline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color);

void draw_text(uint16_t x, uint16_t y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      uint8_t scalex, uint8_t scaley, uint16_t max, const unsigned char *string);
void draw_text_bmp16(uint16_t *buffer, uint16_t x, uint16_t y,
      uint16_t fgcol, uint16_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      uint8_t scalex, uint8_t scaley, uint16_t max, const unsigned char *string);
void draw_text_bmp32(uint32_t *buffer, uint16_t x, uint16_t y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      uint8_t scalex, uint8_t scaley, uint16_t max, const unsigned char *string);

void draw_string16(uint16_t *surf, uint16_t x, uint16_t y,
      const char *string, uint16_t maxstrlen,
      uint16_t xscale, uint16_t yscale,
      uint16_t fg, uint16_t bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg);
void draw_string32(uint32_t *surf, uint16_t x, uint16_t y,
      const char *string, uint16_t maxstrlen,
      uint16_t xscale, uint16_t yscale,
      uint32_t fg, uint32_t bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg);

void libretro_graph_free(void);

#endif /* LIBRETRO_GRAPH_H */
