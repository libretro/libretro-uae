/***********************************************************/
//  BeUAE - The Be Un*x Amiga Emulator
//
//  BeOS port specific stuff
//
//  (c) 2004-2007 Richard Drummond
//  (c) 2000-2001 Axel Doerfler
//  (c) 1999 Be/R4 Sound - Raphael Moll
//  (c) 1998-1999 David Sowsy
//  (c) 1996-1998 Christian Bauer
//  (c) 1996 Patrick Hanevold
//
/***********************************************************/

#include "be-UAE.h"
#include "be-Window.h"

#include <game/WindowScreen.h>

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "xwin.h"
#include "custom.h"
#include "drawing.h"
#include "picasso96.h"
#include "uae.h"
#include "inputdevice.h"
};

BPoint gOrigin (0, 0);
uint8	gWindowMode;
static class UAEWindow *gWin;

//#pragma mark -
//**************************************************
// c support functions
//**************************************************

static uint32 get_bytes_per_pixel (color_space colorspace)
{
    uint32 bytes_per_pixel;

    switch (colorspace) {
	case B_CMAP8:
	    bytes_per_pixel = 1;
	    break;
	case B_RGB15:
	case B_RGB16:
	    bytes_per_pixel = 2;
	    break;
	case B_RGB32:
	default:
	    bytes_per_pixel = 4;
	    break;
    }
    return bytes_per_pixel;
}

static void get_color_bitshifts (color_space colorspace,
                                 int *redBits,  int *greenBits,  int *blueBits,
                                 int *redShift, int *greenShift, int *blueShift)
{
    switch (colorspace) {
	case B_RGB15:
	    *redBits = *greenBits = *blueBits = 5;
	    *redShift = 10;  *greenShift = 5;  *blueShift = 0;
	    break;
	case B_RGB16:
	    *redBits = *blueBits = 5;  *greenBits = 6;
	    *redShift = 11;  *greenShift = 5;  *blueShift = 0;
	    break;
	case B_RGB32:
	default:
	    *redBits = *greenBits = *blueBits = 8;
	    *redShift = 16;  *greenShift = 8;  *blueShift = 0;
	    break;
    }
}

static uint32 get_screen_space (int width, int height, color_space colorspace)
{
    if (width <= 640 && height <= 480) {
	switch (colorspace) {
	    case B_CMAP8: return  B_8_BIT_640x480;
	    case B_RGB15: return B_15_BIT_640x480;
	    case B_RGB16: return B_16_BIT_640x480;
	    case B_RGB32:
	    default:      return B_32_BIT_640x480;
	}
    } else if (width <= 800 && height <= 600) {
	switch (colorspace) {
	    case B_CMAP8: return  B_8_BIT_800x600;
	    case B_RGB15: return B_15_BIT_800x600;
	    case B_RGB16: return B_16_BIT_800x600;
	    case B_RGB32:
	    default:      return B_32_BIT_800x600;
	}
    } else if (width <= 1024 && height <= 768) {
	switch (colorspace) {
	    case B_CMAP8: return  B_8_BIT_1024x768;
	    case B_RGB15: return B_15_BIT_1024x768;
	    case B_RGB16: return B_16_BIT_1024x768;
	    case B_RGB32:
	    default:      return B_32_BIT_1024x768;
	}
    } else if (width <= 1280 && height <= 1024) {
	switch (colorspace) {
	    case B_CMAP8: return  B_8_BIT_1280x1024;
	    case B_RGB15: return B_15_BIT_1280x1024;
	    case B_RGB16: return B_16_BIT_1280x1024;
	    case B_RGB32:
	    default:      return B_32_BIT_1280x1024;
	}
    }

    write_log ("BEOSGFX: Unsupported screen resolution.\n");
    return ~0UL;
}

static BPoint get_screen_centre (void)
{
    BScreen screen;
    BRect frame = screen.Frame ();
    BPoint size = frame.RightBottom ();

    return BPoint ((size.x + 1.0) / 2, (size.y + 1.0) / 2);
}


