/***********************************************************
* UAE - The U*nix Amiga Emulator                           *
*                                                          *
* UAE-Control - Emulator control from inside emulation     *
*  (c) 1996 Tauno Taipaleenmaki <tataipal@raita.oulu.fi>   *
*  (c) 1998 Cloanto <info@cloanto.com>                     *
*                                                          *
* Version 1.1                                              *
*                                                          *
* Requires Amiga OS v. 36 or higher. Compiled with SAS/C   *
*                                                          *
* Users with KS 1.3 or below should use the command line   *
* version "uaectrl"                                        *
***********************************************************/

#define VERSION_STRING "\0$VER: UAE-Control 1.1"
#define VERSION_SHELL " UAE-Control v. 1.1\n © 1996 Tauno Taipaleenmaki, © 1998 Cloanto\n\n"

#include <intuition/intuition.h>
#ifndef _DCC
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#else
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>
#endif
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "UAE-Control.h"
#include "uae_pragmas.h"

#define MAX_DRV_NAME  25

struct Window *window;
struct Screen *scr;
struct Gadget *glist;
APTR VisInfo;
struct UAE_CONFIG config;

UBYTE *version_string = VERSION_STRING;

struct TextAttr topaz8 =
{
    (STRPTR) "topaz.font", 8, 0x00, 0x01
};

struct NewWindow newwindow =
{
    0, 0, 0, 0, 0, 1,
IDCMP_CLOSEWINDOW | BUTTONIDCMP | CYCLEIDCMP | INTEGERIDCMP | STRINGIDCMP,
    WFLG_DRAGBAR | WFLG_CLOSEGADGET | WFLG_ACTIVATE | WFLG_DEPTHGADGET,
    NULL, NULL, NULL, NULL, NULL,
    0, 0, 0, 0, CUSTOMSCREEN
};

int setup_window(void);
void quit_program(int error, char *text);
void print_drive_status(void);
BOOL get_string(UBYTE * buff, LONG max_len);


/*****************************************
* Main program                           *
*****************************************/
int main()
{
    int quit = 0, i;
    struct IntuiMessage *msg;
    struct Gadget *button;
    struct StringInfo *strinfo;
    char buf[257];
    UWORD koodi, msgID;
    ULONG classi, number;
    APTR address;
    ULONG CycleTags[3];

    CycleTags[0] = GTCY_Active;
    CycleTags[1] = (ULONG) & number;
    CycleTags[2] = TAG_DONE;

    /* Read UAE configuration */
    i = GetUaeConfig(&config);
    i = setup_window();
    if (i == 0) {
	quit_program(1, "Cannot setup a window!");
	return 1;
    }
    while (quit == 0) {
	WaitPort(window->UserPort);
	while (msg = (struct IntuiMessage *) GT_GetIMsg(window->UserPort)) {
	    classi = msg->Class;
	    koodi = msg->Code;
	    address = msg->IAddress;
	    if (classi == IDCMP_GADGETUP) {
		msgID = ((struct Gadget *) msg->IAddress)->GadgetID;
		button = (struct Gadget *) msg->IAddress;
		if (button->SpecialInfo) {
		    strinfo = (struct StringInfo *) button->SpecialInfo;
		}
	    } else
		msgID = msg->Code;

	    GT_ReplyIMsg((struct IntuiMessage *) msg);
	    switch (classi) {
	    case IDCMP_CLOSEWINDOW:
		quit = 1;
		break;
	    case IDCMP_GADGETUP:
		switch (msgID) {
		case GAD_EXITEMU:
		    ExitEmu();
		    break;
		case GAD_EJECT_DF0:
		    EjectDisk(0);
		    Delay(30);
		    GetUaeConfig(&config);
		    print_drive_status();
		    break;
		case GAD_EJECT_DF1:
		    EjectDisk(1);
		    Delay(30);
		    GetUaeConfig(&config);
		    print_drive_status();
		    break;
		case GAD_EJECT_DF2:
		    EjectDisk(2);
		    Delay(30);
		    GetUaeConfig(&config);
		    print_drive_status();
		    break;
		case GAD_EJECT_DF3:
		    EjectDisk(3);
		    Delay(30);
		    GetUaeConfig(&config);
		    print_drive_status();
		    break;
		case GAD_SOUND:
		    if (config.do_output_sound)
			DisableSound();
		    else
			EnableSound();

		    Delay(30);
		    GetUaeConfig(&config);
		    break;
		case GAD_JOYSTICK:
		    if (config.do_fake_joystick)
			DisableJoystick();
		    else
			EnableJoystick();

		    Delay(30);
		    GetUaeConfig(&config);
		    break;
		case GAD_FRAMERATE:
		    SetFrameRate(strinfo->LongInt);
		    Delay(30);
		    GetUaeConfig(&config);
		    break;
		case GAD_INSERT_DF0:
		    strcpy(buf, config.df0_name);
		    if (get_string(buf, 255)) {
			InsertDisk((UBYTE *) buf, 0);
			Delay(30);
			GetUaeConfig(&config);
			print_drive_status();
		    }
		    break;
		case GAD_INSERT_DF1:
		    strcpy(buf, config.df1_name);
		    if (get_string(buf, 255)) {
			InsertDisk((UBYTE *) buf, 1);
			Delay(30);
			GetUaeConfig(&config);
			print_drive_status();
		    }
		    break;
		case GAD_INSERT_DF2:
		    strcpy(buf, config.df2_name);
		    if (get_string(buf, 255)) {
			InsertDisk((UBYTE *) buf, 2);
			Delay(30);
			GetUaeConfig(&config);
			print_drive_status();
		    }
		    break;
		case GAD_INSERT_DF3:
		    strcpy(buf, config.df3_name);
		    if (get_string(buf, 255)) {
			InsertDisk((UBYTE *) buf, 3);
			Delay(30);
			GetUaeConfig(&config);
			print_drive_status();
		    }
		    break;
		case GAD_LANGUAGE:
		    number = config.keyboard;
		    number++;
		    if (number == 5)
			number = 0;
		    ChangeLanguage(number);
		    Delay(30);
		    GetUaeConfig(&config);
		    break;
		case GAD_RESET:
		    ColdReboot();
		    break;
		case GAD_DEBUG:
		    DebugFunc();
		    break;
		default:
		    break;
		}
		break;
	    default:
		break;
	    }
	}
    }
    quit_program(0, "");
    return (0);
}

