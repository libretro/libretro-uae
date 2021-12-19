#ifndef __MACCESS_H__
#define __MACCESS_H__

#include <stdlib.h>

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory access functions
  *
  * Copyright 2020 Google
  */

#include <retro_endianness.h>

#define ALIGN_POINTER_TO32(p) ((~(unsigned long)(p)) & 3)

STATIC_INLINE uae_u64 do_get_mem_quad(uae_u64 *a)
{
   return retro_get_unaligned_64be(a);
}

STATIC_INLINE uae_u32 do_get_mem_long (uae_u32 *a)
{
   return retro_get_unaligned_32be(a);
}

STATIC_INLINE uae_u16 do_get_mem_word (uae_u16 *a)
{
   return retro_get_unaligned_16be(a);
}

STATIC_INLINE uae_u8 do_get_mem_byte (uae_u8 *a)
{
   return *a;
}

/*#define do_get_mem_byte(a) ((uae_u32)*(uae_u8 *)(a))*/

STATIC_INLINE void do_put_mem_quad(uae_u64 *a, uae_u64 v)
{
   retro_set_unaligned_64be(a, v);
}

STATIC_INLINE void do_put_mem_long (uae_u32 *a, uae_u32 v)
{
   retro_set_unaligned_32be(a, v);
}

STATIC_INLINE void do_put_mem_word (uae_u16 *a, uae_u16 v)
{
   retro_set_unaligned_16be(a, v);
}

STATIC_INLINE void do_put_mem_byte (uae_u8 *a, uae_u8 v)
{
   *a = v;
}


STATIC_INLINE uae_u64 do_byteswap_64(uae_u64 v)
{
	return SWAP64(v);
}

STATIC_INLINE uae_u32 do_byteswap_32(uae_u32 v)
{
	return SWAP32(v);
}

STATIC_INLINE uae_u16 do_byteswap_16(uae_u16 v)
{
	return SWAP16(v);
}

STATIC_INLINE uae_u32 do_get_mem_word_unswapped(uae_u16 *a)
{
	return *a;
}

#define call_mem_get_func(func, addr) ((*func)(addr))
#define call_mem_put_func(func, addr, v) ((*func)(addr, v))

#endif
