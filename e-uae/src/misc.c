/*
 * A collection of ugly and random junk brought in from Win32
 * which desparately needs to be tidied up
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "mmemory.h"
#include "custom.h"
#include "newcpu.h"
#include "events.h"
#include "uae.h"
#include "autoconf.h"
#include "traps.h"
#include "enforcer.h"
#include "picasso96.h"

static uae_u32 REGPARAM2 misc_demux (TrapContext *context)
{
//use the extern int (6 #13)
// d0 0=opensound      d1=unit d2=samplerate d3=blksize ret: sound frequency
// d0 1=closesound     d1=unit
// d0 2=writesamples   d1=unit a0=addr      write blksize samples to card
// d0 3=readsamples    d1=unit a0=addr      read samples from card ret: d0=samples read
      // make sure you have from amigaside blksize*4 mem alloced
      // d0=-1 no data available d0=-2 no recording open
          // d0 > 0 there are more blksize Data in the que
          // do the loop until d0 get 0
          // if d0 is greater than 200 bring a message
          // that show the user that data is lost
          // maximum blocksbuffered are 250 (8,5 sec)
// d0 4=writeinterrupt d1=unit  d0=0 no interrupt happen for this unit
          // d0=-2 no playing open

        //note units for now not support use only unit 0

// d0=10 get clipboard size      d0=size in bytes
// d0=11 get clipboard data      a0=clipboarddata
                                  //Note: a get clipboard size must do before
// d0=12 write clipboard data    a0=clipboarddata
// d0=13 setp96mouserate         d1=hz value
// d0=100 open dll               d1=dll name in windows name conventions
// d0=101 get dll function addr  d1=dllhandle a0 function/var name
// d0=102 exec dllcode           a0=addr of function (see 101)
// d0=103 close dll
// d0=104 screenlost
// d0=105 mem offset
// d0=106 16Bit byteswap
// d0=107 32Bit byteswap
// d0=108 free swap array
// d0=200 ahitweak               d1=offset for dsound position pointer

    int opcode = m68k_dreg (&context->regs, 0);

    switch (opcode) {
        int i, slen, t, todo, byte1, byte2;
        uae_u32 src, num_vars;
        static int cap_pos, clipsize;
#if 0
        LPTSTR p, p2, pos1, pos2;
        static  LPTSTR clipdat;
#endif
        int cur_pos;

/*
 * AHI emulation support
 */
#ifdef AHI
	case 0:
	    cap_pos = 0;
	    sound_freq_ahi = m68k_dreg (&context->regs, 2);
	    amigablksize = m68k_dreg (&context->regs, 3);
	    sound_freq_ahi = ahi_open_sound();
	    uaevar.changenum--;
	    return sound_freq_ahi;
	case 1:
	    ahi_close_sound();
	    sound_freq_ahi = 0;
	    return 0;
	case 2:
	    addr=(char *)m68k_areg (&context->regs, 0);
	    for (i = 0; i < (amigablksize*4); i += 4) {
		ahisndbufpt[0] = get_long((unsigned int)addr + i);
		ahisndbufpt+=1;
		/*ahisndbufpt[0]=chipmem_bget((unsigned int)addr+i+2);
		  ahisndbufpt+=1;
		  ahisndbufpt[0]=chipmem_bget((unsigned int)addr+i+1);
		  ahisndbufpt+=1;
		  ahisndbufpt[0]=chipmem_bget((unsigned int)addr+i);
		  ahisndbufpt+=1;*/
	    }
	    ahi_finish_sound_buffer();
	    return amigablksize;
	case 3:
	    if (norec)
		return -1;
	    if (!ahi_on)
		return -2;
	    i = IDirectSoundCaptureBuffer_GetCurrentPosition (lpDSB2r, &t, &cur_pos);
	    t = amigablksize*4;

	    if (cap_pos <= cur_pos)
		todo = cur_pos - cap_pos;
	    else
		todo = cur_pos + (RECORDBUFFER * t) - cap_pos;
	    if (todo < t) {
	        //if no complete buffer ready exit
		return -1;
	    }
	    i = IDirectSoundCaptureBuffer_Lock (lpDSB2r, cap_pos, t, &pos1, &byte1, &pos2, &byte2, 0);

	    if ((cap_pos + t) < (t * RECORDBUFFER)) {
		cap_pos=cap_pos+t;
	    } else {
		cap_pos = 0;
	    }
	    addr= (char *) m68k_areg (&context->regs, 0);
	    sndbufrecpt= (unsigned int*) pos1;
	    t = t / 4;
	    for (i=0; i < t; i++) {
		put_long ((uae_u32) addr, sndbufrecpt[0]);
		addr += 4;
		sndbufrecpt += 1;
	    }
	    t = t * 4;
	    i = IDirectSoundCaptureBuffer_Unlock (lpDSB2r, pos1, byte1, pos2, byte2);
	    return (todo - t) / t;
	case 4:
	    if (!ahi_on)
		return -2;
	    i = intcount;
	    intcount = 0;
	    return i;
	case 5:
	    if (!ahi_on)
		return 0;
	    ahi_updatesound (1);
	    return 1;
