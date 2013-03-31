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
# import "SDL.h"
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

static int     gArgc;
static char  **gArgv;
BOOL           gFinderLaunch;

NSString *getApplicationName (void)
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
@interface NSApplication (EUAE)
- (void)setAppleMenu:(NSMenu *)menu;
@end


@interface EUAE_Application : NSApplication
@end

@implementation EUAE_Application

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
@implementation EUAE_Main

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
    /* E-UAE: Removed @"q" key equivalent so it doesn't interfere with using the command key as an emulated amiga key */
    [appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@""];


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
    EUAE_Main		*euae_main;

    /* Ensure the application object is initialised */
    [EUAE_Application sharedApplication];

#ifdef SDL_USE_CPS
    {
	CPSProcessSerNum PSN;
	/* Tell the dock about us */
	if (!CPSGetCurrentProcess(&PSN))
	    if (!CPSEnableForegroundOperation(&PSN,0x03,0x3C,0x2C,0x1103))
		if (!CPSSetFrontProcess(&PSN))
		    [EUAE_Application sharedApplication];
    }
#endif /* SDL_USE_CPS */

    /* Set up the menubar */
    [NSApp setMainMenu:[[NSMenu alloc] init]];
    setApplicationMenu ();
    setupWindowMenu ();

    /* Create SDLMain and make it the app delegate */
    euae_main = [[EUAE_Main alloc] init];
    [NSApp setDelegate:euae_main];

    /* Start the main event loop */
    [NSApp run];

    [euae_main release];
    [pool release];
}

- (BOOL) applicationShouldOpenUntitledFile:(NSApplication *)sender
{
    return NO;
}

extern NSString *finderLaunchFilename;
- (BOOL) application:(NSApplication *)theApplication openFile:(NSString *)filename
{
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

#ifdef USE_SDL
    setenv ("SDL_ENABLEAPPEVENTS", "1", 1);


    if (init_sdl ()) 
#endif
    {
      
	/* Hand off to main application code */
	real_main (gArgc, gArgv);
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
    char logfile_path[MAX_PATH] = "~/Library/Logs/E-UAE.log";

    /* Copy the arguments into a global variable */
    /* This is passed if we are launched by double-clicking */
    if (argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
	gArgc = 1;
	gFinderLaunch = YES;
        gFinderLaunch = YES;
        cfgfile_subst_home (logfile_path, MAX_PATH);
        set_logfile (logfile_path);
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
void target_save_options (FILE *f, const struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

void target_default_options (struct uae_prefs *p)
{
}
