/*
 * UAE - The Un*x Amiga Emulator
 *
 * Debugger
 *
 * (c) 1995 Bernd Schmidt
 * (c) 2006 Toni Wilen
 *
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include <ctype.h>
#ifdef HAVE_SIGNAL
#include <signal.h>
#endif

#include "options.h"
#if 0
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cpu_prefetch.h"
#include "debug.h"
#include "cia.h"
#include "xwin.h"
#include "identify.h"
#include "audio.h"
#include "disk.h"
#include "savestate.h"
#include "autoconf.h"
#include "filesys.h"
#include "akiko.h"
#include "inputdevice.h"
#include "crc32.h"
#include "cpummu.h"
#include "rommgr.h"
#include "inputrecord.h"
#include "calc.h"
#include "cpummu.h"
#include "cpummu030.h"
#include "misc.h"
#else
#include "memory.h"
#include "debug.h"
#include "xwin.h"
#endif

/* internal members */
#if 0
static uaecptr skipaddr_start, skipaddr_end;
static int skipaddr_doskip;
static uae_u32 skipins;
static int do_skip;
static int debug_rewind;
#endif
int debugger_active;
int memwatch_access_validator;
int memwatch_enabled;
int debugging;
int exception_debugging;
int no_trace_exceptions;
int debug_copper = 0;
int debug_dma = 0, debug_heatmap = 0;
int debug_sprite_mask = 0xff;
int debug_illegal = 0;
uae_u64 debug_illegal_mask;
#if 0
static int debug_mmu_mode;

static uaecptr processptr;
static uae_char *processname;

static uaecptr debug_copper_pc;
#endif
extern int audio_channel_mask;
extern int inputdevice_logging;
extern void my_trim (TCHAR *s);

struct peekdma peekdma_data;

#ifdef MMUEMU
int safe_addr (uaecptr addr, int size);
#endif

/*#define console_out               printf*/
#define console_flush()           fflush( stdout )
#define console_get( input, len ) fgets( input, len, stdin )
/*#define console_out_f printf*/

void deactivate_debugger (void)
{
}

void activate_debugger (void)
{
}

void activate_debugger_new(void)
{
}

void activate_debugger_new_pc(uaecptr pc, int len)
{
}

#if 0
int firsthist = 0;
int lasthist = 0;
static struct regstruct history[MAX_HIST];

static TCHAR help[] = {0};

void debug_help (void)
{
}


static int debug_linecounter;
#define MAX_LINECOUNTER 1000

static int debug_out (const TCHAR *format, ...)
{
	return 1;
}
#endif

#ifdef MMUEMU
uae_u32 get_byte_debug (uaecptr addr)
{
	return 0xff;
}

uae_u32 get_word_debug (uaecptr addr)
{
	return 0xffff;
}

uae_u32 get_long_debug (uaecptr addr)
{
	return 0xffffffff;
}

uae_u32 get_iword_debug (uaecptr addr)
{
	return 0xffff;
}

uae_u32 get_ilong_debug (uaecptr addr)
{
	return 0xffffffff;
}

int safe_addr (uaecptr addr, int size)
{
	return 0;
}
#endif

int debug_safe_addr (uaecptr addr, int size)
{
	return 0;
}

#if 0
static bool iscancel (int counter)
{
	return true;
}

static bool isoperator(TCHAR **cp)
{
	return false;
}

static void ignore_ws (TCHAR **c)
{
}
static TCHAR peekchar (TCHAR **c)
{
	return **c;
}
static TCHAR readchar (TCHAR **c)
{
	return 0;
}
static TCHAR next_char (TCHAR **c)
{
	return 0;
}
static TCHAR peek_next_char (TCHAR **c)
{
	return 0;
}
static int more_params (TCHAR **c)
{
	return 0;
}

static uae_u32 readint (TCHAR **c);
static uae_u32 readhex (TCHAR **c);

static int readregx (TCHAR **c, uae_u32 *valp)
{
	return 0;
}

static bool readbinx (TCHAR **c, uae_u32 *valp)
{
	return true;
}

static bool readhexx (TCHAR **c, uae_u32 *valp)
{
	return true;
}

static bool readintx (TCHAR **c, uae_u32 *valp)
{
	return true;
}


static int checkvaltype2 (TCHAR **c, uae_u32 *val, TCHAR def)
{
	return 0;
}

static int readsize (int val, TCHAR **c)
{
	return 0;
}

static int checkvaltype (TCHAR **cp, uae_u32 *val, int *size, TCHAR def)
{
	return 0;
}


static uae_u32 readnum (TCHAR **c, int *size, TCHAR def)
{
	return 0;
}

