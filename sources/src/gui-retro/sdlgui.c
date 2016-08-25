/*
	modded for libretro-uae
*/

/*
  Hatari - sdlgui.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.

  A tiny graphical user interface for Hatari.
*/
const char SDLGui_fileid[] = "Hatari sdlgui.c : " __DATE__ " " __TIME__;

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "utype.h"

typedef struct{
     int16_t x, y;
     uint16_t w, h;
} SDL_Rect;

#include "dialog.h"
#include "sdlgui.h"

extern int retroh,retrow;
#define SDL_Delay GetTicks2
#define TEXTURE_WIDTH retrow
#define TEXTURE_HEIGHT retroh

unsigned short int bmp[1024*1024];

#define B ((rgba>> 8)&0xff)>>3 
#define G ((rgba>>16)&0xff)>>3
#define R ((rgba>>24)&0xff)>>3

#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))

extern void Draw_text(unsigned  short *buffer,int x,int y,unsigned  short  fgcol,unsigned  short int bgcol ,int scalex,int scaley , int max,char *string,...);
extern void DrawFBoxBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short color);
extern void input_gui();

static const char *cross[] = {
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
};

void draw_cross(int x,int y) {

	int i,j,idx;
	int dx=32,dy=20;
	unsigned  short color;

	for(j=y;j<y+dy;j++){
		idx=0;
		for(i=x;i<x+dx;i++){
			if(cross[j-y][idx]=='.')DrawPointBmp(bmp,i,j,0xffff);
			else if(cross[j-y][idx]=='X')DrawPointBmp(bmp,i,j,0);
			idx++;			
		}
	}
}

#define DrawBoxF( x,  y,  z,  dx,  dy, rgba) \
	DrawFBoxBmp(bmp, x, y, dx, dy,RGB565(((rgba>>24)&0xff)>>3,((rgba>>16)&0xff)>>3,((rgba>>8)&0xff)>>3 ))

static int current_object = 0;				/* Current selected object */

int sdlgui_fontwidth=10;			/* Width of the actual font */
int sdlgui_fontheight=16;			/* Height of the actual font */

#define fontwidth sdlgui_fontwidth
#define fontheight sdlgui_fontheight

/*-----------------------------------------------------------------------*/
/**
 * Initialize the GUI.
 */
int SDLGui_Init(void)
{
	return 0;
}


/*-----------------------------------------------------------------------*/
/**
 * Uninitialize the GUI.
 */
int SDLGui_UnInit(void)
{
	return 0;
}


/*-----------------------------------------------------------------------*/
/**
 * Inform the SDL-GUI about the actual SDL_Surface screen pointer and
 * prepare the font to suit the actual resolution.
 */

int SDLGui_SetScreen(/*SDL_Surface *pScrn*/)
{
	memset(bmp, 0, sizeof(bmp));

	sdlgui_fontwidth  = 10;//8;
	sdlgui_fontheight = 16;//2*8;
	return 0;
}

/*-----------------------------------------------------------------------*/
/**
 * Return character size for current font in given arguments.
 */
void SDLGui_GetFontSize(int *width, int *height)
{
	*width = sdlgui_fontwidth;
	*height = sdlgui_fontheight;
}

/*-----------------------------------------------------------------------*/
/**
 * Center a dialog so that it appears in the middle of the screen.
 * Note: We only store the coordinates in the root box of the dialog,
 * all other objects in the dialog are positioned relatively to this one.
 */
void SDLGui_CenterDlg(SGOBJ *dlg)
{

	dlg[0].x = (TEXTURE_WIDTH/*pSdlGuiScrn.w*//fontwidth-dlg[0].w)/2;
	dlg[0].y = (TEXTURE_HEIGHT/*pSdlGuiScrn.h*//fontheight-dlg[0].h)/2;
}

/*-----------------------------------------------------------------------*/
/**
 * Draw a text string.
 */

