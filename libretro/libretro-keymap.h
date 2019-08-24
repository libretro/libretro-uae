
/* scancode key to MAC scan code mapping table */
const int keyboard_translation[512] =
{
	/* ST,  PC Code */
	-1,    /* 0 */ 	-1,    /* 1 */ 	-1,    /* 2 */ 	-1,    /* 3 */ 	-1,    /* 4 */ 	-1,    /* 5 */ 	-1,    /* 6 */ 	-1,    /* 7 */
	AK_BS,  /* SDLK_BACKSPACE=8 */
	AK_TAB,  /* SDLK_TAB=9 */
	-1,    /* 10 */ -1,    /* 11 */
	-1,  /* SDLK_CLEAR = 12 */
	AK_RET,  /* SDLK_RETURN = 13 */
	-1,    /* 14 */	-1,    /* 15 */	-1,    /* 16 */ -1,    /* 17 */	-1,    /* 18 */
	AKS_PAUSE,    /* SDLK_PAUSE = 19 */
	-1,    /* 20 */	-1,    /* 21 */	-1,    /* 22 */	-1,    /* 23 */	-1,    /* 24 */	-1,    /* 25 */	-1,    /* 26 */
	AK_ESC,  /* SDLK_ESCAPE = 27 */
	-1,    /* 28 */	-1,    /* 29 */	-1,    /* 30 */	-1,    /* 31 */
	AK_SPC,  /* SDLK_SPACE = 32 */
	-1,    /* SDLK_EXCLAIM = 33 */
	-1,    /* SDLK_QUOTEDBL = 34 */
	AK_NUMBERSIGN,    /* SDLK_HASH = 35 */
	-1,    /* SDLK_DOLLAR = 36 */
	-1,    /* 37 */
	-1,    /* SDLK_AMPERSAND = 38 */
	AK_QUOTE,  /* SDLK_QUOTE = 39 */
	-1,  /* SDLK_LEFTPAREN = 40 */
	AK_MINUS,  /* SDLK_RIGHTPAREN = 41 */
	-1,    /* SDLK_ASTERISK = 42 */
	AK_EQUAL,  /* SDLK_PLUS = 43 */
	AK_COMMA,  /* SDLK_COMMA = 44 */
	AK_MINUS,  /* SDLK_MINUS = 45 */
	AK_PERIOD,  /* SDLK_PERIOD = 46 */
	AK_SLASH,  /* SDLK_SLASH = 47 */
	AK_0,  /* SDLK_0 = 48 */
	AK_1,  /* SDLK_1 = 49 */
	AK_2,  /* SDLK_2 = 50 */
	AK_3,  /* SDLK_3 = 51 */
	AK_4,  /* SDLK_4 = 52 */
	AK_5,  /* SDLK_5 = 53 */
	AK_6,  /* SDLK_6 = 54 */
	AK_7,  /* SDLK_7 = 55 */
	AK_8,  /* SDLK_8 = 56 */
	AK_9,  /* SDLK_9 = 57 */
	-1,    /* SDLK_COLON = 58 */
	AK_SEMICOLON,    /* SDLK_SEMICOLON = 59 */
	AK_LTGT,    /* SDLK_LESS = 60 */
	AK_EQUAL,  /* SDLK_EQUALS = 61 */
	AK_LTGT,    /* SDLK_GREATER  = 62 */
	-1,    /* SDLK_QUESTION = 63 */
	-1,    /* SDLK_AT = 64 */
	-1,    /* 65 */  /* Skip uppercase letters */
	-1,    /* 66 */	-1,    /* 67 */	-1,    /* 68 */	-1,    /* 69 */	-1,    /* 70 */	-1,    /* 71 */	-1,    /* 72 */	-1,    /* 73 */	-1,    /* 74 */
	-1,    /* 75 */	-1,    /* 76 */ -1,    /* 77 */	-1,    /* 78 */	-1,    /* 79 */	-1,    /* 80 */	-1,    /* 81 */	-1,    /* 82 */	-1,    /* 83 */
	-1,    /* 84 */	-1,    /* 85 */	-1,    /* 86 */	-1,    /* 87 */	-1,    /* 88 */	-1,    /* 89 */	-1,    /* 90 */
	AK_LBRACKET,  /* SDLK_LEFTBRACKET = 91 */
	AK_BACKSLASH,  /* SDLK_BACKSLASH = 92 */     /* Might be 0x60 for UK keyboards */
	AK_RBRACKET,  /* SDLK_RIGHTBRACKET = 93 */
	-1,   /* SDLK_CARET = 94 */
	-1,    /* SDLK_UNDERSCORE = 95 */
	AK_BACKQUOTE,    /* SDLK_BACKQUOTE = 96 */
/*A*/AK_A,
/*B*/AK_B,
/*C*/AK_C,
/*D*/AK_D,
/*E*/AK_E,
/*F*/AK_F,
/*G*/AK_G,
/*H*/AK_H,
/*I*/AK_I,
/*J*/AK_J,
/*K*/AK_K,
/*L*/AK_L,
/*M*/AK_M,
/*N*/AK_N,
/*O*/AK_O,
/*P*/AK_P,
/*Q*/AK_Q,
/*R*/AK_R,
/*S*/AK_S,
/*T*/AK_T,
/*U*/AK_U,
/*V*/AK_V,
/*W*/AK_W,
/*X*/AK_X,
/*Y*/AK_Y,
/*Z*/AK_Z,
	-1,    /* 123 */	-1,    /* 124 */	-1,    /* 125 */	-1,    /* 126 */
	AK_DEL,  /* SDLK_DELETE = 127 */
	/* End of ASCII mapped keysyms */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143*/
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159*/
	0x0d, 0x0c, 0x1a, 0x28, 0x27, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175*/
	-1, -1, -1, -1, 0x0D, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191*/
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207*/
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x0C, /* 208-223*/
	-1, -1, -1, -1, 0x28, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239*/
	-1, -1, -1, -1, -1, -1, 0x27, -1, -1, -1, -1, -1, 0x1A, -1, -1, -1, /* 240-255*/
	/* Numeric keypad: */
	AK_NP0,    /* SDLK_KP0 = 256 */
	AK_NP1,    /* SDLK_KP1 = 257 */
	AK_NP2,    /* SDLK_KP2 = 258 */
	AK_NP3,    /* SDLK_KP3 = 259 */
	AK_NP4,    /* SDLK_KP4 = 260 */
	AK_NP5,    /* SDLK_KP5 = 261 */
	AK_NP6,    /* SDLK_KP6 = 262 */
	AK_NP7,    /* SDLK_KP7 = 263 */
	AK_NP8,    /* SDLK_KP8 = 264 */
	AK_NP9,    /* SDLK_KP9 = 265 */
	AK_NPDEL ,    /* SDLK_KP_PERIOD = 266 */
	AK_NPDIV,    /* SDLK_KP_DIVIDE = 267 */
	AK_NPMUL,    /* SDLK_KP_MULTIPLY = 268 */
	AK_NPSUB,    /* SDLK_KP_MINUS = 269 */
	AK_NPADD,    /* SDLK_KP_PLUS = 270 */
	AK_ENT,    /* SDLK_KP_ENTER = 271 */
	-1,    /* SDLK_KP_EQUALS = 272 */
	/* Arrows + Home/End pad */
	AK_UP,    /* SDLK_UP = 273 */
	AK_DN,    /* SDLK_DOWN = 274 */
	AK_RT,    /* SDLK_RIGHT = 275 */
	AK_LF,    /* SDLK_LEFT = 276 */
	AK_HELP,    /* SDLK_INSERT = 277 */
	-1, //AK_NPLPAREN,    /* SDLK_HOME = 278 */
	-1, //AK_NPRPAREN,    /* SDLK_END = 279 */
	AK_RAMI,    /* SDLK_PAGEUP = 280 */
	AK_LAMI,    /* SDLK_PAGEDOWN = 281 */
	/* Function keys */
	AK_F1,    /* SDLK_F1 = 282 */
	AK_F2,    /* SDLK_F2 = 283 */
	AK_F3,    /* SDLK_F3 = 284 */
	AK_F4,    /* SDLK_F4 = 285 */
	AK_F5,    /* SDLK_F5 = 286 */
	AK_F6,    /* SDLK_F6 = 287 */
	AK_F7,    /* SDLK_F7 = 288 */
	AK_F8,    /* SDLK_F8 = 289 */
	AK_F9,    /* SDLK_F9 = 290 */
	AK_F10,    /* SDLK_F10 = 291 */
	-1,      /* SDLK_F11 = 292 */
	-1,      /* SDLK_F12 = 293 */
	-1  ,    /* SDLK_F13 = 294 */
	-1,      /* SDLK_F14 = 295 */
	-1,      /* SDLK_F15 = 296 */
	-1,      /* 297 */	-1,      /* 298 */	-1,      /* 299 */
	/* Key state modifier keys */
	-1,      /* SDLK_NUMLOCK = 300 */
	AK_CAPSLOCK,    /* SDLK_CAPSLOCK = 301 */
	-1,    /* SDLK_SCROLLOCK = 302 */
	AK_RSH,    /* SDLK_RSHIFT = 303 */
	AK_LSH,    /* SDLK_LSHIFT = 304 */
	AK_RCTRL,    /* SDLK_RCTRL = 305 */
	AK_CTRL,    /* SDLK_LCTRL = 306 */
	AK_RALT,    /* SDLK_RALT = 307 */
	AK_LALT,    /* SDLK_LALT = 308 */
	AK_RAMI,      /* SDLK_RMETA = 309 */
	AK_LAMI,      /* SDLK_LMETA = 310 */
	AK_LAMI,      /* SDLK_LSUPER = 311 */
	AK_RAMI,      /* SDLK_RSUPER = 312 */
	-1,      /* SDLK_MODE = 313 */     /* "Alt Gr" key */
	-1,      /* SDLK_COMPOSE = 314 */
	/* Miscellaneous function keys */
	-1,    /* SDLK_HELP = 315 */
	-1/*AKS_SCREENSHOT*/,    /* SDLK_PRINT = 316 */
	-1,      /* SDLK_SYSREQ = 317 */
	-1,      /* SDLK_BREAK = 318 */
	-1,      /* SDLK_MENU = 319 */
	-1,      /* SDLK_POWER = 320 */
	-1,      /* SDLK_EURO = 321 */
	-1     /* SDLK_UNDO = 322 */
};

