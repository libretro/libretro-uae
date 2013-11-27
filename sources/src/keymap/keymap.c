/*
 * UAE - The Un*x Amiga Emulator
 *
 * Support for platform-independent key-mapping
 *
 * Copyright 2004 Richard Drummond
 *           2010-2011 Mustafa Tufan
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include "keymap.h"
#include "keyboard.h"

/* internal prototypes */
int getcapslock (void);
void clearallkeys (void);
void my_kbd_handler (int keyboard, int scancode, int newstate);

/* internal members */
static struct uae_input_device_kbr_default keytrans_amiga[] = {

	{ DIK_ESCAPE, {{INPUTEVENT_KEY_ESC, 0}} },

	{ DIK_F1,  {{INPUTEVENT_KEY_F1, 0}, {INPUTEVENT_SPC_FLOPPY0, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_EFLOPPY0, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },
	{ DIK_F2,  {{INPUTEVENT_KEY_F2, 0}, {INPUTEVENT_SPC_FLOPPY1, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_EFLOPPY1, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },
	{ DIK_F3,  {{INPUTEVENT_KEY_F3, 0}, {INPUTEVENT_SPC_FLOPPY2, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_EFLOPPY2, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },
	{ DIK_F4,  {{INPUTEVENT_KEY_F4, 0}, {INPUTEVENT_SPC_FLOPPY3, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_EFLOPPY3, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },
	{ DIK_F5,  {{INPUTEVENT_KEY_F5, 0}, {INPUTEVENT_SPC_STATERESTOREDIALOG, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_STATESAVEDIALOG, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },
	{ DIK_F6,  {{INPUTEVENT_KEY_F6, 0}} },
	{ DIK_F7,  {{INPUTEVENT_KEY_F7, 0}} },
	{ DIK_F8,  {{INPUTEVENT_KEY_F8, 0}} },
	{ DIK_F9,  {{INPUTEVENT_KEY_F9, 0}} },
	{ DIK_F10, {{INPUTEVENT_KEY_F10, 0}} },

	{ DIK_1, {{INPUTEVENT_KEY_1, 0}} },
	{ DIK_2, {{INPUTEVENT_KEY_2, 0}} },
	{ DIK_3, {{INPUTEVENT_KEY_3, 0}} },
	{ DIK_4, {{INPUTEVENT_KEY_4, 0}} },
	{ DIK_5, {{INPUTEVENT_KEY_5, 0}} },
	{ DIK_6, {{INPUTEVENT_KEY_6, 0}} },
	{ DIK_7, {{INPUTEVENT_KEY_7, 0}} },
	{ DIK_8, {{INPUTEVENT_KEY_8, 0}} },
	{ DIK_9, {{INPUTEVENT_KEY_9, 0}} },
	{ DIK_0, {{INPUTEVENT_KEY_0, 0}} },

	{ DIK_TAB, {{INPUTEVENT_KEY_TAB, 0}} },

	{ DIK_A, {{INPUTEVENT_KEY_A, 0}} },
	{ DIK_B, {{INPUTEVENT_KEY_B, 0}} },
	{ DIK_C, {{INPUTEVENT_KEY_C, 0}} },
	{ DIK_D, {{INPUTEVENT_KEY_D, 0}} },
	{ DIK_E, {{INPUTEVENT_KEY_E, 0}} },
	{ DIK_F, {{INPUTEVENT_KEY_F, 0}} },
	{ DIK_G, {{INPUTEVENT_KEY_G, 0}} },
	{ DIK_H, {{INPUTEVENT_KEY_H, 0}} },
	{ DIK_I, {{INPUTEVENT_KEY_I, 0}} },
	{ DIK_J, {{INPUTEVENT_KEY_J, 0}} },
	{ DIK_K, {{INPUTEVENT_KEY_K, 0}} },
	{ DIK_L, {{INPUTEVENT_KEY_L, 0}} },
	{ DIK_M, {{INPUTEVENT_KEY_M, 0}} },
	{ DIK_N, {{INPUTEVENT_KEY_N, 0}} },
	{ DIK_O, {{INPUTEVENT_KEY_O, 0}} },
	{ DIK_P, {{INPUTEVENT_KEY_P, 0}} },
	{ DIK_Q, {{INPUTEVENT_KEY_Q, 0}} },
	{ DIK_R, {{INPUTEVENT_KEY_R, 0}} },
	{ DIK_S, {{INPUTEVENT_KEY_S, 0}} },
	{ DIK_T, {{INPUTEVENT_KEY_T, 0}} },
	{ DIK_U, {{INPUTEVENT_KEY_U, 0}} },
	{ DIK_W, {{INPUTEVENT_KEY_W, 0}} },
	{ DIK_V, {{INPUTEVENT_KEY_V, 0}} },
	{ DIK_X, {{INPUTEVENT_KEY_X, 0}} },
	{ DIK_Y, {{INPUTEVENT_KEY_Y, 0}} },
	{ DIK_Z, {{INPUTEVENT_KEY_Z, 0}} },

	{ DIK_CAPITAL, {{INPUTEVENT_KEY_CAPS_LOCK, 0}} },

	{ DIK_NUMPAD1, {{INPUTEVENT_KEY_NP_1, 0}} },
	{ DIK_NUMPAD2, {{INPUTEVENT_KEY_NP_2, 0}} },
	{ DIK_NUMPAD3, {{INPUTEVENT_KEY_NP_3, 0}} },
	{ DIK_NUMPAD4, {{INPUTEVENT_KEY_NP_4, 0}} },
	{ DIK_NUMPAD5, {{INPUTEVENT_KEY_NP_5, 0}} },
	{ DIK_NUMPAD6, {{INPUTEVENT_KEY_NP_6, 0}} },
	{ DIK_NUMPAD7, {{INPUTEVENT_KEY_NP_7, 0}} },
	{ DIK_NUMPAD8, {{INPUTEVENT_KEY_NP_8, 0}} },
	{ DIK_NUMPAD9, {{INPUTEVENT_KEY_NP_9, 0}} },
	{ DIK_NUMPAD0, {{INPUTEVENT_KEY_NP_0, 0}} },
	{ DIK_DECIMAL, {{INPUTEVENT_KEY_NP_PERIOD, 0}} },
	{ DIK_ADD,     {{INPUTEVENT_KEY_NP_ADD, 0}, {INPUTEVENT_SPC_VOLUME_UP, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_MASTER_VOLUME_UP, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_CONTROL}, {INPUTEVENT_SPC_INCREASE_REFRESHRATE, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}}  },
	{ DIK_SUBTRACT, {{INPUTEVENT_KEY_NP_SUB, 0}, {INPUTEVENT_SPC_VOLUME_DOWN, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_MASTER_VOLUME_DOWN, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_CONTROL}, {INPUTEVENT_SPC_DECREASE_REFRESHRATE, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },
	{ DIK_MULTIPLY, {{INPUTEVENT_KEY_NP_MUL, 0}, {INPUTEVENT_SPC_VOLUME_MUTE, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_MASTER_VOLUME_MUTE, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_CONTROL}} },
	{ DIK_DIVIDE,   {{INPUTEVENT_KEY_NP_DIV, 0}, {INPUTEVENT_SPC_STATEREWIND, ID_FLAG_QUALIFIER_SPECIAL}} },
	{ DIK_NUMPADENTER, {{INPUTEVENT_KEY_ENTER, 0}} },

	{ DIK_MINUS,  {{INPUTEVENT_KEY_SUB, 0}} },
	{ DIK_EQUALS, {{INPUTEVENT_KEY_EQUALS, 0}} },
	{ DIK_BACK,   {{INPUTEVENT_KEY_BACKSPACE, 0}} },
	{ DIK_RETURN, {{INPUTEVENT_KEY_RETURN, 0}} },
	{ DIK_SPACE,  {{INPUTEVENT_KEY_SPACE, 0}} },

	{ DIK_LSHIFT,   {{INPUTEVENT_KEY_SHIFT_LEFT, 0}, {INPUTEVENT_SPC_QUALIFIER_SHIFT, 0}} },
	{ DIK_LCONTROL, {{INPUTEVENT_KEY_CTRL, 0}, {INPUTEVENT_SPC_QUALIFIER_CONTROL, 0}} },
	{ DIK_LWIN,     {{INPUTEVENT_KEY_AMIGA_LEFT, 0}} },
	{ DIK_LMENU,    {{INPUTEVENT_KEY_ALT_LEFT, 0}, {INPUTEVENT_SPC_QUALIFIER_ALT, 0}} },
	{ DIK_RMENU,    {{INPUTEVENT_KEY_ALT_RIGHT, 0}, {INPUTEVENT_SPC_QUALIFIER_ALT, 0}} },
	{ DIK_RWIN,     {{INPUTEVENT_KEY_AMIGA_RIGHT, 0}} },
	{ DIK_APPS,     {{INPUTEVENT_KEY_AMIGA_RIGHT, 0}} },
	{ DIK_RCONTROL, {{INPUTEVENT_KEY_CTRL, 0}, {INPUTEVENT_SPC_QUALIFIER_CONTROL, 0}} },
	{ DIK_RSHIFT,   {{INPUTEVENT_KEY_SHIFT_RIGHT, 0}, {INPUTEVENT_SPC_QUALIFIER_SHIFT, 0}} },

	{ DIK_UP, {{INPUTEVENT_KEY_CURSOR_UP, 0}} },
	{ DIK_DOWN, {{INPUTEVENT_KEY_CURSOR_DOWN, 0}} },
	{ DIK_LEFT, {{INPUTEVENT_KEY_CURSOR_LEFT, 0}} },
	{ DIK_RIGHT, {{INPUTEVENT_KEY_CURSOR_RIGHT, 0}} },

	{ DIK_INSERT, {{INPUTEVENT_KEY_AMIGA_LEFT, 0}} },
	{ DIK_DELETE, {{INPUTEVENT_KEY_DEL, 0}} },
	{ DIK_HOME, {{INPUTEVENT_KEY_AMIGA_RIGHT, 0}} },
	{ DIK_NEXT, {{INPUTEVENT_KEY_HELP, 0}} },
	{ DIK_PRIOR, {{INPUTEVENT_SPC_FREEZEBUTTON, 0}} },

	{ DIK_LBRACKET, {{INPUTEVENT_KEY_LEFTBRACKET, 0}} },
	{ DIK_RBRACKET, {{INPUTEVENT_KEY_RIGHTBRACKET, 0}} },
	{ DIK_SEMICOLON, {{INPUTEVENT_KEY_SEMICOLON, 0}} },
	{ DIK_APOSTROPHE, {{INPUTEVENT_KEY_SINGLEQUOTE, 0}} },
	{ DIK_GRAVE, {{INPUTEVENT_KEY_BACKQUOTE, 0}} },
	{ DIK_BACKSLASH, {{INPUTEVENT_KEY_BACKSLASH, 0}} },
	{ DIK_COMMA, {{INPUTEVENT_KEY_COMMA, 0}} },
	{ DIK_PERIOD, {{INPUTEVENT_KEY_PERIOD, 0}} },
	{ DIK_SLASH, {{INPUTEVENT_KEY_DIV, 0}} },
	{ DIK_OEM_102, {{INPUTEVENT_KEY_30, 0}} },
	{ DIK_SYSRQ, {{INPUTEVENT_SPC_SCREENSHOT_CLIPBOARD, 0}, {INPUTEVENT_SPC_SCREENSHOT, ID_FLAG_QUALIFIER_SPECIAL}} },

	{ DIK_END,   {{INPUTEVENT_SPC_QUALIFIER_SPECIAL, 0}} },
	{ DIK_PAUSE, {{INPUTEVENT_SPC_PAUSE, 0}, {INPUTEVENT_SPC_WARP, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_IRQ7, ID_FLAG_QUALIFIER_SPECIAL | ID_FLAG_QUALIFIER_SHIFT}} },

	{ DIK_F12, {{INPUTEVENT_SPC_ENTERGUI, 0}, {INPUTEVENT_SPC_ENTERDEBUGGER, ID_FLAG_QUALIFIER_SPECIAL}, {INPUTEVENT_SPC_ENTERDEBUGGER, ID_FLAG_QUALIFIER_SHIFT}, {INPUTEVENT_SPC_TOGGLEDEFAULTSCREEN, ID_FLAG_QUALIFIER_CONTROL}} },

	{ DIK_MEDIASTOP, {{INPUTEVENT_KEY_CDTV_STOP, 0}} },
	{ DIK_PLAYPAUSE, {{INPUTEVENT_KEY_CDTV_PLAYPAUSE, 0}} },
	{ DIK_PREVTRACK, {{INPUTEVENT_KEY_CDTV_PREV, 0}} },
	{ DIK_NEXTTRACK, {{INPUTEVENT_KEY_CDTV_NEXT, 0}} },

	{ -1, {{0}} } };

static struct uae_input_device_kbr_default keytrans_pc1[] = { { DIK_ESCAPE, {{INPUTEVENT_KEY_ESC, 0}} },

	{ DIK_F1, {{INPUTEVENT_KEY_F1, 0}} },
	{ DIK_F2, {{INPUTEVENT_KEY_F2, 0}} },
	{ DIK_F3, {{INPUTEVENT_KEY_F3, 0}} },
	{ DIK_F4, {{INPUTEVENT_KEY_F4, 0}} },
	{ DIK_F5, {{INPUTEVENT_KEY_F5, 0}} },
	{ DIK_F6, {{INPUTEVENT_KEY_F6, 0}} },
	{ DIK_F7, {{INPUTEVENT_KEY_F7, 0}} },
	{ DIK_F8, {{INPUTEVENT_KEY_F8, 0}} },
	{ DIK_F9, {{INPUTEVENT_KEY_F9, 0}} },
	{ DIK_F10, {{INPUTEVENT_KEY_F10, 0}} },
	{ DIK_F11, {{INPUTEVENT_KEY_F11, 0}} },
	{ DIK_F12, {{INPUTEVENT_KEY_F12, 0}} },

	{ DIK_1, {{INPUTEVENT_KEY_1, 0}} },
	{ DIK_2, {{INPUTEVENT_KEY_2, 0}} },
	{ DIK_3, {{INPUTEVENT_KEY_3, 0}} },
	{ DIK_4, {{INPUTEVENT_KEY_4, 0}} },
	{ DIK_5, {{INPUTEVENT_KEY_5, 0}} },
	{ DIK_6, {{INPUTEVENT_KEY_6, 0}} },
	{ DIK_7, {{INPUTEVENT_KEY_7, 0}} },
	{ DIK_8, {{INPUTEVENT_KEY_8, 0}} },
	{ DIK_9, {{INPUTEVENT_KEY_9, 0}} },
	{ DIK_0, {{INPUTEVENT_KEY_0, 0}} },

	{ DIK_TAB, {{INPUTEVENT_KEY_TAB, 0}} },

	{ DIK_A, {{INPUTEVENT_KEY_A, 0}} },
	{ DIK_B, {{INPUTEVENT_KEY_B, 0}} },
	{ DIK_C, {{INPUTEVENT_KEY_C, 0}} },
	{ DIK_D, {{INPUTEVENT_KEY_D, 0}} },
	{ DIK_E, {{INPUTEVENT_KEY_E, 0}} },
	{ DIK_F, {{INPUTEVENT_KEY_F, 0}} },
	{ DIK_G, {{INPUTEVENT_KEY_G, 0}} },
	{ DIK_H, {{INPUTEVENT_KEY_H, 0}} },
	{ DIK_I, {{INPUTEVENT_KEY_I, 0}} },
	{ DIK_J, {{INPUTEVENT_KEY_J, 0}} },
	{ DIK_K, {{INPUTEVENT_KEY_K, 0}} },
	{ DIK_L, {{INPUTEVENT_KEY_L, 0}} },
	{ DIK_M, {{INPUTEVENT_KEY_M, 0}} },
	{ DIK_N, {{INPUTEVENT_KEY_N, 0}} },
	{ DIK_O, {{INPUTEVENT_KEY_O, 0}} },
	{ DIK_P, {{INPUTEVENT_KEY_P, 0}} },
	{ DIK_Q, {{INPUTEVENT_KEY_Q, 0}} },
	{ DIK_R, {{INPUTEVENT_KEY_R, 0}} },
	{ DIK_S, {{INPUTEVENT_KEY_S, 0}} },
	{ DIK_T, {{INPUTEVENT_KEY_T, 0}} },
	{ DIK_U, {{INPUTEVENT_KEY_U, 0}} },
	{ DIK_W, {{INPUTEVENT_KEY_W, 0}} },
	{ DIK_V, {{INPUTEVENT_KEY_V, 0}} },
	{ DIK_X, {{INPUTEVENT_KEY_X, 0}} },
	{ DIK_Y, {{INPUTEVENT_KEY_Y, 0}} },
	{ DIK_Z, {{INPUTEVENT_KEY_Z, 0}} },

	{ DIK_CAPITAL, {{INPUTEVENT_KEY_CAPS_LOCK, 0}} },

	{ DIK_NUMPAD1, {{INPUTEVENT_KEY_NP_1, 0}} },
	{ DIK_NUMPAD2, {{INPUTEVENT_KEY_NP_2, 0}} },
	{ DIK_NUMPAD3, {{INPUTEVENT_KEY_NP_3, 0}} },
	{ DIK_NUMPAD4, {{INPUTEVENT_KEY_NP_4, 0}} },
	{ DIK_NUMPAD5, {{INPUTEVENT_KEY_NP_5, 0}} },
	{ DIK_NUMPAD6, {{INPUTEVENT_KEY_NP_6, 0}} },
	{ DIK_NUMPAD7, {{INPUTEVENT_KEY_NP_7, 0}} },
	{ DIK_NUMPAD8, {{INPUTEVENT_KEY_NP_8, 0}} },
	{ DIK_NUMPAD9, {{INPUTEVENT_KEY_NP_9, 0}} },
	{ DIK_NUMPAD0, {{INPUTEVENT_KEY_NP_0, 0}} },
	{ DIK_DECIMAL, {{INPUTEVENT_KEY_NP_PERIOD, 0}} },
	{ DIK_ADD, {{INPUTEVENT_KEY_NP_ADD, 0}} },
	{ DIK_SUBTRACT, {{INPUTEVENT_KEY_NP_SUB, 0}} },
	{ DIK_MULTIPLY, {{INPUTEVENT_KEY_NP_MUL, 0}} },
	{ DIK_DIVIDE, {{INPUTEVENT_KEY_NP_DIV, 0}} },
	{ DIK_NUMPADENTER, {{INPUTEVENT_KEY_ENTER, 0}} },

	{ DIK_MINUS, {{INPUTEVENT_KEY_SUB, 0}} },
	{ DIK_EQUALS, {{INPUTEVENT_KEY_EQUALS, 0}} },
	{ DIK_BACK, {{INPUTEVENT_KEY_BACKSPACE, 0}} },
	{ DIK_RETURN, {{INPUTEVENT_KEY_RETURN, 0}} },
	{ DIK_SPACE, {{INPUTEVENT_KEY_SPACE, 0}} },

	{ DIK_LSHIFT, {{INPUTEVENT_KEY_SHIFT_LEFT, 0}} },
	{ DIK_LCONTROL, {{INPUTEVENT_KEY_CTRL, 0}} },
	{ DIK_LWIN, {{INPUTEVENT_KEY_AMIGA_LEFT, 0}} },
	{ DIK_LMENU, {{INPUTEVENT_KEY_ALT_LEFT, 0}} },
	{ DIK_RMENU, {{INPUTEVENT_KEY_ALT_RIGHT, 0}} },
	{ DIK_RWIN, {{INPUTEVENT_KEY_AMIGA_RIGHT, 0}} },
	{ DIK_APPS, {{INPUTEVENT_KEY_APPS, 0}} },
	{ DIK_RCONTROL, {{INPUTEVENT_KEY_CTRL, 0}} },
	{ DIK_RSHIFT, {{INPUTEVENT_KEY_SHIFT_RIGHT, 0}} },

	{ DIK_UP, {{INPUTEVENT_KEY_CURSOR_UP, 0}} },
	{ DIK_DOWN, {{INPUTEVENT_KEY_CURSOR_DOWN, 0}} },
	{ DIK_LEFT, {{INPUTEVENT_KEY_CURSOR_LEFT, 0}} },
	{ DIK_RIGHT, {{INPUTEVENT_KEY_CURSOR_RIGHT, 0}} },

	{ DIK_LBRACKET, {{INPUTEVENT_KEY_LEFTBRACKET, 0}} },
	{ DIK_RBRACKET, {{INPUTEVENT_KEY_RIGHTBRACKET, 0}} },
	{ DIK_SEMICOLON, {{INPUTEVENT_KEY_SEMICOLON, 0}} },
	{ DIK_APOSTROPHE, {{INPUTEVENT_KEY_SINGLEQUOTE, 0}} },
	{ DIK_GRAVE, {{INPUTEVENT_KEY_BACKQUOTE, 0}} },
	{ DIK_BACKSLASH, {{INPUTEVENT_KEY_2B, 0}} },
	{ DIK_COMMA, {{INPUTEVENT_KEY_COMMA, 0}} },
	{ DIK_PERIOD, {{INPUTEVENT_KEY_PERIOD, 0}} },
	{ DIK_SLASH, {{INPUTEVENT_KEY_DIV, 0}} },
	{ DIK_OEM_102, {{INPUTEVENT_KEY_30, 0}} },

	{ DIK_INSERT, {{INPUTEVENT_KEY_INSERT, 0}} },
	{ DIK_DELETE, {{INPUTEVENT_KEY_DEL, 0}} },
	{ DIK_HOME, {{INPUTEVENT_KEY_HOME, 0}} },
	{ DIK_END, {{INPUTEVENT_KEY_END, 0}} },
    { DIK_PRIOR, {{INPUTEVENT_KEY_PAGEUP, 0}} },
	{ DIK_NEXT, {{INPUTEVENT_KEY_PAGEDOWN, 0}} },
	{ DIK_SCROLL, {{INPUTEVENT_KEY_HELP, 0}} },
    { DIK_SYSRQ, {{INPUTEVENT_KEY_SYSRQ, 0}} },

	{ DIK_MEDIASTOP, {{INPUTEVENT_KEY_CDTV_STOP, 0}} },
	{ DIK_PLAYPAUSE, {{INPUTEVENT_KEY_CDTV_PLAYPAUSE, 0}} },
	{ DIK_PREVTRACK, {{INPUTEVENT_KEY_CDTV_PREV, 0}} },
	{ DIK_NEXTTRACK, {{INPUTEVENT_KEY_CDTV_NEXT, 0}} },

	{ -1, {{0, 0}} }
};

static struct uae_input_device_kbr_default *keytrans[] = {
	keytrans_amiga,
	keytrans_pc1,
	keytrans_pc1
};

static int kb_np[] = { DIK_NUMPAD4, -1, DIK_NUMPAD6, -1, DIK_NUMPAD8, -1, DIK_NUMPAD2, -1, DIK_NUMPAD0, DIK_NUMPAD5, -1, DIK_DECIMAL, -1, DIK_NUMPADENTER, -1, -1 };
static int kb_ck[] = { DIK_LEFT, -1, DIK_RIGHT, -1, DIK_UP, -1, DIK_DOWN, -1, DIK_RCONTROL, DIK_RMENU, -1, DIK_RSHIFT, -1, -1 };
static int kb_se[] = { DIK_A, -1, DIK_D, -1, DIK_W, -1, DIK_S, -1, DIK_LMENU, -1, DIK_LSHIFT, -1, -1 };
static int kb_np3[] = { DIK_NUMPAD4, -1, DIK_NUMPAD6, -1, DIK_NUMPAD8, -1, DIK_NUMPAD2, -1, DIK_NUMPAD0, DIK_NUMPAD5, -1, DIK_DECIMAL, -1, DIK_NUMPADENTER, -1, -1 };
static int kb_ck3[] = { DIK_LEFT, -1, DIK_RIGHT, -1, DIK_UP, -1, DIK_DOWN, -1, DIK_RCONTROL, -1, DIK_RSHIFT, -1, DIK_RMENU, -1, -1 };
static int kb_se3[] = { DIK_A, -1, DIK_D, -1, DIK_W, -1, DIK_S, -1, DIK_LMENU, -1, DIK_LSHIFT, -1, DIK_LCONTROL, -1, -1 };

static int kb_cd32_np[] = { DIK_NUMPAD4, -1, DIK_NUMPAD6, -1, DIK_NUMPAD8, -1, DIK_NUMPAD2, -1, DIK_NUMPAD0, DIK_NUMPAD5, DIK_NUMPAD1, -1, DIK_DECIMAL, DIK_NUMPAD3, -1, DIK_NUMPAD7, -1, DIK_NUMPAD9, -1, DIK_DIVIDE, -1, DIK_SUBTRACT, -1, DIK_MULTIPLY, -1, -1 };
static int kb_cd32_ck[] = { DIK_LEFT, -1, DIK_RIGHT, -1, DIK_UP, -1, DIK_DOWN, -1, DIK_RCONTROL, -1, DIK_RMENU, -1, DIK_NUMPAD7, -1, DIK_NUMPAD9, -1, DIK_DIVIDE, -1, DIK_SUBTRACT, -1, DIK_MULTIPLY, -1, -1 };
static int kb_cd32_se[] = { DIK_A, -1, DIK_D, -1, DIK_W, -1, DIK_S, -1, -1, DIK_LMENU, -1, DIK_LSHIFT, -1, DIK_NUMPAD7, -1, DIK_NUMPAD9, -1, DIK_DIVIDE, -1, DIK_SUBTRACT, -1, DIK_MULTIPLY, -1, -1 };

static int kb_cdtv[] = { DIK_NUMPAD1, -1, DIK_NUMPAD3, -1, DIK_NUMPAD7, -1, DIK_NUMPAD9, -1, -1 };

static int kb_xa1[] = { DIK_NUMPAD4, -1, DIK_NUMPAD6, -1, DIK_NUMPAD8, -1, DIK_NUMPAD2, DIK_NUMPAD5, -1, DIK_LCONTROL, -1, DIK_LMENU, -1, DIK_SPACE, -1, -1 };
static int kb_xa2[] = { DIK_D, -1, DIK_G, -1, DIK_R, -1, DIK_F, -1, DIK_A, -1, DIK_S, -1, DIK_Q, -1 };
static int kb_arcadia[] = { DIK_F2, -1, DIK_1, -1, DIK_2, -1, DIK_5, -1, DIK_6, -1, -1 };
static int kb_arcadiaxa[] = { DIK_1, -1, DIK_2, -1, DIK_3, -1, DIK_4, -1, DIK_6, -1, DIK_LBRACKET, DIK_LSHIFT, -1, DIK_RBRACKET, -1, DIK_C, -1, DIK_5, -1, DIK_Z, -1, DIK_X, -1, -1 };

static int *kbmaps[] = {
	kb_np, kb_ck, kb_se, kb_np3, kb_ck3, kb_se3,
	kb_cd32_np, kb_cd32_ck, kb_cd32_se,
	kb_xa1, kb_xa2, kb_arcadia, kb_arcadiaxa, kb_cdtv
};

static int specialpressed (void)
{
	return input_getqualifiers () & ID_FLAG_QUALIFIER_SPECIAL;
}
static int shiftpressed (void)
{
	return input_getqualifiers () & ID_FLAG_QUALIFIER_SHIFT;
}

/* REMOVEME:
 */
#if 0
static int altpressed (void)
{
	return input_getqualifiers () & ID_FLAG_QUALIFIER_ALT;
}
#endif

static int ctrlpressed (void)
{
	return input_getqualifiers () & ID_FLAG_QUALIFIER_CONTROL;
}

static int capslockstate;
static int host_capslockstate, host_numlockstate, host_scrolllockstate;

/*int getcapslockstate (void)
{
	return capslockstate;
}
void setcapslockstate (int state)
{
	capslockstate = state;
}*/

int getcapslock (void)
{
	int capstable[7];

	// this returns bogus state if caps change when in exclusive mode..
	host_capslockstate = 0; //GetKeyState (VK_CAPITAL) & 1;
	host_numlockstate = 0; //GetKeyState (VK_NUMLOCK) & 1;
	host_scrolllockstate = 0; //GetKeyState (VK_SCROLL) & 1;
	capstable[0] = DIK_CAPITAL;
	capstable[1] = host_capslockstate;
	capstable[2] = DIK_NUMLOCK;
	capstable[3] = host_numlockstate;
	capstable[4] = DIK_SCROLL;
	capstable[5] = host_scrolllockstate;
	capstable[6] = 0;
	capslockstate = inputdevice_synccapslock (capslockstate, capstable);
	return capslockstate;
}

void clearallkeys (void)
{
	inputdevice_updateconfig (&changed_prefs, &currprefs);
}

static const int np[] = {
	DIK_NUMPAD0, 0, DIK_NUMPADPERIOD, 0, DIK_NUMPAD1, 1, DIK_NUMPAD2, 2,
	DIK_NUMPAD3, 3, DIK_NUMPAD4, 4, DIK_NUMPAD5, 5, DIK_NUMPAD6, 6, DIK_NUMPAD7, 7,
	DIK_NUMPAD8, 8, DIK_NUMPAD9, 9, -1 };

void my_kbd_handler (int keyboard, int scancode, int newstate)
{
        int code = 0;
        int scancode_new;
        int defaultguikey;
        bool amode = currprefs.input_keyboard_type == 0;
        bool special = false;
        static int swapperdrive = 0;

	scancode_new = scancode;
	if (!specialpressed () && inputdevice_iskeymapped (keyboard, scancode))
		scancode = 0;

	defaultguikey = amode ? DIK_F12 : DIK_NUMLOCK;

        if (newstate && code == 0 && amode) {

                switch (scancode)
                {
                case DIK_1:
                case DIK_2:
                case DIK_3:
                case DIK_4:
                case DIK_5:
                case DIK_6:
                case DIK_7:
                case DIK_8:
                case DIK_9:
                case DIK_0:
                        if (specialpressed ()) {
                                int num = scancode - DIK_1;
                                if (shiftpressed ())
                                        num += 10;
                                if (ctrlpressed ()) {
                                       swapperdrive = num;
                                        if (swapperdrive > 3)
                                                swapperdrive = 0;
                                } else {
                                        int i;
                                        for (i = 0; i < 4; i++) {
                                                if (!_tcscmp (currprefs.floppyslots[i].df, currprefs.dfxlist[num]))
                                                        changed_prefs.floppyslots[i].df[0] = 0;
                                        }
                                        _tcscpy (changed_prefs.floppyslots[swapperdrive].df, currprefs.dfxlist[num]);
                                        config_changed = 1;
                                }
                                                                special = true;
                        }
                        break;
                case DIK_NUMPAD0:
                case DIK_NUMPAD1:
                case DIK_NUMPAD2:
                case DIK_NUMPAD3:
                case DIK_NUMPAD4:
                case DIK_NUMPAD5:
                case DIK_NUMPAD6:
                case DIK_NUMPAD7:
                case DIK_NUMPAD8:
                case DIK_NUMPAD9:
                case DIK_NUMPADPERIOD:
                        if (specialpressed ()) {
                                int i = 0, v = -1;
                                while (np[i] >= 0) {
                                        v = np[i + 1];
                                        if (np[i] == scancode)
                                                break;
                                        i += 2;
                                }
                                if (v >= 0)
                                        code = AKS_STATESAVEQUICK + v * 2 + ((shiftpressed () || ctrlpressed ()) ? 0 : 1);
                        }
                        break;
                }
        }

        if (code) {
                inputdevice_add_inputcode (code, 1);
                return;
        }

        scancode = scancode_new;
        if (!specialpressed () && newstate) {
                if (scancode == DIK_CAPITAL) {
                        host_capslockstate = host_capslockstate ? 0 : 1;
                        capslockstate = host_capslockstate;
                }
                if (scancode == DIK_NUMLOCK) {
                        host_numlockstate = host_numlockstate ? 0 : 1;
                        capslockstate = host_numlockstate;
                }
                if (scancode == DIK_SCROLL) {
                        host_scrolllockstate = host_scrolllockstate ? 0 : 1;
                        capslockstate = host_scrolllockstate;
                }
        }

		if (special) {
			inputdevice_checkqualifierkeycode (keyboard, scancode, newstate);
			return;
		}

//	write_log ("KBDHANDLER_2: kbd2= %d, scancode= %d (0x%02x), state= %d\n", keyboard, scancode, scancode, newstate);

        inputdevice_translatekeycode (keyboard, scancode, newstate);
}

void keyboard_settrans (void)
{
	inputdevice_setkeytranslation (keytrans, kbmaps);
}

/*int target_checkcapslock (int scancode, int *state)
{
	if (scancode != DIK_CAPITAL && scancode != DIK_NUMLOCK && scancode != DIK_SCROLL)
		return 0;
	if (*state == 0)
		return -1;
	if (scancode == DIK_CAPITAL)
		*state = host_capslockstate;
	if (scancode == DIK_NUMLOCK)
		*state = host_numlockstate;
	if (scancode == DIK_SCROLL)
		*state = host_scrolllockstate;
	return 1;
}*/