static uae_u32 readint (TCHAR **c)
{
	return 0;
}
static uae_u32 readhex (TCHAR **c)
{
	return 0;
}
#if 0
static uae_u32 readbin (TCHAR **c)
{
	return 0;
}
#endif
static uae_u32 readint_2 (TCHAR **c, int *size)
{
	return 0;
}
static uae_u32 readhex_2 (TCHAR **c, int *size)
{
	return 0;
}

static int next_string (TCHAR **c, TCHAR *out, int max, int forceupper)
{
	return 0;
}

static void converter (TCHAR **c)
{
}

int notinrom (void)
{
	return 0;
}

static uae_u32 lastaddr (void)
{
	return currprefs.chipmem_size;
}

static uaecptr nextaddr2 (uaecptr addr, uaecptr *next)
{
	return addr;
}

static uaecptr nextaddr (uaecptr addr, uaecptr last, uaecptr *end)
{
	return addr;
}

uaecptr dumpmem2 (uaecptr addr, TCHAR *out, int osize)
{
	return addr;
}

static void dumpmem (uaecptr addr, uaecptr *nxmem, int lines)
{
}

static void dump_custom_regs (int aga)
{
}

static void dump_vectors (uaecptr addr)
{
}

static void disassemble_wait (FILE *file, unsigned long insn)
{
}

#define NR_COPPER_RECORDS 100000
/* Record copper activity for the debugger.  */
struct cop_rec
{
	int hpos, vpos;
	uaecptr addr;
};
static struct cop_rec *cop_record[2];
static int nr_cop_records[2], curr_cop_set;

#define NR_DMA_REC_HPOS 256
#define NR_DMA_REC_VPOS 1000
static struct dma_rec *dma_record[2];
static int dma_record_toggle;
#endif
void record_dma_reset (void)
{
}

void record_copper_reset (void)
{
}


STATIC_INLINE uae_u32 ledcolor (uae_u32 c, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *a)
{
	return 0;
}

STATIC_INLINE void putpixel (uae_u8 *buf, int bpp, int x, xcolnr c8)
{
}

#define lc(x) ledcolor (x, xredcolors, xgreencolors, xbluecolors, NULL);
void debug_draw_cycles (uae_u8 *buf, int bpp, int line, int width, int height, uae_u32 *xredcolors, uae_u32 *xgreencolors, uae_u32 *xbluescolors)
{
}

void record_dma_event (int evt, int hpos, int vpos)
{
}

void record_dma_write(uae_u16 reg, uae_u32 dat, uae_u32 addr, int hpos, int vpos, int type, int extra)
{
}

void record_dma_read_value(uae_u32 v)
{
}
void record_dma_read(uae_u16 reg, uae_u32 addr, int hpos, int vpos, int type, int extra)
{
}

struct dma_rec *record_dma (uae_u16 reg, uae_u16 dat, uae_u32 addr, int hpos, int vpos, int type)
{
	struct dma_rec *dr = NULL;
	return dr;
}
#if 0
static void decode_dma_record (int hpos, int vpos, int toggle, bool logfile)
{
}
#endif
void log_dma_record (void)
{
}

void record_copper_blitwait (uaecptr addr, int hpos, int vpos)
{
}

void record_copper (uaecptr addr, uae_u16 word1, uae_u16 word2, int hpos, int vpos)
{
}
#if 0
static struct cop_rec *find_copper_records (uaecptr addr)
{
	return 0;
}

/* simple decode copper by Mark Cox */
static void decode_copper_insn (FILE* file, unsigned long insn, unsigned long addr)
{
}

static uaecptr decode_copperlist (FILE* file, uaecptr address, int nolines)
{
	return address;
}

static int copper_debugger (TCHAR **c)
{
	return 0;
}

#define MAX_CHEAT_VIEW 100
struct trainerstruct {
	uaecptr addr;
	int size;
};

static struct trainerstruct *trainerdata;
static int totaltrainers;

static void clearcheater(void)
{
}
static int addcheater(uaecptr addr, int size)
{
	return 0;
}
static void listcheater(int mode, int size)
{
}

static void deepcheatsearch (TCHAR **c)
{
}

/* cheat-search by Toni Wilen (originally by Holger Jakob) */
static void cheatsearch (TCHAR **c)
{
}
#endif

#if 0
struct breakpoint_node bpnodes[BREAKPOINT_TOTAL];
static addrbank **debug_mem_banks;
static addrbank *debug_mem_area;
struct memwatch_node mwnodes[MEMWATCH_TOTAL];
static struct memwatch_node mwhit;

static uae_u8 *illgdebug, *illghdebug;
static int illgdebug_break;

static void illg_free (void)
{
	xfree (illgdebug);
	illgdebug = NULL;
	xfree (illghdebug);
	illghdebug = NULL;
}

