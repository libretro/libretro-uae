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

#include "libretro-glue.h"

#define PIX_BYTES 2

#define TD_POSY 30

#define LOG_MSG(...) 
#define LOG_MSG2(...) 

extern int retrow; 
extern int retroh;

unsigned short int clut[] = {
	0x0000,  /* full background transparency */
	0x0200,  /* background semi transparent */
	0x06FF,  /* opaque + light orange */
	0x06AA,  /* opaque + dark orange  */
} ;

unsigned short int* pixbuf = NULL;

extern unsigned short int  bmp[1024*1024];
extern short signed int SNDBUF[1024*2];
extern int  sndbufpos;

int pause_emulation;
int prefs_changed = 0;

int vsync_enabled = 0;
//int stat_count;
int opt_scrw = 0;
int opt_scrh = 0;
unsigned long stat_value = 0;

int opt_scanline = 0;

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
	setmousestate (0, 0, dx, 0);
    setmousestate (0, 1, dy, 0);	
}

void retro_mouse_but0(int down){

	setmousebuttonstate (0, 0, down);

}

void retro_mouse_but1(int down){

	setmousebuttonstate (0, 1, down);
}

static jflag[5]={0,0,0,0,0};

void retro_joy(int port, unsigned char joy0){
//0x01,0x02,0x04,0x08,0x80
// UP  DWN  LEFT RGT  BTN0
// 0    1     2   3    4

	if(joy0&0x80){
		if(jflag[4]==0){
			setjoybuttonstate(port, 0, 1); // joy0, button0, state ON
			jflag[4]=1;
		}
	}else {
		if(jflag[4]==1){
			setjoybuttonstate(port, 0, 0); // joy0, button0, state OFF
			jflag[4]=0;
		}
	}

	//Left
	if(joy0&0x04){
		if(jflag[2]==0){
			setjoystickstate(port, 0, -1, 1);
			jflag[2]=1;
		}
	}else {
		if(jflag[2]==1){
			setjoystickstate(port, 0, 0, 1);
			jflag[2]=0;
		}
	}

	//Down
	if(joy0&0x02){
		if(jflag[1]==0){
			setjoystickstate(port, 1, 1, 1);
			jflag[1]=1;
		}
	}else {
		if(jflag[1]==1){
			setjoystickstate(port, 1, 0, 1);
			jflag[1]=0;
		}
	}

	//Right
	if(joy0&0x08){
		if(jflag[3]==0){
			setjoystickstate(port, 0, 1, 1);
			jflag[3]=1;
		}
	}else {
		if(jflag[3]==1){
			setjoystickstate(port, 0, 0, 1);
			jflag[3]=0;
		}
	}

	//UP
	if(joy0&0x01){
		if(jflag[0]==0){

			setjoystickstate(port, 1, -1, 1);
			jflag[0]=1;
		}
	}else {
		if(jflag[0]==1){
			setjoystickstate(port, 1, 0, 1);
			jflag[0]=0;
		}
	}


}

/* cursor hiding */

/* --- keyboard input --- */

void retro_key_down(int key){

	inputdevice_do_keyboard (key, 1);

}

void retro_key_up(int key){

	inputdevice_do_keyboard (key, 0);
	
}

extern int pauseg,SND;
int RLOOP=1;

int retro_renderSound(short* samples, int sampleCount)
{
   int i; 

   if (sampleCount < 1 || SND!=1 || pauseg==1)
      return 0;

   for(i=0;i<sampleCount;i+=2)
   {
      retro_audio_cb( samples[i], samples[i+1]);
   }
}

void InitOSGLU(void)
{

}

void  UnInitOSGLU(void)
{

}

void ScreenUpdate ()
{
}

