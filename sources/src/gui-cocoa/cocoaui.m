/*
 * UAE - The Un*x Amiga Emulator
 *
 * Interface to the Cocoa Mac OS X GUI
 *
 * Copyright 1996 Bernd Schmidt
 * Copyright 2004,2010 Steven J. Saunders
 *           2010,2011 Mustafa TUFAN
 */

#include <stdlib.h>
#include <stdarg.h>

#include "sysconfig.h"
#include "sysdeps.h"

#include "uae.h"
#include "options.h"
#include "gui.h"
#include "inputdevice.h"
#include "disk.h"
#include "ar.h"

#include "custom.h"
#include "xwin.h"
#include "drawing.h"
#include "savestate.h"

#ifdef USE_SDL
#include "SDL.h"
#endif

// MacOSX < 10.5
#ifndef NSINTEGER_DEFINED
#define NSINTEGER_DEFINED
#ifdef __LP64__ || NS_BUILD_32_LIKE_64
typedef long           NSInteger;
typedef unsigned long  NSUInteger;
#define NSIntegerMin   LONG_MIN
#define NSIntegerMax   LONG_MAX
#define NSUIntegerMax  ULONG_MAX
#else
typedef int            NSInteger;
typedef unsigned int   NSUInteger;
#define NSIntegerMin   INT_MIN
#define NSIntegerMax   INT_MAX
#define NSUIntegerMax  UINT_MAX
#endif
#endif

static unsigned long memsizes[] = {
        /* 0 */ 0,  
        /* 1 */ 0x00040000, /* 256K */
        /* 2 */ 0x00080000, /* 512K */
        /* 3 */ 0x00100000, /* 1M */
        /* 4 */ 0x00200000, /* 2M */
        /* 5 */ 0x00400000, /* 4M */
        /* 6 */ 0x00800000, /* 8M */
        /* 7 */ 0x01000000, /* 16M */
        /* 8 */ 0x02000000, /* 32M */
        /* 9 */ 0x04000000, /* 64M */
        /* 10*/ 0x08000000, //128M
        /* 11*/ 0x10000000, //256M
        /* 12*/ 0x20000000, //512M
        /* 13*/ 0x40000000, //1GB
        /* 14*/ 0x00180000, //1.5MB
        /* 15*/ 0x001C0000, //1.8MB
        /* 16*/ 0x80000000, //2GB
        /* 17*/ 0x18000000, //384M
        /* 18*/ 0x30000000, //768M
        /* 19*/ 0x60000000, //1.5GB
        /* 20*/ 0xA8000000, //2.5GB
        /* 21*/ 0xC0000000, //3GB
};

int quickstart_model = 0, quickstart_conf = 0, quickstart_compa = 1;
int quickstart_floppy = 1, quickstart_cd = 0, quickstart_ntsc = 0;
int quickstart_cdtype = 0;
char quickstart_cddrive[16];
int quickstart_ok, quickstart_ok_floppy;
extern TCHAR config_filename[256];
//----------

#import <Cocoa/Cocoa.h>

#define COCOA_GUI_MAX_PATH MAX_DPATH

/* These prototypes aren't declared in the sdlgfx header for some reason */
extern void toggle_fullscreen (int mode);
extern int is_fullscreen (void);

/* Defined in SDLmain.m */
extern NSString *getApplicationName(void);

/* Prototypes */
int ensureNotFullscreen (void);
void restoreFullscreen (void);

/* Globals */
static BOOL wasFullscreen = NO; // used by ensureNotFullscreen() and restoreFullscreen()

/* Objective-C class for an object to respond to events */
@interface PuaeGui : NSObject
{
	NSString *applicationName;
	NSArray *diskImageTypes;
	NSArray *CDImageTypes;
	NSArray *HDDFileTypes;
	NSArray *KickRomTypes;
	NSArray *FlashRamTypes;
	NSArray *CartridgeTypes;
	NSArray *SaveStateTypes;
}
+ (id) sharedInstance;
- (void)createMenus;
- (void)createMenuItemInMenu:(NSMenu *)menu withTitle:(NSString *)title action:(SEL)anAction tag:(int)tag;
- (void)createMenuItemInMenu:(NSMenu *)menu withTitle:(NSString *)title action:(SEL)anAction tag:(int)tag keyEquivalent:(NSString *)keyEquiv keyEquivalentMask:(NSUInteger)mask;
- (BOOL)validateMenuItem:(id <NSMenuItem>)item;
- (void)insertDisk:(id)sender;
- (void)ejectDisk:(id)sender;
- (void)ejectAllDisks:(id)sender;
- (void)changePort0:(id)sender;
- (void)changePort1:(id)sender;
- (void)swapGamePorts:(id)sender;
- (void)displayOpenPanelForInsertIntoDriveNumber:(int)driveNumber;
- (void)openPanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo;
- (void)resetAmiga:(id)sender;
- (void)pauseAmiga:(id)sender;
#ifdef ACTION_REPLAY
- (void)actionReplayFreeze:(id)sender;
#endif
- (void)grabMouse:(id)sender;
- (void)goFullscreen:(id)sender;
- (void)toggleInhibitDisplay:(id)sender;
- (void)changeChipMem:(id)sender;
- (void)changeBogoMem:(id)sender;
- (void)changeFastMem:(id)sender;
- (void)changeZ3FastMem:(id)sender;
- (void)changeZ3ChipMem:(id)sender;
//- (void)changeGfxMem:(id)sender;
- (void)changeCPU:(id)sender;
- (void)changeCPUSpeed:(id)sender;
- (void)changeFPU:(id)sender;
- (void)changeMonitoremu:(id)sender;
- (void)changeBlitter:(id)sender;
- (void)changeCollision:(id)sender;
@end

@implementation PuaeGui

+ (id) sharedInstance
{
	static id sharedInstance = nil;
	if (sharedInstance == nil) sharedInstance = [[self alloc] init];

	return sharedInstance;
}