/*****************************************
* Quits the program                      *
*****************************************/
void quit_program(int error, char *text)
{
    if (error > 0) {
	printf(VERSION_SHELL);
	printf(" ERROR: %s\n", text);
    }
    if (scr)
	UnlockPubScreen(NULL, scr);
    if (window)
	CloseWindow(window);
    if (VisInfo)
	FreeVisualInfo(VisInfo);
    if (glist)
	FreeGadgets(glist);
}

/*****************************************
* Opens up the window & sets the gadgets *
*****************************************/
int setup_window(void)
{
    struct Gadget *g;
    struct NewGadget ng;
    UWORD offy;
    ULONG drawtags[20];
    static const char *keyb_langs[] =
    {
	"US",
	"DE",
	"SE",
	"FR",
	"IT",
	NULL
    };
    scr = LockPubScreen(NULL);
    if (!scr)
	return (0);
    VisInfo = GetVisualInfo(scr, TAG_DONE);
    if (!VisInfo)
	return (0);
    offy = scr->WBorTop + scr->RastPort.TxHeight + 1;

    /* Create buttons */
    g = CreateContext(&glist);
    if (!g)
	return (0);

    ng.ng_TextAttr = &topaz8;
    ng.ng_VisualInfo = VisInfo;
    ng.ng_Flags = PLACETEXT_IN;
    ng.ng_LeftEdge = 284;
    ng.ng_TopEdge = offy + 86;
    ng.ng_Width = 96;
    ng.ng_Height = 15;
    ng.ng_GadgetText = (UBYTE *) "Hard Reset";
    ng.ng_GadgetID = GAD_RESET;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 105;
    ng.ng_GadgetText = (UBYTE *) "Debug";
    ng.ng_GadgetID = GAD_DEBUG;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 135;
    ng.ng_GadgetText = (UBYTE *) "Quit UAE";
    ng.ng_GadgetID = GAD_EXITEMU;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    /* Eject buttons */
    ng.ng_LeftEdge = 262;
    ng.ng_TopEdge = offy + 11;
    ng.ng_Width = 70;
    ng.ng_GadgetID = GAD_EJECT_DF0;
    ng.ng_GadgetText = (UBYTE *) "Eject";
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 28;
    ng.ng_GadgetID = GAD_EJECT_DF1;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 45;
    ng.ng_GadgetID = GAD_EJECT_DF2;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 62;
    ng.ng_GadgetID = GAD_EJECT_DF3;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    /* Insert buttons */
    ng.ng_LeftEdge = 332;
    ng.ng_TopEdge = offy + 11;
    ng.ng_GadgetID = GAD_INSERT_DF0;
    ng.ng_GadgetText = (UBYTE *) "Insert";
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 28;
    ng.ng_GadgetID = GAD_INSERT_DF1;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 45;
    ng.ng_GadgetID = GAD_INSERT_DF2;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_TopEdge = offy + 62;
    ng.ng_GadgetID = GAD_INSERT_DF3;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    /* Sound & Joystick buttons */
    ng.ng_LeftEdge = 133;
    ng.ng_TopEdge = 97;
    ng.ng_GadgetID = GAD_SOUND;
    ng.ng_GadgetText = (UBYTE *) "Sound:";
    ng.ng_Flags = PLACETEXT_LEFT;
    g = CreateGadget(CHECKBOX_KIND, g, &ng, GTCB_Checked, config.do_output_sound ? 1 : 0, TAG_DONE);

    ng.ng_TopEdge = 114;
    ng.ng_GadgetID = GAD_JOYSTICK;
    ng.ng_GadgetText = (UBYTE *) "Fake Joystick:";
    g = CreateGadget(CHECKBOX_KIND, g, &ng, GTCB_Checked, config.do_fake_joystick ? 1 : 0, TAG_DONE);

    ng.ng_TopEdge = 129;
    ng.ng_GadgetID = GAD_LANGUAGE;
    ng.ng_GadgetText = (UBYTE *) "Language:";
    g = CreateGadget(CYCLE_KIND, g, &ng,
		     GTCY_Labels, (ULONG) keyb_langs,
		     GTCY_Active, config.keyboard,
		     TAG_DONE);

    ng.ng_TopEdge = 146;
    ng.ng_GadgetID = GAD_FRAMERATE;
    ng.ng_GadgetText = (UBYTE *) "Framerate:";
    g = CreateGadget(INTEGER_KIND, g, &ng, GTIN_Number, config.framerate, TAG_DONE);

    newwindow.Title = "UAE-Control";
    newwindow.Width = 419;
    newwindow.Height = 171;
    newwindow.LeftEdge = 20;
    newwindow.TopEdge = 20;
    newwindow.FirstGadget = glist;
    newwindow.Screen = scr;	/* Store screen ptr */
    window = OpenWindow(&newwindow);
    if (!window)
	return (0);

    /* Draw texts etc... */
    SetAPen(window->RPort, 1);
    SetDrMd(window->RPort, JAM2);
    Move(window->RPort, 19, offy + 11 + 10);
    Text(window->RPort, (UBYTE *) "DF0:", 4);
    Move(window->RPort, 19, offy + 28 + 10);
    Text(window->RPort, (UBYTE *) "DF1:", 4);
    Move(window->RPort, 19, offy + 45 + 10);
    Text(window->RPort, (UBYTE *) "DF2:", 4);
    Move(window->RPort, 19, offy + 62 + 10);
    Text(window->RPort, (UBYTE *) "DF3:", 4);

    drawtags[0] = GT_VisualInfo;
    drawtags[1] = (ULONG) VisInfo;
    drawtags[2] = GTBB_Recessed;
    drawtags[3] = 1;
    drawtags[4] = TAG_DONE;
    DrawBevelBoxA(window->RPort, 14, offy + 10, 390, 17, (struct TagItem *) &drawtags);
    DrawBevelBoxA(window->RPort, 14, offy + 27, 390, 17, (struct TagItem *) &drawtags);
    DrawBevelBoxA(window->RPort, 14, offy + 44, 390, 17, (struct TagItem *) &drawtags);
    DrawBevelBoxA(window->RPort, 14, offy + 61, 390, 17, (struct TagItem *) &drawtags);

    print_drive_status();

    return (1);
}

