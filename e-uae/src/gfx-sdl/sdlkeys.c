 /*
  * UAE - The Un*x Amiga Emulator
  *
  * SDL keyboard driver which maps SDL cooked keysyms to raw
  * Amiga keycodes
  *
  * Copyright 2004 Richard Drummond
  *
  * Based on previous SDL keymapping
  * Copyright 2001 Bernd Lachner (EMail: dev@lachner-net.de)
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include "keyboard.h"
#include "hotkeys.h"

#include "sdlgfx.h"

#include <SDL.h>

/*
 * This function knows about all keys that are common
 * between different keyboard languages.
 */
static int kc_decode (int keysym)
{
    switch (keysym) {
	case SDLK_b:		return AK_B;
	case SDLK_c:		return AK_C;
	case SDLK_d:		return AK_D;
	case SDLK_e:		return AK_E;
	case SDLK_f:		return AK_F;
	case SDLK_g:		return AK_G;
	case SDLK_h:		return AK_H;
	case SDLK_i:		return AK_I;
	case SDLK_j:		return AK_J;
	case SDLK_k:		return AK_K;
	case SDLK_l:		return AK_L;
	case SDLK_n:		return AK_N;
	case SDLK_o:		return AK_O;
	case SDLK_p:		return AK_P;
	case SDLK_r:		return AK_R;
	case SDLK_s:		return AK_S;
	case SDLK_t:		return AK_T;
	case SDLK_u:		return AK_U;
	case SDLK_v:		return AK_V;
	case SDLK_x:		return AK_X;

	case SDLK_0:		return AK_0;
	case SDLK_1:		return AK_1;
	case SDLK_2:		return AK_2;
	case SDLK_3:		return AK_3;
	case SDLK_4:		return AK_4;
	case SDLK_5:		return AK_5;
	case SDLK_6:		return AK_6;
	case SDLK_7:		return AK_7;
	case SDLK_8:		return AK_8;
	case SDLK_9:		return AK_9;

	case SDLK_KP0:		return AK_NP0;
	case SDLK_KP1:		return AK_NP1;
	case SDLK_KP2:		return AK_NP2;
	case SDLK_KP3:		return AK_NP3;
	case SDLK_KP4:		return AK_NP4;
	case SDLK_KP5:		return AK_NP5;
	case SDLK_KP6:		return AK_NP6;
	case SDLK_KP7:		return AK_NP7;
	case SDLK_KP8:		return AK_NP8;
	case SDLK_KP9:		return AK_NP9;
	case SDLK_KP_DIVIDE:	return AK_NPDIV;
	case SDLK_KP_MULTIPLY:	return AK_NPMUL;
	case SDLK_KP_MINUS:	return AK_NPSUB;
	case SDLK_KP_PLUS:	return AK_NPADD;
	case SDLK_KP_PERIOD:	return AK_NPDEL;
	case SDLK_KP_ENTER:	return AK_ENT;

	case SDLK_F1: 		return AK_F1;
	case SDLK_F2: 		return AK_F2;
	case SDLK_F3: 		return AK_F3;
	case SDLK_F4: 		return AK_F4;
	case SDLK_F5: 		return AK_F5;
	case SDLK_F6: 		return AK_F6;
	case SDLK_F7: 		return AK_F7;
	case SDLK_F8: 		return AK_F8;
	case SDLK_F9: 		return AK_F9;
	case SDLK_F10: 		return AK_F10;

	case SDLK_BACKSPACE: 	return AK_BS;
	case SDLK_DELETE: 	return AK_DEL;
	case SDLK_LCTRL: 	return AK_CTRL;
	case SDLK_RCTRL: 	return AK_RCTRL;
	case SDLK_TAB: 		return AK_TAB;
	case SDLK_LALT: 	return AK_LALT;
	case SDLK_RALT: 	return AK_RALT;
	case SDLK_RMETA: 	return AK_RAMI;
	case SDLK_LMETA: 	return AK_LAMI;
	case SDLK_RETURN: 	return AK_RET;
	case SDLK_SPACE: 	return AK_SPC;
	case SDLK_LSHIFT: 	return AK_LSH;
	case SDLK_RSHIFT: 	return AK_RSH;
	case SDLK_ESCAPE: 	return AK_ESC;

	case SDLK_INSERT: 	return AK_HELP;
	case SDLK_HOME: 	return AK_NPLPAREN;
	case SDLK_END: 		return AK_NPRPAREN;
	case SDLK_CAPSLOCK: 	return AK_CAPSLOCK;

	case SDLK_UP: 		return AK_UP;
	case SDLK_DOWN: 	return AK_DN;
	case SDLK_LEFT: 	return AK_LF;
	case SDLK_RIGHT: 	return AK_RT;

	case SDLK_PAGEUP:
	case SDLK_RSUPER:	return AK_RAMI;

	case SDLK_PAGEDOWN:
	case SDLK_LSUPER:	return AK_LAMI;

	case SDLK_PAUSE: 	return AKS_PAUSE;
	case SDLK_SCROLLOCK:	return AKS_INHIBITSCREEN;
	case SDLK_PRINT: 	return AKS_SCREENSHOT;
	default: return -1;
    }
}