-(PuaeGui *) init
{
	self = [super init];

	if (self) {
		applicationName = [[NSString alloc] initWithString:getApplicationName()];
		diskImageTypes =[[NSArray alloc] initWithObjects:@"adf", @"adz", @"gz", @"exe", @"zip", @"dms", @"fdi", 
#ifdef CAPS        
            @"ipf",
#endif
            nil]; // Note: Use lowercase for these

		CDImageTypes =[[NSArray alloc] initWithObjects:@"cue", @"ccd", @"mds", @"iso", nil];
		HDDFileTypes =[[NSArray alloc] initWithObjects:@"hdf", @"vhd", @"rdr", @"hdz", @"rdz", nil];
		KickRomTypes =[[NSArray alloc] initWithObjects:@"rom", @"roz", nil];
		FlashRamTypes =[[NSArray alloc] initWithObjects:@"nvr", nil];
		CartridgeTypes =[[NSArray alloc] initWithObjects:@"crt", @"rom", @"roz", nil];
		SaveStateTypes =[[NSArray alloc] initWithObjects:@"uss", nil];
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

	// Create a menu for manipulating the emulated amiga
	NSMenu *vAmigaMenu = [[NSMenu alloc] initWithTitle:@"Machine"];
	
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Reset" action:@selector(resetAmiga:) tag:0];
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Hard Reset" action:@selector(resetAmiga:) tag:1];
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Leds on Screen" action:@selector(LedsOnScreen:) tag:2];
	
#ifdef ACTION_REPLAY
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Action Replay Freeze" action:@selector(actionReplayFreeze:) tag:0];
#endif
	[vAmigaMenu addItem:[NSMenuItem separatorItem]];

	// quick starts
	NSMenu *quickstartMenu = [[NSMenu alloc] initWithTitle:@"QuickStart"];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 500" action:@selector(QuickStart:) tag:0];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 500+" action:@selector(QuickStart:) tag:1];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 600" action:@selector(QuickStart:) tag:2];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 1000" action:@selector(QuickStart:) tag:3];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 1200" action:@selector(QuickStart:) tag:4];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 3000" action:@selector(QuickStart:) tag:5];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 4000" action:@selector(QuickStart:) tag:6];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Amiga 4000T" action:@selector(QuickStart:) tag:7];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"CD32" action:@selector(QuickStart:) tag:8];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"CDTV" action:@selector(QuickStart:) tag:9];
		[self createMenuItemInMenu:quickstartMenu withTitle:@"Arcadia" action:@selector(QuickStart:) tag:10];
	menuItem = [[NSMenuItem alloc] initWithTitle:@"QuickStart" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:quickstartMenu];
	[vAmigaMenu addItem:menuItem];
	[menuItem release];
	[quickstartMenu release];

	[vAmigaMenu addItem:[NSMenuItem separatorItem]];

	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Save State" action:@selector(selectSaveState:) tag:0];
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Load State" action:@selector(selectLoadState:) tag:0];

	[vAmigaMenu addItem:[NSMenuItem separatorItem]];

	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Select Kick ROM" action:@selector(selectKickROM:) tag:0];
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Select Cartridge ROM" action:@selector(selectCartridge:) tag:0];
	[self createMenuItemInMenu:vAmigaMenu withTitle:@"Select Flash RAM" action:@selector(selectFlashRAM:) tag:0];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"Machine" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:vAmigaMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:1];
	[menuItem release];
	[vAmigaMenu release];

	// DISK MENU START
	NSMenu *dskMenu = [[NSMenu alloc] initWithTitle:@"Disk"];

		// Add menu items for inserting into floppy drives 1 - 4
		NSMenu *insertFloppyMenu = [[NSMenu alloc] initWithTitle:@"Insert Floppy"];
			for (driveNumber=0; driveNumber<4; driveNumber++) {
				[self createMenuItemInMenu:insertFloppyMenu withTitle:[NSString stringWithFormat:@"DF%d...",driveNumber] action:@selector(insertDisk:) tag:driveNumber];
			}

		menuItem = [[NSMenuItem alloc] initWithTitle:@"Insert Floppy" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:insertFloppyMenu];
		[dskMenu addItem:menuItem];
		[menuItem release];

		[dskMenu addItem:[NSMenuItem separatorItem]];

		// Add menu items for ejecting from floppy drives 1 - 4
		NSMenu *ejectFloppyMenu = [[NSMenu alloc] initWithTitle:@"Eject Floppy"];
			[self createMenuItemInMenu:ejectFloppyMenu withTitle:@"All" action:@selector(ejectAllDisks:) tag:0];
			[ejectFloppyMenu addItem:[NSMenuItem separatorItem]];
			for (driveNumber=0; driveNumber<4; driveNumber++) {
				[self createMenuItemInMenu:ejectFloppyMenu withTitle:[NSString stringWithFormat:@"DF%d",driveNumber] action:@selector(ejectDisk:) tag:driveNumber];
			}
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Eject Floppy" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:ejectFloppyMenu];
		[dskMenu addItem:menuItem];
		[menuItem release];

		[dskMenu addItem:[NSMenuItem separatorItem]];

		NSMenu *hddMenu = [[NSMenu alloc] initWithTitle:@"Mount HDF"];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Mount HDF" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:hddMenu];
		[dskMenu addItem:menuItem];
		[menuItem release];

		NSMenu *vhddMenu = [[NSMenu alloc] initWithTitle:@"Mount Folder as HDD"];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Mount Folder as HDD" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:vhddMenu];
		[dskMenu addItem:menuItem];
		[menuItem release];

		NSMenu *CDMenu = [[NSMenu alloc] initWithTitle:@"Mount CD Image"];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Mount CD Image" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:CDMenu];
		[dskMenu addItem:menuItem];
		[menuItem release];

		NSMenu *CDEMenu = [[NSMenu alloc] initWithTitle:@"Eject CD Image"];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Eject CD Image" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:CDEMenu];
		[dskMenu addItem:menuItem];
		[menuItem release];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"Disk" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:dskMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:2];
	[dskMenu release];
	[menuItem release];
	// DISK MENU END

	// MEM MENU START
	NSMenu *memMenu = [[NSMenu alloc] initWithTitle:@"Memory"];

		NSMenu *chipMenu = [[NSMenu alloc] initWithTitle:@"Chip Mem"];
			[self createMenuItemInMenu:chipMenu withTitle:@"256 KB" action:@selector(changeChipMem:) tag:1];
			[self createMenuItemInMenu:chipMenu withTitle:@"512 KB" action:@selector(changeChipMem:) tag:2];
			[self createMenuItemInMenu:chipMenu withTitle:@"1 MB" action:@selector(changeChipMem:) tag:3];
			[self createMenuItemInMenu:chipMenu withTitle:@"1.5 MB" action:@selector(changeChipMem:) tag:14];
			[self createMenuItemInMenu:chipMenu withTitle:@"2 MB" action:@selector(changeChipMem:) tag:4];
			[self createMenuItemInMenu:chipMenu withTitle:@"4 MB" action:@selector(changeChipMem:) tag:5];
			[self createMenuItemInMenu:chipMenu withTitle:@"8 MB" action:@selector(changeChipMem:) tag:6];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Chip Mem" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:chipMenu];
		[memMenu addItem:menuItem];
		[menuItem release];

		NSMenu *bogoMenu = [[NSMenu alloc] initWithTitle:@"Slow Mem"];
			[self createMenuItemInMenu:bogoMenu withTitle:@"None" action:@selector(changeBogoMem:) tag:0];
			[self createMenuItemInMenu:bogoMenu withTitle:@"512 KB" action:@selector(changeBogoMem:) tag:2];
			[self createMenuItemInMenu:bogoMenu withTitle:@"1 MB" action:@selector(changeBogoMem:) tag:3];
			[self createMenuItemInMenu:bogoMenu withTitle:@"1.5 MB" action:@selector(changeBogoMem:) tag:14];
			[self createMenuItemInMenu:bogoMenu withTitle:@"1.8 MB" action:@selector(changeBogoMem:) tag:15];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Slow Mem" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:bogoMenu];
		[memMenu addItem:menuItem];
		[menuItem release];

		NSMenu *fastMenu = [[NSMenu alloc] initWithTitle:@"Fast Mem"];
			[self createMenuItemInMenu:fastMenu withTitle:@"None" action:@selector(changeFastMem:) tag:0];
			[self createMenuItemInMenu:fastMenu withTitle:@"1 MB" action:@selector(changeFastMem:) tag:3];
			[self createMenuItemInMenu:fastMenu withTitle:@"2 MB" action:@selector(changeFastMem:) tag:4];
			[self createMenuItemInMenu:fastMenu withTitle:@"4 MB" action:@selector(changeFastMem:) tag:5];
			[self createMenuItemInMenu:fastMenu withTitle:@"8 MB" action:@selector(changeFastMem:) tag:6];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Fast Mem" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:fastMenu];
		[memMenu addItem:menuItem];
		[menuItem release];

		NSMenu *z3fastMenu = [[NSMenu alloc] initWithTitle:@"Z3 Fast Mem"];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"None" action:@selector(changeZ3FastMem:) tag:0];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"1 MB" action:@selector(changeZ3FastMem:) tag:3];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"2 MB" action:@selector(changeZ3FastMem:) tag:4];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"4 MB" action:@selector(changeZ3FastMem:) tag:5];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"8 MB" action:@selector(changeZ3FastMem:) tag:6];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"16 MB" action:@selector(changeZ3FastMem:) tag:7];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"32 MB" action:@selector(changeZ3FastMem:) tag:8];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"64 MB" action:@selector(changeZ3FastMem:) tag:9];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"128 MB" action:@selector(changeZ3FastMem:) tag:10];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"256 MB" action:@selector(changeZ3FastMem:) tag:11];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"384 MB" action:@selector(changeZ3FastMem:) tag:17];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"512 MB" action:@selector(changeZ3FastMem:) tag:12];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"768 MB" action:@selector(changeZ3FastMem:) tag:18];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"1 GB" action:@selector(changeZ3FastMem:) tag:13];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"1.5 GB" action:@selector(changeZ3FastMem:) tag:19];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"2 GB" action:@selector(changeZ3FastMem:) tag:16];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"2.5 GB" action:@selector(changeZ3FastMem:) tag:20];
			[self createMenuItemInMenu:z3fastMenu withTitle:@"3 GB" action:@selector(changeZ3FastMem:) tag:21];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Z3 Fast Mem" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:z3fastMenu];
		[memMenu addItem:menuItem];
		[menuItem release];

		NSMenu *z3chipMenu = [[NSMenu alloc] initWithTitle:@"32bit Chip Mem"];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"None" action:@selector(changeZ3ChipMem:) tag:0];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"16 MB" action:@selector(changeZ3ChipMem:) tag:7];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"32 MB" action:@selector(changeZ3ChipMem:) tag:8];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"64 MB" action:@selector(changeZ3ChipMem:) tag:9];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"128 MB" action:@selector(changeZ3ChipMem:) tag:10];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"256 MB" action:@selector(changeZ3ChipMem:) tag:11];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"512 MB" action:@selector(changeZ3ChipMem:) tag:12];
			[self createMenuItemInMenu:z3chipMenu withTitle:@"1 GB" action:@selector(changeZ3ChipMem:) tag:13];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"32bit Chip Mem" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:z3chipMenu];
		[memMenu addItem:menuItem];
		[menuItem release];

/*		NSMenu *gfxMenu = [[NSMenu alloc] initWithTitle:@"Gfx Mem"];
			[self createMenuItemInMenu:gfxMenu withTitle:@"None" action:@selector(changeGfxMem:) tag:0];
			[self createMenuItemInMenu:gfxMenu withTitle:@"1 MB" action:@selector(changeGfxMem:) tag:3];
			[self createMenuItemInMenu:gfxMenu withTitle:@"2 MB" action:@selector(changeGfxMem:) tag:4];
			[self createMenuItemInMenu:gfxMenu withTitle:@"4 MB" action:@selector(changeGfxMem:) tag:5];
			[self createMenuItemInMenu:gfxMenu withTitle:@"8 MB" action:@selector(changeGfxMem:) tag:6];
			[self createMenuItemInMenu:gfxMenu withTitle:@"16 MB" action:@selector(changeGfxMem:) tag:7];
			[self createMenuItemInMenu:gfxMenu withTitle:@"32 MB" action:@selector(changeGfxMem:) tag:8];
			[self createMenuItemInMenu:gfxMenu withTitle:@"64 MB" action:@selector(changeGfxMem:) tag:9];
			[self createMenuItemInMenu:gfxMenu withTitle:@"128 MB" action:@selector(changeGfxMem:) tag:10];
			[self createMenuItemInMenu:gfxMenu withTitle:@"256 MB" action:@selector(changeGfxMem:) tag:11];
			[self createMenuItemInMenu:gfxMenu withTitle:@"512 MB" action:@selector(changeGfxMem:) tag:12];
			[self createMenuItemInMenu:gfxMenu withTitle:@"1 GB" action:@selector(changeGfxMem:) tag:13];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Gfx Mem" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:gfxMenu];
		[memMenu addItem:menuItem];
		[menuItem release];*/

	menuItem = [[NSMenuItem alloc] initWithTitle:@"Memory" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:memMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:3];
	[memMenu release];
	[menuItem release];
	// MEM MENU END

	// SYSTEM MENU START
	NSMenu *systemMenu = [[NSMenu alloc] initWithTitle:@"System"];

		NSMenu *cpuMenu = [[NSMenu alloc] initWithTitle:@"CPU"];
			[self createMenuItemInMenu:cpuMenu withTitle:@"68000" action:@selector(changeCPU:) tag:0];
			[self createMenuItemInMenu:cpuMenu withTitle:@"68010" action:@selector(changeCPU:) tag:1];
			[self createMenuItemInMenu:cpuMenu withTitle:@"68020" action:@selector(changeCPU:) tag:2];
			[self createMenuItemInMenu:cpuMenu withTitle:@"68030" action:@selector(changeCPU:) tag:3];
			[self createMenuItemInMenu:cpuMenu withTitle:@"68040" action:@selector(changeCPU:) tag:4];
			[self createMenuItemInMenu:cpuMenu withTitle:@"68060" action:@selector(changeCPU:) tag:6];
			[cpuMenu addItem:[NSMenuItem separatorItem]];
			[self createMenuItemInMenu:cpuMenu withTitle:@"24-bit Addressing" action:@selector(changeCPU:) tag:10];
			[self createMenuItemInMenu:cpuMenu withTitle:@"More Compatible" action:@selector(changeCPU:) tag:11];
