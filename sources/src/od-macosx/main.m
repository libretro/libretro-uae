/*
 * E-UAE - The portable Amiga emulator
 *
 * Main entry point for our Cocoa-ized app and support
 * routines.
 *
 * Based on SDLMain.m from libSDL.
 * Initial Version: Darrell Walisser <dwaliss1@purdue.edu>
 * Non-NIB-Code & other changes: Max Horn <max@quendi.de>
 *
 * Adaption for E-UAE:
 * Steven J. Saunders
 * Richard Drummond
 */

#import "sysconfig.h"
#import "sysdeps.h"

#import "options.h"
#import "uae.h"

#ifdef USE_SDL
#import "SDL.h"
#endif

#import "main.h"
#import <sys/param.h> /* for MAXPATHLEN */
#import <unistd.h>

/* Use this flag to determine whether we use CPS (docking) or not */
#define	SDL_USE_CPS	1

#ifdef SDL_USE_CPS
/* Portions of CPS.h */
typedef struct CPSProcessSerNum
{
    UInt32	lo;
    UInt32	hi;
} CPSProcessSerNum;

extern OSErr CPSGetCurrentProcess (CPSProcessSerNum *psn);
extern OSErr CPSEnableForegroundOperation (CPSProcessSerNum *psn, UInt32 _arg2, UInt32 _arg3, UInt32 _arg4, UInt32 _arg5);
extern OSErr CPSSetFrontProcess (CPSProcessSerNum *psn);

#endif /* SDL_USE_CPS */

// Function found in gui-cocoa/cocoaui.m
extern void cocoa_gui_early_setup (void);

static int     gArgc;
static char  **gArgv;
BOOL           gFinderLaunch = NO;
BOOL           gFinishedLaunching = NO;

NSString *getApplicationName ()
{
	NSDictionary *dict;
	NSString *appName = 0;

	/* Determine the application name */
	dict = (NSDictionary *) CFBundleGetInfoDictionary (CFBundleGetMainBundle ());
	if (dict)
		appName = [dict objectForKey: @"CFBundleName"];

	if (![appName length])
		appName = [[NSProcessInfo processInfo] processName];

	return appName;
}

/* Fix warnings generated when using the setAppleMenu method and compiling
in Tiger or later */
@interface NSApplication (PUAE)
- (void)setAppleMenu:(NSMenu *)menu;
@end


@interface PUAE_Application : NSApplication
@end

@implementation PUAE_Application

/* Invoked from the Quit menu item */
- (void)terminate:(id)sender
{
#ifdef USE_SDL
    /* Post a SDL_QUIT event */
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent (&event);
#else
    /* FIXME */
#endif
}

- (void)sendEvent:(NSEvent *)anEvent
{
    if (NSKeyDown == [anEvent type] || NSKeyUp == [anEvent type]) {
	if ([anEvent modifierFlags] & NSCommandKeyMask)
	    [super sendEvent: anEvent];
    } else
	[super sendEvent: anEvent];
}

@end


/* The main class of the application, the application's delegate */
@implementation PUAE_Main

/* Set the working directory to the .app's parent directory */
- (void) setupWorkingDirectory:(BOOL)shouldChdir
{
    if (shouldChdir) {
	char parentdir[MAXPATHLEN];
	CFURLRef url  = CFBundleCopyBundleURL (CFBundleGetMainBundle());
	CFURLRef url2 = CFURLCreateCopyDeletingLastPathComponent (0, url);

	if (CFURLGetFileSystemRepresentation (url2, true, (UInt8 *)parentdir, MAXPATHLEN)) {
	    chdir (parentdir) == 0;   /* chdir to the binary app's parent */
	}
	CFRelease (url);
	CFRelease (url2);
    }
}

-(void)performAbout:(id)sender
{
	[NSApp orderFrontStandardAboutPanelWithOptions: nil];
}

