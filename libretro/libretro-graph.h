#ifndef LIBRETRO_GRAPH_H
#define LIBRETRO_GRAPH_H

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
void draw_fbox_bmp(unsigned short *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha);
void draw_fbox_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha);

void draw_box_bmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
void draw_box_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color);

void draw_point_bmp(unsigned short *buffer, int x, int y, unsigned short color);

void draw_hline(int x, int y, int dx, int dy, uint32_t color);
void draw_hline_bmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
void draw_hline_bmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color);

void draw_vline_bmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
void draw_vline_bmp(unsigned short *buffer, int x1, int y1, int x2, int y2, unsigned short color);

void draw_string(unsigned short *surf, unsigned short int x, unsigned short int y,
      const char *string, unsigned short int maxstrlen,
      unsigned short int xscale, unsigned short int yscale,
      unsigned short int fg, unsigned short int bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg);
void draw_string32(uint32_t *surf, unsigned short int x, unsigned short int y,
      const char *string, unsigned short int maxstrlen,
      unsigned short int xscale, unsigned short int yscale,
      uint32_t fg, uint32_t bg, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg);

void draw_text(unsigned short int x, unsigned short int y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      unsigned short int scalex, unsigned short int scaley, unsigned short int max, unsigned char *string);
void draw_text_bmp(unsigned short *buffer, unsigned short int x, unsigned short int y,
      unsigned short int fgcol, unsigned short int bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      unsigned short int scalex, unsigned short int scaley, unsigned short int max, unsigned char *string);
void draw_text_bmp32(uint32_t *buffer, unsigned short int x, unsigned short int y,
      uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, libretro_graph_bg_t draw_bg,
      unsigned short int scalex, unsigned short int scaley, unsigned short int max, unsigned char *string);

void libretro_graph_free(void);

#endif /* LIBRETRO_GRAPH_H */
