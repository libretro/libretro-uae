/********************************************

        RETRO GLUE

*********************************************/

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"

#include "uae.h"
#include "mmemory.h"
#include "xwin.h"
#include "custom.h"

#define PIX_BYTES 2

#define TD_POSY 30

unsigned short int clut[] = {
	0x0000,  /* full background transparency */
	0x0200,  /* background semi transparent */
	0x06FF,  /* opaque + light orange */
	0x06AA,  /* opaque + dark orange  */
} ;

unsigned short int* pixbuf = NULL;

extern unsigned short int bmp[640*480];

int pause_emulation;
int prefs_changed = 0;

int vsync_enabled = 0;
int stat_count;
int opt_scrw = 0;
int opt_scrh = 0;
unsigned long stat_value = 0;

int opt_scanline = 0;

//void enter_options(void) {}

void gui_init (int argc, char **argv)
{
	
}

//void show_gui_message(char* msg) {}


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

int machdep_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

void machdep_default_options (struct uae_prefs *p)
{
}

void machdep_save_options (FILE *f, const struct uae_prefs *p)
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

void retro_joy0(unsigned char joy0){
//0x01,0x02,0x04,0x08,0x80
// UP  DWN  LEFT RGT  BTN0
// 0    1     2   3    4

	//btn0
	if(joy0&0x80){
		if(jflag[4]==0){
			setjoybuttonstate(0, 0, 1); // joy0, button0, state ON
			jflag[4]=1;
		}
	}else {
		if(jflag[4]==1){
			setjoybuttonstate(0, 0, 0); // joy0, button0, state OFF
			jflag[4]=0;
		}
	}

	//Left
	if(joy0&0x04){
		if(jflag[2]==0){
			setjoystickstate(0, 0, -1, 1);
			jflag[2]=1;
		}
	}else {
		if(jflag[2]==1){
			setjoystickstate(0, 0, 0, 1);
			jflag[2]=0;
		}
	}

	//Down
	if(joy0&0x02){
		if(jflag[1]==0){
			setjoystickstate(0, 1, 1, 1);
			jflag[1]=1;
		}
	}else {
		if(jflag[1]==1){
			setjoystickstate(0, 1, 0, 1);
			jflag[1]=0;
		}
	}

	//Right
	if(joy0&0x08){
		if(jflag[3]==0){
			setjoystickstate(0, 0, 1, 1);
			jflag[3]=1;
		}
	}else {
		if(jflag[3]==1){
			setjoystickstate(0, 0, 0, 1);
			jflag[3]=0;
		}
	}

	//UP
	if(joy0&0x01){
		if(jflag[0]==0){
			setjoystickstate(0, 1, -1, 1);
			jflag[0]=1;
		}
	}else {
		if(jflag[0]==1){
			setjoystickstate(0, 1, 0, 1);
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

extern int SND;

int retro_renderSound(short* samples, int sampleCount) {
	int i;
	if (sampleCount < 1 || SND!=1) {
		return 0;
	}
	for(i=0;i<sampleCount;i+=2)retro_audio_cb( samples[i], samples[i]);

}

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

long GetTicks(void)
{
#ifndef _ANDROID_
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return tv.tv_sec*1000000 + tv.tv_usec;
#else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec*1000000 + now.tv_nsec/1000;
#endif
                                                                              
} 

int RLOOP=1;
void retro_loop(){

	while(RLOOP==1)
		testloop();
	
	RLOOP=1;
}

int InitOSGLU(void)
{

 umain(0,NULL);

}

int  UnInitOSGLU(void)
{

}

void ScreenUpdate () {

}


void retro_flush_screen (struct vidbuf_description *gfxinfo, int ystart, int yend) {

/*
	unsigned short int* buff = pixbuf;
	yend++; 

	buff += ystart * gfxvidinfo.width;

	unsigned short int* dest = (unsigned short int*) &bmp[0];
	dest += ystart * gfxvidinfo.width;
	memcpy(dest,buff,gfxvidinfo.width*2*(yend-ystart));
*/
}


void retro_flush_block (struct vidbuf_description *gfxinfo, int ystart, int yend) {
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

	currprefs.gfx_width = 640; //currprefs.gfx_width_win;
#ifdef ENABLE_LOG_SCREEN
	currprefs.gfx_height = 256;
	currprefs.gfx_linedbl = 0;	//disable line doubling
#else
	currprefs.gfx_height = 480; //currprefs.gfx_height_win;
#endif	
	opt_scrw = currprefs.gfx_width;
	opt_scrh = currprefs.gfx_height;
	if (currprefs.gfx_width >= 640) {
		currprefs.gfx_lores = 0;
	} else {
		currprefs.gfx_lores = 1;
	}
	vsync_enabled = currprefs.gfx_vsync;
	LOG_MSG2("screen w=%i", currprefs.gfx_width);
	LOG_MSG2("screen h=%i", 480);

#ifdef ENABLE_LOG_SCREEN
	pixbuf = (unsigned int*) malloc(currprefs.gfx_width * 576 * PIX_BYTES);
#else
	pixbuf = (unsigned short int*) &bmp[0];//malloc(currprefs.gfx_width * currprefs.gfx_height * PIX_BYTES);
#endif
	
	printf("graphics init  pixbuf=%p color_mode=%d width=%d\n", pixbuf, currprefs.color_mode, currprefs.gfx_width_win);
	if (pixbuf == NULL) {
		printf("Error: not enough memory to initialize screen buffer!\n");
		return -1;
	}
	memset(pixbuf, 0x80, currprefs.gfx_width * currprefs.gfx_height * PIX_BYTES);

	gfxvidinfo.width = currprefs.gfx_width;
	gfxvidinfo.height = currprefs.gfx_height;
	gfxvidinfo.maxblocklines = 1000;
	gfxvidinfo.pixbytes = PIX_BYTES;
	gfxvidinfo.rowbytes = gfxvidinfo.width * gfxvidinfo.pixbytes ;
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

void screenshot (int type) {

}

void toggle_fullscreen(void) {
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

#include "inputdevice.h"


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

static char *get_joystick_name (int joy)
{
    return "retro pad";
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

struct inputdevice_functions inputdevicefunc_joystick = {
    init_joysticks, 
    close_joysticks, 
    acquire_joystick,
    unacquire_joystick,
    read_joysticks, 
    get_joystick_num, 
    get_joystick_name,
    get_joystick_widget_num,
    get_joystick_widget_type,
    get_joystick_widget_first
};

void input_get_default_joystick (struct uae_input_device *uid)
{
    uid[0].enabled = 1;
}


/***************************************************************
  Mouse functions
****************************************************************/


static int init_mouse (void)
{
	return 1;
}

static void close_mouse (void)
{
	
}

static int acquire_mouse (int num, int flags)
{

    return 1;
}

static void unacquire_mouse (int num)
{
}

static void read_mouse (void)
{
}

static int get_mouse_num (void)
{
    return 2;
}

static char *get_mouse_name (int mouse)
{
    return 0;
}

static int get_mouse_widget_num (int mouse)
{
    return 0;
}

static int get_mouse_widget_type (int mouse, int num, char *name, uae_u32 *code)
{
    return IDEV_WIDGET_NONE;
}

static int get_mouse_widget_first (int mouse, int type)
{
    return -1;
}


struct inputdevice_functions inputdevicefunc_mouse = {
    init_mouse, close_mouse, acquire_mouse, unacquire_mouse,
    read_mouse, get_mouse_num, get_mouse_name,
    get_mouse_widget_num, get_mouse_widget_type,
    get_mouse_widget_first
};


void input_get_default_mouse (struct uae_input_device * uid) {

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

static char *get_kb_name (int mouse)
{
    return "Default keyboard";
}

static int get_kb_widget_num (int mouse)
{
    return 255; //fix me
}

static int get_kb_widget_type (int mouse, int num, char *name, uae_u32 *code)
{
    return IDEV_WIDGET_NONE;
}

static int get_kb_widget_first (int mouse, int type)
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
    get_kb_name,
    get_kb_widget_num, 
    get_kb_widget_type,
    get_kb_widget_first
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

void handle_events() {
 
}