#ifdef MMUEMU
			[self createMenuItemInMenu:cpuMenu withTitle:@"68040 MMU" action:@selector(changeCPU:) tag:12];
#endif
		menuItem = [[NSMenuItem alloc] initWithTitle:@"CPU" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:cpuMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *cpuspeedMenu = [[NSMenu alloc] initWithTitle:@"CPU Speed"];
			[self createMenuItemInMenu:cpuspeedMenu withTitle:@"Fastest Possible but maintain chipset timing" action:@selector(changeCPUSpeed:) tag:0];
			[self createMenuItemInMenu:cpuspeedMenu withTitle:@"Approximate A500/A1200 Cycle Exact" action:@selector(changeCPUSpeed:) tag:1];
			[self createMenuItemInMenu:cpuspeedMenu withTitle:@"Cycle Exact" action:@selector(changeCPUSpeed:) tag:2];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"CPU Speed" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:cpuspeedMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *fpuMenu = [[NSMenu alloc] initWithTitle:@"FPU"];
			[self createMenuItemInMenu:fpuMenu withTitle:@"None" action:@selector(changeFPU:) tag:0];
			[self createMenuItemInMenu:fpuMenu withTitle:@"68881" action:@selector(changeFPU:) tag:1];
			[self createMenuItemInMenu:fpuMenu withTitle:@"68882" action:@selector(changeFPU:) tag:2];
			[self createMenuItemInMenu:fpuMenu withTitle:@"CPU Internal" action:@selector(changeFPU:) tag:3];
			[fpuMenu addItem:[NSMenuItem separatorItem]];
			[self createMenuItemInMenu:fpuMenu withTitle:@"More Compatible" action:@selector(changeFPU:) tag:10];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"FPU" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:fpuMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *chipsetMenu = [[NSMenu alloc] initWithTitle:@"Chipset"];
			[self createMenuItemInMenu:chipsetMenu withTitle:@"OCS" action:@selector(changeChipset:) tag:0];
			[self createMenuItemInMenu:chipsetMenu withTitle:@"ECS Agnus" action:@selector(changeChipset:) tag:1];
			[self createMenuItemInMenu:chipsetMenu withTitle:@"ECS Denise" action:@selector(changeChipset:) tag:2];
			[self createMenuItemInMenu:chipsetMenu withTitle:@"ECS Full" action:@selector(changeChipset:) tag:3];
			[self createMenuItemInMenu:chipsetMenu withTitle:@"AGA" action:@selector(changeChipset:) tag:4];
			[chipsetMenu addItem:[NSMenuItem separatorItem]];
			[self createMenuItemInMenu:chipsetMenu withTitle:@"NTSC" action:@selector(changeChipset:) tag:10];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Chipset" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:chipsetMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *chipsetXMenu = [[NSMenu alloc] initWithTitle:@"Chipset Extra"];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"" action:@selector(changeChipsetX:) tag:0];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"Generic" action:@selector(changeChipsetX:) tag:1];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"CDTV" action:@selector(changeChipsetX:) tag:2];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"CD32" action:@selector(changeChipsetX:) tag:3];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A500" action:@selector(changeChipsetX:) tag:4];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A500+" action:@selector(changeChipsetX:) tag:5];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A600" action:@selector(changeChipsetX:) tag:6];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A1000" action:@selector(changeChipsetX:) tag:7];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A1200" action:@selector(changeChipsetX:) tag:8];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A2000" action:@selector(changeChipsetX:) tag:9];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A3000" action:@selector(changeChipsetX:) tag:10];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A3000T" action:@selector(changeChipsetX:) tag:11];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A4000" action:@selector(changeChipsetX:) tag:12];
			[self createMenuItemInMenu:chipsetXMenu withTitle:@"A4000T" action:@selector(changeChipsetX:) tag:13];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Chipset Extra" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:chipsetXMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *monitoremuMenu = [[NSMenu alloc] initWithTitle:@"Monitor"];
			[self createMenuItemInMenu:monitoremuMenu withTitle:@"-" action:@selector(changeMonitoremu:) tag:0];
			[self createMenuItemInMenu:monitoremuMenu withTitle:@"default" action:@selector(changeMonitoremu:) tag:1];
			[self createMenuItemInMenu:monitoremuMenu withTitle:@"A2024" action:@selector(changeMonitoremu:) tag:2];
			[self createMenuItemInMenu:monitoremuMenu withTitle:@"Graffiti" action:@selector(changeMonitoremu:) tag:3];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Monitor" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:monitoremuMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *blitterMenu = [[NSMenu alloc] initWithTitle:@"Blitter"];
			[self createMenuItemInMenu:blitterMenu withTitle:@"Immediate Blits" action:@selector(changeBlitter:) tag:0];
			[self createMenuItemInMenu:blitterMenu withTitle:@"Cycle-exact Blitter" action:@selector(changeBlitter:) tag:1];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Blitter" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:blitterMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

		NSMenu *collisionMenu = [[NSMenu alloc] initWithTitle:@"Collision Level"];
			[self createMenuItemInMenu:collisionMenu withTitle:@"None" action:@selector(changeCollision:) tag:0];
			[self createMenuItemInMenu:collisionMenu withTitle:@"Sprites Only" action:@selector(changeCollision:) tag:1];
			[self createMenuItemInMenu:collisionMenu withTitle:@"Sprites and Sprites vs. Playfield" action:@selector(changeCollision:) tag:2];
			[self createMenuItemInMenu:collisionMenu withTitle:@"Full" action:@selector(changeCollision:) tag:3];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Collision Level" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:collisionMenu];
		[systemMenu addItem:menuItem];
		[menuItem release];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"System" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:systemMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:4];
	[systemMenu release];
	[menuItem release];
	// SYSTEM MENU END

	// SOUND MENU START
	NSMenu *soundMenu = [[NSMenu alloc] initWithTitle:@"Sound"];

		NSMenu *semuMenu = [[NSMenu alloc] initWithTitle:@"Emulation"];
			[self createMenuItemInMenu:semuMenu withTitle:@"Disabled" action:@selector(changeSound:) tag:0];
			[self createMenuItemInMenu:semuMenu withTitle:@"Disabled, but emulated" action:@selector(changeSound:) tag:1];
			[self createMenuItemInMenu:semuMenu withTitle:@"Enabled" action:@selector(changeSound:) tag:2];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Emulation" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:semuMenu];
		[soundMenu addItem:menuItem];
		[menuItem release];

		NSMenu *sfilterMenu = [[NSMenu alloc] initWithTitle:@"Filter"];
			[self createMenuItemInMenu:sfilterMenu withTitle:@"Always Off" action:@selector(changeSoundFilter:) tag:0];
			[self createMenuItemInMenu:sfilterMenu withTitle:@"Emulated (A500)" action:@selector(changeSoundFilter:) tag:1];
			[self createMenuItemInMenu:sfilterMenu withTitle:@"Emulated (A1200)" action:@selector(changeSoundFilter:) tag:2];
			[self createMenuItemInMenu:sfilterMenu withTitle:@"Always On (A500)" action:@selector(changeSoundFilter:) tag:3];
			[self createMenuItemInMenu:sfilterMenu withTitle:@"Always On (A1200)" action:@selector(changeSoundFilter:) tag:4];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Filter" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:sfilterMenu];
		[soundMenu addItem:menuItem];
		[menuItem release];

		NSMenu *sinterMenu = [[NSMenu alloc] initWithTitle:@"Interpolation"];
			[self createMenuItemInMenu:sinterMenu withTitle:@"" action:@selector(changeSoundInterpolation:) tag:0];
			[self createMenuItemInMenu:sinterMenu withTitle:@"Anti" action:@selector(changeSoundInterpolation:) tag:1];
			[self createMenuItemInMenu:sinterMenu withTitle:@"Sinc" action:@selector(changeSoundInterpolation:) tag:2];
			[self createMenuItemInMenu:sinterMenu withTitle:@"RH" action:@selector(changeSoundInterpolation:) tag:3];
			[self createMenuItemInMenu:sinterMenu withTitle:@"Crux" action:@selector(changeSoundInterpolation:) tag:4];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Interpolation" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:sinterMenu];
		[soundMenu addItem:menuItem];
		[menuItem release];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"Sound" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:soundMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:5];
	[soundMenu release];
	[menuItem release];
	// SOUND MENU END

	// GFX MENU START
	NSMenu *graphicsMenu = [[NSMenu alloc] initWithTitle:@"Graphics"];

		NSMenu *lmodeMenu = [[NSMenu alloc] initWithTitle:@"Line Mode"];
			[self createMenuItemInMenu:lmodeMenu withTitle:@"Normal" action:@selector(changeGfxLineMode:) tag:0];
			[self createMenuItemInMenu:lmodeMenu withTitle:@"Double" action:@selector(changeGfxLineMode:) tag:1];
			[self createMenuItemInMenu:lmodeMenu withTitle:@"Scanlines" action:@selector(changeGfxLineMode:) tag:2];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Line Mode" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:lmodeMenu];
		[graphicsMenu addItem:menuItem];
		[menuItem release];

		NSMenu *centeringMenu = [[NSMenu alloc] initWithTitle:@"Centering"];
			[self createMenuItemInMenu:centeringMenu withTitle:@"Horizontal" action:@selector(changeGfxCentering:) tag:0];
			[self createMenuItemInMenu:centeringMenu withTitle:@"Vertical" action:@selector(changeGfxCentering:) tag:1];
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Centering" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:centeringMenu];
		[graphicsMenu addItem:menuItem];
		[menuItem release];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"Graphics" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:graphicsMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:6];
	[graphicsMenu release];
	[menuItem release];
	// GFX MENU END

	// Create a menu for changing aspects of emulator control
	NSMenu *controlMenu = [[NSMenu alloc] initWithTitle:@"Control"];

		NSMenu *portMenu = [[NSMenu alloc] initWithTitle:@"Game Port 0"];
			[self createMenuItemInMenu:portMenu withTitle:@"None" action:@selector(changePort0:) tag:JSEM_END];
			[self createMenuItemInMenu:portMenu withTitle:@"Joystick 0" action:@selector(changePort0:) tag:JSEM_JOYS];
			[self createMenuItemInMenu:portMenu withTitle:@"Joystick 1" action:@selector(changePort0:) tag:JSEM_JOYS+1];
			[self createMenuItemInMenu:portMenu withTitle:@"Mouse" action:@selector(changePort0:) tag:JSEM_MICE];
			[self createMenuItemInMenu:portMenu withTitle:@"Keyboard Layout A (NumPad, 0 & 5 = Fire)" action:@selector(changePort0:) tag:JSEM_KBDLAYOUT];
			[self createMenuItemInMenu:portMenu withTitle:@"Keyboard Layout B (Cursor, RCtrl & Alt = Fire)" action:@selector(changePort0:) tag:JSEM_KBDLAYOUT+1];
			[self createMenuItemInMenu:portMenu withTitle:@"Keyboard Layout C (WASD, LAlt = Fire)" action:@selector(changePort0:) tag:JSEM_KBDLAYOUT+2];
