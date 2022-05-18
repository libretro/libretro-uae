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

#define VKBDX_GAP_POS 0
#define VKBDY_GAP_POS 1
#define VKBDX_GAP_PAD 0
#define VKBDY_GAP_PAD 4

#define VKBD_NUMPAD             -2
#define VKBD_RESET              -3
#define VKBD_STATUSBAR_SAVEDISK -4
#define VKBD_JOYMOUSE           -5
#define VKBD_TURBOFIRE          -6
#define VKBD_ASPECT_ZOOM        -7
#define VKBD_SHIFTLOCK          -10

#define VKBD_MOUSE_UP           -11
#define VKBD_MOUSE_DOWN         -12
#define VKBD_MOUSE_LEFT         -13
#define VKBD_MOUSE_RIGHT        -14
#define VKBD_MOUSE_LMB          -15
#define VKBD_MOUSE_RMB          -16
#define VKBD_MOUSE_MMB          -17

#endif /* LIBRETRO_VKBD_H */
