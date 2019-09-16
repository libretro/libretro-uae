/********************************************

        RETRO GLUE

*********************************************/
 
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"

#include "uae.h"
#include "memory.h"
#include "xwin.h"
#include "custom.h"

#include "hrtimer.h"

#include "inputdevice.h"
void inputdevice_release_all_keys (void);

#include "drawing.h"
#include "hotkeys.h"

#include "libretro.h"
#include "libretro-glue.h"
#include "libretro-mapper.h"
extern unsigned int uae_devices[4];
extern unsigned int inputdevice_finalized;
extern int pix_bytes;

#define TD_POSY 30

#define LOG_MSG(...) 
#define LOG_MSG2(...) 

extern int defaultw;
extern int defaulth;

unsigned short int clut[] = {
	0x0000,  /* full background transparency */
	0x0200,  /* background semi transparent */
	0x06FF,  /* opaque + light orange */
	0x06AA,  /* opaque + dark orange  */
} ;

unsigned short int* pixbuf = NULL;

extern unsigned short int  bmp[1024*1024];
void retro_audio_cb(short l, short r);

int prefs_changed = 0;
int vsync_enabled = 0;
//int stat_count;
int opt_scrw = 0;
int opt_scrh = 0;
unsigned long stat_value = 0;

void gui_init (int argc, char **argv)
{
	
}

/*
 * Handle target-specific cfgfile options
 */
void target_save_options (struct zfile* f, struct uae_prefs *p)
//void target_save_options (FILE *f, const struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

void target_default_options (struct uae_prefs *p, int type)
{
}


void retro_mouse(int dx, int dy)
{
   changed_prefs.jports[0].id = JSEM_MICE;
	setmousestate (0, 0, dx, 0);
    setmousestate (0, 1, dy, 0);
}

void retro_mouse_but0(int down)
{
   changed_prefs.jports[0].id = JSEM_MICE;
	setmousebuttonstate (0, 0, down);
}

void retro_mouse_but1(int down)
{
   changed_prefs.jports[0].id = JSEM_MICE;
	setmousebuttonstate (0, 1, down);
}

static unsigned int jflag[4][11]={0,0,0,0,0,0,0,0,0,0,0};

void retro_joy(unsigned int port, unsigned long joy){
// 0x001,0x002,0x004,0x008,0x010,0x020,0x040,0x200,0x400,0x800,0x100
// UP    DWN   LFT   RGT   A  	 B     X     Y     L     R     STRT
// 0     1     2     3     4     5     6     7     8     9     10
// setjoybuttonstate(port, button, state)

//if(joy!=0) printf("JOY: %d, PORT: %d\n", joy, port);

	// B
	if(joy&0x020){
		if(jflag[port][4]==0){
			setjoybuttonstate(port, 0, 1);
			jflag[port][4]=1;
		}
	}else {
		if(jflag[port][4]==1){
			setjoybuttonstate(port, 0, 0);
			jflag[port][4]=0;
		}
	}

	// A
	if(joy&0x010){
		if(jflag[port][5]==0){
			setjoybuttonstate(port, 1, 1);
			jflag[port][5]=1;
		}
	}else {
		if(jflag[port][5]==1){
			setjoybuttonstate(port, 1, 0);
			jflag[port][5]=0;
		}
	}

	// Y
	if(joy&0x200){
		if(jflag[port][6]==0){
			setjoybuttonstate(port, 2, 1);
			jflag[port][6]=1;
		}
	}else {
		if(jflag[port][6]==1){
			setjoybuttonstate(port, 2, 0);
			jflag[port][6]=0;
		}
	}

	// X
	if(joy&0x040){
		if(jflag[port][7]==0){
			setjoybuttonstate(port, 3, 1);
			jflag[port][7]=1;
		}
	}else {
		if(jflag[port][7]==1){
			setjoybuttonstate(port, 3, 0);
			jflag[port][7]=0;
		}
	}

	// L
	if(joy&0x400){
		if(jflag[port][8]==0){
			setjoybuttonstate(port, 4, 1);
			jflag[port][8]=1;
		}
	}else {
		if(jflag[port][8]==1){
			setjoybuttonstate(port, 4, 0);
			jflag[port][8]=0;
		}
	}

	// R
	if(joy&0x800){
		if(jflag[port][9]==0){
			setjoybuttonstate(port, 5, 1);
			jflag[port][9]=1;
		}
	}else {
		if(jflag[port][9]==1){
			setjoybuttonstate(port, 5, 0);
			jflag[port][9]=0;
		}
	}

	// Start
	if(joy&0x100){
		if(jflag[port][10]==0){
			setjoybuttonstate(port, 6, 1);
			jflag[port][10]=1;
		}
	}else {
		if(jflag[port][10]==1){
			setjoybuttonstate(port, 6, 0);
			jflag[port][10]=0;
		}
	}

	// Up
	if(joy&0x001){
		if(jflag[port][0]==0){
			setjoystickstate(port, 1, -1, 1);
			jflag[port][0]=1;
		}
	}else {
		if(jflag[port][0]==1){
			setjoystickstate(port, 1, 0, 1);
			jflag[port][0]=0;
		}
	}


	// Down
	if(joy&0x002){
		if(jflag[port][1]==0){
			setjoystickstate(port, 1, 1, 1);
			jflag[port][1]=1;
		}
	}else {
		if(jflag[port][1]==1){
			setjoystickstate(port, 1, 0, 1);
			jflag[port][1]=0;
		}
	}

	// Left
	if(joy&0x004){
		if(jflag[port][2]==0){
			setjoystickstate(port, 0, -1, 1);
			jflag[port][2]=1;
		}
	}else {
		if(jflag[port][2]==1){
			setjoystickstate(port, 0, 0, 1);
			jflag[port][2]=0;
		}
	}


	// Right
	if(joy&0x008){
		if(jflag[port][3]==0){
			setjoystickstate(port, 0, 1, 1);
			jflag[port][3]=1;
		}
	}else {
		if(jflag[port][3]==1){
			setjoystickstate(port, 0, 0, 1);
			jflag[port][3]=0;
		}
	}


}

