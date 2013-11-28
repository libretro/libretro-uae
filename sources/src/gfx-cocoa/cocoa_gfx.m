/*
 * UAE - The Un*x Amiga Emulator
 *
 * Cocoa graphics support
 *
 * Copyright 2011 Mustafa Tufan
 */

#include "cocoa_gfx.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "../keymap/keymap.h"

#include "options.h"
#include "uae.h"
#include "xwin.h"
#include "custom.h"
#include "drawing.h"
#include "keyboard.h"
#include "keybuf.h"
#include "gui.h"
#include "debug.h"
#include "inputdevice.h"
#include "hotkeys.h"

#ifdef PICASSO96
#include "picasso96.h"
extern int screen_is_picasso;
#endif
// -----
@implementation PUAE_GLView
 
- (void)commonGLSetup
{
    glDisable (GL_ALPHA_TEST);
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_SCISSOR_TEST);
    glDisable (GL_DITHER);
    glDisable (GL_CULL_FACE);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask (GL_FALSE);
    glStencilMask (0);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glDisable (GL_BLEND);
    glHint (GL_TRANSFORM_HINT_APPLE, GL_FASTEST);
    
    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}
 
// prepare open gl: called by view
- (void)prepareOpenGL
{
    glContext = [self openGLContext];
    cglContext = [glContext CGLContextObj];
    
    CGLLockContext(cglContext);
 
    // sync to VBlank
    GLint swapInt = 1;
    [glContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    [self commonGLSetup];
    
    CGLUnlockContext(cglContext);
        
    isOpenGLReady = true;
}

@end

// -----
void gfx_default_options (struct uae_prefs *p)
{
}

void screenshot (int mode, int doprepare)
{
	write_log ("Screenshot not supported yet\n");
}

int debuggable (void)
{
	return 1;
}

int gfx_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
	return 0;
}

int target_checkcapslock (int scancode, int *state)
{
}

int is_fullscreen (void)
{
}

#ifdef PICASSO96
void gfx_set_picasso_modeinfo (uae_u32 w, uae_u32 h, uae_u32 depth, RGBFTYPE rgbfmt)
{
}

int picasso_palette (void)
{
	return 0;
}

void DX_Invalidate (int first, int last)
{
}

int DX_Fill (int dstx, int dsty, int width, int height, uae_u32 color, RGBFTYPE rgbtype)
{
}
#endif

void gfx_set_picasso_state (int on)
{
}

void graphics_leave (void)
{
}

int input_get_default_mouse (struct uae_input_device *uid, int i, int port, int af)
{
}

uae_u8 *gfx_lock_picasso (int fullupdate)
{
}

void gfx_unlock_picasso (void)
{
}

void gfx_set_picasso_colors (RGBFTYPE rgbfmt)
{
}

void toggle_fullscreen (int mode)
{
}

int getcapslockstate (void)
{
	return 0;
}

void setcapslockstate (int state)
{
}

void toggle_mousegrab (void)
{
}

int graphics_init (void)
{
	int success = 0;
	return success;
}

int graphics_setup (void)
{
	int result = 0;
	return result;
}

int check_prefs_changed_gfx (void)
{
	return 0;
}

void handle_events (void)
{
}

//
// --kbd
//
static unsigned int get_kb_num (void)
{
	return 1;
}

static const char *get_kb_friendlyname (unsigned int kb)
{
	return "Default keyboard";
}
static const char *get_kb_uniquename (unsigned int kb)
{
	return "DEFKEYB1";
}

static unsigned int get_kb_widget_num (unsigned int kb)
{
	return 255; // fix me
}

static int get_kb_widget_first (unsigned int kb, int type)
{
	return 0;
}

static int get_kb_widget_type (unsigned int kb, unsigned int num, char *name, uae_u32 *code)
{
	return 0;
}

static int init_kb (void)
{
	return 0;
}

static void close_kb (void)
{
}

static void read_kb (void)
{
}

static int acquire_kb (unsigned int num, int flags)
{
	return 1;
}

static void unacquire_kb (unsigned int num)
{
}

static int get_kb_flags (int num)
{
	return 0;
}

struct inputdevice_functions inputdevicefunc_keyboard =
{
	init_kb,
	close_kb,
	acquire_kb,
	unacquire_kb,
	read_kb,
	get_kb_num,
	get_kb_friendlyname,
	get_kb_uniquename,
	get_kb_widget_num,
	get_kb_widget_type,
	get_kb_widget_first,
	get_kb_flags
};

//
// --mouse
//
static int init_mouse (void)
{
	return 1;
}

static void close_mouse (void)
{
}

static int acquire_mouse (unsigned int num, int flags)
{
	return 1;
}

static void unacquire_mouse (unsigned int num)
{
}

static unsigned int get_mouse_num (void)
{
	return 1;
}

static const char *get_mouse_friendlyname (unsigned int mouse)
{
	return "Default mouse";
}
static const char *get_mouse_uniquename (unsigned int mouse)
{
	return "DEFMOUSE1";
}

static unsigned int get_mouse_widget_num (unsigned int mouse)
{
	return 0;
}

static int get_mouse_widget_first (unsigned int mouse, int type)
{
	return -1;
}

static int get_mouse_widget_type (unsigned int mouse, unsigned int num, char *name, uae_u32 *code)
{
	return 0;
}

static void read_mouse (void)
{
}

static int get_mouse_flags (int num)
{
	return 1;
}

struct inputdevice_functions inputdevicefunc_mouse = {
	init_mouse,
	close_mouse,
	acquire_mouse,
	unacquire_mouse,
	read_mouse,
	get_mouse_num,
	get_mouse_friendlyname,
	get_mouse_uniquename,
	get_mouse_widget_num,
	get_mouse_widget_type,
	get_mouse_widget_first,
	get_mouse_flags
};