static void setApplicationMenu (void)
{
    /* warning: this code is very odd */
    NSMenu *appleMenu;
    NSMenuItem *menuItem;
    NSString *title;
    NSString *appName;

    appName = getApplicationName ();
    appleMenu = [[NSMenu alloc] initWithTitle:@""];

    /* Add menu items */
    title = [@"About " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = [@"Hide " stringByAppendingString:appName];
    /* E-UAE: Removed @"h" key equivalent so it doesn't interfere with using the command key as an emulated amiga key */
    [appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@""];

    /* E-UAE: Removed @"h" key equivalent so it doesn't interfere with using the command key as an emulated amiga key */
    menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@""];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

    [appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = [@"Quit " stringByAppendingString:appName];
    menuItem = [appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
	/* PUAE: Made the key equivalent Option-Cmd-q so it doesn't interfere with using the command key as an emulated amiga key */
	[menuItem setKeyEquivalentModifierMask:NSCommandKeyMask|NSAlternateKeyMask];

    /* Put menu into the menubar */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:appleMenu];
    [[NSApp mainMenu] addItem:menuItem];

    /* Tell the application object that this is now the application menu */
    [NSApp setAppleMenu:appleMenu];

    /* Finally give up our references to the objects */
    [appleMenu release];
    [menuItem release];
}

/* Create a window menu */
static void setupWindowMenu (void)
{
    NSMenu      *windowMenu;
    NSMenuItem  *windowMenuItem;
    NSMenuItem  *menuItem;

    windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

    /* "Minimize" item */
    /* E-UAE: Removed @"m" key equivalent so it doesn't interfere with using the command key as an emulated amiga key */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@""];
    [windowMenu addItem:menuItem];
    [menuItem release];

    /* Put menu into the menubar */
    windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
    [windowMenuItem setSubmenu:windowMenu];
    [[NSApp mainMenu] addItem:windowMenuItem];

    /* Tell the application object that this is now the window menu */
    [NSApp setWindowsMenu:windowMenu];

    /* Finally give up our references to the objects */
    [windowMenu release];
    [windowMenuItem release];
}

/* Replacement for NSApplicationMain */
static void CustomApplicationMain (int argc, char **argv)
{
    NSAutoreleasePool	*pool = [[NSAutoreleasePool alloc] init];
    PUAE_Main		*puae_main;

    /* Ensure the application object is initialised */
    [PUAE_Application sharedApplication];

#ifdef SDL_USE_CPS
    {
	CPSProcessSerNum PSN;
	/* Tell the dock about us */
	if (!CPSGetCurrentProcess(&PSN))
	    if (!CPSEnableForegroundOperation(&PSN,0x03,0x3C,0x2C,0x1103))
		if (!CPSSetFrontProcess(&PSN))
		    [PUAE_Application sharedApplication];
    }
#endif /* SDL_USE_CPS */

    /* Set up the menubar */
    [NSApp setMainMenu:[[NSMenu alloc] init]];
    setApplicationMenu ();
    setupWindowMenu ();

	/* Set up the UAE menus */
	cocoa_gui_early_setup();

    /* Create SDLMain and make it the app delegate */
    puae_main = [[PUAE_Main alloc] init];
    [NSApp setDelegate:puae_main];

    /* Start the main event loop */
    [NSApp run];

    [puae_main release];
    [pool release];
}

- (BOOL) applicationShouldOpenUntitledFile:(NSApplication *)sender
{
    return NO;
}

NSString *finderLaunchFilename = nil;
- (BOOL) application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	/* At the moment we are only supporting *launching* UAE by double-clicking
	 * a config or disk-image file in the Finder. So if the user attempts to
	 * dbl-click a file after the App is already running we return NO (failed)
	 * TODO: Make this work.
	 */
	if (gFinishedLaunching)
		return NO;
		
    if (filename != nil) {
	if (finderLaunchFilename != nil)
	    [finderLaunchFilename release];
	finderLaunchFilename = [[NSString alloc] initWithString:filename];
	return YES;
    }
	
    return NO;
}