/* cursor hiding */

/* --- keyboard input --- */

void retro_key_down(int key)
{
	inputdevice_do_keyboard (key, 1);
}

void retro_key_up(int key)
{
	inputdevice_do_keyboard (key, 0);
}

int retro_renderSound(short* samples, int sampleCount)
{
   int i; 

   if (sampleCount < 1)
      return 0;

   for(i=0;i<sampleCount;i+=2)
   {
      retro_audio_cb( samples[i], samples[i+1]);
   }
}

void ScreenUpdate ()
{
}

void retro_flush_screen (struct vidbuf_description *gfxinfo, int ystart, int yend)
{
	co_switch(mainThread);
}


void retro_flush_block (struct vidbuf_description *gfxinfo, int ystart, int yend)
{
}

void retro_flush_line (struct vidbuf_description *gfxinfo, int y) {
}

void retro_flush_clear_screen(struct vidbuf_description *gfxinfo) {
}


int retro_lockscr(struct vidbuf_description *gfxinfo) {
   return 1;
}

void retro_unlockscr(struct vidbuf_description *gfxinfo) {

}


int graphics_init(void) {

	if (pixbuf != NULL) {
		return 1;
	}
	currprefs.gfx_size_win.width = defaultw;

#ifdef ENABLE_LOG_SCREEN
	currprefs.gfx_height = 256;
	currprefs.gfx_linedbl = 0;	//disable line doubling
#else
	currprefs.gfx_size_win.height = defaulth;
#endif	
	opt_scrw = currprefs.gfx_size_win.width;
	opt_scrh = currprefs.gfx_size_win.height;

	//if (currprefs.gfx_size_win.width>= 640) {
	//currprefs.gfx_lores = 0;
	//} else {
	//	currprefs.gfx_lores = 1;
	//}
	//vsync_enabled = currprefs.gfx_vsync;
	LOG_MSG2("screen w=%i", currprefs.gfx_size_win.width);
	LOG_MSG2("screen h=%i", currprefs.gfx_size_win.height);

#ifdef ENABLE_LOG_SCREEN
	pixbuf = (unsigned int*) malloc(currprefs.gfx_size_win.width * 576 * pix_bytes);
#else
	pixbuf = (unsigned short int*) &bmp[0];
#endif
	//printf("graphics init: pixbuf=%p color_mode=%d width=%d height=%d\n", pixbuf, currprefs.color_mode, currprefs.gfx_size_win.width, currprefs.gfx_size_win.height);
	if (pixbuf == NULL) {
		printf("Error: not enough memory to initialize screen buffer!\n");
		return -1;
	}
	//memset(pixbuf, 0x80, currprefs.gfx_size_win.width * currprefs.gfx_size_win.height * pix_bytes);

	gfxvidinfo.width_allocated = currprefs.gfx_size_win.width;
	gfxvidinfo.height_allocated = currprefs.gfx_size_win.height;
	gfxvidinfo.maxblocklines = 1000;
	gfxvidinfo.pixbytes = pix_bytes;
	gfxvidinfo.rowbytes = gfxvidinfo.width_allocated * gfxvidinfo.pixbytes ;
	gfxvidinfo.bufmem = (unsigned char*)pixbuf;
	gfxvidinfo.emergmem = 0;
	gfxvidinfo.linemem = 0;

	gfxvidinfo.lockscr = retro_lockscr;
	gfxvidinfo.unlockscr = retro_unlockscr;
	gfxvidinfo.flush_block = retro_flush_block;
	gfxvidinfo.flush_clear_screen = retro_flush_clear_screen;
	gfxvidinfo.flush_screen = retro_flush_screen;
	gfxvidinfo.flush_line = retro_flush_line;

	prefs_changed = 1;
    inputdevice_release_all_keys ();
    reset_hotkeys ();
    reset_drawing ();
	return 1;
}

