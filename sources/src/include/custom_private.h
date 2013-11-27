 /*
  * E-UAE - The portable Amiga Emulator
  *
  * Custom chip emulation internals
  *
  * Copyright Richard Drummond 2006
  *
  * Based on code:
  * Copyright (c) Bernd Schmidmt 1995
  */

/*
 * TODO: migrate more custom chip internals here
 */


/*
 * Direct access to chip memory from custom-chip emulation
 *
 * TODO: do this more cleanly
 */
STATIC_INLINE void do_chipmem_wput (uaecptr addr, uae_u32 w)
{
    uae_u16 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u16 *)(chipmemory + addr);
    do_put_mem_word (m, w);
}

STATIC_INLINE uae_u16 do_chipmem_wget (uaecptr addr)
{
    uae_u16 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u16 *)(chipmemory + addr);
    return do_get_mem_word (m);
}

STATIC_INLINE uae_u32 do_chipmem_lget (uaecptr addr)
{
    uae_u32 *m;

    addr -= chipmem_start & chipmem_mask;
    addr &= chipmem_mask;
    m = (uae_u32 *)(chipmemory + addr);
    return do_get_mem_long (m);
}

#define chipmem_wput do_chipmem_wput
#define chipmem_wget do_chipmem_wget
#define chipmem_lget do_chipmem_lget
