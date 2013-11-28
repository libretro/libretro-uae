/*
 * UAE - The Un*x Amiga Emulator
 *
 * Cocoa graphics support
 *
 * Copyright 2011 Mustafa Tufan
 */

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CVDisplayLink.h>

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

// OpenGL
NSOpenGLContext *glContext;
CGLContextObj    cglContext;
BOOL             isOpenGLReady;