int is_fullscreen (void)
{
    return 1;
}

int is_vsync (void)
{
    return vsync_enabled;
}


int mousehack_allowed (void)
{
    return 0;
}

int graphics_setup(void) {
	//32bit mode
	//Rw, Gw, Bw,   Rs, Gs, Bs,   Aw, As, Avalue, swap
	if (pix_bytes == 2)
		alloc_colors64k (5, 6, 5, 11, 5, 0, 0, 0, 0, 0); 
	else
		alloc_colors64k (8, 8, 8, 16, 8, 0, 0, 0, 0, 0);

	return 1;
}

void graphics_leave(void) {
}


void graphics_notify_state (int state) {
}



void gfx_save_options (FILE * f, const struct uae_prefs * p) {
}

int  gfx_parse_option (struct uae_prefs *p, const char *option, const char *value) {
return 0;
}



void gfx_default_options(struct uae_prefs *p) {
}

void screenshot (int type,int f) {

}

void toggle_fullscreen(int mode) {
}

int check_prefs_changed_gfx (void) {
    if (prefs_changed)
        prefs_changed = 0;
    else
        return 0;

    changed_prefs.gfx_size_win.width = defaultw;
    changed_prefs.gfx_size_win.height = defaulth;

    currprefs.gfx_size_win.width    = changed_prefs.gfx_size_win.width;
    currprefs.gfx_size_win.height   = changed_prefs.gfx_size_win.height;
    currprefs.gfx_xcenter           = changed_prefs.gfx_xcenter;
    currprefs.gfx_ycenter           = changed_prefs.gfx_ycenter;

    gfxvidinfo.width_allocated      = currprefs.gfx_size_win.width;
    gfxvidinfo.height_allocated     = currprefs.gfx_size_win.height;

    reset_drawing();
    //printf("check_prefs_changed_gfx: %d:%d, xcenter:%d ycenter:%d\n", changed_prefs.gfx_size_win.width, changed_prefs.gfx_size_win.height, changed_prefs.gfx_xcenter, changed_prefs.gfx_ycenter);
    return 0;
}

void clean_led_area(void) {
/*
    int size = 11 * opt_scrw * gfxvidinfo.pixbytes;
    unsigned short int* addr;

    addr = pixbuf;
    addr+= (opt_scrh-11-TD_POSY)* opt_scrw;
    memset(addr, 0, size);
*/
}



/***************************************************************
  Joystick functions
****************************************************************/


static int init_joysticks (void)
{
   LOG_MSG(("init_joysticks" ));

   return 1;
}

static void close_joysticks (void)
{
}

static int acquire_joystick (int num, int flags)
{
    return 1;
}

static int get_joystick_flags (int num)
{
	return 0;
}
static void unacquire_joystick (int num)
{
}

static void read_joysticks (void)
{
}

static int get_joystick_num (void)
{
    return 4;
}

