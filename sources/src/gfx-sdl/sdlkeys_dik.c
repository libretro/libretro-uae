/*
 * UAE - The Un*x Amiga Emulator
 *
 * SDL keyboard driver which maps SDL cooked keysyms to
 * Direct Input keycodes
 *
 * Copyright 2010-2011 Mustafa Tufan
 *
 */

#include "sysconfig.h"
#include "include/sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include "keyboard.h"
#include "hotkeys.h"
#include "../keymap/keymap.h"

#include "sdlgfx.h"
#include <SDL.h>
#include "sdlkeys_dik.h"

/*
 * This function knows about all keys that are common
 * between different keyboard languages.
 *
 */
static int kc_dik_decode (int key)
{
    switch (key) {
	case SDLK_ESCAPE: 	return DIK_ESCAPE;
//
	case SDLK_1:		return DIK_1;
	case SDLK_2:		return DIK_2;
	case SDLK_3:		return DIK_3;
	case SDLK_4:		return DIK_4;
	case SDLK_5:		return DIK_5;
	case SDLK_6:		return DIK_6;
	case SDLK_7:		return DIK_7;
	case SDLK_8:		return DIK_8;
	case SDLK_9:		return DIK_9;
	case SDLK_0:		return DIK_0;
	case SDLK_BACKSPACE:	return DIK_BACK;
//
	case SDLK_TAB: 		return DIK_TAB;
/*	case SDLK_q:		return DIK_Q; in decode */
/*	case SDLK_w:		return DIK_W; in decode */
	case SDLK_e:		return DIK_E;
	case SDLK_r:		return DIK_R;
	case SDLK_t:		return DIK_T;
/*	case SDLK_y:		return DIK_Y; in decode */
	case SDLK_u:		return DIK_U;
	case SDLK_i:		return DIK_I;
	case SDLK_o:		return DIK_O;
	case SDLK_p:		return DIK_P;
	case SDLK_RETURN: 	return DIK_RETURN;
	case SDLK_LCTRL: 	return DIK_LCONTROL;
//
/*	case SDLK_a:		return DIK_A; in decode */
	case SDLK_s:		return DIK_S;
	case SDLK_d:		return DIK_D;
	case SDLK_f:		return DIK_F;
	case SDLK_g:		return DIK_G;
	case SDLK_h:		return DIK_H;
	case SDLK_j:		return DIK_J;
	case SDLK_k:		return DIK_K;
	case SDLK_l:		return DIK_L;
	case SDLK_LSHIFT: 	return DIK_LSHIFT;
//
/*	case SDLK_z:		return DIK_Z; in decode */
	case SDLK_x:		return DIK_X;
	case SDLK_c:		return DIK_C;
	case SDLK_v:		return DIK_V;
	case SDLK_b:		return DIK_B;
	case SDLK_n:		return DIK_N;
/*	case SDLK_m:		return DIK_M; in decode */
	case SDLK_RSHIFT: 	return DIK_RSHIFT;
	case SDLK_SPACE: 	return DIK_SPACE;
//
	case SDLK_F1: 		return DIK_F1;
	case SDLK_F2: 		return DIK_F2;
	case SDLK_F3: 		return DIK_F3;
	case SDLK_F4: 		return DIK_F4;
	case SDLK_F5: 		return DIK_F5;
	case SDLK_F6: 		return DIK_F6;
	case SDLK_F7: 		return DIK_F7;
	case SDLK_F8: 		return DIK_F8;
	case SDLK_F9: 		return DIK_F9;
	case SDLK_F10: 		return DIK_F10;
	case SDLK_F11: 		return DIK_F11;
	case SDLK_F12: 		return DIK_F12;
	case SDLK_F13: 		return DIK_F13;
	case SDLK_F14: 		return DIK_F14;
	case SDLK_F15: 		return DIK_F15;
	case SDLK_NUMLOCK:	return DIK_NUMLOCK;
	case SDLK_CAPSLOCK:	return DIK_CAPITAL;
	case SDLK_SCROLLOCK:	return DIK_SCROLL;
//
	case SDLK_KP7:		return DIK_NUMPAD7;
	case SDLK_KP8:		return DIK_NUMPAD8;
	case SDLK_KP9:		return DIK_NUMPAD9;
	case SDLK_KP_MINUS:	return DIK_SUBTRACT;
	case SDLK_KP4:		return DIK_NUMPAD4;
	case SDLK_KP5:		return DIK_NUMPAD5;
	case SDLK_KP6:		return DIK_NUMPAD6;
	case SDLK_KP_PLUS:	return DIK_ADD;
	case SDLK_KP1:		return DIK_NUMPAD1;
	case SDLK_KP2:		return DIK_NUMPAD2;
	case SDLK_KP3:		return DIK_NUMPAD3;
	case SDLK_KP0:		return DIK_NUMPAD0;
	case SDLK_KP_PERIOD:	return DIK_DECIMAL;
	case SDLK_KP_ENTER:	return DIK_NUMPADENTER;
	case SDLK_KP_DIVIDE:	return DIK_DIVIDE;
	case SDLK_KP_MULTIPLY:	return DIK_MULTIPLY;
	case SDLK_KP_EQUALS:	return DIK_NUMPADEQUALS;
//
	case SDLK_DELETE: 	return DIK_DELETE;
	case SDLK_RCTRL: 	return DIK_RCONTROL;
	case SDLK_LALT: 	return DIK_LMENU;
	case SDLK_RALT: 	return DIK_RMENU;

	case SDLK_INSERT: 	return DIK_INSERT;
	case SDLK_HOME: 	return DIK_HOME;
	case SDLK_END: 		return DIK_END;

	case SDLK_UP: 		return DIK_UP;
	case SDLK_PAGEUP:	return DIK_PRIOR;
	case SDLK_LEFT: 	return DIK_LEFT;
	case SDLK_RIGHT: 	return DIK_RIGHT;
	case SDLK_DOWN: 	return DIK_DOWN;
	case SDLK_PAGEDOWN:	return DIK_NEXT;

	case SDLK_PAUSE: 	return DIK_PAUSE;
//	case SDLK_PRINT: 	return AKS_SCREENSHOT_FILE;
//	case SDLK_WORLD_0:	return 0;	//e"
	case SDLK_LMETA:	return DIK_LWIN;	//mac LCMD
	case SDLK_RMETA:	return DIK_RWIN;	//mac RCMD

	default: return -1;
    }
}