/*
 * Handle keys specific to French (and Belgian) keymaps.
 *
 * Number keys are broken
 */
static int decode_fr (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_Q;
	case SDLK_m:		return AK_SEMICOLON;
	case SDLK_q:		return AK_A;
	case SDLK_y:		return AK_Y;
	case SDLK_w:		return AK_Z;
	case SDLK_z:		return AK_W;
	case SDLK_LEFTBRACKET:	return AK_LBRACKET;
	case SDLK_RIGHTBRACKET: return AK_RBRACKET;
	case SDLK_COMMA:	return AK_M;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_PERIOD:
	case SDLK_SEMICOLON:	return AK_COMMA;
	case SDLK_RIGHTPAREN:	return AK_MINUS;
	case SDLK_EQUALS:	return AK_SLASH;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	case SDLK_SLASH:	return AK_PERIOD;
	case SDLK_MINUS:	return AK_EQUAL;
	case SDLK_BACKSLASH:	return AK_BACKSLASH;
	default: return -1;
    }
}

/*
 * Handle keys specific to US keymaps.
 */
static int decode_us (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_y:		return AK_Y;
	case SDLK_w:		return AK_W;
	case SDLK_z:		return AK_Z;
	case SDLK_LEFTBRACKET:	return AK_LBRACKET;
	case SDLK_RIGHTBRACKET:	return AK_RBRACKET;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_SLASH:	return AK_SLASH;
	case SDLK_SEMICOLON:	return AK_SEMICOLON;
	case SDLK_MINUS:	return AK_MINUS;
	case SDLK_EQUALS:	return AK_EQUAL;
	case SDLK_QUOTE:	return AK_QUOTE;
	case SDLK_BACKQUOTE:	return AK_BACKQUOTE;
	case SDLK_BACKSLASH:	return AK_BACKSLASH;
	default: return -1;
    }
}

/*
 * Handle keys specific to German keymaps.
 */
static int decode_de (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_w:		return AK_W;
	case SDLK_y:		return AK_Z;
	case SDLK_z:		return AK_Y;
	/* German umlaut oe */
	case SDLK_WORLD_86:	return AK_SEMICOLON;
	/* German umlaut ae */
	case SDLK_WORLD_68:	return AK_QUOTE;
	/* German umlaut ue */
	case SDLK_WORLD_92:	return AK_LBRACKET;
	case SDLK_PLUS:
	case SDLK_ASTERISK:	return AK_RBRACKET;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	/* German sharp s */
	case SDLK_WORLD_63:	return AK_MINUS;
	case SDLK_QUOTE:	return AK_EQUAL;
	case SDLK_CARET:	return AK_BACKQUOTE;
	case SDLK_MINUS:	return AK_SLASH;
	default: return -1;
    }
}

/*
 * Handle keys specific to Danish keymaps.
 *
 * Incomplete. SDL (up to 1.2.6 at least) doesn't define
 * enough keysms for a complete Danish mapping.
 * Missing dead-key support for diaeresis, acute and circumflex
 */
