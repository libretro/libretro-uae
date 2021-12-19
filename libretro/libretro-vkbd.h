#ifndef LIBRETRO_VKBD_H
#define LIBRETRO_VKBD_H

#include "libretro-graph.h"

extern bool retro_vkbd;
extern bool retro_capslock;
extern signed char retro_vkbd_ready;

extern void print_vkbd(void);
extern void input_vkbd(void);
extern void toggle_vkbd(void);

extern unsigned int opt_vkbd_theme;
extern libretro_graph_alpha_t opt_vkbd_alpha;

extern retro_input_state_t input_state_cb;
extern int vkflag[10];

#define VKBDX 11
#define VKBDY 8

#endif /* LIBRETRO_VKBD_H */
