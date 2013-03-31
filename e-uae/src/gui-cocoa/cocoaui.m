/*
 * UAE - The Un*x Amiga Emulator
 *
 * Interface to the Cocoa Mac OS X GUI
 *
 * Copyright 1996 Bernd Schmidt
 * Copyright 2004 Steven J. Saunders
 */
#include <stdlib.h>
#include <stdarg.h>

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gui.h"
#include "inputdevice.h"
#include "disk.h"

#ifdef USE_SDL
#include "SDL.h"
#endif

#import <Cocoa/Cocoa.h>

/* These prototypes aren't declared in the sdlgfx header for some reason */
extern void toggle_fullscreen (void);
extern int is_fullscreen (void);

/* Defined in SDLmain.m */
extern NSString *getApplicationName(void);

/* Prototypes */
int ensureNotFullscreen (void);
void restoreFullscreen (void);

/* Globals */
static NSString *currentFloppyPath = nil;
static BOOL wasFullscreen = NO; // used by ensureNotFullscreen() and restoreFullscreen()

/* This global is expected to be set (by code someplace else -
currently SDLMain.m) to the complete path to a config file if the app
was launched via the finder. */
NSString *finderLaunchFilename = nil;
extern BOOL gFinderLaunch; /* Set to YES by SDLMain.m if app launched from the finder */

/* Objective-C class for an object to respond to events */
@interface EuaeGui : NSObject
{
    NSString *applicationName;
    NSArray *diskImageTypes;
}
+ (id) sharedInstance;
- (void)createMenus;
- (void)createMenuItemInMenu:(NSMenu *)menu withTitle:(NSString *)title action:(SEL)anAction
    keyEquivalent:(NSString *)keyEquiv tag:(int)tag;
- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem;
- (void)insertDisk:(id)sender;
- (void)ejectDisk:(id)sender;
- (void)changePort0:(id)sender;
- (void)changePort1:(id)sender;
- (void)displayOpenPanelForInsertIntoDriveNumber:(int)driveNumber;
- (void)openPanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo;
@end

@implementation EuaeGui

+ (id) sharedInstance
{
    static id sharedInstance = nil;

    if (sharedInstance == nil) sharedInstance = [[self alloc] init];

    return sharedInstance;
}

-(EuaeGui *) init
{
    self = [super init];

    if (self) {
        applicationName = [[NSString alloc] initWithString:getApplicationName()];
        diskImageTypes =[[NSArray alloc] initWithObjects:@"adf", @"adz",
            @"zip", @"dms", @"fdi", nil]; // Note: Use lowercase for these
    }

    return self;
}

-(void) dealloc
{
    [applicationName release];
    [diskImageTypes release];
    [super dealloc];
}

-(NSArray *) diskImageTypes
{
    return diskImageTypes;
}

-(NSString *)applicationName
{
    return applicationName;
}