/* Called when the internal event loop has just started running */
- (void) applicationDidFinishLaunching: (NSNotification *) note
{
    /* Set the working directory to the .app's parent directory */
    [self setupWorkingDirectory:gFinderLaunch];

	gFinishedLaunching = YES;
	
#ifdef USE_SDL
    setenv ("SDL_ENABLEAPPEVENTS", "1", 1);


    /* if (init_sdl ()) */
#endif
    {
	/* Hand off to main application code */
		
		/* If the application was launched by double-clicking a file in the
		 * Finder, then create a fake argc and argv to pass to real_main that
		 * contains the appropriate arguments for UAE to load that file.
		 */
		if (gFinderLaunch) {
			char *myArgv[3];
			char arg1[3] = { '-', 'f', '\0' };
			char arg2[MAX_PATH]; 
			
			myArgv[0] = gArgv[0];
			myArgv[1] = &arg1[0];
			myArgv[2] = &arg2[0];
				
			/* If we were launched from the Finder, but without a config file
			 * being selected, we need to ask for one now.
			 */
			if (finderLaunchFilename == nil) {
				// Try to use the same directory the user used last time they loaded a config file
				NSString *configPath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedConfigFilePath"];
				
				NSOpenPanel *oPanel = [NSOpenPanel openPanel];
				[oPanel setTitle:[NSString stringWithFormat:@"%@: Open Configuration File",getApplicationName()]];
				
				if ([oPanel respondsToSelector:@selector(setMessage:)])
					[oPanel setMessage:@"Select a Configuration File:"];
				[oPanel setPrompt:@"Choose"];
				
				int result = [oPanel runModalForDirectory:configPath file:nil types:[NSArray arrayWithObjects:@"uaerc", nil]];
				
				if (result == NSOKButton) {
					finderLaunchFilename = [[oPanel filenames] objectAtIndex:0];
					[[NSUserDefaults standardUserDefaults] setObject:[finderLaunchFilename stringByDeletingLastPathComponent] 
						forKey:@"LastUsedConfigFilePath"];
				}
				else {
					/* The user clicked the "Cancel" button so quit */
					exit(EXIT_SUCCESS);
				}
			}
			
			/* Check the extension of the file that was double-clicked.
			 * If it's ".uaerc" then we keep the switch argument set to "-f",
			 * otherwise it must be a disk image, so we change it to "-0"
			 */
			NSString *ext = [[finderLaunchFilename pathExtension] lowercaseString];
			if ([ext compare:@"uaerc"] != NSOrderedSame) 
				myArgv[1][1] = '0';
				
			/* Copy in the filename */
			NSData *data = [finderLaunchFilename dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
			[data getBytes:myArgv[2] length:MAX_PATH];
			NSUInteger len = [data length];
			myArgv[2][(len >= MAX_PATH) ? (MAX_PATH - 1) : len] = '\0';
			
			real_main(3, myArgv);
		} 
		else {
			real_main (gArgc, gArgv);
		}
    }

    if (finderLaunchFilename != nil) {
	[finderLaunchFilename release];
	finderLaunchFilename = nil;
    }

    /* We're done, thank you for playing */
    exit (EXIT_SUCCESS);
}

@end


#ifdef main
#  undef main
#endif

/* Main entry point to executable - should *not* be SDL_main! */
int main (int argc, char **argv)
{
    /* Copy the arguments into a global variable */
    /* This is passed if we are launched by double-clicking */
    if (argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
	gArgc = 1;
        gFinderLaunch = YES;
    } else {
	gArgc = argc;
	gFinderLaunch = NO;
    }
    gArgv = argv;

    CustomApplicationMain (argc, argv);

    return 0;
}


/* Dummy for now */
void setup_brkhandler (void)
{
}


/*
 * Handle target-specific cfgfile options
 */
void target_save_options (struct zfile *f, struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const TCHAR *option, const TCHAR *value)
{
    return 0;
}

void target_default_options (struct uae_prefs *p, int type)
{
	p->use_gl = 1;
}
