/*
	modded for libretro-uae
*/

/*
  Hatari - dlgSystem.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
  
  This file contains 2 system panels :
    - 1 for the old uae CPU
    - 1 for the new WinUae cpu
    
  The selection is made during compilation with the ENABLE_WINUAE_CPU define

*/
const char DlgSystem_fileid[] = "Hatari dlgSystem.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"

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

extern struct uae_prefs currprefs, changed_prefs;


  int nCpuLevel;
  bool bCompatibleCpu;            /* Prefetch mode */
  bool bCycleExactCpu;


/* The new WinUae cpu "System" dialog: */

#define DLGSYS_68000      4
#define DLGSYS_68010      5
#define DLGSYS_68020      6
#define DLGSYS_68030      7
#define DLGSYS_68040      8
#define DLGSYS_68060      9
#define DLGSYS_ST         12
#define DLGSYS_STE        13
#define DLGSYS_TT         14
#define DLGSYS_FALCON     15
#define DLGSYS_8MHZ       18
#define DLGSYS_16MHZ      19
#define DLGSYS_32MHZ      20
#define DLGSYS_DSPOFF     23
#define DLGSYS_DSPDUMMY   24
#define DLGSYS_DSPON      25
#define DLGSYS_24BITS     28
#define DLGSYS_PREFETCH   29
#define DLGSYS_CYC_EXACT  30
#define DLGSYS_RTC        31
#define DLGSYS_TIMERD     32
#define DLGSYS_BLITTER    33
#define DLGSYS_MMU_EMUL   34
#define DLGSYS_FPU_NONE   37
#define DLGSYS_FPU_68881  38
#define DLGSYS_FPU_68882  39
#define DLGSYS_FPU_CPU_IN 40
#define DLGSYS_FPU_COMPAT 41
#define DLGSYS_EXIT       42

static SGOBJ systemdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 60,25, NULL },
	{ SGTEXT, 0, 0, 23,1, 14,1, "System options" },

	{ SGBOX, 0, 0, 19,3, 11,9, NULL },
	{ SGTEXT, 0, 0, 20,3, 8,1, "CPU type" },
	{ SGRADIOBUT, 0, 0, 20,5, 7,1, "68000" },
	{ SGRADIOBUT, 0, 0, 20,6, 7,1, "68010" },
	{ SGRADIOBUT, 0, 0, 20,7, 7,1, "68020" },
	{ SGRADIOBUT, 0, 0, 20,8, 13,1, "68030" },
	{ SGRADIOBUT, 0, 0, 20,9, 13,1, "68040" },
	{ SGRADIOBUT, 0, 0, 20,10, 7,1, "68060" },

	{ SGBOX , 0, 0, 2,3, 15,9, NULL },
	{ SGTEXT, 0, 0, 3,3, 13,1, NULL },
	{ SGTEXT, 0, 0, 3,5, 8,1, NULL },
	{ SGTEXT, 0, 0, 3,6, 8,1, NULL },
	{ SGTEXT, 0, 0, 3,7, 8,1, NULL },
	{ SGTEXT, 0, 0, 3,8, 8,1, NULL },

	{ SGBOX, 0, 0, 32,3, 12,9, NULL },
	{ SGTEXT, 0, 0, 33,3, 15,1, NULL },
	{ SGTEXT, 0, 0, 33,5, 8,1, NULL },
	{ SGTEXT, 0, 0, 33,6, 8,1, NULL },
	{ SGTEXT, 0, 0, 33,7, 8,1, NULL},

	{ SGBOX, 0, 0, 46,3, 12,9, NULL },
	{ SGTEXT, 0, 0, 47,3, 11,1, NULL },
	{ SGTEXT, 0, 0, 47,5, 6,1, NULL },
	{ SGTEXT, 0, 0, 47,6, 7,1, NULL },
	{ SGTEXT, 0, 0, 47,7, 6,1, NULL },

	{ SGBOX, 0, 0, 2,13, 28,9, NULL },
	{ SGTEXT, 0, 0, 3,13, 11,1, "CPU Pamameters" },
	{ SGTEXT, 0, 0, 3,15, 15,1, NULL  },
	{ SGCHECKBOX, 0, 0, 3,16, 32,1, "Prefetch mode, slower" },
	{ SGCHECKBOX, 0, 0, 3,17, 32,1, "Cycle exact,   slower" },
	{ SGTEXT, 0, 0, 3,18, 27,1, NULL  },
	{ SGTEXT, 0, 0, 3,19, 15,1, NULL  },
	{ SGTEXT, 0, 0, 3,20, 20,1, NULL  },
	{ SGTEXT, 0, 0, 3,21, 15,1, NULL  },

	{ SGBOX, 0, 0, 32,13, 26,9, NULL },
	{ SGTEXT, 0, 0, 33,13, 11,1, NULL },
	{ SGTEXT, 0, 0, 33,15, 6,1, NULL },
	{ SGTEXT, 0, 0, 33,16, 7,1, NULL },
	{ SGTEXT, 0, 0, 33,17, 7,1, NULL },
	{ SGTEXT, 0, 0, 33,18, 14,1, NULL },
	{ SGTEXT, 0, 0, 33,20, 25,1, NULL },

	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 21,23, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