- (void)createMenus
{
    int driveNumber;
    NSMenuItem *menuItem;
    NSString *menuTitle;

    // Create a menu for disk operations
    NSMenu *diskMenu = [[NSMenu alloc] initWithTitle:@"Disk"];

    for (driveNumber=0; driveNumber<4; driveNumber++) {
        menuTitle = [[NSString alloc] initWithFormat:@"Insert DF%d:",driveNumber];
        [self createMenuItemInMenu:diskMenu withTitle:menuTitle action:@selector(insertDisk:) keyEquivalent:@"" tag:driveNumber];
        [menuTitle release];
    }

    [diskMenu addItem:[NSMenuItem separatorItem]];

    for (driveNumber=0; driveNumber<4; driveNumber++) {
        menuTitle = [[NSString alloc] initWithFormat:@"Eject DF%d:",driveNumber];
        [self createMenuItemInMenu:diskMenu withTitle:menuTitle action:@selector(ejectDisk:) keyEquivalent:@"" tag:driveNumber];
        [menuTitle release];
    }

    menuItem = [[NSMenuItem alloc] initWithTitle:@"Disk" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:diskMenu];

    [[NSApp mainMenu] insertItem:menuItem atIndex:1];

    [diskMenu release];
    [menuItem release];

    // Create a menu for input settings
    NSMenu *inputMenu = [[NSMenu alloc] initWithTitle:@"Devices"];

    NSMenu *port0Menu = [[NSMenu alloc] initWithTitle:@"Joystick Port 0"];
    NSMenu *port1Menu = [[NSMenu alloc] initWithTitle:@"Joystick Port 1"];

    [self createMenuItemInMenu:port0Menu withTitle:@"None" action:@selector(changePort0:) keyEquivalent:@"" tag:JSEM_NONE];
    [self createMenuItemInMenu:port0Menu withTitle:@"Mouse" action:@selector(changePort0:) keyEquivalent:@"" tag:JSEM_MICE];
    [self createMenuItemInMenu:port0Menu withTitle:@"First Joystick" action:@selector(changePort0:) keyEquivalent:@""
        tag:JSEM_JOYS];
    [self createMenuItemInMenu:port0Menu withTitle:@"Second Joystick" action:@selector(changePort0:) keyEquivalent:@""
        tag:JSEM_JOYS+1];
    [self createMenuItemInMenu:port0Menu withTitle:@"Numeric Keypad 2/4/6/8 + 5" action:@selector(changePort0:)
        keyEquivalent:@"" tag:JSEM_KBDLAYOUT];
    [self createMenuItemInMenu:port0Menu withTitle:@"Cursor Keys + Right Ctrl/Alt" action:@selector(changePort0:)
        keyEquivalent:@"" tag:JSEM_KBDLAYOUT+1];
    [self createMenuItemInMenu:port0Menu withTitle:@"Keyboard T/B/F/H + Left Alt" action:@selector(changePort0:)
        keyEquivalent:@"" tag:JSEM_KBDLAYOUT+2];

    menuItem = [[NSMenuItem alloc] initWithTitle:@"Joystick Port 0" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:port0Menu];
    [inputMenu addItem:menuItem];
    [menuItem release];

    [self createMenuItemInMenu:port1Menu withTitle:@"None" action:@selector(changePort1:) keyEquivalent:@"" tag:JSEM_NONE];
    [self createMenuItemInMenu:port1Menu withTitle:@"Mouse" action:@selector(changePort1:) keyEquivalent:@"" tag:JSEM_MICE];
    [self createMenuItemInMenu:port1Menu withTitle:@"First Joystick" action:@selector(changePort1:) keyEquivalent:@""
        tag:JSEM_JOYS];
    [self createMenuItemInMenu:port1Menu withTitle:@"Second Joystick" action:@selector(changePort1:) keyEquivalent:@""
        tag:JSEM_JOYS+1];
    [self createMenuItemInMenu:port1Menu withTitle:@"Numeric Keypad 2/4/6/8 + 5" action:@selector(changePort1:)
        keyEquivalent:@"" tag:JSEM_KBDLAYOUT];
    [self createMenuItemInMenu:port1Menu withTitle:@"Cursor Keys + Right Ctrl/Alt" action:@selector(changePort1:)
        keyEquivalent:@"" tag:JSEM_KBDLAYOUT+1];
    [self createMenuItemInMenu:port1Menu withTitle:@"Keyboard T/B/F/H + Left Alt" action:@selector(changePort1:)
        keyEquivalent:@"" tag:JSEM_KBDLAYOUT+2];

    menuItem = [[NSMenuItem alloc] initWithTitle:@"Joystick Port 1" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:port1Menu];
    [inputMenu addItem:menuItem];
    [menuItem release];

    menuItem = [[NSMenuItem alloc] initWithTitle:@"Devices" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:inputMenu];

    [[NSApp mainMenu] insertItem:menuItem atIndex:2];

    [inputMenu release];
    [menuItem release];

}

- (void)createMenuItemInMenu:(NSMenu *)menu withTitle:(NSString *)title action:(SEL)anAction
    keyEquivalent:(NSString *)keyEquiv tag:(int)tag;
{
    NSMenuItem *menuItem = [[NSMenuItem alloc] initWithTitle:title action:anAction keyEquivalent:keyEquiv];
    [menuItem setTag:tag];
    [menuItem setTarget:self];
    [menu addItem:menuItem];
    [menuItem release];
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
    SEL menuAction = [menuItem action];
    int tag = [menuItem tag];

    // Disabled drives can't have disks inserted or ejected
    if (((menuAction == @selector(insertDisk:)) || (menuAction == @selector(ejectDisk:))) &&
        (gui_data.drive_disabled[tag]))
        return NO;

    // Eject DFx should be disabled if there's no disk in DFx
    if ((menuAction == @selector(ejectDisk:)) && (disk_empty(tag)))
        return NO;

    // The current settings for the joystick/mouse ports should be indicated
    if (menuAction == @selector(changePort0:)) {
        if (currprefs.jport0 == tag) [menuItem setState:NSOnState];
        else [menuItem setState:NSOffState];

        // and joystick options should be unavailable if there are no joysticks
        if ((tag == JSEM_JOYS) || (tag == (JSEM_JOYS+1))) {
            if ((tag - JSEM_JOYS) >= inputdevice_get_device_total (IDTYPE_JOYSTICK))
                return NO;
        }

        // and we should not allow both ports to be set to the same setting
        if ((tag != JSEM_NONE) && (currprefs.jport1 == tag))
            return NO;

        return YES;
    }
    if (menuAction == @selector(changePort1:)) {
        if (currprefs.jport1 == tag) [menuItem setState:NSOnState];
        else [menuItem setState:NSOffState];

        // and joystick options should be unavailable if there are no joysticks
        if ((tag == JSEM_JOYS) || (tag == (JSEM_JOYS+1))) {
            if ((tag - JSEM_JOYS) >= inputdevice_get_device_total (IDTYPE_JOYSTICK))
                return NO;
        }

        // and we should not allow both ports to be set to the same setting
        if ((tag != JSEM_NONE) && (currprefs.jport0 == tag))
            return NO;

        return YES;
    }

    return YES;
}

