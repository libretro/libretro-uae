#include <stdio.h>
#include <SDL.h>
#include "SDL_ttf.h"

#define iconsizex 100
#define iconsizey 120
#define bosluk 10
#define TITLE_X 52
#define TITLE_Y 9

extern SDL_Surface *display;
#ifdef USE_GL
extern struct gl_buffer_t glbuffer;
extern void flush_gl_buffer (const struct gl_buffer_t *buffer, int first_line, int last_line);
extern void render_gl_buffer (const struct gl_buffer_t *buffer, int first_line, int last_line);
#endif

void write_text(int x, int y, char* txt);
void blit_image(SDL_Surface* img, int x, int y);
void selected_hilite (int ix, int iy, int mx, int my, SDL_Surface* img, int hangi);

enum { menu_sel_foo, menu_sel_expansion, menu_sel_prefs, menu_sel_keymaps, menu_sel_floppy, menu_sel_reset, menu_sel_storage, menu_sel_run, menu_sel_exit, menu_sel_tweaks };

SDL_Surface* pMouse_Pointer = NULL;
SDL_Surface* pMenu_Surface = NULL;
SDL_Surface* icon_expansion = NULL;
SDL_Surface* icon_preferences = NULL;
SDL_Surface* icon_keymaps = NULL;
SDL_Surface* icon_floppy = NULL;
SDL_Surface* icon_reset = NULL;
SDL_Surface* icon_storage = NULL;
SDL_Surface* icon_run = NULL;
SDL_Surface* icon_exit = NULL;
//SDL_Surface* icon_tweaks = NULL;

TTF_Font *amiga_font = NULL;
SDL_Color text_color;
SDL_Rect rect;