void print_drive_status(void)
{
    char empty[80];
    int len;
    UWORD offy = scr->WBorTop + scr->RastPort.TxHeight + 1;

    for (len = 0; len < 80; len++)
	empty[len] = ' ';
    empty[79] = '\0';

    SetAPen(window->RPort, 1);
    SetDrMd(window->RPort, JAM2);

    Move(window->RPort, 59, offy + 11 + 10);
    if (config.disk_in_df0) {
	len = strlen(config.df0_name);
	if (len > MAX_DRV_NAME)
	    len = MAX_DRV_NAME;
	Text(window->RPort, (UBYTE *) config.df0_name, len);
    } else {
	Text(window->RPort, (UBYTE *) empty, MAX_DRV_NAME);
    }
    Move(window->RPort, 59, offy + 28 + 10);
    if (config.disk_in_df1) {
	len = strlen(config.df1_name);
	if (len > MAX_DRV_NAME)
	    len = MAX_DRV_NAME;
	Text(window->RPort, (UBYTE *) config.df1_name, len);
    } else {
	Text(window->RPort, (UBYTE *) empty, MAX_DRV_NAME);
    }
    Move(window->RPort, 59, offy + 45 + 10);
    if (config.disk_in_df2) {
	len = strlen(config.df2_name);
	if (len > MAX_DRV_NAME)
	    len = MAX_DRV_NAME;
	Text(window->RPort, (UBYTE *) config.df2_name, len);
    } else {
	Text(window->RPort, (UBYTE *) empty, MAX_DRV_NAME);
    }
    Move(window->RPort, 59, offy + 62 + 10);
    if (config.disk_in_df3) {
	len = strlen(config.df3_name);
	if (len > MAX_DRV_NAME)
	    len = MAX_DRV_NAME;
	Text(window->RPort, (UBYTE *) config.df3_name, len);
    } else {
	Text(window->RPort, (UBYTE *) empty, MAX_DRV_NAME);
    }
}

