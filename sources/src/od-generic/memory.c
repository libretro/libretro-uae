
/*
 * PUAE - The Un*x Amiga Emulator
 *
 * OS-specific memory support functions
 *
 * Copyright 2004 Richard Drummond
 * Copyright     -2013 Toni Wilen
 * Copyright 2010-2013 Mustafa Tufan
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "autoconf.h"
#ifndef ANDROID
#include <sys/sysctl.h>
#endif
#include "include/memory_uae.h"
#include "misc.h"

uae_u32 max_z3fastmem;

#if !defined(NATMEM_OFFSET)
void protect_roms (bool protect) {}
#else

#include "include/newcpu.h"

#define MEMORY_DEBUG 0

typedef int BOOL;
#ifdef __x86_64__
static int os_64bit = 1;
#else
static int os_64bit = 0;
#endif

#define VAMODE 1

/* JIT can access few bytes outside of memory block if it executes code at the very end of memory block */
#define BARRIER 32

#define MAXZ3MEM32 0x7F000000
#define MAXZ3MEM64 0xF0000000
#define MAX_SHMID 256

typedef void * LPVOID;
typedef size_t SIZE_T;

#define IPC_PRIVATE	0x01
#define IPC_RMID	0x02
#define IPC_CREAT	0x04
#define IPC_STAT	0x08

#define PAGE_READWRITE			0x00
#define PAGE_READONLY			0x02
#define PAGE_EXECUTE_READWRITE	0x40

#define MEM_COMMIT		0x00001000
#define MEM_RESERVE		0x00002000
#define MEM_DECOMMIT	0x00004000
#define MEM_RELEASE		0x00008000
#define MEM_WRITE_WATCH	0x00200000

/* One shmid data structure for each shared memory segment in the system. */
struct shmid_ds {
	key_t  key;
	size_t size;
	size_t rosize;
	void   *addr;
	char   name[MAX_PATH];
	void   *attached;
	int    mode;
	void   *natmembase; /* if != NULL then shmem is shared from natmem */
	bool   fake;
	int    maprom;
};

typedef struct {
	int dwPageSize;
} SYSTEM_INFO;

static struct shmid_ds shmids[MAX_SHMID];
uae_u8 *natmem_offset, *natmem_offset_end;
static uae_u8 *p96mem_offset;
static int p96mem_size;
static SYSTEM_INFO si;
int maxmem;
uae_u32 natmem_size;

#include <sys/mman.h>

struct virt_alloc_s;
typedef struct virt_alloc_s
{
	int mapping_size;
	char* address;
	struct virt_alloc_s* next;
	struct virt_alloc_s* prev;
	int state;
}virt_alloc;
static virt_alloc* vm=0;

static void GetSystemInfo(SYSTEM_INFO *si)
{
	si->dwPageSize = sysconf(_SC_PAGESIZE); //PAGE_SIZE <asm/page.h>
}

/*
 * mmap() anonymous space, depending on the system's mmap() style. On systems
 * that use the /dev/zero mapping idiom, zerofd will be set to the file descriptor
 * of the opened /dev/zero.
 */
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

static void *mmap_anon(void *addr, size_t len, int prot, int flags, off_t offset)
{
	void *result;

#if defined(MAP_SHARED) && defined(MAP_PRIVATE)
	flags = (flags & ~MAP_SHARED) | MAP_PRIVATE;
#endif

#ifdef MAP_ANONYMOUS
	/* BSD-style anonymous mapping */
	result = mmap(addr, len, prot, flags | MAP_ANONYMOUS, -1, offset);
#else
	/* SysV-style anonymous mapping */
	int fd;
	fd = open("/dev/zero", O_RDWR);
	if (fd < 0){
		write_log ( "MMAP: Cannot open /dev/zero for R+W. Error: ");
		return NULL;
	}

	result = mmap(addr, len, prot, flags, fd, offset);
	close(fd);
#endif /* MAP_ANONYMOUS */
	if (result == MAP_FAILED) {
		write_log("MMAPed failed addr: 0x%08X, %d bytes (%d MB)\n",
					addr, (uae_u32)len, (uae_u32)len / 0x100000);
	} else {
		write_log("MMAPed OK range: 0x%08X - 0x%08X, %d bytes (%d MB)\n",
					PTR_TO_UINT32(result), PTR_TO_UINT32(result) + (uae_u32)len,
					(uae_u32)len, (uae_u32)len / 0x100000);
	}
	return result;
}

