 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory access functions
  *
  * Copyright 1996 Bernd Schmidt
  */

STATIC_INLINE uae_u32 do_get_mem_long (uae_u32 *a)
{
    uae_u32 retval;

    __asm__ ("bswap %0" : "=r" (retval) : "0" (*a) : "cc");
    return retval;
}

STATIC_INLINE uae_u32 do_get_mem_word (uae_u16 *a)
{
    uae_u32 retval;

#ifdef X86_PPRO_OPT
    __asm__ ("movzwl %w1,%k0\n\tshll $16,%k0\n\tbswap %k0\n" : "=&r" (retval) : "m" (*a) : "cc");
#else
    __asm__ ("xorl %k0,%k0\n\tmovw %w1,%w0\n\trolw $8,%w0" : "=&r" (retval) : "m" (*a) : "cc");
#endif
    return retval;
}

#define do_get_mem_byte(a) ((uae_u32)*((uae_u8 *)a))

STATIC_INLINE void do_put_mem_long (uae_u32 *a, uae_u32 v)
{
    __asm__ ("bswap %0" : "=r" (v) : "0" (v) : "cc");
    *a = v;
}

STATIC_INLINE void do_put_mem_word (uae_u16 *a, uae_u32 v)
{
#ifdef X86_PPRO_OPT
    __asm__ ("bswap %0" : "=&r" (v) : "0" (v << 16) : "cc");
#else
    __asm__ ("rolw $8,%w0" : "=r" (v) : "0" (v) : "cc");
#endif
    *a = v;
}

#define do_put_mem_byte(a,v) (*(uae_u8 *)(a) = (v))

#define call_mem_get_func(func,addr) ((*func)(addr))
#define call_mem_put_func(func,addr,v) ((*func)(addr,v))

#define ALIGN_POINTER_TO32(p) ((~(unsigned long)(p)) & 3)