/*
 * Handle keys specific to French (and Belgian) keymaps.
 *
 * Number keys are broken
 */
static int decode_dik_fr (int key)
{
	switch (key) {
	case SDLK_a:		return DIK_Q;
	case SDLK_m:		return DIK_SEMICOLON;
	case SDLK_q:		return DIK_A;
	case SDLK_y:		return DIK_Y;
	case SDLK_w:		return DIK_Z;
	case SDLK_z:		return DIK_W;
	case SDLK_LEFTBRACKET:	return DIK_LBRACKET;
	case SDLK_RIGHTBRACKET: return DIK_RBRACKET;
	case SDLK_COMMA:	return DIK_M;
	case SDLK_LESS:
	case SDLK_GREATER:	return DIK_OEM_102;
	case SDLK_PERIOD:
	case SDLK_SEMICOLON:	return DIK_COMMA;
	case SDLK_RIGHTPAREN:	return DIK_MINUS;
	case SDLK_EQUALS:	return DIK_SLASH;
//	case SDLK_HASH:		return DIK_NUMBERSIGN;
	case SDLK_SLASH:	return DIK_PERIOD;
	case SDLK_MINUS:	return DIK_EQUALS;
	case SDLK_BACKSLASH:	return DIK_BACKSLASH;
	default: return -1;
	}
}

/*
 * Handle keys specific to US keymaps.
 */
static int decode_dik_us (int key)
{
	switch (key) {
	case SDLK_a:		return DIK_A;
	case SDLK_m:		return DIK_M;
	case SDLK_q:		return DIK_Q;
	case SDLK_y:		return DIK_Y;
	case SDLK_w:		return DIK_W;
	case SDLK_z:		return DIK_Z;
	case SDLK_LEFTBRACKET:	return DIK_LBRACKET;
	case SDLK_RIGHTBRACKET:	return DIK_RBRACKET;
	case SDLK_COMMA:	return DIK_COMMA;
	case SDLK_PERIOD:	return DIK_PERIOD;
	case SDLK_SLASH:	return DIK_SLASH;
	case SDLK_SEMICOLON:	return DIK_SEMICOLON;
	case SDLK_MINUS:	return DIK_MINUS;
	case SDLK_EQUALS:	return DIK_EQUALS;
//	case SDLK_QUOTE:	return DIK_QUOTE;
	case SDLK_BACKQUOTE:	return DIK_GRAVE;
	case SDLK_BACKSLASH:	return DIK_BACKSLASH;
	default: return -1;
	}
}

/*
 * Handle keys specific to German keymaps.
 */