void retro_flush_screen (struct vidbuf_description *gfxinfo, int ystart, int yend)
{
	if(pauseg==1)
      pause_select();
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
	currprefs.gfx_size_win.width=retrow;

#ifdef ENABLE_LOG_SCREEN
	currprefs.gfx_height = 256;
	currprefs.gfx_linedbl = 0;	//disable line doubling
#else
	currprefs.gfx_size_win.height= retroh;
#endif	
	opt_scrw = currprefs.gfx_size_win.width;
	opt_scrh = currprefs.gfx_size_win.height;

	if (currprefs.gfx_size_win.width>= 640) {
	//currprefs.gfx_lores = 0;
	} else {
	//	currprefs.gfx_lores = 1;
	}
	//vsync_enabled = currprefs.gfx_vsync;
	LOG_MSG2("screen w=%i", currprefs.gfx_size_win.width);
	LOG_MSG2("screen h=%i", currprefs.gfx_size_win.height);

#ifdef ENABLE_LOG_SCREEN
	pixbuf = (unsigned int*) malloc(currprefs.gfx_size_win.width * 576 * PIX_BYTES);
#else
	pixbuf = (unsigned short int*) &bmp[0];
#endif
	
	//printf("graphics init  pixbuf=%p color_mode=%d width=%d\n", pixbuf, currprefs.color_mode, currprefs.gfx_width_win);
	if (pixbuf == NULL) {
		printf("Error: not enough memory to initialize screen buffer!\n");
		return -1;
	}
	memset(pixbuf, 0x80, currprefs.gfx_size_win.width * currprefs.gfx_size_win.height * PIX_BYTES);

	gfxvidinfo.width_allocated = currprefs.gfx_size_win.width;
	gfxvidinfo.height_allocated = currprefs.gfx_size_win.height;
	gfxvidinfo.maxblocklines = 1000;
	gfxvidinfo.pixbytes = PIX_BYTES;
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
	alloc_colors64k (5, 6, 5, 11, 5, 0, 0, 0, 0, 0); 

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
	if (prefs_changed) {
		prefs_changed = 0;
		return 1;
	}
	return 0;
}

void clean_led_area(void) {
	int size = 11 * opt_scrw * gfxvidinfo.pixbytes;
	unsigned short int* addr;

	addr = pixbuf;
	addr+= (opt_scrh-11-TD_POSY)* opt_scrw;
	memset(addr, 0, size);
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
    return 2;
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

static TCHAR *get_joystick_friendlyname (int joy) {
  switch (joy) {
    case 0: return "Retro pad 0";
    case 1: return "Retro pad 1";
    default: return "Retro pad 2";
  }
}

static char *get_joystick_uniquename (int joy) {
  switch (joy) {
    case 0: return "retropad0";
    case 1: return "retropad1";
    default: return "retropad2";
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

int input_get_default_joystick (struct uae_input_device *uid, int num, int port, int af, int mode, bool gp) {
  if (port == 1) {
    uid[1].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_JOY1_HORIZ;
    uid[1].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_JOY1_VERT;
    uid[1].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_JOY1_FIRE_BUTTON;
    uid[1].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_JOY1_2ND_BUTTON;
    uid[1].eventid[ID_BUTTON_OFFSET + 2][0] =  INPUTEVENT_JOY1_3RD_BUTTON;

    uid[1].enabled = 1;
  }

  if (port == 0) {
    uid[0].eventid[ID_AXIS_OFFSET + 0][0]   =  INPUTEVENT_JOY2_HORIZ;
    uid[0].eventid[ID_AXIS_OFFSET + 1][0]   =  INPUTEVENT_JOY2_VERT;
    uid[0].eventid[ID_BUTTON_OFFSET + 0][0] =  INPUTEVENT_JOY2_FIRE_BUTTON;
    uid[0].eventid[ID_BUTTON_OFFSET + 1][0] =  INPUTEVENT_JOY2_2ND_BUTTON;
    uid[0].eventid[ID_BUTTON_OFFSET + 2][0] =  INPUTEVENT_JOY2_3RD_BUTTON;

    uid[0].enabled = 1;
  }

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
	return "DEFMOUSE1";
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