static int decode_dk (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_w:		return AK_W;
	case SDLK_y:		return AK_Y;
	case SDLK_z:		return AK_Z;
	/* Danish AE */
	case SDLK_WORLD_88:	return AK_SEMICOLON;
	/* Danish o oblique */
	case SDLK_WORLD_68:	return AK_QUOTE;
	/* Danish A ring */
	case SDLK_WORLD_69:	return AK_LBRACKET;
	/* one half - SDL has no 'section'? */
	case SDLK_WORLD_70:	return AK_BACKQUOTE;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	case SDLK_PLUS:		return AK_MINUS;
	case SDLK_MINUS:	return AK_SLASH;
	default: return -1;
    }
}

/*
 * Handle keys specific to SE keymaps.
 */
static int decode_se (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_w:		return AK_W;
	case SDLK_y:		return AK_Y;
	case SDLK_z:		return AK_Z;
	case SDLK_WORLD_86:	return AK_SEMICOLON;
	case SDLK_WORLD_68:	return AK_QUOTE;
	case SDLK_WORLD_69:	return AK_LBRACKET;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_MINUS:	return AK_SLASH;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_PLUS:
	case SDLK_QUESTION:	return AK_EQUAL;
	case SDLK_AT:
	case SDLK_WORLD_29:	return AK_BACKQUOTE;
	case SDLK_CARET:	return AK_RBRACKET;
	case SDLK_BACKSLASH:	return AK_MINUS;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	default: return -1;
    }
}

/*
 * Handle keys specific to Italian keymaps.
 */
static int decode_it (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_w:		return AK_W;
	case SDLK_y:		return AK_Y;
	case SDLK_z:		return AK_Z;
	case SDLK_WORLD_82:	return AK_SEMICOLON;
	case SDLK_WORLD_64:	return AK_QUOTE;
	case SDLK_WORLD_72:	return AK_LBRACKET;
	case SDLK_PLUS:
	case SDLK_ASTERISK:	return AK_RBRACKET;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_BACKSLASH:	return AK_BACKQUOTE;
	case SDLK_QUOTE:	return AK_MINUS;
	case SDLK_WORLD_76:	return AK_EQUAL;
	case SDLK_MINUS:	return AK_SLASH;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	default: return -1;
    }
}

/*
 * Handle keys specific to Spanish keymaps.
 */
static int decode_es (int keysym)
{
    switch (keysym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_w:		return AK_W;
	case SDLK_y:		return AK_Y;
	case SDLK_z:		return AK_Z;
	case SDLK_WORLD_81:	return AK_SEMICOLON;
	case SDLK_PLUS:
	case SDLK_ASTERISK:	return AK_RBRACKET;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_BACKSLASH:	return AK_BACKQUOTE;
	case SDLK_QUOTE:	return AK_MINUS;
	case SDLK_WORLD_76:	return AK_EQUAL;
	case SDLK_MINUS:	return AK_SLASH;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	default: return -1;
    }
}

int keysym2amiga (int keysym)
{
    int amiga_keycode = kc_decode (keysym);

    if (amiga_keycode == -1) {
	switch (currprefs.keyboard_lang) {
	    case KBD_LANG_FR:
		amiga_keycode = decode_fr (keysym); break;
	    case KBD_LANG_US:
		amiga_keycode = decode_us (keysym); break;
	    case KBD_LANG_DE:
		amiga_keycode = decode_de (keysym); break;
	    case KBD_LANG_DK:
		amiga_keycode = decode_dk (keysym); break;
	    case KBD_LANG_SE:
		amiga_keycode = decode_se (keysym); break;
	    case KBD_LANG_IT:
		amiga_keycode = decode_it (keysym); break;
	    case KBD_LANG_ES:
		amiga_keycode = decode_es (keysym); break;
	}
    }
    return amiga_keycode;
}


/*
 * Default hotkeys
 *
 * We need a better way of doing this. ;-)
 */