static int decode_dik_de (int key)
{
	switch (key) {
	case SDLK_a:		return DIK_A;
	case SDLK_m:		return DIK_M;
	case SDLK_q:		return DIK_Q;
	case SDLK_w:		return DIK_W;
	case SDLK_y:		return DIK_Z;
	case SDLK_z:		return DIK_Y;
	case SDLK_WORLD_86:	return DIK_SEMICOLON;	/* German umlaut oe */
//	case SDLK_WORLD_68:	return DIK_QUOTE;	/* German umlaut ae */
	case SDLK_WORLD_92:	return DIK_LBRACKET;	/* German umlaut ue */
	case SDLK_PLUS:
	case SDLK_ASTERISK:	return DIK_RBRACKET;
	case SDLK_COMMA:	return DIK_COMMA;
	case SDLK_PERIOD:	return DIK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return DIK_OEM_102;
//	case SDLK_HASH:		return DIK_NUMBERSIGN;
	case SDLK_WORLD_63:	return DIK_MINUS;	/* German sharp s */
	case SDLK_QUOTE:	return DIK_EQUALS;
	case SDLK_CARET:	return DIK_GRAVE;
	case SDLK_MINUS:	return DIK_SLASH;
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
static int decode_dik_dk (int key)
{
	switch (key) {
	case SDLK_a:		return DIK_A;
	case SDLK_m:		return DIK_M;
	case SDLK_q:		return DIK_Q;
	case SDLK_w:		return DIK_W;
	case SDLK_y:		return DIK_Y;
	case SDLK_z:		return DIK_Z;
	case SDLK_WORLD_88:	return DIK_SEMICOLON;	/* Danish AE */
//	case SDLK_WORLD_68:	return DIK_QUOTE;	/* Danish o oblique */
	case SDLK_WORLD_69:	return DIK_LBRACKET;	/* Danish A ring */
	case SDLK_WORLD_70:	return DIK_GRAVE;	/* one half - SDL has no 'section'? */
	case SDLK_COMMA:	return DIK_COMMA;
	case SDLK_PERIOD:	return DIK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return DIK_OEM_102;
//	case SDLK_HASH:		return DIK_NUMBERSIGN;
	case SDLK_PLUS:		return DIK_MINUS;
	case SDLK_MINUS:	return DIK_SLASH;
	default: return -1;
	}
}

/*
 * Handle keys specific to SE keymaps.
 */
static int decode_dik_se (int key)
{
	switch (key) {
	case SDLK_a:		return DIK_A;
	case SDLK_m:		return DIK_M;
	case SDLK_q:		return DIK_Q;
	case SDLK_w:		return DIK_W;
	case SDLK_y:		return DIK_Y;
	case SDLK_z:		return DIK_Z;
	case SDLK_WORLD_86:	return DIK_SEMICOLON;
//	case SDLK_WORLD_68:	return DIK_QUOTE;
	case SDLK_WORLD_69:	return DIK_LBRACKET;
	case SDLK_COMMA:	return DIK_COMMA;
	case SDLK_PERIOD:	return DIK_PERIOD;
	case SDLK_MINUS:	return DIK_SLASH;
	case SDLK_LESS:
	case SDLK_GREATER:	return DIK_OEM_102;
	case SDLK_PLUS:
	case SDLK_QUESTION:	return DIK_EQUALS;
	case SDLK_AT:
	case SDLK_WORLD_29:	return DIK_GRAVE;
	case SDLK_CARET:	return DIK_RBRACKET;
	case SDLK_BACKSLASH:	return DIK_MINUS;
//	case SDLK_HASH:		return DIK_NUMBERSIGN;
	default: return -1;
	}
}

/*
 * Handle keys specific to Italian keymaps.
 */
static int decode_dik_it (int key)
{
	switch (key) {
		case SDLK_a:		return DIK_A;
		case SDLK_m:		return DIK_M;
		case SDLK_q:		return DIK_Q;
		case SDLK_w:		return DIK_W;
		case SDLK_y:		return DIK_Y;
		case SDLK_z:		return DIK_Z;
		case SDLK_WORLD_82:	return DIK_SEMICOLON;
//		case SDLK_WORLD_64:	return DIK_QUOTE;
		case SDLK_WORLD_72:	return DIK_LBRACKET;
		case SDLK_PLUS:
		case SDLK_ASTERISK:	return DIK_RBRACKET;
		case SDLK_COMMA:	return DIK_COMMA;
		case SDLK_PERIOD:	return DIK_PERIOD;
		case SDLK_LESS:
		case SDLK_GREATER:	return DIK_OEM_102;
		case SDLK_BACKSLASH:	return DIK_GRAVE;
		case SDLK_QUOTE:	return DIK_MINUS;
		case SDLK_WORLD_76:	return DIK_EQUALS;
		case SDLK_MINUS:	return DIK_SLASH;
//		case SDLK_HASH:		return DIK_NUMBERSIGN;
		default: return -1;
	}
}

/*
 * Handle keys specific to Spanish keymaps.
 */
static int decode_dik_es (int key)
{
	switch (key) {
		case SDLK_a:		return DIK_A;
		case SDLK_m:		return DIK_M;
		case SDLK_q:		return DIK_Q;
		case SDLK_w:		return DIK_W;
		case SDLK_y:		return DIK_Y;
		case SDLK_z:		return DIK_Z;
		case SDLK_WORLD_81:	return DIK_SEMICOLON;
		case SDLK_PLUS:
		case SDLK_ASTERISK:	return DIK_RBRACKET;
		case SDLK_COMMA:	return DIK_COMMA;
		case SDLK_PERIOD:	return DIK_PERIOD;
		case SDLK_LESS:
		case SDLK_GREATER:	return DIK_OEM_102;
		case SDLK_BACKSLASH:	return DIK_GRAVE;
		case SDLK_QUOTE:	return DIK_MINUS;
		case SDLK_WORLD_76:	return DIK_EQUALS;
		case SDLK_MINUS:	return DIK_SLASH;
//		case SDLK_HASH:		return DIK_NUMBERSIGN;
		default: return -1;
	}
}

/*
 * Handle keys specific to FI keymaps.
 */

static int decode_dik_fi (int key)
{
	switch (key) {
		case SDLK_a:		return AK_A;
		case SDLK_m:		return AK_M;
		case SDLK_q:		return AK_Q;
		case SDLK_w:		return AK_W;
		case SDLK_y:		return AK_Y;
		case SDLK_z:		return AK_Z;
		case SDLK_WORLD_0:	return AK_LBRACKET;
		case SDLK_WORLD_1:	return AK_QUOTE;
		case SDLK_WORLD_2:	return AK_LBRACKET;
		case SDLK_WORLD_3:	return AK_LBRACKET;
		case SDLK_WORLD_4:	return AK_LBRACKET;
		case SDLK_WORLD_5:	return AK_LBRACKET;

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
		default:		return -1;
	}
}

/*
 * Handle keys specific to TR keymaps.
 */
static int decode_dik_tr (int key)
{
	switch (key) {
		case SDLK_a:		return DIK_A;
		case SDLK_m:		return DIK_M;
		case SDLK_q:		return DIK_Q;
		case SDLK_y:		return DIK_Y;
		case SDLK_w:		return DIK_W;
		case SDLK_z:		return DIK_Z;

		case SDLK_LEFTBRACKET:	return DIK_LBRACKET;	//tr kbd g
		case SDLK_RIGHTBRACKET:	return DIK_RBRACKET;	//tr kbd u
		case SDLK_COMMA:	return DIK_COMMA;	//tr kbd o
		case SDLK_PERIOD:	return DIK_PERIOD;	//tr kbd c
		case SDLK_SLASH:	return DIK_SLASH;	//tr kbd .
		case SDLK_SEMICOLON:	return DIK_SEMICOLON;	//tr kbd s
		case SDLK_QUOTE:	return DIK_APOSTROPHE;	//tr kbd i
//		case SDLK_		return DIK_GRAVE;	//tr kdb ;,
		case SDLK_MINUS:	return DIK_MINUS;	//?*
		case SDLK_EQUALS:	return DIK_EQUALS;	//-_

		case SDLK_BACKQUOTE:	return DIK_GRAVE;
		case SDLK_BACKSLASH:	return DIK_BACKSLASH;
		default: return -1;
	}
}

/*

*/
int sdlk2dik (int key)
{
	int amiga_keycode = kc_dik_decode (key);

	if (amiga_keycode == -1) {
		switch (currprefs.keyboard_lang) {
			case KBD_LANG_FR:
				amiga_keycode = decode_dik_fr (key); break;
			case KBD_LANG_US:
				amiga_keycode = decode_dik_us (key); break;
			case KBD_LANG_DE:
				amiga_keycode = decode_dik_de (key); break;
			case KBD_LANG_DK:
				amiga_keycode = decode_dik_dk (key); break;
			case KBD_LANG_SE:
				amiga_keycode = decode_dik_se (key); break;
			case KBD_LANG_IT:
				amiga_keycode = decode_dik_it (key); break;
			case KBD_LANG_ES:
				amiga_keycode = decode_dik_es (key); break;
			case KBD_LANG_FI:
				amiga_keycode = decode_dik_fi (key); break;
			case KBD_LANG_TR:
				amiga_keycode = decode_dik_tr (key); break;
		}
	}
	return amiga_keycode;
}
