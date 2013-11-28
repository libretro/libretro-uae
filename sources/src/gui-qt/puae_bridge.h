#ifndef PUAE_BRIDGE_H
#define PUAE_BRIDGE_H

#include <stdio.h>
#include "sysdeps.h"

extern "C" {
extern void inputdevice_updateconfig (const struct uae_prefs *srcprefs, struct uae_prefs *dstprefs);
}
extern void read_rom_list (void);

#endif // PUAE_BRIDGE_H