static void *VirtualAlloc(LPVOID lpAddress, int dwSize, DWORD flAllocationType, DWORD flProtect) {
#if MEMORY_DEBUG > 0
	write_log ("VirtualAlloc Addr: 0x%08X, Size: %zu bytes (%d MB), Type: %x, Protect: %d\n", lpAddress, dwSize, dwSize/0x100000, flAllocationType, flProtect);
#endif
	void *memory = NULL;

	if ((flAllocationType == MEM_COMMIT && lpAddress == NULL) || flAllocationType & MEM_RESERVE) {
		memory = malloc(dwSize);
		if (memory == NULL)
			write_log ("VirtualAlloc failed errno %d\n", errno);
#if (MEMORY_DEBUG > 0) && defined(__x86_64__)
		else if ((uaecptr)memory != (0x00000000ffffffff & (uaecptr)memory))
			write_log ("VirtualAlloc allocated 64bit high mem at 0x%08x %08x\n",
						(uae_u32)((uaecptr)memory >> 32),
						(uae_u32)(0x00000000ffffffff & (uaecptr)memory));
#endif
		return memory;
	} else {
		return lpAddress;
	}

	void* answer;
	long pgsz;

	if ((flAllocationType & (MEM_RESERVE | MEM_COMMIT)) == 0) {
		return NULL;
	}

	if (flAllocationType & MEM_RESERVE && VALUE_TO_PTR(lpAddress) & 0xffff) {
		dwSize   += VALUE_TO_PTR(lpAddress) &  0xffff;
		lpAddress = (LPVOID)(VALUE_TO_PTR(lpAddress) & ~0xffff);
	}

	pgsz = sysconf(_SC_PAGESIZE);
	if ( flAllocationType & MEM_COMMIT && PTR_TO_UINT32(lpAddress) % pgsz) {
		dwSize    += PTR_TO_UINT32(lpAddress) % pgsz;
		lpAddress  = (LPVOID)(VALUE_TO_PTR(lpAddress) - (VALUE_TO_PTR(lpAddress) % pgsz));
	}

	if (flAllocationType & MEM_RESERVE && dwSize < 0x10000) {
		dwSize = 0x10000;
	}
	if (dwSize % pgsz) {
		dwSize += pgsz - dwSize % pgsz;
	}

	if (lpAddress != 0) {
		//check whether we can allow to allocate this
		virt_alloc* str = vm;
		while (str) {
			if (PTR_TO_UINT32(lpAddress) >= PTR_TO_UINT32(str->address) + str->mapping_size) {
				str = str->prev;
				continue;
			}
			if (PTR_TO_UINT32(lpAddress) + dwSize <= PTR_TO_UINT32(str->address)) {
				str = str->prev;
				continue;
			}
			if (str->state == 0) {
				//FIXME
				if (   (PTR_TO_UINT32(lpAddress)          >= PTR_TO_UINT32(str->address))
					&& (PTR_TO_UINT32(lpAddress) + dwSize <= PTR_TO_UINT32(str->address) + str->mapping_size)
					&& (flAllocationType & MEM_COMMIT)) {
						write_log ("VirtualAlloc: previously reserved memory 0x%08X\n", lpAddress);
						return lpAddress; //returning previously reserved memory
				}
				write_log ("VirtualAlloc: does not commit or not entirely within reserved, and\n");
			}
			write_log ("VirtualAlloc: (0x%08X, %u) overlaps with (0x%08X, %u, state=%d)\n", PTR_TO_UINT32(lpAddress), dwSize, PTR_TO_UINT32(str->address), str->mapping_size, str->state);
			return NULL;
		}
	}

	answer = mmap_anon(lpAddress, dwSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, 0);
	if (answer != (void *)-1 && lpAddress && answer != lpAddress) {
		/* It is dangerous to try mmap() with MAP_FIXED since it does not always detect conflicts or non-allocation and chaos ensues after
	   	   a successful call but an overlapping or non-allocated region.  */
		munmap (answer, dwSize);
		answer = (void *) -1;
		errno = EINVAL;
		write_log ("VirtualAlloc: cannot satisfy requested address\n");
	}
	if (answer == (void*)-1) {
		write_log ("VirtualAlloc: mmap(0x%08X, %u) failed with errno=%d\n", PTR_TO_UINT32(lpAddress), dwSize, errno);
		return NULL;
	} else {
		virt_alloc *new_vm = malloc(sizeof(virt_alloc));
		new_vm->mapping_size = dwSize;
		new_vm->address = (char*)answer;
        new_vm->prev = vm;
		if (flAllocationType == MEM_RESERVE)
			new_vm->state = 0;
		else
			new_vm->state = 1;
		if (vm)
			vm->next = new_vm;
		vm = new_vm;
		vm->next = 0;
		write_log ("VirtualAlloc: provides %u bytes starting at 0x%08X\n", dwSize, PTR_TO_UINT32(answer));
		return answer;
	}
}

static bool VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) {
	return true;
	virt_alloc* str = vm;
	int answer;

#if MEMORY_DEBUG > 0
	write_log ("VirtualFree: Addr: 0x%08X, Size: %d bytes (%d MB), Type: 0x%08X)\n", PTR_TO_UINT32(lpAddress), dwSize, dwSize/0x100000, dwFreeType);
#endif

	while (str) {
		if (lpAddress != str->address) {
			str = str->prev;
			continue;
		}

		answer = munmap(str->address, str->mapping_size);
		if (str->next) str->next->prev = str->prev;
		if (str->prev) str->prev->next = str->next;
		if (vm == str) vm = str->prev;
		write_log ("VirtualFree: failed munmap(0x%08X, %d)\n", PTR_TO_UINT32(str->address), str->mapping_size);
		xfree (str);
		return 0;
	}
	write_log ("VirtualFree: ok\n");
	return -1;
}

