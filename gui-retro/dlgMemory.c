/*
	modded for libretro-uae
*/

/*
  Hatari - dlgMemory.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
*/
const char DlgMemory_fileid[] = "Hatari dlgMemory.c : " __DATE__ " " __TIME__;

#include "libretro-mapper.h"
#include "dialog.h"
#include "sdlgui.h"
#include "file.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "disk.h"
#ifdef LIBRETRO_FSUAE 
#include "newcpu.h"
#endif
#include "autoconf.h"
#include "custom.h"
#include "inputdevice.h"
#include "memory.h"
#include "savestate.h"
#include "filesys.h"
#include "zfile.h"
#include "gensound.h"

#define DLGMEMC_512KB    3
#define DLGMEMC_1MB      4
#define DLGMEMC_2MB      5
#define DLGMEMC_3MB      6
#define DLGMEMC_4MB      7
#define DLGMEMC_5MB      8
#define DLGMEMC_6MB      9
#define DLGMEMC_7MB      10
#define DLGMEMC_8MB      11

#define DLGMEMB_512KB    13
#define DLGMEMB_1MB      14

#define DLGMEMF_512KB    16
#define DLGMEMF_1MB      17
#define DLGMEMF_2MB      18
#define DLGMEMF_3MB      19
#define DLGMEMF_4MB      20
#define DLGMEMF_5MB      21
#define DLGMEMF_6MB      22
#define DLGMEMF_7MB      23
#define DLGMEMF_8MB      24

#define DLGMEM_EXIT     25

static int chipm[9]={0x80000,0x100000,0x200000,0x300000,0x400000,0x500000,0x600000,0x700000,0x800000};

static char dlgSnapShotName[36+1];


/* The memory dialog: */
static SGOBJ memorydlg[] =
{
	{ SGBOX, 0, 0, 0,0, 40,22, NULL },

	{ SGBOX, 0, 0, 1,1, 38,19, NULL },
	{ SGTEXT, 0, 0, 15,2, 12+5,1, "Chip Memory setup" },

	{ SGRADIOBUT, 0, 0, 18-11,4, 9,1, "512 KiB" },
	{ SGRADIOBUT, 0, 0, 18-11,5, 7,1, "1 MiB" },
	{ SGRADIOBUT, 0, 0, 18-11,6, 7,1, "2 MiB" },
	{ SGRADIOBUT, 0, 0, 18,4, 7,1, "3 MiB" },
	{ SGRADIOBUT, 0, 0, 18,5, 7,1, "4 MiB" },
	{ SGRADIOBUT, 0, 0, 18,6, 7,1, "5 MiB" },
	{ SGRADIOBUT, 0, 0, 29,4, 7,1, "6 MiB" },
	{ SGRADIOBUT, 0, 0, 29,5, 7,1, "7 MiB" },
	{ SGRADIOBUT, 0, 0, 29,6, 7,1, "8 MiB" },

	{ SGTEXT, 0, 0, 15,7+1, 12+5,1, "Bogo Memory setup" },

	{ SGRADIOBUT, 0, 0, 18,7+3, 9,1, "512 KiB" },
	{ SGRADIOBUT, 0, 0, 18,7+4, 7,1, "1 MiB" },

	{ SGTEXT, 0, 0, 15,13, 12+5,1, "Fast Memory setup" },

	{ SGRADIOBUT, 0, 0, 18-11,15, 9,1, "512 KiB" },
	{ SGRADIOBUT, 0, 0, 18-11,16, 7,1, "1 MiB" },
	{ SGRADIOBUT, 0, 0, 18-11,17, 7,1, "2 MiB" },
	{ SGRADIOBUT, 0, 0, 18, 15,7,1, "3 MiB" },
	{ SGRADIOBUT, 0, 0, 18, 16,7,1, "4 MiB" },
	{ SGRADIOBUT, 0, 0, 18, 17,7,1, "5 MiB" },
	{ SGRADIOBUT, 0, 0, 29, 15,7,1, "6 MiB" },
	{ SGRADIOBUT, 0, 0, 29, 16,7,1, "7 MiB" },
	{ SGRADIOBUT, 0, 0, 29, 17,7,1, "8 MiB" },

	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 10,20, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};



/**
 * Show and process the memory dialog.
 * @return  true if a memory snapshot has been loaded, false otherwise
 */