// Invoked when the user selects one of the 'Insert DFx:' menu items
- (void)insertDisk:(id)sender
{
    [self displayOpenPanelForInsertIntoDriveNumber:[((NSMenuItem*)sender) tag]];
}

// Invoked when the user selects one of the 'Eject DFx:' menu items
- (void)ejectDisk:(id)sender
{
    disk_eject([((NSMenuItem*)sender) tag]);
}

// Invoked when the user selects an option from the 'Port 0' menu
- (void)changePort0:(id)sender
{
    changed_prefs.jport0 = [((NSMenuItem*)sender) tag];

    if( changed_prefs.jport0 != currprefs.jport0 )
        inputdevice_config_change();
}

// Invoked when the user selects an option from the 'Port 1' menu
- (void)changePort1:(id)sender
{
    changed_prefs.jport1 = [((NSMenuItem*)sender) tag];

    if( changed_prefs.jport1 != currprefs.jport1 )
        inputdevice_config_change();
}

- (void)displayOpenPanelForInsertIntoDriveNumber:(int)driveNumber
{
    ensureNotFullscreen();

    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
    [oPanel setTitle:[NSString stringWithFormat:@"%@: Insert Disk Image",applicationName]];

    // Make sure setMessage (OS X 10.3+) is available before calling it
    if ([oPanel respondsToSelector:@selector(setMessage:)])
        [oPanel setMessage:[NSString stringWithFormat:@"Select a Disk Image for DF%d:", driveNumber]];

    [oPanel setPrompt:@"Choose"];
    NSString *contextInfo = [[NSString alloc] initWithFormat:@"%d",driveNumber];

    [oPanel beginSheetForDirectory:currentFloppyPath file:nil
                             types:diskImageTypes
                    modalForWindow:[NSApp mainWindow]
                     modalDelegate:self
                    didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:)
                       contextInfo:contextInfo];
}

// Called when a floppy selection panel ended
- (void)openPanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
#if 0 // This currently breaks
    restoreFullscreen();
#endif

    if (returnCode != NSOKButton) return;

    int drive = [((NSString*)contextInfo) intValue];
    [((NSString*)contextInfo) release];

    if ((drive >= 0) && (drive < 4)) {
	    NSArray *files = [sheet filenames];
	    NSString *file = [files objectAtIndex:0];

	    strncpy (changed_prefs.df[drive], [file lossyCString], 255);
	    changed_prefs.df[drive][255] = '\0';

        if (currentFloppyPath != nil) {
            [currentFloppyPath release];
            currentFloppyPath = nil;
        }
	}
}

@end

/*
 * Revert to windowed mode if in fullscreen mode. Returns 1 if the
 * mode was initially fullscreen and was successfully changed. 0 otherwise.
 */
int ensureNotFullscreen (void)
{
    int result = 0;

    if (is_fullscreen ()) {
		toggle_fullscreen ();

		if (is_fullscreen ())
			write_log ("Cannot activate GUI in full-screen mode\n");
		else {
		  result = 1;
		  wasFullscreen = YES;
        }
        }
#ifdef USE_SDL
    // Un-hide the mouse
    SDL_ShowCursor(SDL_ENABLE);
#endif

    return result;
}

void restoreFullscreen (void)
{
#ifdef USE_SDL
    // Re-hide the mouse
    SDL_ShowCursor(SDL_DISABLE);
#endif

    if ((!is_fullscreen ()) && (wasFullscreen == YES))
        toggle_fullscreen();

    wasFullscreen = NO;
}

void gui_init (int argc, char **argv)
{
}