static uae_u8 *virtualallocwithlock (LPVOID addr, SIZE_T size, DWORD allocationtype, DWORD protect)
{
	uae_u8 *p = (uae_u8*)VirtualAlloc (addr, size, allocationtype, protect);
	return p;
}

static void virtualfreewithlock (LPVOID addr, SIZE_T size, DWORD freetype)
{
	VirtualFree(addr, size, freetype);
}

void cache_free (uae_u8 *cache)
{
	virtualfreewithlock (cache, 0, MEM_RELEASE);
}

uae_u8 *cache_alloc (int size)
{
	size = size < sysconf(_SC_PAGESIZE) ? sysconf(_SC_PAGESIZE) : size;

	void *cache = mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!cache) {
		write_log ("Cache_Alloc of %d failed. ERR=%d\n", size, errno);
	}
	return (uae_u8 *) cache;

//fixme:	return virtualallocwithlock (NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

static uae_u32 lowmem (void)
{
	uae_u32 change = 0;
	if (currprefs.z3fastmem_size + currprefs.z3fastmem2_size + currprefs.z3chipmem_size >= 8 * 1024 * 1024) {
		if (currprefs.z3fastmem2_size) {
			change = currprefs.z3fastmem2_size;
			currprefs.z3fastmem2_size = 0;
		} else if (currprefs.z3chipmem_size) {
			if (currprefs.z3chipmem_size <= 16 * 1024 * 1024) {
				change = currprefs.z3chipmem_size;
				currprefs.z3chipmem_size = 0;
			} else {
				change = currprefs.z3chipmem_size / 2;
				currprefs.z3chipmem_size /= 2;
			}
		} else {
			change = currprefs.z3fastmem_size - currprefs.z3fastmem_size / 4;
			currprefs.z3fastmem2_size = changed_prefs.z3fastmem2_size = currprefs.z3fastmem_size / 4;
			currprefs.z3fastmem_size /= 2;
			changed_prefs.z3fastmem_size = currprefs.z3fastmem_size;
		}
	} else if (currprefs.rtgmem_type && currprefs.rtgmem_size >= 1 * 1024 * 1024) {
		change = currprefs.rtgmem_size - currprefs.rtgmem_size / 2;
		currprefs.rtgmem_size /= 2;
		changed_prefs.rtgmem_size = currprefs.rtgmem_size;
	}
	if (currprefs.z3fastmem2_size < 128 * 1024 * 1024)
		currprefs.z3fastmem2_size = changed_prefs.z3fastmem2_size = 0;
	return change;
}

/*int mman_GetWriteWatch (PVOID lpBaseAddress, SIZE_T dwRegionSize, PVOID *lpAddresses, PULONG_PTR lpdwCount, PULONG lpdwGranularity)
{
	return GetWriteWatch (0, lpBaseAddress, dwRegionSize, lpAddresses, lpdwCount, lpdwGranularity);
}
void mman_ResetWatch (PVOID lpBaseAddress, SIZE_T dwRegionSize)
{
	if (ResetWriteWatch (lpBaseAddress, dwRegionSize))
		write_log (_T("ResetWriteWatch() failed, %d\n"), GetLastError ());
}*/

static uae_u64 size64;

static void clear_shm (void)
{
	shm_start = NULL;
	for (int i = 0; i < MAX_SHMID; i++) {
		memset (&shmids[i], 0, sizeof (struct shmid_ds));
		shmids[i].key = -1;
	}
}

