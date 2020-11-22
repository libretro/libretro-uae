#ifndef LIBRETRO_GRAPH_H
#define LIBRETRO_GRAPH_H

typedef struct
{
   int x, y;
   int dx, dy;
} box;

typedef enum {
   GRAPH_ALPHA_0 = 0,
   GRAPH_ALPHA_25,
   GRAPH_ALPHA_50,
   GRAPH_ALPHA_75,
   GRAPH_ALPHA_100
} libretro_graph_alpha_t;

void DrawFBoxBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color, libretro_graph_alpha_t alpha);
void DrawFBoxBmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color, libretro_graph_alpha_t alpha);

void DrawBoxBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
void DrawBoxBmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color);

void DrawPointBmp(unsigned short *buffer, int x, int y, unsigned short color);

void DrawHlineBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
void DrawHlineBmp32(uint32_t *buffer, int x, int y, int dx, int dy, uint32_t color);
void DrawHline(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);

void DrawVlineBmp(unsigned short *buffer, int x, int y, int dx, int dy, unsigned short color);
void DrawlineBmp(unsigned short *buffer, int x1, int y1, int x2, int y2, unsigned short color);

void Draw_string(unsigned short *surf, signed short int x, signed short int y, const char *string, unsigned short int maxstrlen, unsigned short int xscale, unsigned short int yscale, unsigned short int fg, unsigned short int bg, libretro_graph_alpha_t alpha, bool draw_bg);
void Draw_string32(uint32_t *surf, signed short int x, signed short int y, const char *string, unsigned short int maxstrlen, unsigned short int xscale, unsigned short int yscale, uint32_t fg, uint32_t bg, libretro_graph_alpha_t alpha, bool draw_bg);

void Draw_text(unsigned short *buffer, int x, int y, unsigned short fgcol, unsigned short int bgcol, libretro_graph_alpha_t alpha, bool draw_bg, int scalex, int scaley, int max, char *string, ...);
void Draw_text32(uint32_t *buffer, int x, int y, uint32_t fgcol, uint32_t bgcol, libretro_graph_alpha_t alpha, bool draw_bg, int scalex, int scaley, int max, char *string, ...);

void LibretroGraphFree(void);

#endif /* LIBRETRO_GRAPH_H */