#ifdef ARCADE
			[self createMenuItemInMenu:portMenu withTitle:@"X-Arcade (Left)" action:@selector(changePort0:) tag:JSEM_KBDLAYOUT+3];
			[self createMenuItemInMenu:portMenu withTitle:@"X-Arcade (Right)" action:@selector(changePort0:) tag:JSEM_KBDLAYOUT+4];
#endif
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Game Port 0" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:portMenu];
		[controlMenu addItem:menuItem];
		[menuItem release];
		[portMenu release];

		portMenu = [[NSMenu alloc] initWithTitle:@"Game Port 1"];
			[self createMenuItemInMenu:portMenu withTitle:@"None" action:@selector(changePort1:) tag:JSEM_END];
			[self createMenuItemInMenu:portMenu withTitle:@"Joystick 0" action:@selector(changePort1:) tag:JSEM_JOYS];
			[self createMenuItemInMenu:portMenu withTitle:@"Joystick 1" action:@selector(changePort1:) tag:JSEM_JOYS+1];
			[self createMenuItemInMenu:portMenu withTitle:@"Mouse" action:@selector(changePort1:) tag:JSEM_MICE];
			[self createMenuItemInMenu:portMenu withTitle:@"Keyboard Layout A (NumPad, 0 & 5 = Fire)" action:@selector(changePort1:) tag:JSEM_KBDLAYOUT];
			[self createMenuItemInMenu:portMenu withTitle:@"Keyboard Layout B (Cursor, RCtrl & Alt = Fire)" action:@selector(changePort1:) tag:JSEM_KBDLAYOUT+1];
			[self createMenuItemInMenu:portMenu withTitle:@"Keyboard Layout C (WASD, LAlt = Fire)" action:@selector(changePort1:) tag:JSEM_KBDLAYOUT+2];
#ifdef ARCADE
			[self createMenuItemInMenu:portMenu withTitle:@"X-Arcade (Left)" action:@selector(changePort1:) tag:JSEM_KBDLAYOUT+3];
			[self createMenuItemInMenu:portMenu withTitle:@"X-Arcade (Right)" action:@selector(changePort1:) tag:JSEM_KBDLAYOUT+4];
#endif
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Game Port 1" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:portMenu];
		[controlMenu addItem:menuItem];
		[menuItem release];
		[portMenu release];

	[self createMenuItemInMenu:controlMenu withTitle:@"Swap Port 0 and 1" action:@selector(swapGamePorts:) tag:0];

	[controlMenu addItem:[NSMenuItem separatorItem]];
	
	[self createMenuItemInMenu:controlMenu withTitle:@"Grab Mouse" action:@selector(grabMouse:) tag:0  keyEquivalent:@"g" keyEquivalentMask:NSCommandKeyMask|NSAlternateKeyMask];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"Control" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:controlMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:7];
	[controlMenu release];
	[menuItem release];

	// Create a menu for changing aspects of emulator control
	NSMenu *displayMenu = [[NSMenu alloc] initWithTitle:@"Display"];
		[self createMenuItemInMenu:displayMenu withTitle:@"Fullscreen" action:@selector(goFullscreen:) tag:0  keyEquivalent:@"s" keyEquivalentMask:NSCommandKeyMask|NSAlternateKeyMask];
		[self createMenuItemInMenu:displayMenu withTitle:@"Inhibit" action:@selector(toggleInhibitDisplay:) tag:0];
	menuItem = [[NSMenuItem alloc] initWithTitle:@"Display" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:displayMenu];
	[[NSApp mainMenu] insertItem:menuItem atIndex:8];
	[displayMenu release];
	[menuItem release];
}

- (void)createMenuItemInMenu:(NSMenu *)menu withTitle:(NSString *)title action:(SEL)anAction tag:(int)tag
{
	[self createMenuItemInMenu:menu withTitle:title action:anAction tag:tag keyEquivalent:@"" keyEquivalentMask:NSCommandKeyMask];
}

