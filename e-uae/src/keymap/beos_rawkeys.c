 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for mapping BeOS raw key codes to platform-independent
  * UAE key codes.
  *
  * Copyright 2004 Richard Drummond
  */

#ifdef __BEOS__

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include "keymap.h"
#include "keymap_common.h"

/*
 * BeOS raw key codes
 */
#define RAWKEY_ESCAPE		1

#define RAWKEY_F1		2
#define RAWKEY_F2		3
#define RAWKEY_F3		4
#define RAWKEY_F4		5
#define RAWKEY_F5		6
#define RAWKEY_F6		7
#define RAWKEY_F7		8
#define RAWKEY_F8		9
#define RAWKEY_F9		10
#define RAWKEY_F10		11
#define RAWKEY_F11		12
#define RAWKEY_F12		13

#define RAWKEY_PRINTSCR		14
#define RAWKEY_SCROLL_LOCK	15
#define RAWKEY_PAUSE		16

#define RAWKEY_1		18
#define RAWKEY_2		19
#define RAWKEY_3		20
#define RAWKEY_4		21
#define RAWKEY_5		22
#define RAWKEY_6		23
#define RAWKEY_7		24
#define RAWKEY_8		25
#define RAWKEY_9		26
#define RAWKEY_0		27

#define RAWKEY_TAB		38
#define RAWKEY_ENTER		71
#define RAWKEY_BACKSPACE	30
#define RAWKEY_SPACE		94

#define RAWKEY_A		60
#define RAWKEY_B		80
#define RAWKEY_C		78
#define RAWKEY_D		62
#define RAWKEY_E		41
#define RAWKEY_F		63
#define RAWKEY_G		64
#define RAWKEY_H		65
#define RAWKEY_I		46
#define RAWKEY_J		66
#define RAWKEY_K		67
#define RAWKEY_L		68
#define RAWKEY_M		82
#define RAWKEY_N		81
#define RAWKEY_O		47
#define RAWKEY_P		48
#define RAWKEY_Q		39
#define RAWKEY_R		42
#define RAWKEY_S		61
#define RAWKEY_T		43
#define RAWKEY_U		45
#define RAWKEY_V		79
#define RAWKEY_W		40
#define RAWKEY_X		77
#define RAWKEY_Y		44
#define RAWKEY_Z		76

#define RAWKEY_MINUS		28
#define RAWKEY_EQUALS		29
#define RAWKEY_LEFTBRACKET	49
#define RAWKEY_RIGHTBRACKET	50
#define RAWKEY_BACKSLASH	51
#define RAWKEY_SEMICOLON	69
#define RAWKEY_SINGLEQUOTE	70
#define RAWKEY_COMMA		83
#define RAWKEY_PERIOD		84
#define RAWKEY_SLASH		85
#define RAWKEY_GRAVE		17
#define RAWKEY_LTGT		104

#define RAWKEY_NUMPAD_1		88
#define RAWKEY_NUMPAD_2		89
#define RAWKEY_NUMPAD_3		90
#define RAWKEY_NUMPAD_4		72
#define RAWKEY_NUMPAD_5		73
#define RAWKEY_NUMPAD_6		74
#define RAWKEY_NUMPAD_7		55
#define RAWKEY_NUMPAD_8		56
#define RAWKEY_NUMPAD_9		57
#define RAWKEY_NUMPAD_0		100
#define RAWKEY_NUMPAD_DIVIDE	35
#define RAWKEY_NUMPAD_MULTIPLY	36
#define RAWKEY_NUMPAD_MINUS	37
#define RAWKEY_NUMPAD_PLUS	58
#define RAWKEY_NUMPAD_PERIOD	101
#define RAWKEY_NUMPAD_ENTER	91
#define RAWKEY_NUMLOCK		34

#define RAWKEY_INSERT		31
#define RAWKEY_DELETE		52
#define RAWKEY_HOME		32
#define RAWKEY_END		53
#define RAWKEY_PAGEUP		33
#define RAWKEY_PAGEDOWN		54

#define RAWKEY_CURSOR_UP	87
#define RAWKEY_CURSOR_DOWN	98
#define RAWKEY_CURSOR_LEFT	97
#define RAWKEY_CURSOR_RIGHT	99

#define RAWKEY_LEFT_CTRL	92
#define RAWKEY_LEFT_SHIFT	75
#define RAWKEY_LEFT_ALT	        93
#define RAWKEY_LEFT_SUPER	102
#define RAWKEY_RIGHT_SUPER	103
#define RAWKEY_RIGHT_ALT	95
#define RAWKEY_RIGHT_SHIFT	86
#define RAWKEY_RIGHT_CTRL	96
#define RAWKEY_CAPSLOCK		59

/*
 * Mapping from BeOS raw key codes to UAE key codes
 */
const struct uaekey_hostmap beos_keymap[] =
{
    {RAWKEYS_COMMON},

    {RAWKEY_F11,		UAEKEY_F11},
    {RAWKEY_F12,		UAEKEY_F12},

    {RAWKEY_PRINTSCR,		UAEKEY_PRINTSCR},
    {RAWKEY_SCROLL_LOCK,	UAEKEY_SCROLL_LOCK},
    {RAWKEY_PAUSE,		UAEKEY_PAUSE},

    {RAWKEYS_END}
};


/*
 * Hot-key sequences
 */
#include "hotkeys.h"
#include "hotkeys_common.h"

struct uae_hotkeyseq beos_hotkeys[] =
{
    { DEFAULT_HOTKEYS },
    { HOTKEYS_END }
};

#endif
