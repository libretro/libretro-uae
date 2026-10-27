#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "memory.h"
#include "newcpu.h"

/* Linked only by Test-HorizontalCentering.py, never into the shipped core. */
void probe_display(int shift, int accurate)
{
    map_overlay(1);
    regs.halted = 0;
    regs.stopped = 0;
    /* Original synthetic display: solid white 320x256 playfield. */
    memory_put_word(0xdff096, 0x7fff);
    memory_put_word(0xdff09a, 0x7fff);
    memory_put_word(0xdff09c, 0x7fff);
    for (unsigned p = 0x40000; p < 0x45000; p += 2) memory_put_word(p, 0xffff);
    unsigned short copper[] = {
        0x008e, 0x2c81 + shift, 0x0090, 0x2cc1 + shift,
        0x0092, 0x0038 + shift/2, 0x0094, 0x00d0 + shift/2,
        0x0100, 0x1200, 0x0102, 0, 0x0104, 0,
        0x0108, 0, 0x010a, 0,
        0x00e0, 4, 0x00e2, 0,
        0x0180, 0, 0x0182, 0x0fff,
        0xffff, 0xfffe
    };
    for (unsigned i = 0; i < sizeof(copper)/sizeof(*copper); ++i) memory_put_word(0x61000 + i*2, copper[i]);
    memory_put_word(0xdff080, 6);
    memory_put_word(0xdff082, 0x1000);
    memory_put_word(0xdff088, 0);
    memory_put_word(0x60000, 0x60fe);

    currprefs.cs_optimizations = changed_prefs.cs_optimizations = accurate ? 2 : 0;
    regs.intmask = 7;
    regs.s = 1;
    m68k_setpc_normal(0x60000);
    fill_prefetch();
    memory_put_word(0xdff096, 0x8380);
}

void probe_resolution(int hires)
{
    memory_put_word(0x6100e, hires ? 0x00d8 : 0x00d0);
    memory_put_word(0x61012, hires ? 0x9200 : 0x1200);
}