static struct uae_hotkeyseq sdl_hotkeys[] =
{
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_q, -1, -1,           INPUTEVENT_SPC_QUIT) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_r, -1, -1,           INPUTEVENT_SPC_SOFTRESET) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_LSHIFT, SDLK_r, -1,  INPUTEVENT_SPC_HARDRESET)}, \
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_d, -1, -1,           INPUTEVENT_SPC_ENTERDEBUGGER)}, \
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_s, -1, -1,           INPUTEVENT_SPC_TOGGLEFULLSCREEN) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_g, -1, -1,           INPUTEVENT_SPC_TOGGLEMOUSEGRAB) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_i, -1, -1,           INPUTEVENT_SPC_INHIBITSCREEN) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_p, -1, -1,           INPUTEVENT_SPC_SCREENSHOT) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_a, -1, -1,           INPUTEVENT_SPC_SWITCHINTERPOL) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_KP_PLUS, -1, -1,     INPUTEVENT_SPC_INCRFRAMERATE) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_KP_MINUS, -1, -1,    INPUTEVENT_SPC_DECRFRAMERATE) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_F1, -1, -1,	  INPUTEVENT_SPC_FLOPPY0) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_F2, -1, -1,	  INPUTEVENT_SPC_FLOPPY1) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_F3, -1, -1,	  INPUTEVENT_SPC_FLOPPY2) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_F4, -1, -1,	  INPUTEVENT_SPC_FLOPPY3) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_LSHIFT, SDLK_F1, -1, INPUTEVENT_SPC_EFLOPPY0) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_LSHIFT, SDLK_F2, -1, INPUTEVENT_SPC_EFLOPPY1) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_LSHIFT, SDLK_F3, -1, INPUTEVENT_SPC_EFLOPPY2) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_LSHIFT, SDLK_F4, -1, INPUTEVENT_SPC_EFLOPPY3) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_RETURN, -1, -1,      INPUTEVENT_SPC_ENTERGUI) },
    { MAKE_HOTKEYSEQ (SDLK_F12, SDLK_f, -1, -1,		  INPUTEVENT_SPC_FREEZEBUTTON) },
    { HOTKEYS_END }
};

/* Hotkeys for OS X
 * The F12 key doesn't seem to work under MacOS X (at least on the two systems
 * I've tried): key up/down events are only generated when the key is released,
 * so it's no use as a hot-key modifier. Use F11 instead.
 */
static struct uae_hotkeyseq sdl_quartz_hotkeys[] =
{
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_q, -1, -1,           INPUTEVENT_SPC_QUIT) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_r, -1, -1,           INPUTEVENT_SPC_SOFTRESET) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_LSHIFT, SDLK_r, -1,  INPUTEVENT_SPC_HARDRESET) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_d, -1, -1,           INPUTEVENT_SPC_ENTERDEBUGGER) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_s, -1, -1,           INPUTEVENT_SPC_TOGGLEFULLSCREEN) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_g, -1, -1,           INPUTEVENT_SPC_TOGGLEMOUSEGRAB) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_i, -1, -1,           INPUTEVENT_SPC_INHIBITSCREEN) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_p, -1, -1,           INPUTEVENT_SPC_SCREENSHOT) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_a, -1, -1,           INPUTEVENT_SPC_SWITCHINTERPOL) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_KP_PLUS, -1, -1,     INPUTEVENT_SPC_INCRFRAMERATE) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_KP_MINUS, -1, -1,    INPUTEVENT_SPC_DECRFRAMERATE) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_F1, -1, -1,	  INPUTEVENT_SPC_FLOPPY0) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_F2, -1, -1,	  INPUTEVENT_SPC_FLOPPY1) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_F3, -1, -1,	  INPUTEVENT_SPC_FLOPPY2) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_F4, -1, -1,	  INPUTEVENT_SPC_FLOPPY3) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_LSHIFT, SDLK_F1, -1, INPUTEVENT_SPC_EFLOPPY0) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_LSHIFT, SDLK_F2, -1, INPUTEVENT_SPC_EFLOPPY1) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_LSHIFT, SDLK_F3, -1, INPUTEVENT_SPC_EFLOPPY2) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_LSHIFT, SDLK_F4, -1, INPUTEVENT_SPC_EFLOPPY3) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_RETURN, -1, -1,      INPUTEVENT_SPC_ENTERGUI) },
    { MAKE_HOTKEYSEQ (SDLK_F11, SDLK_f, -1, -1,		  INPUTEVENT_SPC_FREEZEBUTTON) },
    { HOTKEYS_END }
};

struct uae_hotkeyseq *get_default_cooked_hotkeys (void)
{
    if (get_sdlgfx_type() == SDLGFX_DRIVER_QUARTZ)
	return sdl_quartz_hotkeys;
    else
	return sdl_hotkeys;
}
