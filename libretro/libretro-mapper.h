#ifndef LIBRETRO_MAPPER_H
#define LIBRETRO_MAPPER_H

#define RETRO_DEVICES                   5

#define RETRO_DEVICE_JOYSTICK           RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 0)
#define RETRO_DEVICE_CD32PAD            RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 1)
#define RETRO_DEVICE_ANALOGJOYSTICK     RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 2)

#define RETRO_DEVICE_ID_JOYPAD_LR       16
#define RETRO_DEVICE_ID_JOYPAD_LL       17
#define RETRO_DEVICE_ID_JOYPAD_LD       18
#define RETRO_DEVICE_ID_JOYPAD_LU       19
#define RETRO_DEVICE_ID_JOYPAD_RR       20
#define RETRO_DEVICE_ID_JOYPAD_RL       21
#define RETRO_DEVICE_ID_JOYPAD_RD       22
#define RETRO_DEVICE_ID_JOYPAD_RU       23

#define RETRO_DEVICE_ID_JOYPAD_LAST     24

#define RETRO_MAPPER_VKBD               24
#define RETRO_MAPPER_STATUSBAR          25
#define RETRO_MAPPER_JOYMOUSE           26
#define RETRO_MAPPER_RESET              27
#define RETRO_MAPPER_ASPECT_RATIO       28
#define RETRO_MAPPER_ZOOM_MODE          29

#define RETRO_MAPPER_LAST               30

#define TOGGLE_VKBD                     -11
#define TOGGLE_STATUSBAR                -12
#define SWITCH_JOYMOUSE                 -13
#define MOUSE_LEFT_BUTTON               -2
#define MOUSE_RIGHT_BUTTON              -3
#define MOUSE_MIDDLE_BUTTON             -4
#define MOUSE_SLOWER                    -5
#define MOUSE_FASTER                    -6
#define JOYSTICK_FIRE                   -7
#define JOYSTICK_2ND_FIRE               -8

typedef struct
{
   int id;
   char value[20];
   char label[25];
} retro_keymap;