void SDLGui_Text(int x, int y, const char *txt)
{       
	Draw_text(bmp,x,y,1,0,1,2,40,(char *)txt);
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a dialog text object.
 */
static void SDLGui_DrawText(const SGOBJ *tdlg, int objnum)
{
	int x, y;
	x = (tdlg[0].x+tdlg[objnum].x)*sdlgui_fontwidth;
	y = (tdlg[0].y+tdlg[objnum].y)*sdlgui_fontheight;
	SDLGui_Text(x, y, tdlg[objnum].txt);
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a edit field object.
 */
static void SDLGui_DrawEditField(const SGOBJ *edlg, int objnum)
{
	int x, y;
	SDL_Rect rect;

	x = (edlg[0].x+edlg[objnum].x)*sdlgui_fontwidth;
	y = (edlg[0].y+edlg[objnum].y)*sdlgui_fontheight;
	SDLGui_Text(x, y, edlg[objnum].txt);

        DrawBoxF(x,y+ edlg[objnum].h * fontheight,0,edlg[objnum].w * fontwidth,1,0xA0A0A0FF);
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a dialog box object.
 */
static void SDLGui_DrawBox(const SGOBJ *bdlg, int objnum)
{
	int x, y, w, h, offset;
	uint32_t grey =  0xC0C0C0FF;
	uint32_t upleftc, downrightc;

	x = bdlg[objnum].x*sdlgui_fontwidth;
	y = bdlg[objnum].y*sdlgui_fontheight;
	if (objnum > 0)                 /* Since the root object is a box, too, */
	{
		/* we have to look for it now here and only */
		x += bdlg[0].x*sdlgui_fontwidth;   /* add its absolute coordinates if we need to */
		y += bdlg[0].y*sdlgui_fontheight;
	}
	w = bdlg[objnum].w*sdlgui_fontwidth;
	h = bdlg[objnum].h*sdlgui_fontheight;

	if (bdlg[objnum].state & SG_SELECTED)
	{
		upleftc = 0x808080FF;
		downrightc = 0xFFFFFFFF;
	}
	else
	{
		upleftc =0xFFFFFFFF;
		downrightc = 0x808080FF;
	}

	/* The root box should be bigger than the screen, so we disable the offset there: */
	if (objnum != 0)
		offset = 1;
	else
		offset = 0;
	/* Draw background: */

        DrawBoxF(x,y,0,w ,h,grey);

	/* Draw upper border: */

        DrawBoxF(x,y - offset,0,w ,1,upleftc);

	/* Draw left border: */

        DrawBoxF(x-offset,y,0,1 ,h,upleftc);

	/* Draw bottom border: */

        DrawBoxF(x,y + h - 1 + offset,0,w ,1,downrightc);

	/* Draw right border: */

        DrawBoxF(x + w - 1 + offset,y,0,1,h,downrightc);
	
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a normal button.
 */
static void SDLGui_DrawButton(const SGOBJ *bdlg, int objnum)
{
	int x,y;

	SDLGui_DrawBox(bdlg, objnum);

	x = (bdlg[0].x + bdlg[objnum].x + (bdlg[objnum].w-strlen(bdlg[objnum].txt))/2) * sdlgui_fontwidth;
	y = (bdlg[0].y + bdlg[objnum].y + (bdlg[objnum].h-1)/2) * sdlgui_fontheight;

	if (bdlg[objnum].state & SG_SELECTED)
	{
		x+=1;
		y+=1;
	}
	SDLGui_Text(x, y, bdlg[objnum].txt);
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a dialog radio button object.
 */
static void SDLGui_DrawRadioButton(const SGOBJ *rdlg, int objnum)
{
	char str[80];
	int x, y;

	x = (rdlg[0].x + rdlg[objnum].x) * sdlgui_fontwidth;
	y = (rdlg[0].y + rdlg[objnum].y) * sdlgui_fontheight;

	if (rdlg[objnum].state & SG_SELECTED)
		str[0]=SGRADIOBUTTON_SELECTED;
	else
		str[0]=SGRADIOBUTTON_NORMAL;
	str[1]=' ';
	strcpy(&str[2], rdlg[objnum].txt);

	SDLGui_Text(x, y, str);
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a dialog check box object.
 */
static void SDLGui_DrawCheckBox(const SGOBJ *cdlg, int objnum)
{
	char str[80];
	int x, y;

	x = (cdlg[0].x + cdlg[objnum].x) * sdlgui_fontwidth;
	y = (cdlg[0].y + cdlg[objnum].y) * sdlgui_fontheight;

	if ( cdlg[objnum].state&SG_SELECTED )
		str[0]=SGCHECKBOX_SELECTED;
	else
		str[0]=SGCHECKBOX_NORMAL;
	str[1]=' ';
	strcpy(&str[2], cdlg[objnum].txt);

	SDLGui_Text(x, y, str);
}


/*-----------------------------------------------------------------------*/
/**
 * Draw a scrollbar button.
 */
static void SDLGui_DrawScrollbar(const SGOBJ *bdlg, int objnum)
{
	SDL_Rect rect;
	int x, y, w, h;
        int offset = 0;

	uint32_t grey0 = 0x808080FF;//SDL_MapRGB(pSdlGuiScrn->format,128,128,128);
	uint32_t grey1 = 0xC4C4C4FF;//SDL_MapRGB(pSdlGuiScrn->format,196,196,196);
	uint32_t grey2 = 0x404040FF;//SDL_MapRGB(pSdlGuiScrn->format, 64, 64, 64);

	x = bdlg[objnum].x * sdlgui_fontwidth;
	y = bdlg[objnum].y * sdlgui_fontheight + bdlg[objnum].h;

	x += bdlg[0].x*sdlgui_fontwidth;   /* add mainbox absolute coordinates */
	y += bdlg[0].y*sdlgui_fontheight;  /* add mainbox absolute coordinates */
	
	w = 1 * sdlgui_fontwidth;
	h = bdlg[objnum].w;

	/* Draw background: */
	DrawBoxF(x,y,0,w ,h,grey0);
	/* Draw upper border: */
	DrawBoxF(x,y - offset,0,w ,1,grey1);
	/* Draw bottom border: */
	DrawBoxF(x,y + h - 1 + offset,0,w ,1,grey2);
	
	
}

/*-----------------------------------------------------------------------*/
/**
 *  Draw a dialog popup button object.
 */
static void SDLGui_DrawPopupButton(const SGOBJ *pdlg, int objnum)
{
	int x, y, w;
	const char *downstr = "\x02";

	SDLGui_DrawBox(pdlg, objnum);

	x = (pdlg[0].x + pdlg[objnum].x) * sdlgui_fontwidth;
	y = (pdlg[0].y + pdlg[objnum].y) * sdlgui_fontheight;
	w = pdlg[objnum].w * sdlgui_fontwidth;

	SDLGui_Text(x, y, pdlg[objnum].txt);
	SDLGui_Text(x+w-sdlgui_fontwidth, y, downstr);
}

//TODO FIX THIS
#if 0
/*-----------------------------------------------------------------------*/
/**
 * Let the user insert text into an edit field object.
 * NOTE: The dlg[objnum].txt must point to an an array that is big enough
 * for dlg[objnum].w characters!
 */
static void SDLGui_EditField(SGOBJ *dlg, int objnum)
{
	size_t cursorPos;                   /* Position of the cursor in the edit field */
	int blinkState = 0;                 /* Used for cursor blinking */
	int bStopEditing = false;           /* true if user wants to exit the edit field */
	char *txt;                          /* Shortcut for dlg[objnum].txt */
	SDL_Rect rect;
	uint32_t grey, cursorCol;
	SDL_Event event;
	int nOldUnicodeMode;

	/* Enable unicode translation to get proper characters with SDL_PollEvent */
	nOldUnicodeMode = SDL_EnableUNICODE(true);

	grey = SDL_MapRGB(pSdlGuiScrn->format, 192, 192, 192);
	cursorCol = SDL_MapRGB(pSdlGuiScrn->format, 128, 128, 128);

	rect.x = (dlg[0].x + dlg[objnum].x) * sdlgui_fontwidth;
	rect.y = (dlg[0].y + dlg[objnum].y) * sdlgui_fontheight;
	rect.w = (dlg[objnum].w + 1) * sdlgui_fontwidth - 1;
	rect.h = dlg[objnum].h * sdlgui_fontheight;

	txt = dlg[objnum].txt;
	cursorPos = strlen(txt);

	do
	{
		/* Look for events */
		if (SDL_PollEvent(&event) == 0)
		{
			/* No event: Wait some time for cursor blinking */
			SDL_Delay(250);
			blinkState ^= 1;
		}
		else
		{
			/* Handle events */
			do
			{
				switch (event.type)
				{
				 case SDL_QUIT:                     /* User wants to quit */
					bQuitProgram = true;
					bStopEditing = true;
					break;
				 case SDL_MOUSEBUTTONDOWN:          /* Mouse pressed -> stop editing */
					bStopEditing = true;
					break;
				 case SDL_KEYDOWN:                  /* Key pressed */
					switch (event.key.keysym.sym)
					{
					 case SDLK_RETURN:
					 case SDLK_KP_ENTER:
						bStopEditing = true;
						break;
					 case SDLK_LEFT:
						if (cursorPos > 0)
							cursorPos -= 1;
						break;
					 case SDLK_RIGHT:
						if (cursorPos < strlen(txt))
							cursorPos += 1;
						break;
					 case SDLK_BACKSPACE:
						if (cursorPos > 0)
						{
							memmove(&txt[cursorPos-1], &txt[cursorPos], strlen(&txt[cursorPos])+1);
							cursorPos -= 1;
						}
						break;
					 case SDLK_DELETE:
						if (cursorPos < strlen(txt))
							memmove(&txt[cursorPos], &txt[cursorPos+1], strlen(&txt[cursorPos+1])+1);
						break;
					 default:
						/* If it is a "good" key then insert it into the text field */
						if (event.key.keysym.unicode >= 32 && event.key.keysym.unicode < 128
						        && event.key.keysym.unicode != PATHSEP)
						{
							if (strlen(txt) < (size_t)dlg[objnum].w)
							{
								memmove(&txt[cursorPos+1], &txt[cursorPos], strlen(&txt[cursorPos])+1);
								txt[cursorPos] = event.key.keysym.unicode;
								cursorPos += 1;
							}
						}
						break;
					}
					break;
				}
			}
			while (SDL_PollEvent(&event));

			blinkState = 1;
		}

		/* Redraw the text field: */
		SDL_FillRect(pSdlGuiScrn, &rect, grey);  /* Draw background */
		/* Draw the cursor: */
		if (blinkState && !bStopEditing)
		{
			SDL_Rect cursorrect;
			cursorrect.x = rect.x + cursorPos * sdlgui_fontwidth;
			cursorrect.y = rect.y;
			cursorrect.w = sdlgui_fontwidth;
			cursorrect.h = rect.h;
			SDL_FillRect(pSdlGuiScrn, &cursorrect, cursorCol);
		}
		SDLGui_Text(rect.x, rect.y, dlg[objnum].txt);  /* Draw text */
		SDL_UpdateRects(pSdlGuiScrn, 1, &rect);
	}
	while (!bStopEditing);

	SDL_EnableUNICODE(nOldUnicodeMode);
}
#else
static void SDLGui_EditField(SGOBJ *dlg, int objnum)
{
}
#endif


/*-----------------------------------------------------------------------*/
/**
 * Draw a whole dialog.
 */
void SDLGui_DrawDialog(const SGOBJ *dlg)
{
	int i;

	memset(bmp, 0, sizeof(bmp));

	for (i = 0; dlg[i].type != -1; i++)
	{
		switch (dlg[i].type)
		{
		 case SGBOX:
			SDLGui_DrawBox(dlg, i);
			break;
		 case SGTEXT:
			SDLGui_DrawText(dlg, i);
			break;
		 case SGEDITFIELD:
			SDLGui_DrawEditField(dlg, i);
			break;
		 case SGBUTTON:
			SDLGui_DrawButton(dlg, i);
			break;
		 case SGRADIOBUT:
			SDLGui_DrawRadioButton(dlg, i);
			break;
		 case SGCHECKBOX:
			SDLGui_DrawCheckBox(dlg, i);
			break;
		 case SGPOPUP:
			SDLGui_DrawPopupButton(dlg, i);
			break;
		 case SGSCROLLBAR:
			SDLGui_DrawScrollbar(dlg, i);
			break;
		}
	}

}


/*-----------------------------------------------------------------------*/
/**
 * Search an object at a certain position.
 */
static int SDLGui_FindObj(const SGOBJ *dlg, int fx, int fy)
{
	int len, i;
	int ob = -1;
	int xpos, ypos;

	len = 0;
	while (dlg[len].type != -1)   len++;

	xpos = fx / sdlgui_fontwidth;
	ypos = fy / sdlgui_fontheight;
	/* Now search for the object: */
	for (i = len; i >= 0; i--)
	{
		/* clicked on a scrollbar ? */
		if (dlg[i].type == SGSCROLLBAR) {
			if (xpos >= dlg[0].x+dlg[i].x && xpos < dlg[0].x+dlg[i].x+1) {
				ypos = dlg[i].y * sdlgui_fontheight + dlg[i].h + dlg[0].y * sdlgui_fontheight;
				if (fy >= ypos && fy < ypos + dlg[i].w) {
					ob = i;
					break;
				}
			}
		}
		/* clicked on another object ? */
		else if (xpos >= dlg[0].x+dlg[i].x && ypos >= dlg[0].y+dlg[i].y
		    && xpos < dlg[0].x+dlg[i].x+dlg[i].w && ypos < dlg[0].y+dlg[i].y+dlg[i].h)
		{
			ob = i;
			break;
		}
	}

	return ob;
}


/*-----------------------------------------------------------------------*/
/**
 * Search a button with a special flag (e.g. SG_DEFAULT or SG_CANCEL).
 */
static int SDLGui_SearchFlaggedButton(const SGOBJ *dlg, int flag)
{
	int i = 0;

	while (dlg[i].type != -1)
	{
		if (dlg[i].flags & flag)
			return i;
		i++;
	}

	return 0;
}


extern int touch;
int gmx,gmy;
int okold=0,boutc=0;

/*-----------------------------------------------------------------------*/
/**SDLGui_DoDialog
 * Show and process a dialog. Returns the button number that has been
 * pressed or SDLGUI_UNKNOWNEVENT if an unsupported event occured (will be
 * stored in parameter pEventOut).
 */
int SDLGui_DoDialog(SGOBJ *dlg, int/*SDL_Event */*pEventOut)
{
	int obj=0;
	int oldbutton=0;
	int retbutton=0;
	int i, j, b;
	int /*SDL_Event*/ sdlEvent;//SDL_Event sdlEvent;
	SDL_Rect rct;
	uint32_t grey;
	
	SDL_Rect dlgrect, bgrect;

        grey = 0xC0C0C0FF;

	dlgrect.x = dlg[0].x * sdlgui_fontwidth;
	dlgrect.y = dlg[0].y * sdlgui_fontheight;
	dlgrect.w = dlg[0].w * sdlgui_fontwidth;
	dlgrect.h = dlg[0].h * sdlgui_fontheight;

	bgrect.x = bgrect.y = 0;
	bgrect.w = dlgrect.w;
	bgrect.h = dlgrect.h;



	/* (Re-)draw the dialog */
	SDLGui_DrawDialog(dlg);

	input_gui();

        if(touch!=-1){b=1;boutc=1;}
	else {b=0;boutc=0;}

	i=gmx;j=gmy;

	/* If current object is the scrollbar, and mouse is still down, we can scroll it */
	/* also if the mouse pointer has left the scrollbar */
	if (dlg[current_object].type == SGSCROLLBAR) {
		if (b == 1/*& SDL_BUTTON(1))*/) {
			obj = current_object;
			dlg[obj].state |= SG_MOUSEDOWN;
			oldbutton = obj;
			retbutton = obj;

		}
		else {
			obj = current_object;
			current_object = 0;
			dlg[obj].state &= SG_MOUSEUP;
			//retbutton = obj;
			//oldbutton = obj;
			okold=1;

		}
	}
	else {
		obj = SDLGui_FindObj(dlg, i, j);
		current_object = obj;
		if (obj > 0 && (dlg[obj].flags&SG_TOUCHEXIT) )
		{
			oldbutton = obj;
			if (b ==1/*2*/ /*& SDL_BUTTON(1)*/)
			{
				dlg[obj].state |= SG_SELECTED;
				retbutton = obj;
			}else{
                                dlg[obj].state &= ~SG_SELECTED;				
			}
		}
	}

        /* The main loop */
	while (retbutton == 0 && !bQuitProgram)
	{

		input_gui();

		draw_cross(gmx,gmy);

                if(touch!=-1 && okold==0 ){

			okold=1;

			obj = SDLGui_FindObj(dlg, gmx, gmy);
				if (obj>0)
				{
					if (dlg[obj].type==SGBUTTON)
					{
						dlg[obj].state |= SG_SELECTED;
						SDLGui_DrawButton(dlg, obj);
						

						oldbutton=obj;//dbgoldobj=obj;
					}
                                        if (dlg[obj].type==SGSCROLLBAR)
					{
						dlg[obj].state |= SG_MOUSEDOWN;
						oldbutton=obj;

					}
					if ( dlg[obj].flags&SG_TOUCHEXIT )
					{
						dlg[obj].state |= SG_SELECTED;
						retbutton = obj;
					}
				}

		}
		else if(touch==-1 && okold==1){
			okold=0;
        
				// It was the left button: Find the object under the mouse cursor /
				obj = SDLGui_FindObj(dlg, gmx, gmy);
				if (obj>0)
				{
					switch (dlg[obj].type)
					{
					 case SGBUTTON:
						if (oldbutton==obj){
							retbutton=obj;}
						break;

					 case SGSCROLLBAR:
 						dlg[obj].state &= SG_MOUSEUP;

						if (oldbutton==obj)
							retbutton=obj;
						break;
					 case SGEDITFIELD:
						SDLGui_EditField(dlg, obj);
						break;
					 case SGRADIOBUT:
						for (i = obj-1; i > 0 && dlg[i].type == SGRADIOBUT; i--)
						{
							dlg[i].state &= ~SG_SELECTED;  // Deselect all radio buttons in this group /
						
							DrawBoxF((dlg[0].x+dlg[i].x)*fontwidth,(dlg[0].y+dlg[i].y)*fontheight,0,fontwidth ,fontheight,grey);
							SDLGui_DrawRadioButton(dlg, i);
							
						}
						for (i = obj+1; dlg[i].type == SGRADIOBUT; i++)
						{
							dlg[i].state &= ~SG_SELECTED;  // Deselect all radio buttons in this group /
							
                                                        DrawBoxF((dlg[0].x+dlg[i].x)*fontwidth,(dlg[0].y+dlg[i].y)*fontheight,0,fontwidth ,fontheight,grey);
							SDLGui_DrawRadioButton(dlg, i);
							
						}
						dlg[obj].state |= SG_SELECTED;  // Select this radio button 
						
                                                DrawBoxF((dlg[0].x+dlg[obj].x)*fontwidth,(dlg[0].y+dlg[obj].y)*fontheight,0,fontwidth ,fontheight,grey);
						SDLGui_DrawRadioButton(dlg, obj);
						
						break;
					 case SGCHECKBOX:
						dlg[obj].state ^= SG_SELECTED;
						
                                                DrawBoxF((dlg[0].x+dlg[obj].x)*fontwidth,(dlg[0].y+dlg[obj].y)*fontheight,0,fontwidth ,fontheight,grey);
						SDLGui_DrawCheckBox(dlg, obj);
						
						break;
					 case SGPOPUP:
						dlg[obj].state |= SG_SELECTED;
						SDLGui_DrawPopupButton(dlg, obj);
						
						retbutton=obj;
						break;
					}
				}
				if (oldbutton > 0)
				{	
					dlg[oldbutton].state &= ~SG_SELECTED;
					SDLGui_DrawButton(dlg, oldbutton);
					oldbutton = 0;//dbgoldobj=0;
				}
				if (obj >= 0 && (dlg[obj].flags&SG_EXIT))
				{
					if(dlg[obj].type==SGBUTTON)dlg[obj].state &= ~SG_SELECTED;
					retbutton = obj;
				}


		}

		if(retbutton ==0)retbutton = 1;//SG_TOUCHEXIT  ;

	}


	if (retbutton == SDLGUI_QUIT)
		bQuitProgram = true;

	return retbutton;
}

