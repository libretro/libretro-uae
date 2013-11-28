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

#import <Cocoa/Cocoa.h>

@interface PUAE_Main : NSObject
@end
