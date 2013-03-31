 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for mapping xfree86 keycodes (e.g. PC and Mac keyboards
  * under XFree86) to platform-independent UAE key codes.
  *
  * Copyright 2004-2005 Richard Drummond
  */

#if 1

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include "keymap.h"
#include "keymap_common.h"

/*
 * Raw scan codes
 */
#define RAWKEY_ESCAPE		9

#define RAWKEY_F1		67
#define RAWKEY_F2		68
#define RAWKEY_F3		69
#define RAWKEY_F4		70
#define RAWKEY_F5		71
#define RAWKEY_F6		72
#define RAWKEY_F7		73
#define RAWKEY_F8		74
#define RAWKEY_F9		75
#define RAWKEY_F10		76
#define RAWKEY_F11		95
#define RAWKEY_F12		96

#define RAWKEY_PRINTSCR		111
#define RAWKEY_SCROLL_LOCK	78
#define RAWKEY_PAUSE		110

#define RAWKEY_1		10
#define RAWKEY_2		11
#define RAWKEY_3		12
#define RAWKEY_4		13
#define RAWKEY_5		14
#define RAWKEY_6		15
#define RAWKEY_7		16
#define RAWKEY_8		17
#define RAWKEY_9		18
#define RAWKEY_0		19

#define RAWKEY_TAB		23
#define RAWKEY_ENTER		36
#define RAWKEY_BACKSPACE	22
#define RAWKEY_SPACE		65

#define RAWKEY_A		38
#define RAWKEY_B		56
#define RAWKEY_C		54
#define RAWKEY_D		40
#define RAWKEY_E		26
#define RAWKEY_F		41
#define RAWKEY_G		42
#define RAWKEY_H		43
#define RAWKEY_I		31
#define RAWKEY_J		44
#define RAWKEY_K		45
#define RAWKEY_L		46
#define RAWKEY_M		58
#define RAWKEY_N		57
#define RAWKEY_O		32
#define RAWKEY_P		33
#define RAWKEY_Q		24
#define RAWKEY_R		27
#define RAWKEY_S		39
#define RAWKEY_T		28
#define RAWKEY_U		30
#define RAWKEY_V		55
#define RAWKEY_W		25
#define RAWKEY_X		53
#define RAWKEY_Y		29
#define RAWKEY_Z		52

#define RAWKEY_MINUS		20
#define RAWKEY_EQUALS		21
#define RAWKEY_LEFTBRACKET	34
#define RAWKEY_RIGHTBRACKET	35
#define RAWKEY_BACKSLASH	51
#define RAWKEY_SEMICOLON	47
#define RAWKEY_SINGLEQUOTE	48
#define RAWKEY_COMMA		59
#define RAWKEY_PERIOD		60
#define RAWKEY_SLASH		61
#define RAWKEY_GRAVE		49
#define RAWKEY_LTGT		94

#define RAWKEY_NUMPAD_1		87
#define RAWKEY_NUMPAD_2		88
#define RAWKEY_NUMPAD_3		89
#define RAWKEY_NUMPAD_4		83
#define RAWKEY_NUMPAD_5		84
#define RAWKEY_NUMPAD_6		85
#define RAWKEY_NUMPAD_7		79
#define RAWKEY_NUMPAD_8		80
#define RAWKEY_NUMPAD_9		81
#define RAWKEY_NUMPAD_0		90
//#define RAWKEY_NUMPAD_EQUALS
#define RAWKEY_NUMPAD_DIVIDE	112
#define RAWKEY_NUMPAD_MULTIPLY	63
#define RAWKEY_NUMPAD_MINUS	82
#define RAWKEY_NUMPAD_PLUS	86
#define RAWKEY_NUMPAD_PERIOD	91
#define RAWKEY_NUMPAD_ENTER	108
#define RAWKEY_NUMLOCK		77

#define RAWKEY_INSERT		105
#define RAWKEY_DELETE		107
#define RAWKEY_HOME		97
#define RAWKEY_END		103
#define RAWKEY_PAGEUP		99
#define RAWKEY_PAGEDOWN		105

#define RAWKEY_CURSOR_UP	98
#define RAWKEY_CURSOR_DOWN	104
#define RAWKEY_CURSOR_LEFT	100
#define RAWKEY_CURSOR_RIGHT	102

#define RAWKEY_LEFT_CTRL	37
#define RAWKEY_LEFT_SHIFT	50
#define RAWKEY_LEFT_ALT	        64
#define RAWKEY_LEFT_SUPER	115
#define RAWKEY_RIGHT_SUPER	116
#define RAWKEY_RIGHT_ALT	113
#define RAWKEY_MENU		117
#define RAWKEY_RIGHT_SHIFT	62
#define RAWKEY_RIGHT_CTRL	109
#define RAWKEY_CAPSLOCK		66

#define RAWKEY_POWER		222
#define RAWKEY_SLEEP		223
#define RAWKEY_WAKE		227

/*
 * Mapping from raw key codes to UAE key codes
 */
const struct uaekey_hostmap x11pc_keymap[] =
{
    {RAWKEYS_COMMON},

    {RAWKEY_F11,		UAEKEY_F11},
    {RAWKEY_F12,		UAEKEY_F12},

    {RAWKEY_PRINTSCR,		UAEKEY_PRINTSCR},
    {RAWKEY_SCROLL_LOCK,	UAEKEY_SCROLL_LOCK},
    {RAWKEY_PAUSE,		UAEKEY_PAUSE},

    {RAWKEY_MENU,		UAEKEY_MENU},

    {RAWKEY_POWER,		UAEKEY_POWER},
    {RAWKEY_SLEEP,		UAEKEY_SLEEP},
    {RAWKEY_WAKE,		UAEKEY_WAKE},

    {RAWKEYS_END}
};

#if 0
/*
 * Mapping of uae modifier key codes to x11pc modifier keys
 * Not actually needed - just here for test purposes.
 */
const int x11pc_modkeytable[] = {
    /* UAEMODKEY_LSHIFT */	RAWKEY_LEFT_SHIFT,
    /* UAEMODKEY_LCTRL */	RAWKEY_LEFT_CTRL,
    /* UAEMODKEY_LALT */	RAWKEY_LEFT_ALT,
    /* UAEMODKEY_LSUPER */	RAWKEY_LEFT_SUPER,
    /* UAEMODKEY_RSUPER */	RAWKEY_RIGHT_SUPER,
    /* UAEMODKEY_RALT */	RAWKEY_RIGHT_ALT,
    /* UAEMODKEY_RCTRL */	RAWKEY_RIGHT_CTRL,
    /* UAEMODKEY_RSHIFT */	RAWKEY_RIGHT_SHIFT,
    /* UAEMODKEY_CAPSLOCK */	RAWKEY_CAPSLOCK
};
#endif

/*
 * Hot-key seqeuences
 */
#include "hotkeys.h"
#include "hotkeys_common.h"

struct uae_hotkeyseq x11pc_hotkeys[] =
{
    { DEFAULT_HOTKEYS },
    { HOTKEYS_END }
};

#endif