/*-----------------------------------------------------------------------*/
/**
 * Show and process the "System" dialog (specific to winUAE cpu).
 */
void Dialog_SystemDlg(void)
{
	int i;
	//MACHINETYPE	mti;

//printf("0cpu:%d curr:%d\n",changed_prefs.cpu_model,currprefs.cpu_model);

	SDLGui_CenterDlg(systemdlg);

	/* Set up dialog from actual values: */

	for (i = DLGSYS_68000; i <= DLGSYS_68060; i++)
	{
		systemdlg[i].state &= ~SG_SELECTED;
	}
	systemdlg[DLGSYS_68000+/*ConfigureParams.System.*/nCpuLevel].state |= SG_SELECTED;
#if 0
	for (i = DLGSYS_ST; i <= DLGSYS_FALCON; i++)
	{
		systemdlg[i].state &= ~SG_SELECTED;
	}
	systemdlg[DLGSYS_ST + ConfigureParams.System.nMachineType].state |= SG_SELECTED;

	/* CPU frequency: */
	for (i = DLGSYS_8MHZ; i <= DLGSYS_16MHZ; i++)
	{
		systemdlg[i].state &= ~SG_SELECTED;
	}
	if (ConfigureParams.System.nCpuFreq == 32)
		systemdlg[DLGSYS_32MHZ].state |= SG_SELECTED;
	else if (ConfigureParams.System.nCpuFreq == 16)
		systemdlg[DLGSYS_16MHZ].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_8MHZ].state |= SG_SELECTED;

	/* DSP mode: */
	for (i = DLGSYS_DSPOFF; i <= DLGSYS_DSPON; i++)
	{
		systemdlg[i].state &= ~SG_SELECTED;
	}
	if (ConfigureParams.System.nDSPType == DSP_TYPE_NONE)
		systemdlg[DLGSYS_DSPOFF].state |= SG_SELECTED;
	else if (ConfigureParams.System.nDSPType == DSP_TYPE_DUMMY)
		systemdlg[DLGSYS_DSPDUMMY].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_DSPON].state |= SG_SELECTED;
#endif
	/* More compatible CPU, Prefetch mode */
	if (/*ConfigureParams.System.*/bCompatibleCpu)
		systemdlg[DLGSYS_PREFETCH].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_PREFETCH].state &= ~SG_SELECTED;
#if 0
	/* Emulate Blitter */
	if (ConfigureParams.System.bBlitter)
		systemdlg[DLGSYS_BLITTER].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_BLITTER].state &= ~SG_SELECTED;

	/* Real time clock CPU */
	if (ConfigureParams.System.bRealTimeClock)
		systemdlg[DLGSYS_RTC].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_RTC].state &= ~SG_SELECTED;

	/* Patch timer D */
	if (ConfigureParams.System.bPatchTimerD)
		systemdlg[DLGSYS_TIMERD].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_TIMERD].state &= ~SG_SELECTED;

	/* Adress space 24 bits */
	if (ConfigureParams.System.bAddressSpace24)
		systemdlg[DLGSYS_24BITS].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_24BITS].state &= ~SG_SELECTED;
#endif	
	/* Cycle exact CPU */
	if (/*ConfigureParams.System.*/bCycleExactCpu)
		systemdlg[DLGSYS_CYC_EXACT].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_CYC_EXACT].state &= ~SG_SELECTED;
#if 0
	/* FPU emulation */
	for (i = DLGSYS_FPU_NONE; i <= DLGSYS_FPU_CPU_IN; i++)
	{
		systemdlg[i].state &= ~SG_SELECTED;
	}
	if (ConfigureParams.System.n_FPUType == FPU_NONE)
		systemdlg[DLGSYS_FPU_NONE].state |= SG_SELECTED;
	else if (ConfigureParams.System.n_FPUType == FPU_68881)
		systemdlg[DLGSYS_FPU_68881].state |= SG_SELECTED;
	else if (ConfigureParams.System.n_FPUType == FPU_68882)
		systemdlg[DLGSYS_FPU_68882].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_FPU_CPU_IN].state |= SG_SELECTED;

	/* More compatible FPU */
	if (ConfigureParams.System.bCompatibleFPU)
		systemdlg[DLGSYS_FPU_COMPAT].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_FPU_COMPAT].state &= ~SG_SELECTED;

	/* MMU Emulation */
	if (ConfigureParams.System.bMMU)
		systemdlg[DLGSYS_MMU_EMUL].state |= SG_SELECTED;
	else
		systemdlg[DLGSYS_MMU_EMUL].state &= ~SG_SELECTED;