static char *get_joystick_uniquename (int joy)
{
    switch (joy)
	{
		case 0:
			return "RetroPad0";
			break;
		case 1:
			return "RetroPad1";
			break;
		case 2:
			return "RetroPad2";
			break;
		case 3:
			return "RetroPad3";
			break;
		default:
			return "RetroPad1";
			break;
	}
}

static int get_joystick_widget_num (int joy)
{
    return 0;
}

static int get_joystick_widget_type (int joy, int num, char *name, uae_u32 *code)
{
    return IDEV_WIDGET_BUTTON;
}

static int get_joystick_widget_first (int joy, int type)
{
    return -1;
}

static TCHAR *get_joystick_friendlyname (int joy)
{
	switch (joy)
	{
		case 0:
			return "RetroPad0";
			break;
		case 1:
			return "RetroPad1";
			break;
		case 2:
			return "RetroPad2";
			break;
		case 3:
			return "RetroPad3";
			break;
		default:
			return "RetroPad1";
			break;
	}
}

struct inputdevice_functions inputdevicefunc_joystick = {
    init_joysticks, 
    close_joysticks, 
    acquire_joystick,
    unacquire_joystick,
    read_joysticks, 
    get_joystick_num, 
    get_joystick_friendlyname,
    get_joystick_uniquename,
    get_joystick_widget_num,
    get_joystick_widget_type,
    get_joystick_widget_first,
    get_joystick_flags
};

int input_get_default_joystick (struct uae_input_device *uid, int num, int port, int af, int mode, bool gp)
//void input_get_default_joystick (struct uae_input_device *uid)
{
    if(uae_devices[0] == RETRO_DEVICE_UAE_CD32PAD)
    {
        uid[0].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_JOY2_HORIZ;
        uid[0].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_JOY2_VERT;
        uid[0].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_JOY2_CD32_RED;
        uid[0].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_JOY2_CD32_BLUE;
        uid[0].eventid[ID_BUTTON_OFFSET + 2][0] =  INPUTEVENT_JOY2_CD32_GREEN;
        uid[0].eventid[ID_BUTTON_OFFSET + 3][0] =  INPUTEVENT_JOY2_CD32_YELLOW;
        uid[0].eventid[ID_BUTTON_OFFSET + 4][0] =  INPUTEVENT_JOY2_CD32_RWD;
        uid[0].eventid[ID_BUTTON_OFFSET + 5][0] =  INPUTEVENT_JOY2_CD32_FFW;
        uid[0].eventid[ID_BUTTON_OFFSET + 6][0] =  INPUTEVENT_JOY2_CD32_PLAY;
    }
    else
    {
        uid[0].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_JOY2_HORIZ;
        uid[0].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_JOY2_VERT;
        uid[0].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_JOY2_FIRE_BUTTON;
        uid[0].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_JOY2_2ND_BUTTON;
    }

    if(uae_devices[1] == RETRO_DEVICE_UAE_CD32PAD)
    {
        uid[1].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_JOY1_HORIZ;
        uid[1].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_JOY1_VERT;
        uid[1].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_JOY1_CD32_RED;
        uid[1].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_JOY1_CD32_BLUE;
        uid[1].eventid[ID_BUTTON_OFFSET + 2][0] =  INPUTEVENT_JOY1_CD32_GREEN;
        uid[1].eventid[ID_BUTTON_OFFSET + 3][0] =  INPUTEVENT_JOY1_CD32_YELLOW;
        uid[1].eventid[ID_BUTTON_OFFSET + 4][0] =  INPUTEVENT_JOY1_CD32_RWD;
        uid[1].eventid[ID_BUTTON_OFFSET + 5][0] =  INPUTEVENT_JOY1_CD32_FFW;
        uid[1].eventid[ID_BUTTON_OFFSET + 6][0] =  INPUTEVENT_JOY1_CD32_PLAY;
    }
    else
    {
        uid[1].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_JOY1_HORIZ;
        uid[1].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_JOY1_VERT;
        uid[1].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_JOY1_FIRE_BUTTON;
        uid[1].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_JOY1_2ND_BUTTON;
    }

    uid[2].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_PAR_JOY1_HORIZ;
    uid[2].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_PAR_JOY1_VERT;
    uid[2].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_PAR_JOY1_FIRE_BUTTON;
    uid[2].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_PAR_JOY1_2ND_BUTTON;

    uid[3].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_PAR_JOY2_HORIZ;
    uid[3].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_PAR_JOY2_VERT;
    uid[3].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_PAR_JOY2_FIRE_BUTTON;
    uid[3].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_PAR_JOY2_2ND_BUTTON;

    uid[0].enabled = 1;
    uid[1].enabled = 1;
    uid[2].enabled = 1;
    uid[3].enabled = 1;

    inputdevice_finalized = 1;
    return 1;
}


