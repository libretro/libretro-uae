 /*
  * UAE - The Un*x Amiga Emulator
  *
  * memory management
  *
  * Copyright 1995 Bernd Schmidt
  */

extern void memory_reset (void);
extern void a1000_reset (void);

#ifdef JIT
extern int special_mem;
#define SPECIAL_MEM_READ 1
#define SPECIAL_MEM_WRITE 2
extern void *cache_alloc (int);
extern void cache_free (void*);

extern int canbang;
void init_shm (void);
#endif

#ifdef ADDRESS_SPACE_24BIT
# define MEMORY_BANKS		256
# define MEMORY_RANGE_MASK	((1<<24)-1)
#else
# define MEMORY_BANKS		65536
# define MEMORY_RANGE_MASK	(~0)
#endif

typedef uae_u32 (*mem_get_func)(uaecptr) REGPARAM;
typedef void (*mem_put_func)(uaecptr, uae_u32) REGPARAM;
typedef uae_u8 *(*xlate_func)(uaecptr) REGPARAM;
typedef int (*check_func)(uaecptr, uae_u32) REGPARAM;

extern char *address_space, *good_address_map;
extern uae_u8 *chipmemory;

extern uae_u32 allocated_chipmem;
extern uae_u32 allocated_fastmem;
extern uae_u32 allocated_bogomem;
extern uae_u32 allocated_gfxmem;
extern uae_u32 allocated_z3fastmem;
extern uae_u32 allocated_a3000mem;

extern uae_u32 wait_cpu_cycle_read (uaecptr addr, int mode);
extern uae_u32 wait_cpu_cycle_read_cycles (uaecptr addr, int mode, int *cycles);
extern void wait_cpu_cycle_write (uaecptr addr, int mode, uae_u32 v);

#undef DIRECT_MEMFUNCS_SUCCESSFUL
#include "machdep/maccess.h"
#include "osdep/memory.h"

#define chipmem_start 0x00000000
#define bogomem_start 0x00C00000
#define a3000mem_start 0x07000000
#define kickmem_start 0x00F80000

extern int ersatzkickfile;
extern int cloanto_rom;
extern uae_u16 kickstart_version;


typedef struct {
    /* These ones should be self-explanatory... */
    mem_get_func lget, wget, bget;
    mem_put_func lput, wput, bput;
    /* Use xlateaddr to translate an Amiga address to a uae_u8 * that can
     * be used to address memory without calling the wget/wput functions.
     * This doesn't work for all memory banks, so this function may call
     * abort(). */
    xlate_func xlateaddr;
    /* To prevent calls to abort(), use check before calling xlateaddr.
     * It checks not only that the memory bank can do xlateaddr, but also
     * that the pointer points to an area of at least the specified size.
     * This is used for example to translate bitplane pointers in custom.c */
    check_func check;
    /* For those banks that refer to real memory, we can save the whole trouble
       of going through function calls, and instead simply grab the memory
       ourselves. This holds the memory address where the start of memory is
       for this particular bank. */
    uae_u8 *baseaddr;
} addrbank;

extern uae_u8 *filesysory;

extern addrbank chipmem_bank;
extern addrbank chipmem_bank_ce2;
extern addrbank kickmem_bank;
extern addrbank custom_bank;
extern addrbank clock_bank;
extern addrbank cia_bank;
extern addrbank rtarea_bank;
extern addrbank expamem_bank;
extern addrbank fastmem_bank;
extern addrbank gfxmem_bank;

extern void rtarea_init (void);
extern void rtarea_setup (void);
extern void expamem_init (void);
extern void expamem_reset (void);

extern uae_u32 gfxmem_start;
extern uae_u8 *gfxmemory;
extern uae_u32 gfxmem_mask;
extern int address_space_24;

/* Default memory access functions */

extern int default_check(uaecptr addr, uae_u32 size) REGPARAM;
extern uae_u8 *default_xlate(uaecptr addr) REGPARAM;

#define bankindex(addr) (((uaecptr)(addr)) >> 16)

extern addrbank *mem_banks[MEMORY_BANKS];

#ifdef JIT
extern uae_u8 *baseaddr[MEMORY_BANKS];
#endif

#define get_mem_bank(addr) (*mem_banks[bankindex(addr)])

#ifdef JIT
# define put_mem_bank(addr, b, realstart) do { \
    (mem_banks[bankindex(addr)] = (b)); \
    if ((b)->baseaddr) \
        baseaddr[bankindex(addr)] = (b)->baseaddr - (realstart); \
    else \
        baseaddr[bankindex(addr)] = (uae_u8*)(((long)b)+1); \
} while (0)
#else
# define put_mem_bank(addr, b, realstart) \
    (mem_banks[bankindex(addr)] = (b));
#endif

extern void memory_init (void);
extern void memory_cleanup (void);
extern void map_banks (addrbank *bank, int first, int count, int realsize);
extern void map_overlay (int chip);