bool preinit_shm (void)
{
	uae_u64 total64;
	uae_u64 totalphys64;
	uae_u32 max_allowed_mman;

	if (natmem_offset)
		VirtualFree (natmem_offset, 0, MEM_RELEASE);
	natmem_offset = NULL;
	if (p96mem_offset)
		VirtualFree (p96mem_offset, 0, MEM_RELEASE);
	p96mem_offset = NULL;

	GetSystemInfo (&si);
	max_allowed_mman = 512 + 256;
	if (os_64bit) {
		max_allowed_mman = 2048;
	}

#ifdef __APPLE__
//xaind
	int mib[2];
	size_t len;

	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE; /* gives a 64 bit int */
	len = sizeof(totalphys64);
	sysctl(mib, 2, &totalphys64, &len, NULL, 0);
	total64 = (uae_u64) totalphys64;
#else
	totalphys64 = sysconf (_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
	total64 = (uae_u64)sysconf (_SC_PHYS_PAGES) * (uae_u64)sysconf(_SC_PAGESIZE);
#endif
	size64 = total64;
	if (os_64bit) {
		if (size64 > MAXZ3MEM64)
			size64 = MAXZ3MEM64;
	} else {
		if (size64 > MAXZ3MEM32)
			size64 = MAXZ3MEM32;
	}
	if (maxmem < 0)
		size64 = MAXZ3MEM64;
	else if (maxmem > 0)
		size64 = maxmem * 1024 * 1024;
	if (size64 < 8 * 1024 * 1024)
		size64 = 8 * 1024 * 1024;
	if (max_allowed_mman * 1024 * 1024 > size64)
		max_allowed_mman = size64 / (1024 * 1024);
	if (!os_64bit) {
		if (max_allowed_mman * 1024 * 1024 > (totalphys64 / 2))
			max_allowed_mman = (totalphys64 / 2) / (1024 * 1024);
	}

	natmem_size = (max_allowed_mman + 1) * 1024 * 1024;
	if (natmem_size < 17 * 1024 * 1024)
		natmem_size = 17 * 1024 * 1024;

	write_log (_T("Total physical RAM %lluM, all RAM %lluM. Attempting to reserve: %uM.\n"), totalphys64 >> 20, total64 >> 20, natmem_size >> 20);
	natmem_offset = 0;
	if (natmem_size <= 768 * 1024 * 1024) {
		uae_u32 p = 0x78000000 - natmem_size;
		for (;;) {
			natmem_offset = (uae_u8*)VirtualAlloc ((void*)VALUE_TO_PTR(p), natmem_size, MEM_RESERVE, PAGE_READWRITE);
//			natmem_offset = (uae_u8*)VirtualAlloc ((void*)p, natmem_size, MEM_RESERVE | (VAMODE == 1 ? MEM_WRITE_WATCH : 0), PAGE_READWRITE);
			if (natmem_offset)
				break;
			p -= 128 * 1024 * 1024;
			if (p <= 128 * 1024 * 1024)
				break;
		}
	}
	if (!natmem_offset) {
		for (;;) {
			natmem_offset = (uae_u8*)VirtualAlloc (NULL, natmem_size, MEM_RESERVE, PAGE_READWRITE);
//			natmem_offset = (uae_u8*)VirtualAlloc (NULL, natmem_size, MEM_RESERVE | (VAMODE == 1 ? MEM_WRITE_WATCH : 0), PAGE_READWRITE);
			if (natmem_offset)
				break;
			natmem_size -= 128 * 1024 * 1024;
			if (!natmem_size) {
				write_log (_T("Can't allocate 257M of virtual address space!?\n"));
				natmem_size = 17 * 1024 * 1024;
				natmem_offset = (uae_u8*)VirtualAlloc (NULL, natmem_size, MEM_RESERVE, PAGE_READWRITE);
//				natmem_offset = (uae_u8*)VirtualAlloc (NULL, natmem_size, MEM_RESERVE | (VAMODE == 1 ? MEM_WRITE_WATCH : 0), PAGE_READWRITE);
				if (!natmem_size) {
					write_log (_T("Can't allocate 17M of virtual address space!? Something is seriously wrong\n"));
					return false;
				}
				break;
			}
		}
	}
	if (natmem_size <= 257 * 1024 * 1024)
		max_z3fastmem = 0;
	else
		max_z3fastmem = natmem_size;

	write_log (_T("Reserved: 0x%08X-0x%08X (%08x %dM)\n"),
		natmem_offset, (uae_u8*)natmem_offset + natmem_size,
		natmem_size, natmem_size >> 20);

	clear_shm ();

//	write_log (_T("Max Z3FastRAM %dM. Total physical RAM %uM\n"), max_z3fastmem >> 20, totalphys64 >> 20);

	canbang = 1;
	return true;
}

static void resetmem (bool decommit)
{
	int i;

	if (!shm_start)
		return;
	for (i = 0; i < MAX_SHMID; i++) {
		struct shmid_ds *s = &shmids[i];
		int size = s->size;
		uae_u8 *shmaddr;
		uae_u8 *result;

		if (!s->attached)
			continue;
		if (!s->natmembase)
			continue;
		if (s->fake)
			continue;
		if (!decommit && ((uae_u8*)s->attached - (uae_u8*)s->natmembase) >= 0x10000000)
			continue;
		shmaddr = natmem_offset + ((uae_u8*)s->attached - (uae_u8*)s->natmembase);
		if (decommit) {
			VirtualFree (shmaddr, size, MEM_DECOMMIT);
		} else {
			result = virtualallocwithlock (shmaddr, size, decommit ? MEM_DECOMMIT : MEM_COMMIT, PAGE_READWRITE);
			if (result != shmaddr)
				write_log (_T("NATMEM: realloc(%p-%p,%d,%d,%s) failed, err=%d\n"), shmaddr, shmaddr + size, size, s->mode, s->name, GetLastError ());
			else
				write_log (_T("NATMEM: rellocated(%p-%p,%d,%s)\n"), shmaddr, shmaddr + size, size, s->name);
		}
	}
}

static ULONG getz2rtgaddr (void)
{
	ULONG start;
	start = changed_prefs.fastmem_size;
	while (start & (changed_prefs.rtgmem_size - 1) && start < 4 * 1024 * 1024)
		start += 1024 * 1024;
	return start + 2 * 1024 * 1024;
}

static uae_u8 *va (uae_u32 offset, uae_u32 len, DWORD alloc, DWORD protect)
{
	uae_u8 *addr;

	addr = (uae_u8*)VirtualAlloc (natmem_offset + offset, len, alloc, protect);
	if (addr) {
		write_log (_T("VA(%p - %p, %4uM, %s)\n"),
			natmem_offset + offset, natmem_offset + offset + len, len >> 20, (alloc & MEM_WRITE_WATCH) ? _T("WATCH") : _T("RESERVED"));
		return addr;
	}
	write_log (_T("VA(%p - %p, %4uM, %s) failed %d\n"),
		natmem_offset + offset, natmem_offset + offset + len, len >> 20, (alloc & MEM_WRITE_WATCH) ? _T("WATCH") : _T("RESERVED"), GetLastError ());
	return NULL;
}

static int doinit_shm (void)
{
	uae_u32 size, totalsize, z3size, natmemsize;
	uae_u32 rtgbarrier, z3chipbarrier, rtgextra;
	int rounds = 0;
	ULONG z3rtgmem_size;

	for (;;) {
		int lowround = 0;
		uae_u8 *blah = NULL;
		if (rounds > 0)
			write_log (_T("NATMEM: retrying %d..\n"), rounds);
		rounds++;

		z3size = 0;
		size = 0x1000000;
		rtgextra = 0;
		z3chipbarrier = 0;
		rtgbarrier = si.dwPageSize;
		z3rtgmem_size = changed_prefs.rtgmem_type ? changed_prefs.rtgmem_size : 0;
		if (changed_prefs.cpu_model >= 68020)
			size = 0x10000000;
		if (changed_prefs.z3fastmem_size || changed_prefs.z3fastmem2_size || changed_prefs.z3chipmem_size) {
			z3size = changed_prefs.z3fastmem_size + changed_prefs.z3fastmem2_size + changed_prefs.z3chipmem_size + (changed_prefs.z3fastmem_start - 0x10000000);
			if (z3rtgmem_size) {
				rtgbarrier = 16 * 1024 * 1024 - ((changed_prefs.z3fastmem_size + changed_prefs.z3fastmem2_size) & 0x00ffffff);
			}
			if (changed_prefs.z3chipmem_size && (changed_prefs.z3fastmem_size || changed_prefs.z3fastmem2_size))
				z3chipbarrier = 16 * 1024 * 1024;
		} else {
			rtgbarrier = 0;
		}
		totalsize = size + z3size + z3rtgmem_size;
		while (totalsize > size64) {
			int change = lowmem ();
			if (!change)
				return 0;
			write_log (_T("NATMEM: %d, %dM > %dM = %dM\n"), ++lowround, totalsize >> 20, size64 >> 20, (totalsize - change) >> 20);
			totalsize -= change;
		}
		if ((rounds > 1 && totalsize < 0x10000000) || rounds > 20) {
			write_log (_T("NATMEM: No special area could be allocated (3)!\n"));
			return 0;
		}
		natmemsize = size + z3size;

		if (z3rtgmem_size) {
			rtgextra = si.dwPageSize;
		} else {
			rtgbarrier = 0;
			rtgextra = 0;
		}
		if (natmemsize + rtgbarrier + z3chipbarrier + z3rtgmem_size + rtgextra + 16 * si.dwPageSize <= natmem_size)
			break;
		write_log (_T("NATMEM: %dM area failed to allocate, err=%d (Z3=%dM,RTG=%dM)\n"),
			natmemsize >> 20, GetLastError (), (changed_prefs.z3fastmem_size + changed_prefs.z3fastmem2_size + changed_prefs.z3chipmem_size) >> 20, z3rtgmem_size >> 20);
		if (!lowmem ()) {
			write_log (_T("NATMEM: No special area could be allocated (2)!\n"));
			return 0;
		}
	}

#if VAMODE == 1

	p96mem_offset = NULL;
	p96mem_size = z3rtgmem_size;
	if (changed_prefs.rtgmem_size && changed_prefs.rtgmem_type) {
		p96mem_offset = natmem_offset + natmemsize + rtgbarrier + z3chipbarrier;
	} else if (changed_prefs.rtgmem_size && !changed_prefs.rtgmem_type) {
		p96mem_offset = natmem_offset + getz2rtgaddr ();
	}

#else

	if (p96mem_offset)
		VirtualFree (p96mem_offset, 0, MEM_RELEASE);
	p96mem_offset = NULL;
	p96mem_size = z3rtgmem_size;
	if (changed_prefs.rtgmem_size && changed_prefs.rtgmem_type) {
		uae_u32 s, l;
		VirtualFree (natmem_offset, 0, MEM_RELEASE);

		s = 0;
		l = natmemsize + rtgbarrier + z3chipbarrier;
		if (!va (s, l, MEM_RESERVE, PAGE_READWRITE))
			return 0;

		s = natmemsize + rtgbarrier + z3chipbarrier;
		l = p96mem_size + rtgextra;
		p96mem_offset = va (s, l, MEM_RESERVE | MEM_WRITE_WATCH, PAGE_READWRITE);
		if (!p96mem_offset) {
			currprefs.rtgmem_size = changed_prefs.rtgmem_size = 0;
			z3rtgmem_size = 0;
			write_log (_T("NATMEM: failed to allocate special Picasso96 GFX RAM, err=%d\n"), GetLastError ());
		}

#if 0
		s = natmemsize + rtgbarrier + z3chipbarrier + p96mem_size + rtgextra + 4096;
		l = natmem_size - s - 4096;
		if (natmem_size > l) {
			if (!va (s, l, 	MEM_RESERVE, PAGE_READWRITE))
				return 0;
		}
#endif

	} else if (changed_prefs.rtgmem_size && !changed_prefs.rtgmem_type) {

		uae_u32 s, l;
		VirtualFree (natmem_offset, 0, MEM_RELEASE);
		// Chip + Z2Fast
		s = 0;
		l = 2 * 1024 * 1024 + changed_prefs.fastmem_size;
		if (!va (s, l, MEM_RESERVE, PAGE_READWRITE)) {
			currprefs.rtgmem_size = changed_prefs.rtgmem_size = 0;
		}
		// After RTG
		s = 2 * 1024 * 1024 + 8 * 1024 * 1024;
		l = natmem_size - (2 * 1024 * 1024 + 8 * 1024 * 1024) + si.dwPageSize;
		if (!va (s, l, MEM_RESERVE, PAGE_READWRITE)) {
			currprefs.rtgmem_size = changed_prefs.rtgmem_size = 0;
		}
		// RTG
		s = getz2rtgaddr ();
		l = 10 * 1024 * 1024 - getz2rtgaddr ();
		p96mem_offset = va (s, l, MEM_RESERVE | MEM_WRITE_WATCH, PAGE_READWRITE);
		if (!p96mem_offset) {
			currprefs.rtgmem_size = changed_prefs.rtgmem_size = 0;
		}

	} else {

		VirtualFree (natmem_offset, 0, MEM_RELEASE);
		if (!VirtualAlloc (natmem_offset, natmem_size, MEM_RESERVE, PAGE_READWRITE)) {
			write_log (_T("NATMEM: No special area could be reallocated! (1) err=%d\n"), GetLastError ());
			return 0;
		}
	}
#endif
	if (!natmem_offset) {
		write_log (_T("NATMEM: No special area could be allocated! err=%d\n"), GetLastError ());
	} else {
		write_log (_T("NATMEM: Our special area: 0x%08X-0x%08X (%08x %dM)\n"),
			natmem_offset, (uae_u8*)natmem_offset + natmemsize,
			natmemsize, natmemsize >> 20);
		if (changed_prefs.rtgmem_size)
			write_log (_T("NATMEM: P96 special area: 0x%08X-0x%08X (%08x %dM)\n"),
			p96mem_offset, (uae_u8*)p96mem_offset + changed_prefs.rtgmem_size,
			changed_prefs.rtgmem_size, changed_prefs.rtgmem_size >> 20);
		canbang = 1;
		if (p96mem_size)
			natmem_offset_end = p96mem_offset + p96mem_size;
		else
			natmem_offset_end = natmem_offset + natmemsize;
	}

	return canbang;
}

bool init_shm (void)
{
	static uae_u32 oz3fastmem_size, oz3fastmem2_size;
	static uae_u32 oz3chipmem_size;
	static uae_u32 ortgmem_size;
	static int ortgmem_type;

	if ( (oz3fastmem_size  == changed_prefs.z3fastmem_size)
	  && (oz3fastmem2_size == changed_prefs.z3fastmem2_size)
	  && (oz3chipmem_size  == changed_prefs.z3chipmem_size)
	  && (ortgmem_size     == changed_prefs.rtgmem_size)
	  && (ortgmem_type     == changed_prefs.rtgmem_type) )
		return false;

	oz3fastmem_size = changed_prefs.z3fastmem_size;
	oz3fastmem2_size = changed_prefs.z3fastmem2_size;
	oz3chipmem_size = changed_prefs.z3chipmem_size;;
	ortgmem_size = changed_prefs.rtgmem_size;
	ortgmem_type = changed_prefs.rtgmem_type;

	doinit_shm ();

	resetmem (false);
	clear_shm ();

	memory_hardreset (2);
	return true;
}

void free_shm (void)
{
	resetmem (true);
	clear_shm ();
}

void mapped_free (uae_u8 *mem)
{
	shmpiece *x = shm_start;

	if (mem == filesysory) {
		while(x) {
			if (mem == x->native_address) {
				int shmid = x->id;
				shmids[shmid].key = -1;
				shmids[shmid].name[0] = '\0';
				shmids[shmid].size = 0;
				shmids[shmid].attached = 0;
				shmids[shmid].mode = 0;
				shmids[shmid].natmembase = 0;
			}
			x = x->next;
		}
		return;
	}

	while(x) {
		if(mem == x->native_address)
			my_shmdt (x->native_address);
		x = x->next;
	}
	x = shm_start;
	while(x) {
		struct shmid_ds blah;
		if (mem == x->native_address) {
			if (my_shmctl (x->id, IPC_STAT, &blah) == 0)
				my_shmctl (x->id, IPC_RMID, &blah);
		}
		x = x->next;
	}
}

static key_t get_next_shmkey (void)
{
	key_t result = -1;
	int i;
	for (i = 0; i < MAX_SHMID; i++) {
		if (shmids[i].key == -1) {
			shmids[i].key = i;
			result = i;
			break;
		}
	}
	return result;
}

STATIC_INLINE key_t find_shmkey (key_t key)
{
	int result = -1;
	if(shmids[key].key == key) {
		result = key;
	}
	return result;
}

int mprotect (void *addr, size_t len, int prot)
{
	int result = 0;
	return result;
}

void *my_shmat (int shmid, void *shmaddr, int shmflg)
{
	void *result = (void *)-1;
	bool got = false, readonly = false, maprom = false;
	int p96special = false;

	unsigned int size = shmids[shmid].size;
	unsigned int readonlysize = size;

	if (shmids[shmid].attached)
		return shmids[shmid].attached;

	if ((uae_u8*)shmaddr < natmem_offset) {
		if(!_tcscmp (shmids[shmid].name, _T("chip"))) {
			shmaddr=natmem_offset;
			got = true;
			if (getz2endaddr () <= 2 * 1024 * 1024 || currprefs.chipmem_size < 2 * 1024 * 1024)
				size += BARRIER;
		} else if(!_tcscmp (shmids[shmid].name, _T("kick"))) {
			shmaddr=natmem_offset + 0xf80000;
			got = true;
			size += BARRIER;
			readonly = true;
			maprom = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("rom_a8"))) {
			shmaddr=natmem_offset + 0xa80000;
			got = true;
			readonly = true;
			maprom = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("rom_e0"))) {
			shmaddr=natmem_offset + 0xe00000;
			got = true;
			readonly = true;
			maprom = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("rom_f0"))) {
			shmaddr=natmem_offset + 0xf00000;
			got = true;
			readonly = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("rtarea"))) {
			shmaddr=natmem_offset + rtarea_base;
			got = true;
			readonly = true;
			readonlysize = RTAREA_TRAPS;
		} else if(!_tcscmp (shmids[shmid].name, _T("fast"))) {
			shmaddr=natmem_offset + 0x200000;
			got = true;
			if (!(currprefs.rtgmem_size && !currprefs.rtgmem_type))
				size += BARRIER;
		} else if(!_tcscmp (shmids[shmid].name, _T("z2_gfx"))) {
			ULONG start = getz2rtgaddr ();
			got = true;
			p96special = true;
			shmaddr = natmem_offset + start;
			p96ram_start = start;
			if (start + currprefs.rtgmem_size < 10 * 1024 * 1024)
				size += BARRIER;
		} else if(!_tcscmp (shmids[shmid].name, _T("ramsey_low"))) {
			shmaddr=natmem_offset + a3000lmem_start;
			if (!a3000hmem_start)
				size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("ramsey_high"))) {
			shmaddr=natmem_offset + a3000hmem_start;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("z3"))) {
			shmaddr=natmem_offset + z3fastmem_start;
			if (!currprefs.z3fastmem2_size)
				size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("z3_2"))) {
			shmaddr=natmem_offset + z3fastmem_start + currprefs.z3fastmem_size;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("z3_chip"))) {
			shmaddr=natmem_offset + z3chipmem_start;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("z3_gfx"))) {
			got = true;
			p96special = true;
			p96ram_start = p96mem_offset - natmem_offset;
			shmaddr = natmem_offset + p96ram_start;
			size += BARRIER;
		} else if(!_tcscmp (shmids[shmid].name, _T("bogo"))) {
			shmaddr=natmem_offset+0x00C00000;
			got = true;
			if (currprefs.bogomem_size <= 0x100000)
				size += BARRIER;
		} else if(!_tcscmp (shmids[shmid].name, _T("filesys"))) {
			static uae_u8 *filesysptr;
			if (filesysptr == NULL)
				filesysptr = xcalloc (uae_u8, size);
			result = filesysptr;
			shmids[shmid].attached = result;
			shmids[shmid].fake = true;
			return result;
		} else if(!_tcscmp (shmids[shmid].name, _T("custmem1"))) {
			shmaddr=natmem_offset + currprefs.custom_memory_addrs[0];
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("custmem2"))) {
			shmaddr=natmem_offset + currprefs.custom_memory_addrs[1];
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("hrtmem"))) {
			shmaddr=natmem_offset + 0x00a10000;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("arhrtmon"))) {
			shmaddr=natmem_offset + 0x00800000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("xpower_e2"))) {
			shmaddr=natmem_offset + 0x00e20000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("xpower_f2"))) {
			shmaddr=natmem_offset + 0x00f20000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("nordic_f0"))) {
			shmaddr=natmem_offset + 0x00f00000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("nordic_f4"))) {
			shmaddr=natmem_offset + 0x00f40000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("nordic_f6"))) {
			shmaddr=natmem_offset + 0x00f60000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp(shmids[shmid].name, _T("superiv_b0"))) {
			shmaddr=natmem_offset + 0x00b00000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("superiv_d0"))) {
			shmaddr=natmem_offset + 0x00d00000;
			size += BARRIER;
			got = true;
		} else if(!_tcscmp (shmids[shmid].name, _T("superiv_e0"))) {
			shmaddr=natmem_offset + 0x00e00000;
			size += BARRIER;
			got = true;
		}
	}

	if (shmids[shmid].key == shmid && shmids[shmid].size) {
		DWORD protect = readonly ? PAGE_READONLY : PAGE_READWRITE;
		shmids[shmid].mode = protect;
		shmids[shmid].rosize = readonlysize;
		shmids[shmid].natmembase = natmem_offset;
		shmids[shmid].maprom = maprom ? 1 : 0;
		if (shmaddr)
			virtualfreewithlock (shmaddr, size, MEM_DECOMMIT);
		result = virtualallocwithlock (shmaddr, size, MEM_COMMIT, PAGE_READWRITE);
		if (result == NULL)
			virtualfreewithlock (shmaddr, 0, MEM_DECOMMIT);
		result = virtualallocwithlock (shmaddr, size, MEM_COMMIT, PAGE_READWRITE);
		if (result == NULL) {
			result = (void*)-1;
			write_log (_T("VA %08X - %08X %x (%dk) failed %d\n"),
				(uae_u8*)shmaddr - natmem_offset, (uae_u8*)shmaddr - natmem_offset + size,
				size, size >> 10, GetLastError ());
		} else {
			shmids[shmid].attached = result;
			write_log (_T("VA %08X - %08X %x (%dk) ok (%08X)%s\n"),
				(uae_u8*)shmaddr - natmem_offset, (uae_u8*)shmaddr - natmem_offset + size,
				size, size >> 10, shmaddr, p96special ? _T(" P96") : _T(""));
		}
	}
	return result;
}