int gui_open (void)
{
    int result;
    NSString *nsHome;
    char *envhome = getenv("HOME");

    [[EuaeGui sharedInstance] createMenus];

    if (gFinderLaunch == YES) {
        static int run_once = 0;
        if (finderLaunchFilename == nil && !run_once) {
            run_once++;
            // Launched from finder without a config file filename, request one from the user
            NSArray *fileTypes = [NSArray arrayWithObjects:@"uaerc", nil];
            NSOpenPanel *oPanel = [NSOpenPanel openPanel];
            [oPanel setTitle:[NSString stringWithFormat:@"%@: Open Configuration File",
                [[EuaeGui sharedInstance] applicationName]]];

            result = [oPanel runModalForDirectory:nil file:nil types:fileTypes];

            if (result == NSOKButton) {
                NSArray *files = [oPanel filenames];
                NSString *file = [files objectAtIndex:0];
                cfgfile_load(&changed_prefs, [file lossyCString], 0);
            } else
                return -2;
        } else {
            // Looks like UAE was launched from the Finder with a file selected

            // What type of file is it?
            NSString *ext = [[finderLaunchFilename pathExtension] lowercaseString];

            // If it's a config file, load it
            if ([ext compare:@"uaerc"] == NSOrderedSame)
                cfgfile_load(&changed_prefs, [finderLaunchFilename lossyCString], 0);
            else {
                // If it's a disk image file, insert it into DF0
                // TODO: Make the drive it goes in configurable?
                if ([[[EuaeGui sharedInstance] diskImageTypes] containsObject:ext]) {
                    strncpy (changed_prefs.df[0], [finderLaunchFilename lossyCString], 255);
                    changed_prefs.df[0][255] = '\0';
                }
            }

            [finderLaunchFilename release];
            finderLaunchFilename = nil;
        }
    }

    /* Slight hack: We want to set the path that the file requester in gui_display()
		will start in to be the floppy_path from the config file, but only if a floppy_path
		was specified in the config. If none was specified prefs.path_floppy will
		be "$HOME/", so compare with that here. If it is "$HOME/" we assume that no
		floppy_path was set in the config file and leave currentFloppyPath as nil,
		which should cause OS X to choose something useful (such as the last used directory
															if uae has been run before.) */
    if ((currentFloppyPath == nil) && (envhome != NULL)) {
		const char *floppy_path = prefs_get_attr("floppy_path");

        nsHome = [NSString stringWithFormat:@"%s/",envhome];

		if ((floppy_path != NULL) && ([nsHome compare:[NSString stringWithCString:floppy_path]] != NSOrderedSame)) {
			currentFloppyPath = [NSString stringWithCString:floppy_path];
            [currentFloppyPath retain];
        }
    }

    return -1;
}

int gui_update (void)
{
    return 0;
}

void gui_exit (void)
{
    if (currentFloppyPath != nil) {
        [currentFloppyPath release];
        currentFloppyPath = nil;
    }
    if (finderLaunchFilename != nil) {
        [finderLaunchFilename release];
        finderLaunchFilename = nil;
    }
}

void gui_fps (int fps, int idle)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

void gui_led (int led, int on)
{
}

void gui_hd_led (int led)
{
    static int resetcounter;

    int old = gui_data.hd;

    if (led == 0) {
		resetcounter--;
		if (resetcounter > 0)
			return;
    }

    gui_data.hd = led;
    resetcounter = 6;
    if (old != gui_data.hd)
		gui_led (5, gui_data.hd);
}

void gui_cd_led (int led)
{
    static int resetcounter;

    int old = gui_data.cd;
    if (led == 0) {
		resetcounter--;
		if (resetcounter > 0)
			return;
    }

    gui_data.cd = led;
    resetcounter = 6;
    if (old != gui_data.cd)
		gui_led (6, gui_data.cd);
}

void gui_filename (int num, const char *name)
{
}

static void getline (char *p)
{
}

void gui_handle_events (void)
{
}

void gui_notify_state (int state)
{
}

void gui_display (int shortcut)
{
    int result;

    if ((shortcut >= 0) && (shortcut < 4)) {
        [[EuaeGui sharedInstance] displayOpenPanelForInsertIntoDriveNumber:shortcut];
    }
}

void gui_message (const char *format,...)
{
    char msg[2048];
    va_list parms;

    ensureNotFullscreen ();

    va_start (parms,format);
    vsprintf (msg, format, parms);
    va_end (parms);

    NSRunAlertPanel(nil, [NSString stringWithCString:msg], nil, nil, nil);

    write_log (msg);

    restoreFullscreen ();
}