//#pragma mark -
//**************************************************
// UAE calls these functions to update the graphic display
//**************************************************

static int beos_lockscr (struct vidbuf_description *gfxinfo)
{
    return 1;
}

static void beos_unlockscr (struct vidbuf_description *gfxinfo)
{
}

static void beos_flush_line (struct vidbuf_description *gfxinfo, int line_no)
{
}

static void beos_flush_screen (struct vidbuf_description *gfxinfo, int first_line, int last_line)
{
}

static void beos_flush_block (struct vidbuf_description *gfxinfo, int first_line, int last_line)
{
    gEmulationWindow->DrawBlock (first_line, last_line);
}

static void beos_flush_clear_screen (struct vidbuf_description *gfxinfo)
{
}


//**************************************************

UAEWindow::UAEWindow(BRect frame,bool useBitmap)
	: BDirectWindow(frame, PACKAGE_NAME, B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fBitmap(NULL),
	fIsConnected(false)
{
    BScreen screen (this);
    color_space colorspace;

    colorspace = screen.ColorSpace ();
    InitColors (colorspace);

    // Move window to right position
    MoveTo (80, 80);

    // Create and set up off-screen buffer
    fBitmap = new BBitmap (BRect (0, 0, gfxvidinfo.width - 1, gfxvidinfo.height - 1), colorspace);

    gfxvidinfo.bufmem        = (uint8 *) fBitmap->Bits ();
    gfxvidinfo.rowbytes      =           fBitmap->BytesPerRow ();;
    gfxvidinfo.pixbytes      =           get_bytes_per_pixel (colorspace);
    gfxvidinfo.lockscr       =           beos_lockscr;
    gfxvidinfo.unlockscr     =           beos_unlockscr;
    gfxvidinfo.flush_block   =           beos_flush_block;
    gfxvidinfo.flush_screen  =           beos_flush_screen;
    gfxvidinfo.maxblocklines =           MAXBLOCKLINES_MAX;

    memset (gfxvidinfo.bufmem, 0, fBitmap->BitsLength ());

    reset_drawing();

    gWindowMode = kWindowBitmap;

    // Create bitmap view
    frame.OffsetTo(0.0,0.0);

    fBitmapView = new BitmapView (frame, fBitmap);
    AddChild (fBitmapView);

    if (currprefs.gfx_afullscreen)
	SetFullScreenMode (true);

    gWin = this;

    fDrawingLock = create_sem (0, "UAE drawing lock");
}

void UAEWindow::FrameResized (float width, float height)
{
}

void UAEWindow::InitColors (color_space colorspace)
{
    if (colorspace == B_CMAP8) {
	// clut color modes
	BScreen screen (this);
	int i = 0;

	for (int r = 0;r < 16;r++) {
	    for (int g = 0;g < 16;g++) {
		for (int b = 0;b < 16;b++)
		    xcolors[i++] = screen.IndexForColor(r << 4 | r, g << 4 | g, b << 4 | b);
	    }
	}
    } else {
	// high and true color modes
	int redBits, greenBits, blueBits;
	int redShift, greenShift, blueShift;

	get_color_bitshifts (colorspace, &redBits, &greenBits, &blueBits, &redShift, &greenShift, &blueShift);
	alloc_colors64k (redBits, greenBits, blueBits, redShift, greenShift, blueShift, 0, 0, 0, 0);
    }
}

void UAEWindow::UpdateBufferInfo (direct_buffer_info *info)
{
    fWindowBounds = info->window_bounds;

    if (IsFullScreen ())
	gOrigin.Set ((fWindowBounds.right  - fWindowBounds.left - gfxvidinfo.width)  / 2,
		     (fWindowBounds.bottom - fWindowBounds.top  - gfxvidinfo.height) / 2);
    else
	gOrigin.Set (0, 0);
}

void UAEWindow::DirectConnected (direct_buffer_info *info)
{
    switch (info->buffer_state & B_DIRECT_MODE_MASK) {
	case B_DIRECT_START:
	    // Start a direct screen connection.
	    fIsConnected = true;
	    UpdateBufferInfo (info);
	    release_sem (fDrawingLock);
	    break;

	case B_DIRECT_STOP:
	    // stop a direct screen connection.
  	    UpdateBufferInfo (info);
	    acquire_sem (fDrawingLock);
	    break;

	case B_DIRECT_MODIFY:
	    // Modify the state of a direct screen connection.
	    acquire_sem (fDrawingLock);
	    UpdateBufferInfo (info);
	    release_sem (fDrawingLock);
	    break;
    }
}

void UAEWindow::UnlockBuffer ()
{
}

uint8 *UAEWindow::LockBuffer ()
{
    return 0;
}

void UAEWindow::DrawLine (int y)
{
}

void UAEWindow::DrawBlock (int yMin, int yMax)
{
    if (Lock ()) {
	BRect src (0,yMin, gfxvidinfo.width -1, yMax);
	BRect dest (gOrigin.x,yMin + gOrigin.y, gfxvidinfo.width + gOrigin.x - 1, yMax + gOrigin.y);
	fBitmapView->DrawBitmapAsync (fBitmap, src, dest);
	Unlock ();
    }
}

void UAEWindow::SetFullScreenMode (bool full)
{
    Lock ();
    Sync ();

    if (!IsFullScreen ()) {
	color_space colorspace = BScreen(this).ColorSpace ();
	uint32 space = get_screen_space (gfxvidinfo.width, gfxvidinfo.height, colorspace);
	if (space != ~0UL)
	    set_screen_space (0, space, false);
    } else
	restoreWorkspaceResolution ();

    SetFullScreen (!IsFullScreen ());

    fScreenCentre = get_screen_centre ();

    fBitmapView->Invalidate ();

    Unlock ();

    be_app->HideCursor ();
}


bool UAEWindow::QuitRequested (void)
{
    gWindowMode = kWindowNone;

    be_app->PostMessage (B_QUIT_REQUESTED);
    return false;
}


void UAEWindow::UpdateMouse ()
{
    if (Lock ()) {
	if (IsActive ()) {
	    BPoint mousePoint;
	    uint32 mouseButtons;

	    fBitmapView->GetMouse (&mousePoint, &mouseButtons, true);

	    if (mousePoint.x >= 0 && mousePoint.x < gfxvidinfo.width &&
		mousePoint.y >= 0 && mousePoint.y < gfxvidinfo.height) {

		setmousebuttonstate (0, 0, mouseButtons & B_PRIMARY_MOUSE_BUTTON);
		setmousebuttonstate (0, 1, mouseButtons & B_SECONDARY_MOUSE_BUTTON);
		setmousebuttonstate (0, 2, mouseButtons & B_TERTIARY_MOUSE_BUTTON);
	    }

	    if (IsFullScreen ()) {
	    	/* Hack to get relative mouse movements when full-screen */
   		fBitmapView->GetMouse (&mousePoint, &mouseButtons, false);

		set_mouse_position ((int) fScreenCentre.x, (int) fScreenCentre.y);
		setmousestate (0, 0, (int) (mousePoint.x - fScreenCentre.x), 0);
  	        setmousestate (0, 1, (int) (mousePoint.y - fScreenCentre.y), 0);
	    }
	}
	Unlock ();
    }
}

void UAEWindow::MessageReceived (BMessage *msg)
{
}

//#pragma mark -
//**************************************************
// The BitmapView is the "indirect" graphics buffer
//**************************************************

BitmapView::BitmapView(BRect frame,BBitmap *bitmap)
	: BView(frame,"",B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{
    SetViewColor (0, 0, 0);
    fBitmap = bitmap;
}

BitmapView::~BitmapView ()
{
    delete fBitmap;
}

void BitmapView::Draw (BRect update)
{
   DrawBitmapAsync (fBitmap, gOrigin);
}

void BitmapView::Pulse (void)
{
}

void BitmapView::MouseMoved (BPoint point, uint32 transit, const BMessage *message)
{
    if (!gWin->IsFullScreen ()) {
	switch (transit) {
	    case B_ENTERED_VIEW:
		be_app->HideCursor ();
		/* Fall through */
	    case B_INSIDE_VIEW:
		setmousestate (0, 0, (int) point.x, 1);
		setmousestate (0, 1, (int) point.y, 1);
 		break;

	    case B_EXITED_VIEW:
		be_app->ShowCursor ();
		break;
	}
    }
}

void BitmapView::WindowActivated (bool active)
{
    if (active)
	inputdevice_acquire ();
    else
	inputdevice_unacquire ();
}

/*
 * Mouse inputdevice functions
 */

/* Hardwire for 3 axes and 3 buttons for now */
#define MAX_BUTTONS	3
#define MAX_AXES	3
#define FIRST_AXIS	0
#define FIRST_BUTTON	MAX_AXES

static int init_mouse (void)
{
    return 1;
}

static void close_mouse (void)
{
    return;
}

static int acquire_mouse (unsigned int num, int flags)
{
    return 1;
}

static void unacquire_mouse (unsigned int num)
{
    return;
}

static unsigned int get_mouse_num (void)
{
    return 1;
}

static const char *get_mouse_name (unsigned int mouse)
{
    return "Default mouse";
}

static unsigned int get_mouse_widget_num (unsigned int mouse)
{
    return MAX_AXES + MAX_BUTTONS;
}

static int get_mouse_widget_first (unsigned int mouse, int type)
{
    switch (type) {
	case IDEV_WIDGET_BUTTON:
	    return FIRST_BUTTON;
	case IDEV_WIDGET_AXIS:
	    return FIRST_AXIS;
    }
    return -1;
}

static int get_mouse_widget_type (unsigned int mouse, unsigned int num, char *name, uae_u32 *code)
{
    if (num >= MAX_AXES && num < MAX_AXES + MAX_BUTTONS) {
	if (name)
	    sprintf (name, "Button %d", num + 1 + MAX_AXES);
	return IDEV_WIDGET_BUTTON;
    } else if (num < MAX_AXES) {
	if (name)
	    sprintf (name, "Axis %d", num + 1);
	return IDEV_WIDGET_AXIS;
    }
    return IDEV_WIDGET_NONE;
}

static void read_mouse (void)
{
    gWin->UpdateMouse ();
}

struct inputdevice_functions inputdevicefunc_mouse = {
    init_mouse,
    close_mouse,
    acquire_mouse,
    unacquire_mouse,
    read_mouse,
    get_mouse_num,
    get_mouse_name,
    get_mouse_widget_num,
    get_mouse_widget_type,
    get_mouse_widget_first
};

/*
 * Default inputdevice config for BeOS mouse
 */
void input_get_default_mouse (struct uae_input_device *uid)
{
    /* Supports only one mouse */
    uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_MOUSE1_HORIZ;
    uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_MOUSE1_VERT;
    uid[0].eventid[ID_AXIS_OFFSET + 2][0]   = INPUTEVENT_MOUSE1_WHEEL;
    uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_FIRE_BUTTON;
    uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_2ND_BUTTON;
    uid[0].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY1_3RD_BUTTON;
    uid[0].enabled = 1;
}

/*
 * Handle gfx specific cfgfile options
 */
void gfx_default_options (struct uae_prefs *p)
{
}

void gfx_save_options (FILE *f, const struct uae_prefs *p)
{
}

int gfx_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

/*
 * Misc functions
 */
void screenshot (int mode)
{
    write_log ("Screenshot not supported yet\n");
}

int debuggable (void)
{
    return true;
}

int mousehack_allowed (void)
{
    return false;
}

void toggle_mousegrab (void)
{
}

int check_prefs_changed_gfx (void)
{
    return 0;
}

void toggle_fullscreen (void)
{
    gWin->SetFullScreenMode(0);
}

int is_fullscreen (void)
{
    return gWin->IsFullScreen ();
}

int is_vsync (void)
{
    return 0;
}

void graphics_notify_state (int state)
{
}

void handle_events (void)
{
}
