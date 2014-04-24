/*
	modded for libretro-uae
*/

/*
  Hatari - sdlgui.h

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Header for the tiny graphical user interface for Hatari.
*/
//
#ifndef HATARI_SDLGUI_H
#define HATARI_SDLGUI_H

#include <stdbool.h>

#define SDL_Delay GetTicks2

enum
{
  SGBOX,
  SGTEXT,
  SGEDITFIELD,
  SGBUTTON,
  SGRADIOBUT,
  SGCHECKBOX,
  SGPOPUP,
  SGSCROLLBAR
};


/* Object flags: */
#define SG_TOUCHEXIT   1   /* Exit immediately when mouse button is pressed down */
#define SG_EXIT        2   /* Exit when mouse button has been pressed (and released) */
#define SG_DEFAULT     4   /* Marks a default button, selectable with return key */
#define SG_CANCEL      8   /* Marks a cancel button, selectable with ESC key */

/* Object states: */
#define SG_SELECTED    1
#define SG_MOUSEDOWN   16
#define SG_MOUSEUP     (((int)-1) - SG_MOUSEDOWN)

/* Special characters: */
#define SGRADIOBUTTON_NORMAL    46//0xfa//12
#define SGRADIOBUTTON_SELECTED  219//0xe//13
#define SGCHECKBOX_NORMAL       196//0xf9//14
#define SGCHECKBOX_SELECTED     197// 0x6//15
#define SGARROWUP               203//0x5e// 1
#define SGARROWDOWN             202//0xb//  2
#define SGFOLDER                '~'// 5

/* Return codes: */
#define SDLGUI_ERROR         -1
#define SDLGUI_QUIT          -2
#define SDLGUI_UNKNOWNEVENT  -3


typedef struct
{
  int type;             /* What type of object */
  int flags;            /* Object flags */
  int state;            /* Object state */
  int x, y;             /* The offset to the upper left corner */
  int w, h;             /* Width and height (for scrollbar : height and position) */
  char *txt;            /* Text string */
}  SGOBJ;

extern int sdlgui_fontwidth;	/* Width of the actual font */
extern int sdlgui_fontheight;	/* Height of the actual font */

extern int SDLGui_Init(void);
extern int SDLGui_UnInit(void);
extern int SDLGui_SetScreen();
extern void SDLGui_GetFontSize(int *width, int *height);
extern void SDLGui_Text(int x, int y, const char *txt);
extern void SDLGui_DrawDialog(const SGOBJ *dlg);
extern int SDLGui_DoDialog(SGOBJ *dlg, int *pEventOut);
extern void SDLGui_CenterDlg(SGOBJ *dlg);
extern char* SDLGui_FileSelect(const char *path_and_name, char **zip_path, bool bAllowNew);
extern bool SDLGui_FileConfSelect(char *dlgname, char *confname, int maxlen, bool bAllowNew);

extern void rsxGui_prepare();
extern void rsxGui_flip(int but);

#endif