- (void)createMenuItemInMenu:(NSMenu *)menu withTitle:(NSString *)title action:(SEL)anAction tag:(int)tag keyEquivalent:(NSString *)keyEquiv keyEquivalentMask:(NSUInteger)mask
{
	NSMenuItem *menuItem = [[NSMenuItem alloc] initWithTitle:title action:anAction keyEquivalent:keyEquiv];
	[menuItem setKeyEquivalentModifierMask:mask];
	[menuItem setTag:tag];
	[menuItem setTarget:self];
	[menu addItem:menuItem];
	[menuItem release];
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)item
{
	NSMenuItem *menuItem = (NSMenuItem *)item;
	BOOL canSetHidden = [menuItem respondsToSelector:@selector(setHidden:)];
	
	SEL menuAction = [menuItem action];
	int tag = [menuItem tag];

	if (menuAction == @selector(LedsOnScreen:)) {
		if (tag == 2) {
			if (changed_prefs.leds_on_screen) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
	}

	// Disabled drives can't have disks inserted or ejected
	if ((menuAction == @selector(insertDisk:)) || (menuAction == @selector(ejectDisk:))) {
		if (gui_data.drive_disabled[tag]) {
			//if (canSetHidden) [menuItem setHidden:YES];
			return NO;
		} else {
			//if (canSetHidden) [menuItem setHidden:NO];
		}
	}
        
	// Eject DFx should be disabled if there's no disk in DFx
	if (menuAction == @selector(ejectDisk:)) {
		if (disk_empty(tag)) {
			[menuItem setTitle:[NSString stringWithFormat:@"DF%d",tag]];
			return NO;
		}
		
		// There's a disk in the drive, show its name in the menu item
		NSString *diskImage = [[NSString stringWithCString:gui_data.df[tag] encoding:NSASCIIStringEncoding] lastPathComponent];
		[menuItem setTitle:[NSString stringWithFormat:@"DF%d (%@)",tag,diskImage]];
		return YES;
	}

	// The current settings for the joystick/mouse ports should be indicated
	if (menuAction == @selector(changePort0:)) {
		if (currprefs.jports[0].id == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		// and joystick options should be unavailable if there are no joysticks
		if (((tag == JSEM_JOYS) || (tag == (JSEM_JOYS+1)))) {
			if ((tag - JSEM_JOYS) >= inputdevice_get_device_total (IDTYPE_JOYSTICK))
				return NO;
		}

		// and we should not allow both ports to be set to the same setting
		if ((tag != JSEM_END) && (currprefs.jports[1].id == tag))
			return NO;

		return YES;
	}

	// Repeat the above for Port 1
	if (menuAction == @selector(changePort1:)) {
		if (currprefs.jports[1].id == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		if (((tag == JSEM_JOYS) || (tag == (JSEM_JOYS+1)))) {
			if ((tag - JSEM_JOYS) >= inputdevice_get_device_total (IDTYPE_JOYSTICK))
				return NO;
		}

		if ((tag != JSEM_END) && (currprefs.jports[0].id == tag))
			return NO;

		return YES;
	}

	long mem_size, v;
	if (menuAction == @selector(changeChipMem:)) {
		mem_size = 0;
	        switch (changed_prefs.chipmem_size) {
		        case 0x00040000: mem_size = 1; break;
		        case 0x00080000: mem_size = 2; break;
		        case 0x00100000: mem_size = 3; break;
		        case 0x00180000: mem_size = 14; break;
		        case 0x00200000: mem_size = 4; break;
		        case 0x00400000: mem_size = 5; break;
		        case 0x00800000: mem_size = 6; break;
        	}
		if (mem_size == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeBogoMem:)) {
		mem_size = 0;
	        switch (changed_prefs.bogomem_size) {
		        case 0x00000000: mem_size = 0; break;
        		case 0x00080000: mem_size = 2; break;
	        	case 0x00100000: mem_size = 3; break;
        		case 0x00180000: mem_size = 14; break;
		        case 0x001C0000: mem_size = 15; break;
	        }
		if (mem_size == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeFastMem:)) {
	        if (changed_prefs.chipmem_size > 0x200000) return NO;

		mem_size = 0;
        	switch (changed_prefs.fastmem_size) {
		        case 0x00000000: mem_size = 0; break;
		        case 0x00100000: mem_size = 3; break;
	        	case 0x00200000: mem_size = 4; break;
		        case 0x00400000: mem_size = 5; break;
		        case 0x00800000: mem_size = 6; break;
	        }
		if (mem_size == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		return YES;
	}

	if (menuAction == @selector(changeZ3FastMem:)) {
		if (changed_prefs.address_space_24) return NO;

		mem_size = 0;
        	v = changed_prefs.z3fastmem_size + changed_prefs.z3fastmem2_size;
	        if      (v < 0x00100000)
        	        mem_size = 0;
	        else if (v < 0x00200000)
        	        mem_size = 3;
	        else if (v < 0x00400000)
        	        mem_size = 4;
	        else if (v < 0x00800000)
        	        mem_size = 5;
	        else if (v < 0x01000000)
        	        mem_size = 6;
	        else if (v < 0x02000000)
        	        mem_size = 7;
	        else if (v < 0x04000000)
        	        mem_size = 8;
	        else if (v < 0x08000000)
        	        mem_size = 9;
	        else if (v < 0x10000000)
        	        mem_size = 10;
	        else if (v < 0x18000000)
        	        mem_size = 11;
	        else if (v < 0x20000000)
        	        mem_size = 17;
	        else if (v < 0x30000000)
        	        mem_size = 12;
	        else if (v < 0x40000000) // 1GB
        	        mem_size = 18;
	        else if (v < 0x60000000) // 1.5GB
        	        mem_size = 13;
	        else if (v < 0x80000000) // 2GB
        	        mem_size = 19;
	        else if (v < 0xA8000000) // 2.5GB
        	        mem_size = 16;
	        else if (v < 0xC0000000) // 3GB
                	mem_size = 20;
        	else
	                mem_size = 21;

		if (mem_size == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		return YES;
	}

	if (menuAction == @selector(changeZ3ChipMem:)) {
		if (changed_prefs.address_space_24) return NO;

		mem_size = 0;
        	v = changed_prefs.z3chipmem_size;
	        if (v < 0x01000000)
        	        mem_size = 0;
	        else if (v < 0x02000000)
        	        mem_size = 7;
	        else if (v < 0x04000000)
        	        mem_size = 8;
	        else if (v < 0x08000000)
        	        mem_size = 9;
	        else if (v < 0x10000000)
        	        mem_size = 10;
	        else if (v < 0x20000000)
        	        mem_size = 11;
	        else if (v < 0x40000000)
                	mem_size = 12;
        	else
	                mem_size = 13;

		if (mem_size == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		return YES;
	}
/*
	if (menuAction == @selector(changeGfxMem:)) {
	        mem_size = 0;
	        switch (changed_prefs.rtgmem_size) {
        		case 0x00000000: mem_size = 0; break;
	        	case 0x00100000: mem_size = 3; break;
		        case 0x00200000: mem_size = 4; break;
        		case 0x00400000: mem_size = 5; break;
	        	case 0x00800000: mem_size = 6; break;
	        	case 0x01000000: mem_size = 7; break;
		        case 0x02000000: mem_size = 8; break;
        		case 0x04000000: mem_size = 9; break;
	        	case 0x08000000: mem_size = 10; break;
		        case 0x10000000: mem_size = 11; break;
	        	case 0x20000000: mem_size = 12; break;
	        	case 0x40000000: mem_size = 13; break;
	        }
		if (mem_size == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}
*/
	if (menuAction == @selector(changeChipset:)) {
		v = 0;
        	switch (changed_prefs.chipset_mask) {
		        case 0: v = 0; break;
        		case 1: v = 1; break;
	        	case 2: v = 2; break;
        		case 3: v = 3; break;
		        case 4: v = 4; break;
        		case 7: v = 4; break;
	        }
		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		if (tag == 10) {
			if (changed_prefs.ntscmode) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
	}

	if (menuAction == @selector(changeChipsetX:)) {
		if (changed_prefs.cs_compatible == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeCPU:)) {
		v = (changed_prefs.cpu_model - 68000) / 10;
		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

		if (tag == 10) {
			if (changed_prefs.cpu_model != 68020) return NO;
			if (changed_prefs.address_space_24) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
		if (tag == 11) {
			if (changed_prefs.cpu_compatible) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
#ifdef MMUEMU
		if (tag == 12) {
			if (changed_prefs.mmu_model) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
#endif
		return YES;
	}

	if (menuAction == @selector(changeCPUSpeed:)) {
		if (changed_prefs.cpu_cycle_exact == 1) {
			v = 2;
		} else {
			if (changed_prefs.m68k_speed == -1) v = 0;
			if (changed_prefs.m68k_speed == 0) v = 1;
		}
		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeFPU:)) {
		v = changed_prefs.fpu_model == 0 ? 0 : (changed_prefs.fpu_model == 68881 ? 1 : (changed_prefs.fpu_model == 68882 ? 2 : 3));
		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];

#ifdef JIT
		if (tag == 10) {
			if (changed_prefs.fpu_strict) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
#endif

		if (changed_prefs.cpu_model > 68030 || changed_prefs.cpu_compatible || changed_prefs.cpu_cycle_exact) {
			if (tag < 3) return NO;
		}
		if (tag == 3) {
			if (!changed_prefs.cpu_model >= 68040) {
				return NO;
			}
		}
		return YES;
	}
	
	if (menuAction == @selector(changeMonitoremu:)) {
		if (changed_prefs.monitoremu == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeBlitter:)) {
		if (tag == 0) {
			if (changed_prefs.immediate_blits) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
		if (tag == 1) {
			if (changed_prefs.blitter_cycle_exact) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
	}

	if (menuAction == @selector(changeCollision:)) {
		if (changed_prefs.collision_level == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeSound:)) {
		v = changed_prefs.produce_sound;
		if (v == 3) v = 2;
		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeSoundInterpolation:)) {
		if (changed_prefs.sound_interpol == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeSoundFilter:)) {
        	v = 0;
		switch (changed_prefs.sound_filter) {
		case 0:
			v = 0;
			break;
		case 1:
			v = changed_prefs.sound_filter_type ? 2 : 1;
			break;
		case 2:  
			v = changed_prefs.sound_filter_type ? 4 : 3;
			break;
		}

		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeGfxLineMode:)) {
		v = changed_prefs.gfx_scanlines;
		if (v == tag) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(changeGfxCentering:)) {
		if (tag == 0) {
			if (changed_prefs.gfx_xcenter) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
		if (tag == 1) {
			if (changed_prefs.gfx_ycenter) [menuItem setState:NSOnState];
			else [menuItem setState:NSOffState];
		}
	}

	if (menuAction == @selector(pauseAmiga:)) {
		if (pause_emulation)
			[menuItem setTitle:@"Resume"];
		else
			[menuItem setTitle:@"Pause"];
		
		return YES;
	}
	
	if (menuAction == @selector(toggleInhibitDisplay:)) {
		if (inhibit_frame) [menuItem setState:NSOnState];
		else [menuItem setState:NSOffState];
	}

	if (menuAction == @selector(actionReplayFreeze:)) 
		return ( (hrtmon_flag == ACTION_REPLAY_IDLE) || (action_replay_flag == ACTION_REPLAY_IDLE) );
	
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

// Invoked when the user selects "Eject All Disks"
- (void)ejectAllDisks:(id)sender
{
	unsigned int i;
	for (i=0; i<4; i++)
		if ((!gui_data.drive_disabled[i]) && (!disk_empty(i)))
			disk_eject(i);
}

// Invoked when the user selects an option from the 'Port 0' menu
- (void)changePort0:(id)sender
{
	changed_prefs.jports[0].id = [((NSMenuItem*)sender) tag];

	if (changed_prefs.jports[0].id != currprefs.jports[0].id) {
		inputdevice_updateconfig (&changed_prefs, &currprefs);
		inputdevice_config_change();
	}
}

// Invoked when the user selects an option from the 'Port 1' menu
- (void)changePort1:(id)sender
{
	changed_prefs.jports[1].id = [((NSMenuItem*)sender) tag];

	if (changed_prefs.jports[1].id != currprefs.jports[1].id) {
		inputdevice_updateconfig (&changed_prefs, &currprefs);
		inputdevice_config_change();
	}
}

- (void)swapGamePorts:(id)sender
{
	changed_prefs.jports[0].id = currprefs.jports[1].id;
	changed_prefs.jports[1].id = currprefs.jports[0].id;
	inputdevice_updateconfig (&changed_prefs, &currprefs);
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

	// Recall the path of the disk image that was loaded last time 
	NSString *nsfloppypath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedDiskImagePath"];
	
	/* If the configuration includes a setting for the "floppy_path" attribute
	 * start the OpenPanel in that directory.. but only the first time. */
	static int run_once = 0;
	if (!run_once) {
		run_once++;
		
		const char *floppy_path = currprefs.path_floppy.path[driveNumber];
		
		if (floppy_path != NULL) {
			char homedir[MAX_PATH];
			snprintf(homedir, MAX_PATH, "%s/", getenv("HOME"));
			
			/* The default value for floppy_path is "$HOME/". We only want to use it if the
			 * user provided an actual value though, so we don't use it if it equals "$HOME/" */
			if (strncmp(floppy_path, homedir, MAX_PATH) != 0)
				nsfloppypath = [NSString stringWithCString:floppy_path encoding:NSASCIIStringEncoding];
		}
	}

    [oPanel beginSheetForDirectory:nsfloppypath file:nil
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
		char *sfile = [file UTF8String];
		strcpy(changed_prefs.floppyslots[drive].df, sfile);
		write_log ("Selected Disk Image: %s for Drive: %d\n", sfile, drive);
		
		// Save the path of this disk image so that future open panels can start in the same directory
		[[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedDiskImagePath"];
	}
}

// Eject CD
- (void)ejectCD:(id)sender
{
	changed_prefs.cdslots[0].name[0] = 0;
	changed_prefs.cdslots[0].inuse = false;
}

// cd image
- (void)displayOpenPanelForCDImage:(int)foo
{
        ensureNotFullscreen();

        NSOpenPanel *oPanel = [NSOpenPanel openPanel];
        [oPanel setTitle:@"Select CD Image"];

        // make sure setMessage (OS X 10.3+) is available before calling it
        if ([oPanel respondsToSelector:@selector(setMessage:)])
                [oPanel setMessage:@"Select Kick ROM"];

        [oPanel setPrompt:@"Select"];
        NSString *contextInfo = [[NSString alloc] initWithString:@"cdimage"];

        // recall the path of kick rom that was loaded last time
        NSString *nscdpath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedCDPath"];

        // If the configuration includes a setting for the "cd_path" attribute
        // start the OpenPanel in that directory.. but only the first time.
        static int cd_run_once = 0;
        if (!cd_run_once) {  
                cd_run_once++;

                const char *cd_path = currprefs.cdslots[0].name;

                if (cd_path != NULL) {
                        char homedir[MAX_PATH];
                        snprintf(homedir, MAX_PATH, "%s/", getenv("HOME"));

                        // default value for cd_path is "$HOME/"
                        if (strncmp(cd_path, homedir, MAX_PATH) != 0)
                                nscdpath = [NSString stringWithCString:cd_path encoding:NSASCIIStringEncoding];
                }
        }

    [oPanel beginSheetForDirectory:nscdpath file:nil
                             types:CDImageTypes
                    modalForWindow:[NSApp mainWindow]
                     modalDelegate:self
                    didEndSelector:@selector(selectCDImagePanelDidEnd:returnCode:contextInfo:)
                       contextInfo:contextInfo];
}

// called after cd image selection panel
- (void)selectCDImagePanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
        if (returnCode != NSOKButton) return;

        NSArray *files = [sheet filenames];
        NSString *file = [files objectAtIndex:0];
        char *sfile = [file UTF8String];
        strcpy(changed_prefs.cdslots[0].name, sfile);
        write_log ("Selected CD Image: %s\n", sfile);

        [[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedCDPath"];
}

// kick.rom
- (void)displayOpenPanelForKickROM:(int)foo
{
	ensureNotFullscreen();

	NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	[oPanel setTitle:@"Select Kick ROM"];

	// make sure setMessage (OS X 10.3+) is available before calling it
	if ([oPanel respondsToSelector:@selector(setMessage:)])
		[oPanel setMessage:@"Select Kick ROM"];

	[oPanel setPrompt:@"Select"];
	NSString *contextInfo = [[NSString alloc] initWithString:@"kick"];

	// recall the path of kick rom that was loaded last time 
	NSString *nskickpath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedKickPath"];

	// If the configuration includes a setting for the "kick_path" attribute
	// start the OpenPanel in that directory.. but only the first time.
	static int kick_run_once = 0;
	if (!kick_run_once) {
		kick_run_once++;
		
		const char *kick_path = currprefs.romfile;
		
		if (kick_path != NULL) {
			char homedir[MAX_PATH];
			snprintf(homedir, MAX_PATH, "%s/", getenv("HOME"));
			
			// default value for kick_path is "$HOME/"
			if (strncmp(kick_path, homedir, MAX_PATH) != 0)
				nskickpath = [NSString stringWithCString:kick_path encoding:NSASCIIStringEncoding];
		}
	}

    [oPanel beginSheetForDirectory:nskickpath file:nil
                             types:KickRomTypes
                    modalForWindow:[NSApp mainWindow]
                     modalDelegate:self
                    didEndSelector:@selector(selectKickROMPanelDidEnd:returnCode:contextInfo:)
                       contextInfo:contextInfo];
}

// called after kick rom selection panel
- (void)selectKickROMPanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode != NSOKButton) return;

	NSArray *files = [sheet filenames];
	NSString *file = [files objectAtIndex:0];
	char *sfile = [file UTF8String];
	strcpy(changed_prefs.romfile, sfile);
	write_log ("Selected Kickrom: %s\n", sfile);
		
	[[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedKickPath"];
}

// flash.rom
- (void)displayOpenPanelForFlashRAM:(int)foo
{
	ensureNotFullscreen();

	NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	[oPanel setTitle:@"Select Flash RAM"];

	// make sure setMessage (OS X 10.3+) is available before calling it
	if ([oPanel respondsToSelector:@selector(setMessage:)])
		[oPanel setMessage:@"Select Flash RAM"];

	[oPanel setPrompt:@"Select"];
	NSString *contextInfo = [[NSString alloc] initWithString:@"flash"];

	// recall the path of flash ram that was loaded last time 
	NSString *nsflashpath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedFlashPath"];

	// If the configuration includes a setting for the "flash_path" attribute
	// start the OpenPanel in that directory.. but only the first time.
	static int flash_run_once = 0;
	if (!flash_run_once) {
		flash_run_once++;
		
		const char *flash_path = currprefs.flashfile;
		
		if (flash_path != NULL) {
			char homedir[MAX_PATH];
			snprintf(homedir, MAX_PATH, "%s/", getenv("HOME"));
			
			// default value for flash_path is "$HOME/"
			if (strncmp(flash_path, homedir, MAX_PATH) != 0)
				nsflashpath = [NSString stringWithCString:flash_path encoding:NSASCIIStringEncoding];
		}
	}

    [oPanel beginSheetForDirectory:nsflashpath file:nil
                             types:FlashRamTypes
                    modalForWindow:[NSApp mainWindow]
                     modalDelegate:self
                    didEndSelector:@selector(selectFlashRAMPanelDidEnd:returnCode:contextInfo:)
                       contextInfo:contextInfo];
}

// called after flash ram selection panel
- (void)selectFlashRAMPanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode != NSOKButton) return;

	NSArray *files = [sheet filenames];
	NSString *file = [files objectAtIndex:0];
	char *sfile = [file UTF8String];
	strcpy(changed_prefs.flashfile, sfile);
	write_log ("Selected Flash RAM: %s\n", sfile);
		
	[[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedFlashPath"];
}

// cartridge rom
- (void)displayOpenPanelForCartridge:(int)foo
{
	ensureNotFullscreen();

	NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	[oPanel setTitle:@"Select Cartridge ROM"];

	// make sure setMessage (OS X 10.3+) is available before calling it
	if ([oPanel respondsToSelector:@selector(setMessage:)])
		[oPanel setMessage:@"Select Cartridge ROM"];

	[oPanel setPrompt:@"Select"];
	NSString *contextInfo = [[NSString alloc] initWithString:@"cart"];

	// recall the path of cartridge that was loaded last time 
	NSString *nscartpath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedCartPath"];

	// If the configuration includes a setting for the "cart_path" attribute
	// start the OpenPanel in that directory.. but only the first time.
	static int cart_run_once = 0;
	if (!cart_run_once) {
		cart_run_once++;
		
		const char *cart_path = currprefs.cartfile;
		
		if (cart_path != NULL) {
			char homedir[MAX_PATH];
			snprintf(homedir, MAX_PATH, "%s/", getenv("HOME"));
			
			// default value for cart_path is "$HOME/"
			if (strncmp(cart_path, homedir, MAX_PATH) != 0)
				nscartpath = [NSString stringWithCString:cart_path encoding:NSASCIIStringEncoding];
		}
	}

    [oPanel beginSheetForDirectory:nscartpath file:nil
                             types:CartridgeTypes
                    modalForWindow:[NSApp mainWindow]
                     modalDelegate:self
                    didEndSelector:@selector(selectCartridgePanelDidEnd:returnCode:contextInfo:)
                       contextInfo:contextInfo];
}

// called after cartridge rom selection panel
- (void)selectCartridgePanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode != NSOKButton) return;

	NSArray *files = [sheet filenames];
	NSString *file = [files objectAtIndex:0];
	char *sfile = [file UTF8String];
	strcpy(changed_prefs.cartfile, sfile);
	write_log ("Selected Cartridge: %s\n", sfile);
		
	[[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedCartPath"];
}

// load savestate
- (void)displayOpenPanelForLoadState:(id)sender
{
	ensureNotFullscreen();

	NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	[oPanel setTitle:@"Select a SaveState to Load"];

	// make sure setMessage (OS X 10.3+) is available before calling it
	if ([oPanel respondsToSelector:@selector(setMessage:)])
		[oPanel setMessage:@"Select a SaveState to Load"];

	[oPanel setPrompt:@"Select"];

	// recall the path of save state that was loaded last time 
	NSString *nssavestatepath = [[NSUserDefaults standardUserDefaults] stringForKey:@"LastUsedSaveStatePath"];

	// If the configuration includes a setting for the "savestate_path" attribute
	// start the OpenPanel in that directory.. but only the first time.
	static int sstate_run_once = 0;
	if (!sstate_run_once) {
		sstate_run_once++;
		
		const char *savestate_path = "./";
		
		if (savestate_path != NULL) {
			char homedir[MAX_PATH];
			snprintf(homedir, MAX_PATH, "%s/", getenv("HOME"));
			
			// default value for savestate_path is "$HOME/"
			if (strncmp(savestate_path, homedir, MAX_PATH) != 0)
				nssavestatepath = [NSString stringWithCString:savestate_path encoding:NSASCIIStringEncoding];
		}
	}

    [oPanel beginSheetForDirectory:nssavestatepath file:@""
                    modalForWindow:[NSApp mainWindow]
                     modalDelegate:self
                    didEndSelector:@selector(selectLoadStatePanelDidEnd:returnCode:contextInfo:)
                       contextInfo:NULL];
}

// called after load a savestate selection panel
- (void)selectLoadStatePanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode != NSOKButton) return;

	NSArray *files = [sheet filenames];
	NSString *file = [files objectAtIndex:0];
	char *sfile = [file UTF8String];
	write_log ("Loading SaveState from: %s ...", sfile);

	[[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedSaveStatePath"];

	savestate_initsave (sfile, 0, 0, 0);
	savestate_state = STATE_DORESTORE;
	write_log ("done\n");
}

// save savestate
- (void)displayOpenPanelForSaveState:(id)sender
{
	ensureNotFullscreen();

	NSSavePanel *sPanel = [NSSavePanel savePanel];
	[sPanel setTitle:@"Select a SaveState to Save"];
	[sPanel setCanSelectHiddenExtension:true];

	// make sure setMessage (OS X 10.3+) is available before calling it
	if ([sPanel respondsToSelector:@selector(setMessage:)])
		[sPanel setMessage:@"Select a SaveState to Save"];

	[sPanel beginSheetForDirectory:NSHomeDirectory() file:nil
			modalForWindow:[NSApp mainWindow]
			 modalDelegate:self
			didEndSelector:@selector(selectSaveStatePanelDidEnd:returnCode:contextInfo:)
			   contextInfo:NULL];
}

// called after save a savestate selection panel
- (void)selectSaveStatePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode != NSOKButton) return;

	NSString *file = [sheet filename];
	char *sfile = [file UTF8String];
	write_log ("Saving SaveState to: %s ...", sfile);

	[[NSUserDefaults standardUserDefaults] setObject:[file stringByDeletingLastPathComponent] forKey:@"LastUsedSaveStatePath"];

//	savestate_initsave (sfile, 0, 0, 0);
	save_state (sfile, "puae");
	write_log ("done\n");
}


- (void)QuickStart:(id)sender
{
	unsigned int romcheck = 0;
	quickstart_model = [((NSMenuItem*)sender) tag];

	changed_prefs.nr_floppies = quickstart_floppy;
	quickstart_ok = built_in_prefs (&changed_prefs, quickstart_model, quickstart_conf, quickstart_compa, romcheck);
	changed_prefs.ntscmode = quickstart_ntsc != 0;
	//quickstart_cd = chnaged_prefs.floppyslots[1].dfxtype == DRV_NONE && (quickstart_model == 8 || quickstart_model == 9);
    config_filename[0] = 0;
	//uae_reset(0);
}

// leds_on_screen
- (void)LedsOnScreen:(id)sender
{
	changed_prefs.leds_on_screen = !changed_prefs.leds_on_screen;
	config_changed = 1;
	check_prefs_changed_gfx ();
}

- (void)resetAmiga:(id)sender
{
	uae_reset([((NSMenuItem *)sender) tag], 0);
}

- (void)pauseAmiga:(id)sender
{
	pausemode(-1); // Found in inputdevice.c -- toggles pause mode when arg is -1
}

#ifdef ACTION_REPLAY
- (void)actionReplayFreeze:(id)sender
{
	action_replay_freeze();
}
#endif

- (void)grabMouse:(id)sender
{
	toggle_mousegrab ();
}

- (void)goFullscreen:(id)sender
{
	toggle_fullscreen(0);
}

- (void)toggleInhibitDisplay:(id)sender
{
	toggle_inhibit_frame (IHF_SCROLLLOCK);
}

// chip mem
- (void)changeChipMem:(id)sender
{
	changed_prefs.chipmem_size = memsizes[[((NSMenuItem*)sender) tag]];
        if (changed_prefs.chipmem_size > 0x200000)
                changed_prefs.fastmem_size = 0;
}

// bogo mem
- (void)changeBogoMem:(id)sender
{
	changed_prefs.bogomem_size = memsizes[[((NSMenuItem*)sender) tag]];
}

// fast mem
- (void)changeFastMem:(id)sender
{
	changed_prefs.fastmem_size = memsizes[[((NSMenuItem*)sender) tag]];
}

// z3 fast mem
- (void)changeZ3FastMem:(id)sender
{
	changed_prefs.z3fastmem_size = memsizes[[((NSMenuItem*)sender) tag]];
}

// z3 chip mem
- (void)changeZ3ChipMem:(id)sender
{
	changed_prefs.z3chipmem_size = memsizes[[((NSMenuItem*)sender) tag]];
}

/* gfx mem
- (void)changeGfxMem:(id)sender
{
	changed_prefs.rtgmem_size = memsizes[[((NSMenuItem*)sender) tag]];
}*/

// chipset
- (void)changeChipset:(id)sender
{
	if ([((NSMenuItem*)sender) tag] < 10) {
		changed_prefs.chipset_mask = [((NSMenuItem*)sender) tag];
	} else {
		changed_prefs.ntscmode = !changed_prefs.ntscmode;
	}
}

// chipset extra
- (void)changeChipsetX:(id)sender
{
	changed_prefs.cs_compatible = [((NSMenuItem*)sender) tag];
	built_in_chipset_prefs (&changed_prefs);
}

// monitor emu
- (void)changeMonitoremu:(id)sender
{
	changed_prefs.monitoremu = [((NSMenuItem*)sender) tag];
	config_changed = 1;
}

// blitter
- (void)changeBlitter:(id)sender
{
	if ([((NSMenuItem*)sender) tag] == 0)
		changed_prefs.immediate_blits = !changed_prefs.immediate_blits;
	if ([((NSMenuItem*)sender) tag] == 1)
		changed_prefs.blitter_cycle_exact = !changed_prefs.blitter_cycle_exact;
	config_changed = 1;
}

// collision
- (void)changeCollision:(id)sender
{
	changed_prefs.collision_level = [((NSMenuItem*)sender) tag];
	config_changed = 1;
}

// cpu
- (void)changeCPU:(id)sender
{
 if ([((NSMenuItem*)sender) tag] < 10) {
	unsigned int newcpu, newfpu;
	newcpu = 68000 + ([((NSMenuItem*)sender) tag] * 10);
	newfpu = changed_prefs.fpu_model;
	changed_prefs.cpu_model = newcpu;

        switch (newcpu) {
        case 68000:
        case 68010:
                changed_prefs.fpu_model = newfpu == 0 ? 0 : (newfpu == 2 ? 68882 : 68881);
                if (changed_prefs.cpu_compatible || changed_prefs.cpu_cycle_exact)
                        changed_prefs.fpu_model = 0;
                changed_prefs.address_space_24 = 1;
                if (newcpu == 0 && changed_prefs.cpu_cycle_exact)
                        changed_prefs.m68k_speed = 0;
                break;
        case 68020:
                changed_prefs.fpu_model = newfpu == 0 ? 0 : (newfpu == 2 ? 68882 : 68881);
                break;
        case 68030:
                changed_prefs.address_space_24 = 0;
                changed_prefs.fpu_model = newfpu == 0 ? 0 : (newfpu == 2 ? 68882 : 68881);
                break;
        case 68040:
                changed_prefs.fpu_model = newfpu ? 68040 : 0;
                changed_prefs.address_space_24 = 0;
                if (changed_prefs.fpu_model)
                        changed_prefs.fpu_model = 68040;
                break;
        case 68060:
                changed_prefs.fpu_model = newfpu ? 68060 : 0;
                changed_prefs.address_space_24 = 0;
                break;
        }
 } else {
	switch ([((NSMenuItem*)sender) tag]) {
	case 10:
		changed_prefs.address_space_24 = !changed_prefs.address_space_24;
		break;
	case 11:
		changed_prefs.cpu_compatible = !changed_prefs.cpu_compatible;
		break;
#ifdef MMUEMU
	case 12:
		changed_prefs.mmu_model = 68040;
		break;
#endif
	}
 }
}

// cpu speed
- (void)changeCPUSpeed:(id)sender
{
	unsigned int v;
	v = [((NSMenuItem*)sender) tag];
	if (v == 0) {
		changed_prefs.m68k_speed = -1;
		changed_prefs.cpu_cycle_exact = 0;
	}
	if (v == 1) {
		changed_prefs.m68k_speed = 0;
		changed_prefs.cpu_cycle_exact = 0;
	}
	if (v == 2) {
		changed_prefs.m68k_speed = 0;
		changed_prefs.cpu_cycle_exact = 1;
	}

	changed_prefs.blitter_cycle_exact = changed_prefs.cpu_cycle_exact;
	if (changed_prefs.cpu_cycle_exact) {
		if (changed_prefs.cpu_model == 68000)
			changed_prefs.cpu_compatible = 1;
		if (changed_prefs.cpu_model <= 68020)
			changed_prefs.m68k_speed = 0;
		changed_prefs.immediate_blits = 0;
		changed_prefs.gfx_framerate = 1;
		changed_prefs.cachesize = 0;
	}
}

// fpu
- (void)changeFPU:(id)sender
{
	unsigned int v;
	v = [((NSMenuItem*)sender) tag];
	if (v < 10) {
	/*	if (v == 1) v = 68881;
		if (v == 2) v = 68882;*/
		changed_prefs.fpu_model = v;
#ifdef JIT
	} else {
		changed_prefs.fpu_strict = !changed_prefs.fpu_strict;
#endif
	}
}

// sound
- (void)changeSound:(id)sender
{
	changed_prefs.produce_sound = [((NSMenuItem*)sender) tag];
	config_changed = 1;
}

// sound interpolation
- (void)changeSoundInterpolation:(id)sender
{
	changed_prefs.sound_interpol = [((NSMenuItem*)sender) tag];
	config_changed = 1;
}

// sound filter
- (void)changeSoundFilter:(id)sender
{
	switch ([((NSMenuItem*)sender) tag]) {
        case 0:
                changed_prefs.sound_filter = 0;
                break;
        case 1:
                changed_prefs.sound_filter = 1;
                changed_prefs.sound_filter_type = 0;
                break;
        case 2:
                changed_prefs.sound_filter = 1;
                changed_prefs.sound_filter_type = 1;
                break;
        case 3:
                changed_prefs.sound_filter = 2;
                changed_prefs.sound_filter_type = 0;
                break;
        case 4:
                changed_prefs.sound_filter = 2;
                changed_prefs.sound_filter_type = 1;
                break;
	}
	config_changed = 1;
}

// gfx - line mode
- (void)changeGfxLineMode:(id)sender
{
	changed_prefs.gfx_scanlines = [((NSMenuItem*)sender) tag];
	config_changed = 1;
}

// gfx - centering
- (void)changeGfxCentering:(id)sender
{
	if ([((NSMenuItem*)sender) tag] == 0)
		changed_prefs.gfx_xcenter = !changed_prefs.gfx_xcenter;
	if ([((NSMenuItem*)sender) tag] == 1)
		changed_prefs.gfx_ycenter = !changed_prefs.gfx_ycenter;
	config_changed = 1;
}

// select kick rom
- (void)selectKickROM:(id)sender
{
	[self displayOpenPanelForKickROM:[((NSMenuItem*)sender) tag]];
}

// select flash ram
- (void)selectFlashRAM:(id)sender
{
	[self displayOpenPanelForFlashRAM:[((NSMenuItem*)sender) tag]];
}

// select cartridge rom
- (void)selectCartridge:(id)sender
{
	[self displayOpenPanelForCartridge:[((NSMenuItem*)sender) tag]];
}

// select savestate to save
- (void)selectSaveState:(id)sender
{
	[self displayOpenPanelForSaveState:[((NSMenuItem*)sender) tag]];
}

// select savestate to load
- (void)selectLoadState:(id)sender
{
	[self displayOpenPanelForLoadState:[((NSMenuItem*)sender) tag]];
}
@end

/*
 * Revert to windowed mode if in fullscreen mode. Returns 1 if the
 * mode was initially fullscreen and was successfully changed. 0 otherwise.
 */
int ensureNotFullscreen (void)
{
	unsigned int result = 0;

	if (is_fullscreen ()) {
		toggle_fullscreen (0);

		if (is_fullscreen ()) {
			write_log ("Cannot activate GUI in full-screen mode\n");
		} else {
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
		toggle_fullscreen(0);

	wasFullscreen = NO;
}

/* This function is called from od-macosx/main.m
 * WARNING: This gets called *before* real_main(...)!
 */
void cocoa_gui_early_setup (void)
{
	[[PuaeGui sharedInstance] createMenus];
}

int gui_init (void)
{
//        read_rom_list ();
        inputdevice_updateconfig (&changed_prefs, &currprefs);

	return 1;
}

int gui_update (void)
{
	return 1;
}

void gui_exit (void)
{
}

static void gui_flicker_led2 (int led, int unitnum, int status)
{
        static int resetcounter[LED_MAX];
        uae_u8 old;
        uae_u8 *p;

        if (led == LED_HD)
                p = &gui_data.hd;
        else if (led == LED_CD)
                p = &gui_data.cd;
        else if (led == LED_MD)
                p = &gui_data.md;
        else
                return;
        old = *p;
        if (status == 0) {
                resetcounter[led]--;
                if (resetcounter[led] > 0)
                        return;
        }

        *p = status;
        resetcounter[led] = 6;
        if (old != *p)
                gui_led (led, *p);
}

void gui_flicker_led (int led, int unitnum, int status)
{
        if (led < 0) {
                gui_flicker_led2 (LED_HD, 0, 0);
                gui_flicker_led2 (LED_CD, 0, 0);
                gui_flicker_led2 (LED_MD, 0, 0);
        } else {
                gui_flicker_led2 (led, unitnum, status);
        }
}

void gui_fps (int fps, int idle, int color)
{
	gui_data.fps  = 0; //LATER: fps
	gui_data.idle = 0; //LATER: idle
        gui_led (LED_FPS, 0);
        gui_led (LED_CPU, 0);
        gui_led (LED_SND, (gui_data.sndbuf_status > 1 || gui_data.sndbuf_status < 0) ? 0 : 1);
}

void gui_led (int led, int on)
{
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
}

void gui_display (int shortcut)
{
	//int result;

	if ((shortcut >= 0) && (shortcut < 4)) {
		[[PuaeGui sharedInstance] displayOpenPanelForInsertIntoDriveNumber:shortcut];
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

	NSRunAlertPanel(nil, [NSString stringWithCString:msg encoding:NSASCIIStringEncoding], nil, nil, nil);

	write_log ("%s", msg);

	restoreFullscreen ();
}
void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected) {}
void gui_lock (void) {}
void gui_unlock (void) {}

static int guijoybutton[MAX_JPORTS];
static int guijoyaxis[MAX_JPORTS][4];
static bool guijoychange;

void gui_gameport_button_change (int port, int button, int onoff)
{
        //write_log ("%d %d %d\n", port, button, onoff);
#ifdef RETROPLATFORM
        int mask = 0;
        if (button == JOYBUTTON_CD32_PLAY)
                mask = RP_JOYSTICK_BUTTON5;
        if (button == JOYBUTTON_CD32_RWD)
                mask = RP_JOYSTICK_BUTTON6;
        if (button == JOYBUTTON_CD32_FFW)
                mask = RP_JOYSTICK_BUTTON7;
        if (button == JOYBUTTON_CD32_GREEN)
                mask = RP_JOYSTICK_BUTTON4;
        if (button == JOYBUTTON_3 || button == JOYBUTTON_CD32_YELLOW)
                mask = RP_JOYSTICK_BUTTON3;
        if (button == JOYBUTTON_1 || button == JOYBUTTON_CD32_RED)
                mask = RP_JOYSTICK_BUTTON1;
        if (button == JOYBUTTON_2 || button == JOYBUTTON_CD32_BLUE)
                mask = RP_JOYSTICK_BUTTON2;
        rp_update_gameport (port, mask, onoff);
#endif
        if (onoff)
                guijoybutton[port] |= 1 << button;
        else
                guijoybutton[port] &= ~(1 << button);
        guijoychange = true;
}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
        int onoff = state ? 100 : 0;
        if (axis < 0 || axis > 3)
                return;
        if (max < 0) {
                if (guijoyaxis[port][axis] == 0)
                        return;
                if (guijoyaxis[port][axis] > 0)
                        guijoyaxis[port][axis]--;
        } else {
                if (state > max)
                        state = max;
                if (state < 0)
                        state = 0;
                guijoyaxis[port][axis] = max ? state * 127 / max : onoff;
#ifdef RETROPLATFORM
                if (axis == DIR_LEFT_BIT)
                        rp_update_gameport (port, RP_JOYSTICK_LEFT, onoff);
                if (axis == DIR_RIGHT_BIT)
                        rp_update_gameport (port, DIR_RIGHT_BIT, onoff);
                if (axis == DIR_UP_BIT)
                        rp_update_gameport (port, DIR_UP_BIT, onoff);
                if (axis == DIR_DOWN_BIT)
                        rp_update_gameport (port, DIR_DOWN_BIT, onoff);
#endif
        }
        guijoychange = true;
}
