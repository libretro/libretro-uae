/***********************************************************/
//  BeUAE - The Be Un*x Amiga Emulator
//
//  BeOS port specific stuff
//
//  (c) 2004-2005 Richard Drummond
//  (c) 2000-2001 Axel Doerfler
//  (c) 1999 Be/R4 Sound - Raphael Moll
//  (c) 1998-1999 David Sowsy
//  (c) 1996-1998 Christian Bauer
//  (c) 1996 Patrick Hanevold
//
/***********************************************************/

#include "be-UAE.h"
#include "be-Window.h"

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"
#include "debug.h"
#include "xwin.h"
}

const char *kApplicationSignature = "application/x-vnd.UAE";
const char *kConfigurationMIMEType = "text/x-vnd.uae-config";

UAEWindow *gEmulationWindow;
BString gSettingsName;
display_mode gDisplayMode;	// original workspace display mode

extern BEntry	*gSettingsEntry;

extern int  argcCopy;
extern char **argvCopy;

void freeCopiedArgs();
void copyArgs(int argc,char **argv);


/*
 *  UAE Constructor: Initialize member variables
 */

UAE::UAE() : BApplication(kApplicationSignature)
{
/* Disable this for just now - it screws specifying
 * a config file on the command line. We need to get
 * OS-specific arguments in a different way.
 */
#if 0   
	// Find application directory and cwd to it
	app_info appInfo;
	GetAppInfo(&appInfo);

	BEntry appEntry(&appInfo.ref);
	BEntry appDirEntry;
	appEntry.GetParent(&appDirEntry);

	BPath appPath;
	appDirEntry.GetPath(&appPath);
	chdir(appPath.Path());
#endif
	RegisterMimeTypes();

	// Initialize other variables
	fEmulationWindow = NULL;
}


void UAE::RefsReceived(BMessage *msg)
{
	if (!IsLaunching())
		return;

	entry_ref ref;

	if (msg->FindRef("refs", &ref) == B_NO_ERROR)
	{
		BEntry entry(&ref);
		if (entry.InitCheck() == B_NO_ERROR)
		{
			BPath path;
			entry.GetPath(&path);

			// set default settings entry
			// gSettingsEntry = new BEntry(entry);

			int argc = 3;
			char **argv = new char *[argc];

			argv[0] = argvCopy[0];	// fake command line arguments
			argv[1] = "-f";
			argv[2] = (char *)path.Path();

			freeCopiedArgs();
			copyArgs(argc,argv);
		}
	}
}


void UAE::ReadyToRun(void)
{
	{
		BScreen screen;
		screen.GetMode(&gDisplayMode);
	}

	// Start the emulation thread
	fEmulationThread = spawn_thread(EmulationThreadFunc, "UAE 68k", B_NORMAL_PRIORITY, this);
	resume_thread(fEmulationThread);
}


bool UAE::QuitRequested(void)
{
	// Quit the thread
	uae_quit();

	status_t status;
	wait_for_thread(fEmulationThread, &status);

	ShowCursor();

	return true;
}


//  The UAE 68k thread's main function
long UAE::EmulationThreadFunc(void *obj)
{
	real_main(argcCopy,argvCopy);
	return 0;
}


status_t UAE::GetIconResource(char *name,icon_size size,BBitmap *dest) const
{
	if (size != B_LARGE_ICON && size != B_MINI_ICON )
		return B_ERROR;

	size_t len = 0;
	const void *data = AppResources()->LoadResource(size == B_LARGE_ICON ? 'ICON' : 'MICN',name,&len);

	if (data == NULL || len != (size_t)(size == B_LARGE_ICON ? 1024 : 256))
		return B_ERROR;

	dest->SetBits(data,(int32)len,0,B_COLOR_8_BIT);
	return B_OK;
}


void UAE::RegisterMimeTypes()
{
	/*** register the configuration MimeType ***/

	BMimeType mimeType(kConfigurationMIMEType);

	if (mimeType.InitCheck() != B_OK)
		return;

	if (mimeType.IsInstalled())	// work has already been done
	{
		mimeType.SetPreferredApp(kApplicationSignature,B_OPEN);
		mimeType.SetShortDescription("UAE configuration");
	}

	BBitmap largeIcon(BRect(0, 0, 31, 31), B_COLOR_8_BIT);
	BBitmap miniIcon(BRect(0, 0, 15, 15), B_COLOR_8_BIT);

	if (mimeType.GetIcon(&largeIcon,B_LARGE_ICON) != B_OK)	// no large icon
	{
		if (GetIconResource("uae:ConfigIcon",B_LARGE_ICON,&largeIcon) == B_OK)
			mimeType.SetIcon(&largeIcon, B_LARGE_ICON);
	}
	if (mimeType.GetIcon(&miniIcon,B_MINI_ICON) != B_OK)	// no mini icon
	{
		if (GetIconResource("uae:ConfigIcon",B_MINI_ICON,&miniIcon) == B_OK)
			mimeType.SetIcon(&miniIcon, B_MINI_ICON);
	}
}


void UAE::GraphicsLeave()
{
    if (fEmulationWindow) {
	fEmulationWindow->Hide();
	fEmulationWindow->PostMessage(B_QUIT_REQUESTED);
	fEmulationWindow = gEmulationWindow = 0;
    }
}


int UAE::GraphicsInit()
{
	if (currprefs.color_mode > 5) {
		write_log ("Bad color mode selected. Using default.\n");
		currprefs.color_mode = 0;
    }

	gfxvidinfo.width      = currprefs.gfx_width_win;
	gfxvidinfo.height     = currprefs.gfx_height_win;

	// Open window - at this point you could activate direct rendering
	fEmulationWindow = new UAEWindow(BRect(0,0,currprefs.gfx_width_win-1,currprefs.gfx_height_win-1),true);
	fEmulationWindow->Show();

	gEmulationWindow = fEmulationWindow;

	return true;
}


/*************************************************************************************/
//  functions called from UAE
//  #pragma mark -


int graphics_setup(void)
{
	return 1;
}


int graphics_init(void)
{
	return ((UAE *)be_app)->GraphicsInit();
}


void graphics_leave(void)
{
	((UAE *)be_app)->GraphicsLeave();
}

/*************************************************************************************/
//  #pragma mark -


void restoreWorkspaceResolution()
{
	BScreen screen;
	display_mode displayMode;

	if (screen.GetMode(&displayMode) == B_OK && memcmp(&displayMode,&gDisplayMode,sizeof(display_mode)))
		screen.SetMode(&gDisplayMode);
}

