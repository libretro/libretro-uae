 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for mapping raw keys codes to platform-independent
  * UAE key codes on AmigaOS and clones.
  *
  * Copyright 2004-2005 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#ifdef TARGET_AMIGAOS

#include "options.h"
#include "inputdevice.h"
#include "keymap.h"
#include "keymap_common.h"

/*
 * Raw scan codes
 */

/* 'Classic' Amiga keys */

#define RAWKEY_ESCAPE		0x45

#define RAWKEY_F1		0x50
#define RAWKEY_F2		0x51
#define RAWKEY_F3		0x52
#define RAWKEY_F4		0x53
#define RAWKEY_F5		0x54
#define RAWKEY_F6		0x55
#define RAWKEY_F7		0x56
#define RAWKEY_F8		0x57
#define RAWKEY_F9		0x58
#define RAWKEY_F10		0x59

#define RAWKEY_1		0x01
#define RAWKEY_2		0x02
#define RAWKEY_3		0x03
#define RAWKEY_4		0x04
#define RAWKEY_5		0x05
#define RAWKEY_6		0x06
#define RAWKEY_7		0x07
#define RAWKEY_8		0x08
#define RAWKEY_9		0x09
#define RAWKEY_0		0x0A

#define RAWKEY_TAB		0x42
#define RAWKEY_ENTER		0x44
#define RAWKEY_BACKSPACE	0x41
#define RAWKEY_SPACE		0x40

#define RAWKEY_A		0x20
#define RAWKEY_B		0x35
#define RAWKEY_C		0x33
#define RAWKEY_D		0x22
#define RAWKEY_E		0x12
#define RAWKEY_F		0x23
#define RAWKEY_G		0x24
#define RAWKEY_H		0x25
#define RAWKEY_I		0x17
#define RAWKEY_J		0x26
#define RAWKEY_K		0x27
#define RAWKEY_L		0x28
#define RAWKEY_M		0x37
#define RAWKEY_N		0x36
#define RAWKEY_O		0x18
#define RAWKEY_P		0x19
#define RAWKEY_Q		0x10
#define RAWKEY_R		0x13
#define RAWKEY_S		0x21
#define RAWKEY_T		0x14
#define RAWKEY_U		0x16
#define RAWKEY_V		0x34
#define RAWKEY_W		0x11
#define RAWKEY_X		0x32
#define RAWKEY_Y		0x15
#define RAWKEY_Z		0x31

#define RAWKEY_MINUS		0x0B
#define RAWKEY_EQUALS		0x0C
#define RAWKEY_LEFTBRACKET	0x1A
#define RAWKEY_RIGHTBRACKET	0x1B
#define RAWKEY_BACKSLASH	0x0D
#define RAWKEY_SEMICOLON	0x29
#define RAWKEY_SINGLEQUOTE	0x2A
#define RAWKEY_COMMA		0x38
#define RAWKEY_PERIOD		0x39
#define RAWKEY_SLASH		0x3A
#define RAWKEY_GRAVE		0x00
#define RAWKEY_LTGT		0x30

#define RAWKEY_NUMPAD_1		0x1D
#define RAWKEY_NUMPAD_2		0x1E
#define RAWKEY_NUMPAD_3		0x1F
#define RAWKEY_NUMPAD_4		0x2D
#define RAWKEY_NUMPAD_5		0x2E
#define RAWKEY_NUMPAD_6		0x2F
#define RAWKEY_NUMPAD_7		0x3D
#define RAWKEY_NUMPAD_8		0x3E
#define RAWKEY_NUMPAD_9		0x3F
#define RAWKEY_NUMPAD_0		0x0F

#define RAWKEY_NUMPAD_LPAREN    0x5A
#define RAWKEY_NUMPAD_RPAREN    0x5B

#define RAWKEY_NUMPAD_DIVIDE	0x5C
#define RAWKEY_NUMPAD_MULTIPLY	0x5D
#define RAWKEY_NUMPAD_MINUS	0x4A
#define RAWKEY_NUMPAD_PLUS	0x5E
#define RAWKEY_NUMPAD_PERIOD	0x3C
#define RAWKEY_NUMPAD_ENTER	0x43

#define RAWKEY_INSERT		0x47
#define RAWKEY_DELETE		0x46
#define RAWKEY_HELP             0x5F

#define RAWKEY_CURSOR_UP	0x4C
#define RAWKEY_CURSOR_DOWN	0x4D
#define RAWKEY_CURSOR_LEFT	0x4F
#define RAWKEY_CURSOR_RIGHT	0x4E

#define RAWKEY_LEFT_CTRL	0x63
#define RAWKEY_LEFT_SHIFT	0x60
#define RAWKEY_LEFT_ALT	        0x64
#define RAWKEY_LEFT_SUPER	0x66
#define RAWKEY_RIGHT_SUPER	0x67
#define RAWKEY_RIGHT_ALT	0x65

#define RAWKEY_RIGHT_SHIFT	0x61

#define RAWKEY_CAPSLOCK		0x62

/* Extra keys on PC/USB keyboards */

#define RAWKEY_F11		0x4B
#define RAWKEY_F12		0x6F

#define RAWKEY_HOME		0x70
#define RAWKEY_END		0x71
#define RAWKEY_PAGEUP		0x48
#define RAWKEY_PAGEDOWN		0x49

#define RAWKEY_PAUSE            0x6E

#define RAWKEY_NUMLOCK 0xFF
#define RAWKEY_RIGHT_CTRL 0xFF
//#define RAWKEY_MENU
//#define RAWKEY_PRINTSCR
//#define RAWKEY_SCROLL_LOCK
//#define RAWKEY_PAUSE
//#define RAWKEY_POWER
//#define RAWKEY_SLEEP
//#define RAWKEY_WAKE

/*
 * Mapping from raw key codes to UAE key codes
 */
const struct uaekey_hostmap amiga_keymap[] =
{
    {RAWKEYS_COMMON},

    {RAWKEY_F11,		UAEKEY_F11},
    {RAWKEY_F12,		UAEKEY_F12},

//    {RAWKEY_PRINTSCR,		UAEKEY_PRINTSCR},
//    {RAWKEY_SCROLL_LOCK,	UAEKEY_SCROLL_LOCK},
    {RAWKEY_PAUSE,		UAEKEY_PAUSE},

//    {RAWKEY_MENU,		UAEKEY_MENU},

//    {RAWKEY_POWER,		UAEKEY_POWER},
//    {RAWKEY_SLEEP,		UAEKEY_SLEEP},
//    {RAWKEY_WAKE,		UAEKEY_WAKE},

    {RAWKEYS_END}
};

/*
 * Hot-key seqeuences
 */

/*
 * Classic Amiga keyboards don't have F11/F12, so we default to
 * using Ctrl+Left Alt for hot-key sequences
 */
#define HOTKEY_MODIFIER        RAWKEY_LEFT_CTRL
#define HOTKEY_MODIFIER2       RAWKEY_LEFT_ALT

#include "hotkeys.h"
#include "hotkeys_common.h"

struct uae_hotkeyseq amiga_hotkeys[] =
{
    { DEFAULT_HOTKEYS },
    { HOTKEYS_END }
};

#endif