#endif

#if 0
/*
 * Support for clipboard hack
 */
	case 10:
	    i = OpenClipboard (0);
	    clipdat = GetClipboardData (CF_TEXT);
	    if (clipdat) {
		clipsize=strlen(clipdat);
		clipsize++;
		return clipsize;
	    } else {
		return 0;
	    }
	case 11:
	    addr = (char *) m68k_areg (&context->regs, 0);
	    for (i=0; i < clipsize; i++) {
		put_byte ((uae_u32) addr, clipdat[0]);
		addr++;
		clipdat++;
	    }
	    CloseClipboard ();
	    return 0;
	case 12:
	    addr = (char *) m68k_areg (&context->regs, 0);
	    addr = (char *) get_real_address ((uae_u32)addr);
	    i = OpenClipboard (0);
	    EmptyClipboard ();
	    slen = strlen (addr);
	    p = GlobalAlloc (GMEM_DDESHARE, slen + 2);
	    p2 = GlobalLock (p);
	    memcpy (p2, addr, slen);
	    p2[slen] = 0;
	    GlobalUnlock (p);
	    i = (int) SetClipboardData (CF_TEXT, p2);
	    CloseClipboard ();
	    GlobalFree (p);
	    return 0;
#endif

/*
 * Hack for higher P96 mouse draw rate
 */
#ifdef PICASSO96
	case 13: {
	    extern int p96hack_vpos2;
	    extern int hack_vpos;
	    extern int p96refresh_active;
	    extern uae_u16 vtotal;
	    extern unsigned int new_beamcon0;
	    p96hack_vpos2 = 15625 / m68k_dreg (&context->regs, 1);
	    p96refresh_active = 1;
	    if (!picasso_on)
		return 0;
	    vtotal = p96hack_vpos2; // only do below if P96 screen is visible
	    new_beamcon0 |= 0x80;
	    hack_vpos = vtotal;
	    return 0;
	}
#endif

/*
 * Support for enforcer emulation
 */
#ifdef ENFORCER
	case 20:
	    return enforcer_enable ();

	case 21:
	    return enforcer_disable ();
#endif

#if 0
	case 25:
	    flushprinter ();
	    return 0;
#endif


