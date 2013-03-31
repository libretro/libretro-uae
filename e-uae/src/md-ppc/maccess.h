 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory access functions
  *
  * Copyright 1996 Bernd Schmidt
  */

static __inline__ uae_u32 do_get_mem_long(uae_u32 *a)
{
    return *a;
}

static __inline__ uae_u16 do_get_mem_word(uae_u16 *a)
{
    return *a;
}

static __inline__ uae_u8 do_get_mem_byte(uae_u8 *a)
{
    return *a;
}

static __inline__ void do_put_mem_long(uae_u32 *a, uae_u32 v)
{
    *a = v;
}

static __inline__ void do_put_mem_word(uae_u16 *a, uae_u16 v)
{
    *a = v;
}

static __inline__ void do_put_mem_byte(uae_u8 *a, uae_u8 v)
{
    *a = v;
}

#define call_mem_get_func(func, addr) ((*func)(addr))
#define call_mem_put_func(func, addr, v) ((*func)(addr, v))