static void illg_init (void)
{
}

/* add special custom register check here */
static void illg_debug_check (uaecptr addr, int rwi, int size, uae_u32 val)
{
}

static void illg_debug_do (uaecptr addr, int rwi, int size, uae_u32 val)
{
}

static int debug_mem_off (uaecptr addr)
{
	return 0;
}

struct smc_item {
	uae_u32 addr;
	uae_u8 cnt;
};

static int smc_size, smc_mode;
static struct smc_item *smc_table;

static void smc_free (void)
{
}

static void initialize_memwatch (int mode);
static void smc_detect_init (TCHAR **c)
{
}

#define SMC_MAXHITS 8
static void smc_detector (uaecptr addr, int rwi, int size, uae_u32 *valp)
{
}
#endif
uae_u8 *save_debug_memwatch (int *len, uae_u8 *dstptr)
{
	return 0;
}

uae_u8 *restore_debug_memwatch (uae_u8 *src)
{
	return 0;
}

void restore_debug_memwatch_finish (void)
{
}

void debug_check_reg(uae_u32 addr, int write, uae_u16 v)
{
}

#if 0
static int memwatch_func (uaecptr addr, int rwi, int size, uae_u32 *valp)
{
	return 1;
}

static int mmu_hit (uaecptr addr, int size, int rwi, uae_u32 *v);


static uae_u32 REGPARAM2 mmu_lget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_wget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_bget (uaecptr addr)
{
	return 0;
}
static void REGPARAM2 mmu_lput (uaecptr addr, uae_u32 v)
{
}
static void REGPARAM2 mmu_wput (uaecptr addr, uae_u32 v)
{
}
static void REGPARAM2 mmu_bput (uaecptr addr, uae_u32 v)
{
}