#if 0
	case 100: {	// open dll
	    char *dllname;
	    uae_u32 result;
	    dllname = (char *) m68k_areg (&context->regs, 0);
	    dllname = (char *) get_real_address ((uae_u32)dllname);
	    result = (uae_u32) LoadLibrary (dllname);
	    write_log ("%s windows dll/alib loaded at %d (0 mean failure)\n", dllname, result);
	    return result;
	}
	case 101: {	//get dll label
	    HMODULE m;
	    char *funcname;
	    m = (HMODULE) m68k_dreg (&context->regs, 1);
	    funcname = (char *) m68k_areg (&context->regs, 0);
	    funcname = (char *) get_real_address ((uae_u32)funcname);
	    return (uae_u32) GetProcAddress (m, funcname);
	}
	case 102:	//execute native code
	    return emulib_ExecuteNativeCode2 ();

	case 103: {	//close dll
	    HMODULE libaddr;
	    libaddr = (HMODULE) m68k_dreg (&context->regs, 1);
	    FreeLibrary (libaddr);
	    return 0;
	}
	case 104: {	//screenlost
	    static int oldnum=0;
	    if (uaevar.changenum == oldnum)
		return 0;
	    oldnum = uaevar.changenum;
	    return 1;
	}
        case 105:	//returns memory offset
	    return (uae_u32) get_real_address (0);
	case 106:	//byteswap 16bit vars
			//a0 = start address
			//d1 = number of 16bit vars
			//returns address of new array
	    src = m68k_areg (&context->regs, 0);
	    num_vars = m68k_dreg (&context->regs, 1);

	    if (bswap_buffer_size < num_vars * 2) {
		bswap_buffer_size = (num_vars + 1024) * 2;
		free (bswap_buffer);
		bswap_buffer = (void*) malloc (bswap_buffer_size);
	    }
	    __asm {
			mov esi, dword ptr [src]
			mov edi, dword ptr [bswap_buffer]
			mov ecx, num_vars

			mov ebx, ecx
			and ecx, 3
			je BSWAP_WORD_4X

		BSWAP_WORD_LOOP:
			mov ax, [esi]
			mov ax, [esi]
			mov dl, al
			mov al, ah
			mov ah, dl
			mov [edi], ax
			add esi, 2
			add edi, 2
			loopne BSWAP_WORD_LOOP

		BSWAP_WORD_4X:
			mov ecx, ebx
			shr ecx, 2
			je BSWAP_WORD_END
		BSWAP_WORD_4X_LOOP:
			mov ax, [esi]
			mov dl, al
			mov al, ah
			mov ah, dl
			mov [edi], ax
			mov ax, [esi+2]
			mov dl, al
			mov al, ah
			mov ah, dl
			mov [edi+2], ax
			mov ax, [esi+4]
			mov dl, al
			mov al, ah
			mov ah, dl
			mov [edi+4], ax
			mov ax, [esi+6]
			mov dl, al
			mov al, ah
			mov ah, dl
			mov [edi+6], ax
			add esi, 8
			add edi, 8
			loopne BSWAP_WORD_4X_LOOP

		BSWAP_WORD_END:
	    }
	    return (uae_u32) bswap_buffer;
        case 107:	//byteswap 32bit vars - see case 106
			//a0 = start address
			//d1 = number of 32bit vars
			//returns address of new array
	    src = m68k_areg (&context->regs, 0);
	    num_vars = m68k_dreg (&context->regs, 1);
	    if (bswap_buffer_size < num_vars * 4) {
		bswap_buffer_size = (num_vars + 16384) * 4;
		free (bswap_buffer);
		bswap_buffer = (void*) malloc (bswap_buffer_size);
	    }
	    __asm {
			mov esi, dword ptr [src]
			mov edi, dword ptr [bswap_buffer]
			mov ecx, num_vars

			mov ebx, ecx
			and ecx, 3
			je BSWAP_DWORD_4X

		BSWAP_DWORD_LOOP:
			mov eax, [esi]
			bswap eax
			mov [edi], eax
			add esi, 4
			add edi, 4
			loopne BSWAP_DWORD_LOOP

		BSWAP_DWORD_4X:
			mov ecx, ebx
			shr ecx, 2
			je BSWAP_DWORD_END
			BSWAP_DWORD_4X_LOOP:
			mov eax, [esi]
			bswap eax
			mov [edi], eax
			mov eax, [esi+4]
			bswap eax
			mov [edi+4], eax
			mov eax, [esi+8]
			bswap eax
			mov [edi+8], eax
			mov [edi+8], eax
			mov eax, [esi+12]
			bswap eax
			mov [edi+12], eax
			add esi, 16
			add edi, 16
		loopne BSWAP_DWORD_4X_LOOP

		BSWAP_DWORD_END:
	    }
	    return (uae_u32) bswap_buffer;
	case 108:	//frees swap array
	    bswap_buffer_size = 0;
	    free (bswap_buffer);
	    bswap_buffer = NULL;
	    return 0;
	case 200:
	    ahitweak = m68k_dreg (&context->regs, 1);
	    return 1;
#endif
	default:
	    return 0x12345678;	// Code for not supported function
    }
}


void misc_hsync_stuff (void)
{
    static int misc_demux_installed;

#ifdef AHI
    {
	static int count;
	if (ahi_on) {
	    count++;
	    //15625/count freebuffer check
	    if (count > 20) {
		ahi_updatesound (1);
		count = 0;
	    }
	}
    }
#endif

    if (!misc_demux_installed) {
	uaecptr a = here ();
	org (RTAREA_BASE + 0xFFC0);
	calltrap (deftrap (misc_demux));
	dw (0x4e75);// rts
	org (a);
	misc_demux_installed = 1;
    }
}