/***************************************************************
  Mouse functions
****************************************************************/

/*
 * Mouse inputdevice functions
 */

/* Hardwire for 3 axes and 3 buttons
 * There is no 3rd axis as such - mousewheel events are
 * supplied by X on buttons 4 and 5.
 */
#define MAX_BUTTONS     3
#define MAX_AXES        2
#define FIRST_AXIS      0
#define FIRST_BUTTON    MAX_AXES

static int init_mouse (void)
{
   return 1;
}

static void close_mouse (void)
{
   return;
}

static int acquire_mouse (int num, int flags)
{
   return 1;
}

static void unacquire_mouse (int num)
{
   return;
}

static int get_mouse_num (void)
{
    return 1;
}

static TCHAR *get_mouse_friendlyname (int mouse)
{
    return "Default mouse";
}

static TCHAR *get_mouse_uniquename (int mouse)
{
	return "RetroMouse";
}

static int get_mouse_widget_num (int mouse)
{
    return MAX_AXES + MAX_BUTTONS;
}

static int get_mouse_widget_first (int mouse, int type)
{
    switch (type) {
	case IDEV_WIDGET_BUTTON:
	    return FIRST_BUTTON;
	case IDEV_WIDGET_AXIS:
	    return FIRST_AXIS;
    }
    return -1;
}

static int get_mouse_widget_type (int mouse, int num, TCHAR *name, uae_u32 *code)
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
    /* We handle mouse input in handle_events() */
}

static int get_mouse_flags (int num)
{
        return 0;
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

int input_get_default_mouse (struct uae_input_device *uid, int num, int port, int af, bool gp, bool wheel)
{
    /* Supports only one mouse */
    uid[0].eventid[ID_AXIS_OFFSET + 0][0]   = INPUTEVENT_MOUSE1_HORIZ;
    uid[0].eventid[ID_AXIS_OFFSET + 1][0]   = INPUTEVENT_MOUSE1_VERT;
    uid[0].eventid[ID_AXIS_OFFSET + 2][0]   = INPUTEVENT_MOUSE1_WHEEL;
    uid[0].eventid[ID_BUTTON_OFFSET + 0][0] = INPUTEVENT_JOY1_FIRE_BUTTON;
    uid[0].eventid[ID_BUTTON_OFFSET + 1][0] = INPUTEVENT_JOY1_2ND_BUTTON;
    uid[0].eventid[ID_BUTTON_OFFSET + 2][0] = INPUTEVENT_JOY1_3RD_BUTTON;
    uid[0].enabled = 1;
	return 0;
}

/***************************************************************
  Keyboard functions
****************************************************************/


static int init_kb (void)
{
	return 1;
}

static void close_kb (void)
{
	
}

static int acquire_kb (int num, int flags)
{
    return 1;
}

static void unacquire_kb (int num)
{
}

static void read_kb (void)
{
}

static int get_kb_num (void)
{
    return 1;
}

static char *get_kb_uniquename (int mouse)
{
    return "Default keyboard";
}

static char *get_kb_friendlyname (int mouse)
{
    return "Default keyboard";
}

static int get_kb_widget_num (int mouse)
{
    return 255; //fix me
}

static int get_kb_widget_type (int mouse, int num, char *name, uae_u32 *code)
{
    if (code) *code = ~0;
    return IDEV_WIDGET_NONE;
}

static int get_kb_widget_first (int mouse, int type)
{
    return 0;
}

static int get_kb_flags (int num)
{
	return 0;
}

struct inputdevice_functions inputdevicefunc_keyboard = {
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


int getcapslockstate (void)
{
    return 0;
}

void setcapslockstate (int state)
{
}

/********************************************************************
    Misc fuctions
*********************************************************************/

int needmousehack(void) {
    return 0;
}

void toggle_mousegrab(void) {
}

/* handle pads in the "options" dialog */
int handle_options_events() {
	return 0;
}

bool handle_events() {
 return 0;
}

 void uae_pause (void)
{

}
 void uae_resume (void)
{
	
}