BOOL
get_string(UBYTE * buff, LONG max_len)
{
    struct Gadget *g, *sg;
    struct NewGadget ng;
    struct Gadget *gl;
    struct Window *win;
    struct IntuiMessage *msg;
    struct StringInfo *strinfo;
    UWORD koodi, msgID;
    ULONG classi;
    APTR address;
    UWORD offy;
    BOOL quit, ok;

    g = CreateContext(&gl);
    if (!g)
	return FALSE;

    offy = scr->WBorTop + scr->RastPort.TxHeight + 1;

    ng.ng_TextAttr = &topaz8;
    ng.ng_VisualInfo = VisInfo;
    ng.ng_Flags = PLACETEXT_IN;
    ng.ng_LeftEdge = 14;
    ng.ng_TopEdge = offy + 34;
    ng.ng_Width = 90;
    ng.ng_Height = 15;
    ng.ng_GadgetText = (UBYTE *) "OK";
    ng.ng_GadgetID = GAD_OK;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_LeftEdge = 246;
    ng.ng_TopEdge = offy + 34;
    ng.ng_GadgetText = (UBYTE *) "Cancel";
    ng.ng_GadgetID = GAD_CANCEL;
    g = CreateGadget(BUTTON_KIND, g, &ng, TAG_DONE);

    ng.ng_LeftEdge = 14;
    ng.ng_TopEdge = offy + 10;
    ng.ng_Width = 322;
    ng.ng_Height = 15;
    ng.ng_GadgetText = NULL;
    ng.ng_GadgetID = GAD_STRING;
    sg = CreateGadget(STRING_KIND, g, &ng,
		      GTST_String, buff,
		      GTST_MaxChars, max_len,
		      TAG_DONE);
    strinfo = (struct StringInfo *) sg->SpecialInfo;
    ok = FALSE;

    newwindow.Title = "Amiga Disk File";
    newwindow.Width = 350;
    newwindow.Height = 65;
    newwindow.FirstGadget = gl;
    newwindow.Screen = scr;

    newwindow.LeftEdge = scr->MouseX - (newwindow.Width / 2);
    if (newwindow.LeftEdge + newwindow.Width > scr->Width)
	newwindow.LeftEdge = scr->Width - newwindow.Width;

    newwindow.TopEdge = scr->MouseY - (newwindow.Height / 2);
    if (newwindow.TopEdge + newwindow.Height > scr->Height)
	newwindow.TopEdge = scr->Height - newwindow.Height;

    win = OpenWindow(&newwindow);
    if (win) {
	ActivateGadget(sg, win, NULL);
	quit = FALSE;
	while (!quit) {
	    WaitPort(win->UserPort);
	    while (msg = (struct IntuiMessage *) GT_GetIMsg(win->UserPort)) {
		classi = msg->Class;
		koodi = msg->Code;
		address = msg->IAddress;
		if (classi == IDCMP_GADGETUP)
		    msgID = ((struct Gadget *) msg->IAddress)->GadgetID;
		else
		    msgID = msg->Code;

		GT_ReplyIMsg((struct IntuiMessage *) msg);
		switch (classi) {
		case IDCMP_CLOSEWINDOW:
		    quit = TRUE;
		    break;
		case IDCMP_GADGETUP:
		    switch (msgID) {
		    case GAD_CANCEL:
			quit = TRUE;
			break;
		    case GAD_OK:
		    case GAD_STRING:
			strcpy(buff, strinfo->Buffer);
			quit = ok = TRUE;
			break;
		    }
		    break;
		default:
		    break;
		}
	    }
	}
	CloseWindow(win);
    }
    FreeGadgets(gl);

    return ok;
}