#define longget(addr) (call_mem_get_func(get_mem_bank(addr).lget, addr))
#define wordget(addr) (call_mem_get_func(get_mem_bank(addr).wget, addr))
#define byteget(addr) (call_mem_get_func(get_mem_bank(addr).bget, addr))
#define longput(addr,l) (call_mem_put_func(get_mem_bank(addr).lput, addr, l))
#define wordput(addr,w) (call_mem_put_func(get_mem_bank(addr).wput, addr, w))
#define byteput(addr,b) (call_mem_put_func(get_mem_bank(addr).bput, addr, b))

STATIC_INLINE uae_u32 get_long(uaecptr addr)
{
    addr &= MEMORY_RANGE_MASK;
    return longget(addr);
}
STATIC_INLINE uae_u32 get_word(uaecptr addr)
{
    addr &= MEMORY_RANGE_MASK;
    return wordget(addr);
}
STATIC_INLINE uae_u32 get_byte(uaecptr addr)
{
    addr &= MEMORY_RANGE_MASK;
    return byteget(addr);
}
/*
 * Read a host pointer from addr
 */
#if SIZEOF_VOID_P == 4
# define get_pointer(addr) ((void *)get_long(addr))
#else
# if SIZEOF_VOID_P == 8
STATIC_INLINE void *get_pointer (uaecptr addr)
{
    const unsigned int n = SIZEOF_VOID_P / 4;
    union {
	void    *ptr;
	uae_u32  longs[SIZEOF_VOID_P / 4];
    } p;
    unsigned int i;

    for (i = 0; i < n; i++) {
#ifdef WORDS_BIGENDIAN
	p.longs[i]     = get_long (addr + i * 4);
#else
	p.longs[n - 1 - i] = get_long (addr + i * 4);
#endif
    }
    return p.ptr;
}
# else
#  error "Unknown or unsupported pointer size."
# endif
#endif
STATIC_INLINE void put_long(uaecptr addr, uae_u32 l)
{
    addr &= MEMORY_RANGE_MASK;
    longput(addr, l);
}
STATIC_INLINE void put_word(uaecptr addr, uae_u32 w)
{
    addr &= MEMORY_RANGE_MASK;
    wordput(addr, w);
}
STATIC_INLINE void put_byte(uaecptr addr, uae_u32 b)
{
    addr &= MEMORY_RANGE_MASK;
    byteput(addr, b);
}
/*
 * Store host pointer v at addr
 */
#if SIZEOF_VOID_P == 4
# define put_pointer(addr, p) (put_long((addr), (uae_u32)(p)))
#else
# if SIZEOF_VOID_P == 8
STATIC_INLINE void put_pointer (uaecptr addr, void *v)
{
    const unsigned int n = SIZEOF_VOID_P / 4;
    union {
	void    *ptr;
	uae_u32  longs[SIZEOF_VOID_P / 4];
    } p;
    unsigned int i;

    p.ptr = v;

    for (i = 0; i < n; i++) {
#ifdef WORDS_BIGENDIAN
	put_long (addr + i * 4, p.longs[i]);
#else
	put_long (addr + i * 4, p.longs[n - 1 - i]);
#endif
    }
}
# endif
#endif
STATIC_INLINE uae_u8 *get_real_address(uaecptr addr)
{
    addr &= MEMORY_RANGE_MASK;
    return get_mem_bank(addr).xlateaddr(addr);
}

STATIC_INLINE int valid_address(uaecptr addr, uae_u32 size)
{
    addr &= MEMORY_RANGE_MASK;
    return get_mem_bank(addr).check(addr, size);
}

/* For faster access in custom chip emulation.  */
extern uae_u32 chipmem_lget (uaecptr) REGPARAM;
extern uae_u32 chipmem_wget (uaecptr) REGPARAM;
extern uae_u32 chipmem_bget (uaecptr) REGPARAM;
extern void chipmem_lput (uaecptr, uae_u32) REGPARAM;
extern void chipmem_wput (uaecptr, uae_u32) REGPARAM;
extern void chipmem_bput (uaecptr, uae_u32) REGPARAM;

extern uae_u32 chipmem_mask, kickmem_mask;
extern uae_u8 *kickmemory;
extern unsigned int kickmem_size;
extern addrbank dummy_bank;

/* 68020+ Chip RAM DMA contention emulation */
extern uae_u32 chipmem_lget_ce2 (uaecptr) REGPARAM;
extern uae_u32 chipmem_wget_ce2 (uaecptr) REGPARAM;
extern uae_u32 chipmem_bget_ce2 (uaecptr) REGPARAM;
extern void chipmem_lput_ce2 (uaecptr, uae_u32) REGPARAM;
extern void chipmem_wput_ce2 (uaecptr, uae_u32) REGPARAM;
extern void chipmem_bput_ce2 (uaecptr, uae_u32) REGPARAM;

#ifdef NATMEM_OFFSET

typedef struct shmpiece_reg {
    uae_u8 *native_address;
    int id;
    uae_u32 size;
    struct shmpiece_reg *next;
    struct shmpiece_reg *prev;
} shmpiece;

extern shmpiece *shm_start;

#endif

extern uae_u8 *mapped_malloc (size_t, const char *);
extern void mapped_free (uae_u8 *);
extern void clearexec (void);
extern void mapkick (void);