bool Dialog_MemDlg(void)
{
	int i;
	int but;

	SDLGui_CenterDlg(memorydlg);

	for (i = DLGMEMC_512KB; i <= DLGMEMC_8MB; i++)
	{
		memorydlg[i].state &= ~SG_SELECTED;
	}

	for (i = DLGMEMB_512KB; i <= DLGMEMB_1MB; i++)
	{
		memorydlg[i].state &= ~SG_SELECTED;
	}

	for (i = DLGMEMF_512KB; i <= DLGMEMF_8MB; i++)
	{
		memorydlg[i].state &= ~SG_SELECTED;
	}

printf("chip:%x fast:%x bogo:%x \n",currprefs.chipmem_size,currprefs.fastmem_size,currprefs.bogomem_size);

switch (currprefs.chipmem_size){

	case 0x80000:memorydlg[DLGMEMC_512KB].state |= SG_SELECTED;
		break;
	case 0x100000:memorydlg[DLGMEMC_1MB].state |= SG_SELECTED;
		break;
	case 0x200000:memorydlg[DLGMEMC_2MB].state |= SG_SELECTED;
		break;
	case 0x300000:memorydlg[DLGMEMC_3MB].state |= SG_SELECTED;
		break;
	case 0x400000:memorydlg[DLGMEMC_4MB].state |= SG_SELECTED;
		break;
	case 0x500000:memorydlg[DLGMEMC_5MB].state |= SG_SELECTED;
		break;
	case 0x600000:memorydlg[DLGMEMC_6MB].state |= SG_SELECTED;
		break;
	case 0x700000:memorydlg[DLGMEMC_7MB].state |= SG_SELECTED;
		break;
	default:
		memorydlg[DLGMEMC_8MB].state |= SG_SELECTED;
		break;
}

switch (currprefs.fastmem_size){

	case 0: break;
	case 0x80000:memorydlg[DLGMEMF_512KB].state |= SG_SELECTED;
		break;
	case 0x100000:memorydlg[DLGMEMF_1MB].state |= SG_SELECTED;
		break;
	case 0x200000:memorydlg[DLGMEMF_2MB].state |= SG_SELECTED;
		break;
	case 0x300000:memorydlg[DLGMEMF_3MB].state |= SG_SELECTED;
		break;
	case 0x400000:memorydlg[DLGMEMF_4MB].state |= SG_SELECTED;
		break;
	case 0x500000:memorydlg[DLGMEMF_5MB].state |= SG_SELECTED;
		break;
	case 0x600000:memorydlg[DLGMEMF_6MB].state |= SG_SELECTED;
		break;
	case 0x700000:memorydlg[DLGMEMF_7MB].state |= SG_SELECTED;
		break;
	default:
		memorydlg[DLGMEMF_8MB].state |= SG_SELECTED;
		break;
}

switch (currprefs.bogomem_size){

	case 0x80000:memorydlg[DLGMEMB_512KB].state |= SG_SELECTED;
		break;
	default:memorydlg[DLGMEMB_1MB].state |= SG_SELECTED;
		break;	
}

	do
	{       
		but = SDLGui_DoDialog(memorydlg, NULL);

		switch (but)
		{


		}

                gui_poll_events();
	}
	while (but != DLGMEM_EXIT && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram );


	for (i = DLGMEMC_512KB; i <= DLGMEMC_8MB; i++)
	{
		if(memorydlg[i].state & SG_SELECTED){

			changed_prefs.chipmem_size=chipm[i-DLGMEMC_512KB];

			if (currprefs.fastmem_size != 0 && changed_prefs.chipmem_size > 0x200000)
				changed_prefs.chipmem_size=0x200000;

			if(changed_prefs.chipmem_size!=currprefs.chipmem_size)memory_reset();
			break;
		}
	}

	for (i = DLGMEMB_512KB; i <= DLGMEMB_1MB; i++)
	{
		if(memorydlg[i].state & SG_SELECTED){

			changed_prefs.bogomem_size=chipm[i-DLGMEMB_512KB];
			if(changed_prefs.bogomem_size!=currprefs.bogomem_size)memory_reset();
			break;
		}
	}

	for (i = DLGMEMF_512KB; i <= DLGMEMF_8MB; i++)
	{

		if(memorydlg[i].state & SG_SELECTED){

			changed_prefs.fastmem_size=chipm[i-DLGMEMF_512KB];

			if(changed_prefs.fastmem_size!=currprefs.fastmem_size){

				if (changed_prefs.fastmem_size > 0 && currprefs.chipmem_size > 0x200000)
					changed_prefs.chipmem_size = 0x200000;

				expamem_reset();
				memory_reset();			
			}
			break;
		}
	}


	return false;
}