static retro_keymap retro_keys[RETROK_LAST] =
{
   {0,                  "---",                 "---"},
   {TOGGLE_VKBD,        "TOGGLE_VKBD",         "Toggle Virtual Keyboard"},
   {TOGGLE_STATUSBAR,   "TOGGLE_STATUSBAR",    "Toggle Statusbar"},
   {SWITCH_JOYMOUSE,    "SWITCH_JOYMOUSE",     "Switch Joystick/Mouse"},
   {MOUSE_SLOWER,       "MOUSE_SLOWER",        "Mouse Slower"},
   {MOUSE_FASTER,       "MOUSE_FASTER",        "Mouse Faster"},
   {MOUSE_LEFT_BUTTON,  "MOUSE_LEFT_BUTTON",   "Mouse Left Button"},
   {MOUSE_RIGHT_BUTTON, "MOUSE_RIGHT_BUTTON",  "Mouse Right Button"},
   {MOUSE_MIDDLE_BUTTON,"MOUSE_MIDDLE_BUTTON", "Mouse Middle Button"},
   {JOYSTICK_FIRE,      "JOYSTICK_FIRE",       "Joystick Fire Button"},
   {JOYSTICK_2ND_FIRE,  "JOYSTICK_2ND_FIRE",   "Joystick 2nd Fire Button"},
   {RETROK_BACKSPACE,   "RETROK_BACKSPACE",    "Backspace"},
   {RETROK_TAB,         "RETROK_TAB",          "Tab"},
/* {RETROK_CLEAR,       "RETROK_CLEAR",        "Clear"}, */
   {RETROK_RETURN,      "RETROK_RETURN",       "Return"},
/* {RETROK_PAUSE,       "RETROK_PAUSE",        "Pause"}, */
   {RETROK_ESCAPE,      "RETROK_ESCAPE",       "Escape"},
   {RETROK_SPACE,       "RETROK_SPACE",        "Space"},
/* {RETROK_EXCLAIM,     "RETROK_EXCLAIM",      "!"}, */
/* {RETROK_QUOTEDBL,    "RETROK_QUOTEDBL",     "\""}, */
/* {RETROK_HASH,        "RETROK_HASH",         "#"}, */
/* {RETROK_DOLLAR,      "RETROK_DOLLAR",       "$"}, */
/* {RETROK_AMPERSAND,   "RETROK_AMPERSAND",    "&"}, */
   {RETROK_QUOTE,       "RETROK_QUOTE",        "\'"},
   {RETROK_LEFTPAREN,   "RETROK_LEFTPAREN",    "("},
   {RETROK_RIGHTPAREN,  "RETROK_RIGHTPAREN",   ")"},
   {RETROK_ASTERISK,    "RETROK_ASTERISK",     "*"},
   {RETROK_PLUS,        "RETROK_PLUS",         "+"},
   {RETROK_COMMA,       "RETROK_COMMA",        ","},
   {RETROK_MINUS,       "RETROK_MINUS",        "-"},
   {RETROK_PERIOD,      "RETROK_PERIOD",       "."},
   {RETROK_SLASH,       "RETROK_SLASH",        "/"},
   {RETROK_0,           "RETROK_0",            "0"},
   {RETROK_1,           "RETROK_1",            "1"},
   {RETROK_2,           "RETROK_2",            "2"},
   {RETROK_3,           "RETROK_3",            "3"},
   {RETROK_4,           "RETROK_4",            "4"},
   {RETROK_5,           "RETROK_5",            "5"},
   {RETROK_6,           "RETROK_6",            "6"},
   {RETROK_7,           "RETROK_7",            "7"},
   {RETROK_8,           "RETROK_8",            "8"},
   {RETROK_9,           "RETROK_9",            "9"},
   {RETROK_COLON,       "RETROK_COLON",        ":"},
   {RETROK_SEMICOLON,   "RETROK_SEMICOLON",    ";"},
   {RETROK_LESS,        "RETROK_LESS",         "<"},
   {RETROK_EQUALS,      "RETROK_EQUALS",       "="},
   {RETROK_GREATER,     "RETROK_GREATER",      ">"},
/* {RETROK_QUESTION,    "RETROK_QUESTION",     "?"}, */
/* {RETROK_AT,          "RETROK_AT",           "@"}, */
   {RETROK_LEFTBRACKET, "RETROK_LEFTBRACKET",  "["},
   {RETROK_BACKSLASH,   "RETROK_BACKSLASH",    "\\"},
   {RETROK_RIGHTBRACKET,"RETROK_RIGHTBRACKET", "]"},
   {RETROK_CARET,       "RETROK_CARET",        "^"},
   {RETROK_UNDERSCORE,  "RETROK_UNDERSCORE",   "_"},
   {RETROK_BACKQUOTE,   "RETROK_BACKQUOTE",    "`"},
   {RETROK_a,           "RETROK_a",            "A"},
   {RETROK_b,           "RETROK_b",            "B"},
   {RETROK_c,           "RETROK_c",            "C"},
   {RETROK_d,           "RETROK_d",            "D"},
   {RETROK_e,           "RETROK_e",            "E"},
   {RETROK_f,           "RETROK_f",            "F"},
   {RETROK_g,           "RETROK_g",            "G"},
   {RETROK_h,           "RETROK_h",            "H"},
   {RETROK_i,           "RETROK_i",            "I"},
   {RETROK_j,           "RETROK_j",            "J"},
   {RETROK_k,           "RETROK_k",            "K"},
   {RETROK_l,           "RETROK_l",            "L"},
   {RETROK_m,           "RETROK_m",            "M"},
   {RETROK_n,           "RETROK_n",            "N"},
   {RETROK_o,           "RETROK_o",            "O"},
   {RETROK_p,           "RETROK_p",            "P"},
   {RETROK_q,           "RETROK_q",            "Q"},
   {RETROK_r,           "RETROK_r",            "R"},
   {RETROK_s,           "RETROK_s",            "S"},
   {RETROK_t,           "RETROK_t",            "T"},
   {RETROK_u,           "RETROK_u",            "U"},
   {RETROK_v,           "RETROK_v",            "V"},
   {RETROK_w,           "RETROK_w",            "W"},
   {RETROK_x,           "RETROK_x",            "X"},
   {RETROK_y,           "RETROK_y",            "Y"},
   {RETROK_z,           "RETROK_z",            "Z"},
   {RETROK_DELETE,      "RETROK_DELETE",       "Delete"},
   {RETROK_KP0,         "RETROK_KP0",          "Numpad 0"},
   {RETROK_KP1,         "RETROK_KP1",          "Numpad 1"},
   {RETROK_KP2,         "RETROK_KP2",          "Numpad 2"},
   {RETROK_KP3,         "RETROK_KP3",          "Numpad 3"},
   {RETROK_KP4,         "RETROK_KP4",          "Numpad 4"},
   {RETROK_KP5,         "RETROK_KP5",          "Numpad 5"},
   {RETROK_KP6,         "RETROK_KP6",          "Numpad 6"},
   {RETROK_KP7,         "RETROK_KP7",          "Numpad 7"},
   {RETROK_KP8,         "RETROK_KP8",          "Numpad 8"},
   {RETROK_KP9,         "RETROK_KP9",          "Numpad 9"},
   {RETROK_KP_PERIOD,   "RETROK_KP_PERIOD",    "Numpad ."},
   {RETROK_KP_DIVIDE,   "RETROK_KP_DIVIDE",    "Numpad /"},
   {RETROK_KP_MULTIPLY, "RETROK_KP_MULTIPLY",  "Numpad *"},
   {RETROK_KP_MINUS,    "RETROK_KP_MINUS",     "Numpad -"},
   {RETROK_KP_PLUS,     "RETROK_KP_PLUS",      "Numpad +"},
   {RETROK_KP_ENTER,    "RETROK_KP_ENTER",     "Numpad Enter"},
   {RETROK_KP_EQUALS,   "RETROK_KP_EQUALS",    "Numpad ="},
   {RETROK_UP,          "RETROK_UP",           "Up"},
   {RETROK_DOWN,        "RETROK_DOWN",         "Down"},
   {RETROK_RIGHT,       "RETROK_RIGHT",        "Right"},
   {RETROK_LEFT,        "RETROK_LEFT",         "Left"},
   {RETROK_INSERT,      "RETROK_INSERT",       "Insert"},
   {RETROK_HOME,        "RETROK_HOME",         "Home"},
   {RETROK_END,         "RETROK_END",          "End"},
   {RETROK_PAGEUP,      "RETROK_PAGEUP",       "Page Up"},
   {RETROK_PAGEDOWN,    "RETROK_PAGEDOWN",     "Page Down"},
   {RETROK_F1,          "RETROK_F1",           "F1"},
   {RETROK_F2,          "RETROK_F2",           "F2"},
   {RETROK_F3,          "RETROK_F3",           "F3"},
   {RETROK_F4,          "RETROK_F4",           "F4"},
   {RETROK_F5,          "RETROK_F5",           "F5"},
   {RETROK_F6,          "RETROK_F6",           "F6"},
   {RETROK_F7,          "RETROK_F7",           "F7"},
   {RETROK_F8,          "RETROK_F8",           "F8"},
   {RETROK_F9,          "RETROK_F9",           "F9"},
   {RETROK_F10,         "RETROK_F10",          "F10"},
   {RETROK_F11,         "RETROK_F11",          "F11"},
   {RETROK_F12,         "RETROK_F12",          "F12"},
   {RETROK_F13,         "RETROK_F13",          "F13"},
   {RETROK_F14,         "RETROK_F14",          "F14"},
   {RETROK_F15,         "RETROK_F15",          "F15"},
/* {RETROK_NUMLOCK,     "RETROK_NUMLOCK",      "Num Lock"}, */
/* {RETROK_CAPSLOCK,    "RETROK_CAPSLOCK",     "Caps Lock"}, */
/* {RETROK_SCROLLOCK,   "RETROK_SCROLLOCK",    "Scroll Lock"}, */
   {RETROK_RSHIFT,      "RETROK_RSHIFT",       "Right Shift"},
   {RETROK_LSHIFT,      "RETROK_LSHIFT",       "Left Shift"},
   {RETROK_RCTRL,       "RETROK_RCTRL",        "Right Control"},
   {RETROK_LCTRL,       "RETROK_LCTRL",        "Left Control"},
   {RETROK_RALT,        "RETROK_RALT",         "Right Alt"},
   {RETROK_LALT,        "RETROK_LALT",         "Left Alt"},
/* {RETROK_RMETA,       "RETROK_RMETA",        "Right Meta"}, */
/* {RETROK_LMETA,       "RETROK_LMETA",        "Left Meta"}, */
   {RETROK_RSUPER,      "RETROK_RSUPER",       "Right Super"},
   {RETROK_LSUPER,      "RETROK_LSUPER",       "Left Super"},
/* {RETROK_MODE,        "RETROK_MODE",         "Mode"}, */
/* {RETROK_COMPOSE,     "RETROK_COMPOSE",      "Compose"}, */
   {RETROK_HELP,        "RETROK_HELP",         "Help"},
/* {RETROK_PRINT,       "RETROK_PRINT",        "Print"}, */
/* {RETROK_SYSREQ,      "RETROK_SYSREQ",       "Sys Reg"}, */
/* {RETROK_BREAK,       "RETROK_BREAK",        "Break"}, */
/* {RETROK_MENU,        "RETROK_MENU",         "Menu"}, */
/* {RETROK_POWER,       "RETROK_POWER",        "Power"}, */
/* {RETROK_EURO,        "RETROK_EURO",         "Euro"}, */
/* {RETROK_UNDO,        "RETROK_UNDO",         "Undo"}, */
/* {RETROK_OEM_102,     "RETROK_OEM_102",      "OEM-102"} */
   {RETROK_LAST, {0}, {0}}
};

