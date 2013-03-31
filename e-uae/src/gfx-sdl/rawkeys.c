 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for mapping SDL raw keycodes
  *
  * Copyright 2004 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "inputdevice.h"
#include "keyboard.h"

#include "hotkeys.h"
#include "keymap/keymap.h"
#include "keymap/keymap_all.h"
#include "sdlgfx.h"

#include <SDL.h>

/*
 * This stuff is hacked together for now to get
 * raw keyboard support working and tested.
 *
 * A cleaner implementation will be coming . . .
 */
struct sdl_raw_keymap
{
    int                          sdl_gfx_driver;
    const char                  *name;
    const struct uaekey_hostmap *keymap;
          struct uae_hotkeyseq  *hotkeys;
    const int                   *modtable;
};

static struct uae_input_device_kbr_default *keyboard = 0;
static const int *modkeytable;

/*
 * Table used to pick keymapping based on SDL gfx driver
 */
static const struct sdl_raw_keymap keymaps[] = {
#if (defined __i386__ || defined __x86_64__ || defined __powerpc__ || defined __ppc__) && defined __linux__
    { SDLGFX_DRIVER_X11,     "x11pc",  x11pc_keymap,  x11pc_hotkeys,  0},
    { SDLGFX_DRIVER_DGA,     "x11pc",  x11pc_keymap,  x11pc_hotkeys,  0},
#endif
#if defined __APPLE__
    { SDLGFX_DRIVER_QUARTZ,  "quartz", quartz_keymap, quartz_hotkeys, quartz_modkeytable},
#endif
#ifdef __BEOS__
    { SDLGFX_DRIVER_BWINDOW, "beos",   beos_keymap,   beos_hotkeys,   0},
#endif
#ifdef TARGET_AMIGAOS
# ifdef __amigaos4__
    { SDLGFX_DRIVER_AMIGAOS4, "amiga", amiga_keymap,  amiga_hotkeys,  0},
# else
    { SDLGFX_DRIVER_CYBERGFX, "amiga", amiga_keymap,  amiga_hotkeys,  0},
# endif
#endif
    { 0, 0, 0, 0, 0 }
};

struct uae_input_device_kbr_default *get_default_raw_keymap (int type)
{
    const struct sdl_raw_keymap *k = &keymaps[0];

    if (!keyboard) {
        free (keyboard);
        keyboard = 0;
    }

    while (k->sdl_gfx_driver != type && k->sdl_gfx_driver != 0)
	k++;

    if (k->keymap) {
	write_log ("Found %s raw keyboard mapping\n", k->name);
	modkeytable = k->modtable;
        keyboard = uaekey_make_default_kbr (k->keymap);
    }

    return keyboard;
}

struct uae_hotkeyseq *get_default_raw_hotkeys (void)
{
    const struct sdl_raw_keymap *k = &keymaps[0];

    while (k->sdl_gfx_driver != get_sdlgfx_type())
	k++;

    return k->hotkeys;
}


/*
 * Map SDL modifier key to raw key code
 *
 * This is required on platforms where the modifiers keys aren't
 * reported with a raw key code - but SDL_GetModState() works, e.g.,
 * on OS X.
 */
int modifier_hack (int *scancode, int *pressed)
{
    int result = 1;

    static int old_modifiers = 0;
    int modifiers = SDL_GetModState ();

    if (modkeytable && modifiers != old_modifiers) {
	if ((modifiers & KMOD_LSHIFT) != (old_modifiers & KMOD_LSHIFT)) {
	     *scancode = modkeytable[UAEMODKEY_LSHIFT];
	     *pressed  = modifiers & KMOD_LSHIFT;
	} else if ((modifiers & KMOD_RSHIFT) != (old_modifiers & KMOD_RSHIFT)) {
	     *scancode = modkeytable[UAEMODKEY_RSHIFT];
	     *pressed  = modifiers & KMOD_RSHIFT;
	} else if ((modifiers & KMOD_LCTRL) != (old_modifiers & KMOD_LCTRL)) {
	     *scancode = modkeytable[UAEMODKEY_LCTRL];
	     *pressed  = modifiers & KMOD_LCTRL;
	} else if ((modifiers & KMOD_RCTRL) != (old_modifiers & KMOD_RCTRL)) {
	     *scancode = modkeytable[UAEMODKEY_RCTRL];
	     *pressed  = modifiers & KMOD_RCTRL;
	} else if ((modifiers & KMOD_LALT) != (old_modifiers & KMOD_LALT)) {
	     *scancode = modkeytable[UAEMODKEY_LALT];
	     *pressed  = modifiers & KMOD_LALT;
	} else if ((modifiers & KMOD_RALT) != (old_modifiers & KMOD_RALT)) {
	     *scancode = modkeytable[UAEMODKEY_RALT];
	     *pressed  = modifiers & KMOD_RALT;
	} else if ((modifiers & KMOD_LMETA) != (old_modifiers & KMOD_LMETA)) {
	     *scancode = modkeytable[UAEMODKEY_LSUPER];
	     *pressed  = modifiers & KMOD_LMETA;
	} else if ((modifiers & KMOD_RMETA) != (old_modifiers & KMOD_RMETA)) {
	     *scancode = modkeytable[UAEMODKEY_RSUPER];
	     *pressed  = modifiers & KMOD_RMETA;
	} else if ((modifiers & KMOD_CAPS) != (old_modifiers & KMOD_CAPS)) {
	     *scancode = modkeytable[UAEMODKEY_CAPSLOCK];
	     *pressed  = modifiers & KMOD_CAPS;
	} else
	     result = 0;
        old_modifiers = modifiers;
    } else
	result = 0;

    return result;
}