static uae_u32 REGPARAM2 debug_lget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_lgeti (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_wgeti (uaecptr addr)
{
	return 0;
}

static uae_u32 REGPARAM2 debug_wget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 debug_bget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 debug_lgeti (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 debug_wgeti (uaecptr addr)
{
	return 0;
}
static void REGPARAM2 debug_lput (uaecptr addr, uae_u32 v)
{
}
static void REGPARAM2 debug_wput (uaecptr addr, uae_u32 v)
{
}
static void REGPARAM2 debug_bput (uaecptr addr, uae_u32 v)
{
}
static int REGPARAM2 debug_check (uaecptr addr, uae_u32 size)
{
	return 0;
}
static uae_u8 *REGPARAM2 debug_xlate (uaecptr addr)
{
	return 0;
}
#endif
uae_u16 debug_wputpeekdma (uaecptr addr, uae_u32 v)
{
	return 0;
}
uae_u16 debug_wgetpeekdma (uaecptr addr, uae_u32 v)
{
	return 0;
}

void debug_getpeekdma_chipram(uaecptr addr, uae_u32 mask, int reg, int ptrreg)
{
}

uae_u32 debug_getpeekdma_value(uae_u32 v)
{
	return 0;
}

uae_u32 debug_putpeekdma_chipset(uaecptr addr, uae_u32 v, uae_u32 mask, int reg)
{
	return 0;
}

uae_u32 debug_putpeekdma_chipram(uaecptr addr, uae_u32 v, uae_u32 mask, int reg, int ptrreg)
{
	return 0;
}

#if 0
void debug_putlpeek (uaecptr addr, uae_u32 v)
{
}
#endif
void debug_wputpeek (uaecptr addr, uae_u32 v)
{
}
void debug_bputpeek (uaecptr addr, uae_u32 v)
{
}
void debug_bgetpeek (uaecptr addr, uae_u32 v)
{
}
void debug_wgetpeek (uaecptr addr, uae_u32 v)
{
}
void debug_lgetpeek (uaecptr addr, uae_u32 v)
{
}
#if 0
struct membank_store
{
	addrbank *addr;
	addrbank store;
};

static struct membank_store *membank_stores;

static int deinitialize_memwatch (void)
{
	return 0;
}

static void initialize_memwatch (int mode)
{
}

static TCHAR *getsizechar (int size)
{
	return "";
}

void memwatch_dump2 (TCHAR *buf, int bufsize, int num)
{
}

static void memwatch_dump (int num)
{
}

static void memwatch (TCHAR **c)
{
}

static void writeintomem (TCHAR **c)
{
}

static uae_u8 *dump_xlate (uae_u32 addr)
{
	return 0;
}

static void memory_map_dump_2 (int log)
{
}
#endif

int debug_bankchange (int mode)
{
	return -1;
}

void memory_map_dump (void)
{
}


void debug_invalid_reg(int reg, int size, uae_u16 v)
{
}

STATIC_INLINE uaecptr BPTR2APTR (uaecptr addr)
{
	return addr << 2;
}
#if 0
static void print_task_info (uaecptr node)
{
}

static void show_exec_tasks (void)
{
}

static uaecptr get_base (const uae_char *name, int offset)
{
	return 0xffffffff;
}

static TCHAR *getfrombstr(uaecptr pp)
{
	return "";
}

static void show_exec_lists (TCHAR *t)
{
}

static uaecptr nextpc;

int instruction_breakpoint (TCHAR **c)
{
	return 1;
}

static int process_breakpoint(TCHAR **c)
{
	return 1;
}

static void savemem (TCHAR **cc)
{
}

static void searchmem (TCHAR **cc)
{
}

static int staterecorder (TCHAR **cc)
{
	return 0;
}

static int debugtest_modes[DEBUGTEST_MAX];
static const TCHAR *debugtest_names[] = {
	_T("Blitter"), _T("Keyboard"), _T("Floppy")
};
#endif
void debugtest (enum debugtest_item di, const TCHAR *format, ...)
{
}
#if 0
static void debugtest_set (TCHAR **inptr)
{
}

static void debug_sprite (TCHAR **inptr)
{
}

static void disk_debug (TCHAR **inptr)
{
}

static void find_ea (TCHAR **inptr)
{
}

static void m68k_modify (TCHAR **inptr)
{
}

static uaecptr nxdis, nxmem;

static bool debug_line (TCHAR *input)
{
	return false;
}

static void debug_1 (void)
{
}

static void addhistory (void)
{
}
#endif
void debug (void)
{
}

const TCHAR *debuginfo (int mode)
{
	return "";
}

void mmu_disasm (uaecptr pc, int lines)
{
}

#if 0
static int mmu_logging;

#define MMU_PAGE_SHIFT 16

struct mmudata {
	uae_u32 flags;
	uae_u32 addr;
	uae_u32 len;
	uae_u32 remap;
	uae_u32 p_addr;
};

static struct mmudata *mmubanks;
static uae_u32 mmu_struct, mmu_callback, mmu_regs;
static uae_u32 mmu_fault_bank_addr, mmu_fault_addr;
static int mmu_fault_size, mmu_fault_rw;
static int mmu_slots;

static struct regstruct mmur;

struct mmunode {
	struct mmudata *mmubank;
	struct mmunode *next;
};

static struct mmunode **mmunl;
extern struct regstruct mmu_backup_regs;

#define MMU_READ_U (1 << 0)
#define MMU_WRITE_U (1 << 1)
#define MMU_READ_S (1 << 2)
#define MMU_WRITE_S (1 << 3)
#define MMU_READI_U (1 << 4)
#define MMU_READI_S (1 << 5)

#define MMU_MAP_READ_U (1 << 8)
#define MMU_MAP_WRITE_U (1 << 9)
#define MMU_MAP_READ_S (1 << 10)
#define MMU_MAP_WRITE_S (1 << 11)
#define MMU_MAP_READI_U (1 << 12)
#define MMU_MAP_READI_S (1 << 13)
#endif

void mmu_do_hit (void)
{
}

#if 0
static void mmu_do_hit_pre (struct mmudata *md, uaecptr addr, int size, int rwi, uae_u32 v)
{
}

static int mmu_hit (uaecptr addr, int size, int rwi, uae_u32 *v)
{
	return 0;
}
#endif

#ifdef JIT
static void mmu_free_node(struct mmunode *mn)
{
}

static void mmu_free(void)
{
}
#endif

#if 0
static int getmmubank(struct mmudata *snptr, uaecptr p)
{
	return 0;
}
#endif

int mmu_init(int mode, uaecptr parm, uaecptr parm2)
{
	return 1;
}

void debug_parser (const TCHAR *cmd, TCHAR *out, uae_u32 outsize)
{
}

bool debug_opcode_watch;

bool debug_sprintf(uaecptr addr, uae_u32 val, int size)
{
	return false;
}

void debug_draw(uae_u8 *buf, int bpp, int line, int width, int height, uae_u32 *xredcolors, uae_u32 *xgreencolors, uae_u32 *xbluescolors)
{
}

void debug_trainer_match(void)
{
}

void debug_init_trainer(const TCHAR *file)
{
}

bool debug_trainer_event(int evt, int state)
{
   return false;
}

bool debug_get_prefetch(int idx, uae_u16 *opword)
{
   return false;
}



struct addrbank *get_mem_bank_real(uaecptr addr)
{
	struct addrbank *ab = &get_mem_bank(addr);
	if (!memwatch_enabled)
		return ab;
#if 0
	struct addrbank *ab2 = debug_mem_banks[addr >> 16];
	if (ab2)
		return ab2;
#endif
	return ab;
}
