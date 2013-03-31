/***********************************************************/
//  BeUAE - The Be Un*x Amiga Emulator
//
//  BeOS port keyboard routines
//
//  (c) 2004-2005 Richard Drummond
//  (c) 2000-2001 Axel Doerfler
//  (c) 1999 Be/R4 Sound - Raphael Moll
//  (c) 1998-1999 David Sowsy
//  (c) 1996-1998 Christian Bauer
//  (c) 1996 Patrick Hanevold
//
/***********************************************************/

#include "be-UAE.h"
#include "be-Window.h"

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "keybuf.h"
#include "inputdevice.h"
#include "hotkeys.h"
#include "keymap/keymap.h"
#include "keymap/beos_rawkeys.h"
};


/* Default translation table */
struct uae_input_device_kbr_default *default_keyboard;

/* Previous key states */
static key_info lastKeyInfo;

/*
 * Keyboard inputdevice functions
 */
static unsigned int get_kb_num (void)
{
    return 1;
}

static const char *get_kb_name (unsigned int kb)
{
    return "Default keyboard";
}

static unsigned int get_kb_widget_num (unsigned int kb)
{
    return 127;
}

static int get_kb_widget_first (unsigned int kb, int type)
{
    return 1;
}

static int get_kb_widget_type (unsigned int kb, unsigned int num, char *name, uae_u32 *code)
{
    // fix me
    *code = num;
    return IDEV_WIDGET_KEY;
}

static int init_kb (void)
{
    get_key_info (&lastKeyInfo);

    default_keyboard = uaekey_make_default_kbr (beos_keymap);
    inputdevice_setkeytranslation (default_keyboard);
    set_default_hotkeys (beos_hotkeys);

    return 1;
}

static void close_kb (void)
{
    if (default_keyboard) {
	free (default_keyboard);
	default_keyboard = 0;
    }
}

static int keyhack (int scancode, int pressed, int num)
{
    return scancode;
}

static void read_kb (void)
{
    int keycode;
    int keyinfo_byte;
    int keyinfo_bit;

    key_info keyInfo;

    get_key_info (&keyInfo);

    if (memcmp (keyInfo.key_states, lastKeyInfo.key_states, sizeof (keyInfo.key_states))) {
	for (keycode = 0; keycode < 0x80; keycode++) {
	    keyinfo_byte = keycode >> 3;
	    keyinfo_bit = 1 << (~keycode & 7);

	    // Key state changed?
	    if ((keyInfo.key_states[keyinfo_byte] & keyinfo_bit) != (lastKeyInfo.key_states[keyinfo_byte] & keyinfo_bit)) {
		int new_state = (keyInfo.key_states[keyinfo_byte] & keyinfo_bit) != 0;
		int ievent;

		if ((ievent = match_hotkey_sequence (keycode, new_state)))
		    handle_hotkey_event (ievent, new_state);
		else
		    inputdevice_translatekeycode (0, keycode, new_state);
	    }
	}
	lastKeyInfo = keyInfo;
    }
}

static int acquire_kb (unsigned int num, int flags)
{
    return 1;
}

static void unacquire_kb (unsigned int num)
{
}

struct inputdevice_functions inputdevicefunc_keyboard =
{
    init_kb,
    close_kb,
    acquire_kb,
    unacquire_kb,
    read_kb,
    get_kb_num,
    get_kb_name,
    get_kb_widget_num,
    get_kb_widget_type,
    get_kb_widget_first
};

int getcapslockstate (void)
{
    return 0;
}

void setcapslockstate (int state)
{
}
