/*
	modded for libretro-uae
*/

/*
  Hatari - dlgSound.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
*/
const char DlgSound_fileid[] = "Hatari dlgSound.c : " __DATE__ " " __TIME__;

#include "dialog.h"
#include "sdlgui.h"
#include "file.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"

#define DLGSOUND_11KHZ      4
#define DLGSOUND_12KHZ      5
#define DLGSOUND_16KHZ      6
#define DLGSOUND_22KHZ      7
#define DLGSOUND_25KHZ      8
#define DLGSOUND_32KHZ      9
#define DLGSOUND_44KHZ      10
#define DLGSOUND_48KHZ      11
#define DLGSOUND_50KHZ      12

#define DLGSOUND_TNONE      14
#define DLGSOUND_TINT       15
#define DLGSOUND_TNORMAL    16
#define DLGSOUND_TEXACT     17

#define DLGSOUND_INONE      19
#define DLGSOUND_IANTI      20
#define DLGSOUND_ISINC      21
#define DLGSOUND_IRH        22
#define DLGSOUND_ICRUX      23
//#define DLGSOUND_CLICK      25
#define DLGSOUND_EXIT      25// 26

static char dlgRecordName[35];

/* The sound dialog: */
static SGOBJ sounddlg[] =
{
	{ SGBOX,      0,0,  0, 0, 40,26, NULL }, //0
	{ SGBOX,      0,0,  1, 1, 38,18, NULL },
	{ SGTEXT,     0,0,  4, 2, 13,1, "SOUND" },

	{ SGTEXT,     0,0,  4, 4, 14,1, "Playback quality:" }, //3
	{ SGRADIOBUT, 0,0,  2, 6, 10,1, "11025 Hz" },
	{ SGRADIOBUT, 0,0,  2, 7, 10,1, "12517 Hz" },
	{ SGRADIOBUT, 0,0,  2, 8, 10,1, "16000 Hz" },
	{ SGRADIOBUT, 0,0, 15, 6, 10,1, "22050 Hz" },
	{ SGRADIOBUT, 0,0, 15, 7, 10,1, "25033 Hz" },
	{ SGRADIOBUT, 0,0, 15, 8, 10,1, "32000 Hz" },
	{ SGRADIOBUT, 0,0, 28, 6, 10,1, "44100 Hz" },
	{ SGRADIOBUT, 0,0, 28, 7, 10,1, "48000 Hz" },
	{ SGRADIOBUT, 0,0, 28, 8, 10,1, "50066 Hz" },

	{ SGTEXT,     0,0,  4,10, 14,1, "Sound type:" }, //13
	{ SGRADIOBUT, 0,0,  2,12,  6,1, "None" },
	{ SGRADIOBUT, 0,0,  8,12, 11,1, "Interrupts" },
	{ SGRADIOBUT, 0,0, 20,12,  7,1, "Normal" },
	{ SGRADIOBUT, 0,0, 27,12,  7,1, "Exact" },

	{ SGTEXT,     0,0,  4,14, 14,1, "Interpol Mode:" }, //18
	{ SGRADIOBUT, 0,0,  2,16, 6,1, "None" },
	{ SGRADIOBUT, 0,0,  8,16, 6,1, "Anti" },
	{ SGRADIOBUT, 0,0, 14,16, 6,1, "Sinc" },
	{ SGRADIOBUT, 0,0, 20,16, 6,1, "Rh" },
	{ SGRADIOBUT, 0,0, 26,16, 6,1, "Crux" },

	{ SGBOX,      0,0,  1,18, 38,7, NULL }, //24
	//{ SGCHECKBOX, 0, 0, 10+11,19, 13,1, NULL/*"Click Sound"*/ },			//25
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 10,23, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


static const int nSoundFreqs[] =
{
	11025,
	12517,
	16000,
	22050,
	25033,
	32000,
	44100,
	48000,
	50066
};


/*-----------------------------------------------------------------------*/
/**
 * Show and process the sound dialog.
 */
void Dialog_SoundDlg(void)
{
	int but, i;

	SDLGui_CenterDlg(sounddlg);

	/* Set up dialog from actual values: */

	for (i = DLGSOUND_11KHZ; i <= DLGSOUND_50KHZ; i++)
		sounddlg[i].state &= ~SG_SELECTED;

	for (i = 0; i <= DLGSOUND_50KHZ-DLGSOUND_11KHZ; i++)
	{
		if (currprefs.sound_freq > nSoundFreqs[i]-500
		    && currprefs.sound_freq < nSoundFreqs[i]+500)
		{
			sounddlg[DLGSOUND_11KHZ + i].state |= SG_SELECTED;
			break;
		}
	}


	for (i = DLGSOUND_TNONE; i <= DLGSOUND_TEXACT; i++)
		sounddlg[i].state &= ~SG_SELECTED;

	for (i = DLGSOUND_INONE; i <= DLGSOUND_ICRUX; i++)
		sounddlg[i].state &= ~SG_SELECTED;

	sounddlg[DLGSOUND_TNONE + currprefs.produce_sound].state |= SG_SELECTED;
	sounddlg[DLGSOUND_INONE + currprefs.sound_interpol].state |= SG_SELECTED;

/*
	printf("pp:%d\n",currprefs.floppyslots[0].dfxclick);

	if(currprefs.floppyslots[0].dfxclick==1)sounddlg[DLGSOUND_CLICK].state |= SG_SELECTED ;
	else sounddlg[DLGSOUND_CLICK].state &= ~SG_SELECTED;
*/

	/* The sound dialog main loop */
	do
	{	
		
		but = SDLGui_DoDialog(sounddlg, NULL);

                gui_poll_events();
	}
	while (but != DLGSOUND_EXIT && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram );

	
	for (i = DLGSOUND_11KHZ; i <= DLGSOUND_50KHZ; i++)
	{
		if (sounddlg[i].state & SG_SELECTED)
		{
			changed_prefs.sound_freq= nSoundFreqs[i-DLGSOUND_11KHZ];
			break;
		}
	}
	
	for (i = DLGSOUND_TNONE; i <= DLGSOUND_TEXACT; i++)
		if(sounddlg[i].state & SG_SELECTED){
			changed_prefs.produce_sound=i-DLGSOUND_TNONE;
			break;
		}

	for (i = DLGSOUND_INONE; i <= DLGSOUND_ICRUX; i++)
		if(sounddlg[i].state & SG_SELECTED){
			changed_prefs.sound_interpol=i-DLGSOUND_INONE;
			break;
		}
/*
	if(sounddlg[DLGSOUND_CLICK].state & SG_SELECTED){
			
			changed_prefs.floppyslots[0].dfxclick = 1;
			changed_prefs.floppyslots[1].dfxclick = 1;
			changed_prefs.floppyslots[2].dfxclick = 1;
			changed_prefs.floppyslots[3].dfxclick = 1; 
	}
	else{		
			changed_prefs.floppyslots[0].dfxclick = -1;
			changed_prefs.floppyslots[1].dfxclick = -1;
			changed_prefs.floppyslots[2].dfxclick = -1;
			changed_prefs.floppyslots[3].dfxclick = -1; 
	}
*/
	config_changed = 1;
	check_prefs_changed_audio();

}