#include "keyboard.h"
static const int keyboard_translation[512] =
{
   -1,             /* 0 */
   -1,             /* 1 */
   -1,             /* 2 */
   -1,             /* 3 */
   -1,             /* 4 */
   -1,             /* 5 */
   -1,             /* 6 */
   -1,             /* 7 */
   AK_BS,          /* 8 = RETROK_BACKSPACE */
   AK_TAB,         /* 9 = RETROK_TAB */
   -1,             /* 10 */
   -1,             /* 11 */
   -1,             /* 12 = RETROK_CLEAR */
   AK_RET,         /* 13 = RETROK_RETURN */
   -1,             /* 14 */
   -1,             /* 15 */
   -1,             /* 16 */
   -1,             /* 17 */
   -1,             /* 18 */
   -1,             /* 19 = RETROK_PAUSE */
   -1,             /* 20 */
   -1,             /* 21 */
   -1,             /* 22 */
   -1,             /* 23 */
   -1,             /* 24 */
   -1,             /* 25 */
   -1,             /* 26 */
   AK_ESC,         /* 27 = RETROK_ESCAPE */
   -1,             /* 28 */
   -1,             /* 29 */
   -1,             /* 30 */
   -1,             /* 31 */
   AK_SPC,         /* 32 = RETROK_SPACE */
   -1,             /* 33 = RETROK_EXCLAIM */
   -1,             /* 34 = RETROK_QUOTEDBL */
   AK_NUMBERSIGN,  /* 35 = RETROK_HASH */
   -1,             /* 36 = RETROK_DOLLAR */
   -1,             /* 37 */
   -1,             /* 38 = RETROK_AMPERSAND */
   AK_QUOTE,       /* 39 = RETROK_QUOTE */
   -1,             /* 40 = RETROK_LEFTPAREN */
   AK_MINUS,       /* 41 = RETROK_RIGHTPAREN */
   -1,             /* 42 = RETROK_ASTERISK */
   AK_EQUAL,       /* 43 = RETROK_PLUS */
   AK_COMMA,       /* 44 = RETROK_COMMA */
   AK_MINUS,       /* 45 = RETROK_MINUS */
   AK_PERIOD,      /* 46 = RETROK_PERIOD */
   AK_SLASH,       /* 47 = RETROK_SLASH */
   AK_0,           /* 48 = RETROK_0 */
   AK_1,           /* 49 = RETROK_1 */
   AK_2,           /* 50 = RETROK_2 */
   AK_3,           /* 51 = RETROK_3 */
   AK_4,           /* 52 = RETROK_4 */
   AK_5,           /* 53 = RETROK_5 */
   AK_6,           /* 54 = RETROK_6 */
   AK_7,           /* 55 = RETROK_7 */
   AK_8,           /* 56 = RETROK_8 */
   AK_9,           /* 57 = RETROK_9 */
   -1,             /* 58 = RETROK_COLON */
   AK_SEMICOLON,   /* 59 = RETROK_SEMICOLON */
   AK_LTGT,        /* 60 = RETROK_LESS */
   AK_EQUAL,       /* 61 = RETROK_EQUALS */
   AK_LTGT,        /* 62 = RETROK_GREATER */
   -1,             /* 63 = RETROK_QUESTION */
   -1,             /* 64 = RETROK_AT */
   -1,             /* 65 */
   -1,             /* 66 */
   -1,             /* 67 */
   -1,             /* 68 */
   -1,             /* 69 */
   -1,             /* 70 */
   -1,             /* 71 */
   -1,             /* 72 */
   -1,             /* 73 */
   -1,             /* 74 */
   -1,             /* 75 */
   -1,             /* 76 */
   -1,             /* 77 */
   -1,             /* 78 */
   -1,             /* 79 */
   -1,             /* 80 */
   -1,             /* 81 */
   -1,             /* 82 */
   -1,             /* 83 */
   -1,             /* 84 */
   -1,             /* 85 */
   -1,             /* 86 */
   -1,             /* 87 */
   -1,             /* 88 */
   -1,             /* 89 */
   -1,             /* 90 */
   AK_LBRACKET,    /* 91 = RETROK_LEFTBRACKET */
   AK_BACKSLASH,   /* 92 = RETROK_BACKSLASH */
   AK_RBRACKET,    /* 93 = RETROK_RIGHTBRACKET */
   -1,             /* 94 = RETROK_CARET */
   -1,             /* 95 = RETROK_UNDERSCORE */
   AK_BACKQUOTE,   /* 96 = RETROK_BACKQUOTE */
   AK_A,           /* 97 = RETROK_a */
   AK_B,           /* 98 = RETROK_b */
   AK_C,           /* 99 = RETROK_c */
   AK_D,           /* 100 = RETROK_d */
   AK_E,           /* 101 = RETROK_e */
   AK_F,           /* 102 = RETROK_f */
   AK_G,           /* 103 = RETROK_g */
   AK_H,           /* 104 = RETROK_h */
   AK_I,           /* 105 = RETROK_i */
   AK_J,           /* 106 = RETROK_j */
   AK_K,           /* 107 = RETROK_k */
   AK_L,           /* 108 = RETROK_l */
   AK_M,           /* 109 = RETROK_m */
   AK_N,           /* 110 = RETROK_n */
   AK_O,           /* 111 = RETROK_o */
   AK_P,           /* 112 = RETROK_p */
   AK_Q,           /* 113 = RETROK_q */
   AK_R,           /* 114 = RETROK_r */
   AK_S,           /* 115 = RETROK_s */
   AK_T,           /* 116 = RETROK_t */
   AK_U,           /* 117 = RETROK_u */
   AK_V,           /* 118 = RETROK_v */
   AK_W,           /* 119 = RETROK_w */
   AK_X,           /* 120 = RETROK_x */
   AK_Y,           /* 121 = RETROK_y */
   AK_Z,           /* 122 = RETROK_z */
   -1,             /* 123 = RETROK_LEFTBRACE */
   -1,             /* 124 = RETROK_BAR */
   -1,             /* 125 = RETROK_RIGHTBRACE */
   -1,             /* 126 = RETROK_TILDE */
   AK_DEL,         /* 127 = RETROK_DELETE */
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,             /* 128-143 */
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,             /* 144-159 */
   0x0d, 0x0c, 0x1a, 0x28, 0x27, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 160-175 */
   -1, -1, -1, -1, 0x0D, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,           /* 176-191 */
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,             /* 192-207 */
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x0C,           /* 208-223 */
   -1, -1, -1, -1, 0x28, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,           /* 224-239 */
   -1, -1, -1, -1, -1, -1, 0x27, -1, -1, -1, -1, -1, 0x1A, -1, -1, -1,         /* 240-255 */
   AK_NP0,         /* 256 = RETROK_KP0 */
   AK_NP1,         /* 257 = RETROK_KP1 */
   AK_NP2,         /* 258 = RETROK_KP2 */
   AK_NP3,         /* 259 = RETROK_KP3 */
   AK_NP4,         /* 260 = RETROK_KP4 */
   AK_NP5,         /* 261 = RETROK_KP5 */
   AK_NP6,         /* 262 = RETROK_KP6 */
   AK_NP7,         /* 263 = RETROK_KP7 */
   AK_NP8,         /* 264 = RETROK_KP8 */
   AK_NP9,         /* 265 = RETROK_KP9 */
   AK_NPDEL,       /* 266 = RETROK_KP_PERIOD */
   AK_NPDIV,       /* 267 = RETROK_KP_DIVIDE */
   AK_NPMUL,       /* 268 = RETROK_KP_MULTIPLY */
   AK_NPSUB,       /* 269 = RETROK_KP_MINUS */
   AK_NPADD,       /* 270 = RETROK_KP_PLUS */
   AK_ENT,         /* 271 = RETROK_KP_ENTER */
   -1,             /* 272 = RETROK_KP_EQUALS */
   AK_UP,          /* 273 = RETROK_UP */
   AK_DN,          /* 274 = RETROK_DOWN */
   AK_RT,          /* 275 = RETROK_RIGHT */
   AK_LF,          /* 276 = RETROK_LEFT */
   AK_HELP,        /* 277 = RETROK_INSERT */
   AK_NPLPAREN,    /* 278 = RETROK_HOME */
   AK_NPRPAREN,    /* 279 = RETROK_END */
   AK_LAMI,        /* 280 = RETROK_PAGEUP */
   AK_RAMI,        /* 281 = RETROK_PAGEDOWN */
   AK_F1,          /* 282 = RETROK_F1 */
   AK_F2,          /* 283 = RETROK_F2 */
   AK_F3,          /* 284 = RETROK_F3 */
   AK_F4,          /* 285 = RETROK_F4 */
   AK_F5,          /* 286 = RETROK_F5 */
   AK_F6,          /* 287 = RETROK_F6 */
   AK_F7,          /* 288 = RETROK_F7 */
   AK_F8,          /* 289 = RETROK_F8 */
   AK_F9,          /* 290 = RETROK_F9 */
   AK_F10,         /* 291 = RETROK_F10 */
   -1,             /* 292 = RETROK_F11 */
   -1,             /* 293 = RETROK_F12 */
   -1,             /* 294 = RETROK_F13 */
   -1,             /* 295 = RETROK_F14 */
   -1,             /* 296 = RETROK_F15 */
   -1,             /* 297 */
   -1,             /* 298 */
   -1,             /* 299 */
   -1,             /* 300 = RETROK_NUMLOCK */
   AK_CAPSLOCK,    /* 301 = RETROK_CAPSLOCK */
   -1,             /* 302 = RETROK_SCROLLOCK */
   AK_RSH,         /* 303 = RETROK_RSHIFT */
   AK_LSH,         /* 304 = RETROK_LSHIFT */
   AK_CTRL,        /* 305 = RETROK_RCTRL */
   AK_CTRL,        /* 306 = RETROK_LCTRL */
   AK_RALT,        /* 307 = RETROK_RALT */
   AK_LALT,        /* 308 = RETROK_LALT */
   AK_RAMI,        /* 309 = RETROK_RMETA */
   AK_LAMI,        /* 310 = RETROK_LMETA */
   AK_LAMI,        /* 311 = RETROK_LSUPER */
   AK_RAMI,        /* 312 = RETROK_RSUPER */
   -1,             /* 313 = RETROK_MODE */
   -1,             /* 314 = RETROK_COMPOSE */
   AK_HELP,        /* 315 = RETROK_HELP */
   -1,             /* 316 = RETROK_PRINT */
   -1,             /* 317 = RETROK_SYSREQ */
   -1,             /* 318 = RETROK_BREAK */
   -1,             /* 319 = RETROK_MENU */
   -1,             /* 320 = RETROK_POWER */
   -1,             /* 321 = RETROK_EURO */
   -1,             /* 322 = RETROK_UNDO */
   AK_LTGT,        /* 323 = RETROK_OEM_102 */
   -1              /* 324 = RETROK_LAST */
};

#endif /* LIBRETRO_MAPPER_H */