#endif
int but;
do
{			
	/* Show the dialog: */
	but=SDLGui_DoDialog(systemdlg, NULL);
        gui_poll_events();
}
while (but != DLGSYS_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);

	/* Read values from dialog: */

	for (i = DLGSYS_68000; i <= DLGSYS_68060; i++)
	{
		if (systemdlg[i].state&SG_SELECTED)
		{
			/*ConfigureParams.System.*/nCpuLevel = i-DLGSYS_68000;
			break;
		}
	}

	int index = nCpuLevel;
	if(index==5)index=6;

	changed_prefs.cpu_model=68000+index*10;

//currprefs.cpu_model=68000;
//printf("1cpu:%d curr:%d\n",changed_prefs.cpu_model,currprefs.cpu_model);

	if (changed_prefs.cpu_model <= 68020)changed_prefs.address_space_24=1;


/*
	for (mti = MACHINE_ST; mti <= MACHINE_FALCON; mti++)
	{
		if (systemdlg[mti + DLGSYS_ST].state&SG_SELECTED)
		{
			ConfigureParams.System.nMachineType = mti;
			break;
		}
	}
*/
/*
	if (systemdlg[DLGSYS_32MHZ].state & SG_SELECTED)
		ConfigureParams.System.nCpuFreq = 32;
	else if (systemdlg[DLGSYS_16MHZ].state & SG_SELECTED)
		ConfigureParams.System.nCpuFreq = 16;
	else
		ConfigureParams.System.nCpuFreq = 8;

	if (systemdlg[DLGSYS_DSPOFF].state & SG_SELECTED)
		ConfigureParams.System.nDSPType = DSP_TYPE_NONE;
	else if (systemdlg[DLGSYS_DSPDUMMY].state & SG_SELECTED)
		ConfigureParams.System.nDSPType = DSP_TYPE_DUMMY;
	else
		ConfigureParams.System.nDSPType = DSP_TYPE_EMU;
*/
	/*ConfigureParams.System.*/bCompatibleCpu = (systemdlg[DLGSYS_PREFETCH].state & SG_SELECTED);

changed_prefs.cpu_compatible =bCompatibleCpu;
	if (changed_prefs.cpu_compatible && currprefs.cpu_cycle_exact) {
				changed_prefs.cpu_cycle_exact = 0;
	}
/*
	ConfigureParams.System.bBlitter = (systemdlg[DLGSYS_BLITTER].state & SG_SELECTED);
	ConfigureParams.System.bRealTimeClock = (systemdlg[DLGSYS_RTC].state & SG_SELECTED);
	ConfigureParams.System.bPatchTimerD = (systemdlg[DLGSYS_TIMERD].state & SG_SELECTED);
	ConfigureParams.System.bAddressSpace24 = (systemdlg[DLGSYS_24BITS].state & SG_SELECTED);
*/
	/*ConfigureParams.System.*/bCycleExactCpu = (systemdlg[DLGSYS_CYC_EXACT].state & SG_SELECTED);

	changed_prefs.cpu_cycle_exact = bCycleExactCpu;
	if (changed_prefs.cpu_cycle_exact && currprefs.cpu_compatible) {
				changed_prefs.cpu_compatible = 0;
	}

	config_changed = 1;
	check_prefs_changed_cpu();
	/* FPU emulation */
/*
	if (systemdlg[DLGSYS_FPU_NONE].state & SG_SELECTED)
		ConfigureParams.System.n_FPUType = FPU_NONE;
	else if (systemdlg[DLGSYS_FPU_68881].state & SG_SELECTED)
		ConfigureParams.System.n_FPUType = FPU_68881;
	else if (systemdlg[DLGSYS_FPU_68882].state & SG_SELECTED)
		ConfigureParams.System.n_FPUType = FPU_68882;
	else
		ConfigureParams.System.n_FPUType = FPU_CPU;

	ConfigureParams.System.bCompatibleFPU = (systemdlg[DLGSYS_FPU_COMPAT].state & SG_SELECTED);
	ConfigureParams.System.bMMU = (systemdlg[DLGSYS_MMU_EMUL].state & SG_SELECTED);
*/
}