void unprotect_maprom (void)
{
	bool protect = false;
	for (int i = 0; i < MAX_SHMID; i++) {
		DWORD old;
		struct shmid_ds *shm = &shmids[i];
		if (shm->mode != PAGE_READONLY)
			continue;
		if (!shm->attached || !shm->rosize)
			continue;
		if (shm->maprom <= 0)
			continue;
		shm->maprom = -1;
	}
}

void protect_roms (bool protect)
{
	if (protect) {
		// protect only if JIT enabled, always allow unprotect
		if (!currprefs.cachesize || currprefs.comptrustbyte || currprefs.comptrustword || currprefs.comptrustlong)
			return;
	}
	struct shmid_ds *shm;
	for (int i = 0; i < MAX_SHMID; i++) {
		DWORD old;
		shm = &shmids[i];
		if (shm->mode != PAGE_READONLY)
			continue;
		if (!shm->attached || !shm->rosize)
			continue;
		if (shm->maprom < 0 && protect)
			continue;
	}
}

int my_shmdt (const void *shmaddr)
{
	return 0;
}

int my_shmget (key_t key, size_t size, int shmflg, const TCHAR *name)
{
	int result = -1;

	if((key == IPC_PRIVATE) || ((shmflg & IPC_CREAT) && (find_shmkey (key) == -1))) {
		write_log (_T("shmget of size %d (%dk) for %s\n"), size, size >> 10, name);
		if ((result = get_next_shmkey ()) != -1) {
			shmids[result].size = size;
			_tcscpy (shmids[result].name, name);
		} else {
			result = -1;
		}
	}
	return result;
}

int my_shmctl (int shmid, int cmd, struct shmid_ds *buf)
{
	int result = -1;

	if ((find_shmkey (shmid) != -1) && buf) {
		switch (cmd)
		{
		case IPC_STAT:
			*buf = shmids[shmid];
			result = 0;
			break;
		case IPC_RMID:
			VirtualFree (shmids[shmid].attached, shmids[shmid].size, MEM_DECOMMIT);
			shmids[shmid].key = -1;
			shmids[shmid].name[0] = '\0';
			shmids[shmid].size = 0;
			shmids[shmid].attached = 0;
			shmids[shmid].mode = 0;
			result = 0;
			break;
		}
	}
	return result;
}

#endif //NATMEM_OFFSET
