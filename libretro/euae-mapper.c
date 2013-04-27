#include "libretro.h"

#include "libretro-euae.h"

#include "graph.h"
#include "vkbd.h"

#ifdef PS3PORT
#include "sys/sys_time.h"
#include "sys/timer.h"
#define usleep  sys_timer_usleep
#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

unsigned short int bmp[TEX_WIDTH * TEX_HEIGHT];

int NPAGE=-1, KCOL=1, BKGCOLOR=0, MAXPAS=6;
int SHIFTON=-1,MOUSEMODE=-1,NUMJOY=0,SHOWKEY=-1,PAS=2,STATUTON=-1;

short signed int SNDBUF[1024*2];
int snd_sampler = 22255 / 60;
char RPATH[512];
char DSKNAME[512];

int gmx=320,gmy=240; //gui mouse

unsigned char MXjoy0; // joy
int touch=-1; // gui mouse btn
int fmousex,fmousey; // emu mouse
int pauseg=0; //enter_gui
int SND=1; //SOUND ON/OFF
int NUMjoy=1;

static int mbt[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

long frame=0;
unsigned long  Ktime=0 , LastFPSTime=0;

int BOXDEC= 32+2;
int STAT_BASEY=CROP_HEIGHT;

extern char * filebrowser(const char *path_and_name);

static retro_input_state_t input_state_cb;
static retro_input_poll_t input_poll_cb;

void retro_set_input_state(retro_input_state_t cb)
{
   	input_state_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   	input_poll_cb = cb;
}

void texture_init(){

	memset(bmp, 0, sizeof(bmp));
}

void Emu_init(){
	
}

void Emu_uninit(){


}

#ifdef AND
#define DEFAULT_PATH "/mnt/sdcard/euae/"
#else
	#ifdef PS3PORT
	#define DEFAULT_PATH "/dev_hdd0/HOMEBREW/UAE/"
	#else
	#define DEFAULT_PATH "/"
	#endif
#endif

#define MDEBUG
#ifdef MDEBUG
#define mprintf printf
#else
#define mprintf(...) 
#endif


#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "gui.h"
#include "xwin.h"
#include "disk.h"


void enter_gui(){		

	enter_options();
	
	return;
}

#if 0
//DBG LOAD DSK
void enter_gui0(){	
	
	char dskimg[512]="\0";
	static int inbrowser=1;
	int ret=0;
	

		sprintf(dskimg,"%s\0",filebrowser(DEFAULT_PATH));

		if(strcmp(dskimg,"EMPTY\0")==0){
			mprintf("Cancel Fileselect(%s)\n",dskimg);
			inbrowser=0;	
			pauseg=0;
			return;		
		}
		else if(strcmp(dskimg,"NO CHOICE\0")==0){
			
		}
		else{
			mprintf("Ok Fileselect(%s)\n",dskimg);			
			strcpy (changed_prefs.df[0], dskimg);
			inbrowser=0;
			pauseg=0;
			return;
		}
		
	
}
#endif 

void Print_Statut(){

	DrawFBoxBmp(bmp,0,CROP_HEIGHT,CROP_WIDTH,STAT_YSZ,RGB565(0,0,0));

	if(MOUSEMODE==-1)
		Draw_text(bmp,STAT_DECX,STAT_BASEY,0xffff,0x8080,1,2,40,"Joy  ");
	else
		Draw_text(bmp,STAT_DECX,STAT_BASEY,0xffff,0x8080,1,2,40,"Mouse");

	Draw_text(bmp,STAT_DECX+40 ,STAT_BASEY,0xffff,0x8080,1,2,40,(SHIFTON>0?"SHFT":""));
        Draw_text(bmp,STAT_DECX+80 ,STAT_BASEY,0xffff,0x8080,1,2,40,"MS:%d",PAS);
	Draw_text(bmp,STAT_DECX+120,STAT_BASEY,0xffff,0x8080,1,2,40,"Joy:%d",NUMjoy);

}

/*
L2  show/hide Statut
R2  
L   show/hide vkbd
R   MOUSE SPEED(gui/emu)
SEL toggle mouse/joy mode
STR toggle num joy 
A   fire/mousea/valid key in vkbd
B   mouseb
X   [unused fixme: switch Shift ON/OFF] / umount in gui
Y   Emu Gui
*/

void Screen_SetFullUpdate(){
	reset_drawing();
}


void update_input(void)
{
	int i;	
	//   RETRO      B    Y    SLT  STA  UP   DWN  LEFT RGT  A    X    L    R    L2   R2   L3   R3
        //   INDEX      0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
	static int vbt[16]={0x1C,0x39,0x01,0x3B,0x01,0x02,0x04,0x08,0x80,0x6D,0x15,0x31,0x24,0x1F,0x6E,0x6F};
	static int oldi=-1;
	static int vkx=0,vky=0;

 	MXjoy0=0;
	if(oldi!=-1){retro_key_up(oldi);oldi=-1;}

   	input_poll_cb();

        if (input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_F11) || input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y) )
		pauseg=1;

	i=10;//show vkey toggle
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
	    	mbt[i]=1;
	else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) ){
	   	mbt[i]=0;
		SHOWKEY=-SHOWKEY;
		Screen_SetFullUpdate();
	}

	i=2;//mouse/joy toggle
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
	    	mbt[i]=1;
	else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) ){
	   	mbt[i]=0;
		MOUSEMODE=-MOUSEMODE;
	}

	i=3;//num joy toggle
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
	    	mbt[i]=1;
	else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) ){
	   	mbt[i]=0;
		NUMJOY++;if(NUMJOY>1)NUMJOY=0;
		NUMjoy=-NUMjoy;
	}
		
        i=11;//mouse gui speed
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
	    	mbt[i]=1;
	else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) ){
	   	mbt[i]=0;
		PAS++;if(PAS>MAXPAS)PAS=1;
	}


/*
//FIXME
        i=9;//switch shift On/Off 
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
	    	mbt[i]=1;
	else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) ){
	   	mbt[i]=0;
		SHIFTON=-SHIFTON;
		Screen_SetFullUpdate();
	}
*/

	i=12;//show/hide statut
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
	    	mbt[i]=1;
	else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) ){
	   	mbt[i]=0;
		STATUTON=-STATUTON;
		Screen_SetFullUpdate();
	}

	if(SHOWKEY==1){

		static int vkflag[5]={0,0,0,0,0};		
		
		if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) && vkflag[0]==0 )
		    	vkflag[0]=1;
		else if (vkflag[0]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) ){
		   	vkflag[0]=0;
			vky -= 1; 
		}

		if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) && vkflag[1]==0 )
		    	vkflag[1]=1;
		else if (vkflag[1]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) ){
		   	vkflag[1]=0;
			vky += 1; 
		}

		if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) && vkflag[2]==0 )
		    	vkflag[2]=1;
		else if (vkflag[2]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ){
		   	vkflag[2]=0;
			vkx -= 1;
		}

		if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) && vkflag[3]==0 )
		    	vkflag[3]=1;
		else if (vkflag[3]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ){
		   	vkflag[3]=0;
			vkx += 1;
		}

		if(vkx<0)vkx=9;
		if(vkx>9)vkx=0;
		if(vky<0)vky=4;
		if(vky>4)vky=0;

		virtual_kdb(bmp,vkx,vky);

		i=8;
		if(input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i)  && vkflag[4]==0) 	
			vkflag[4]=1;
		else if( !input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i)  && vkflag[4]==1) {

			vkflag[4]=0;
			i=check_vkey2(vkx,vky);

			if(i==-2){
				NPAGE=-NPAGE;oldi=-1;
				//Clear interface zone					
				Screen_SetFullUpdate();
				
			}
			else if(i==-1)oldi=-1;
			else if(i==-3){//KDB bgcolor
				Screen_SetFullUpdate();
				KCOL=-KCOL;
				oldi=-1;
			}
			else if(i==-4){//VKbd show/hide 			
				
				oldi=-1;
				Screen_SetFullUpdate();
				SHOWKEY=-SHOWKEY;
			}
			else if(i==-5){//Change Joy number
				NUMjoy=-NUMjoy;
				oldi=-1;
			}
			else {	
				if(i==0x60/*AK_LSH*/){
										
					if(SHIFTON == 1)retro_key_up(i);
					else retro_key_down(i);

					SHIFTON=-SHIFTON;

					Screen_SetFullUpdate();
					
					oldi=-1;
				}
				else {
					oldi=i;
					retro_key_down(i);
				}
			}				

		}

         	if(STATUTON==1)Print_Statut();

		return;
	}

   	static int mbL=0,mbR=0;
	int mouse_l;
	int mouse_r;
  	int16_t mouse_x;
   	int16_t mouse_y;
  
	if(MOUSEMODE==-1){ //Joy mode

		for(i=4;i<9;i++)if( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) )MXjoy0 |= vbt[i]; // Joy press	
		retro_joy0(MXjoy0);

	   	mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
	   	mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
	   	mouse_l    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
	   	mouse_r    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);

		fmousex=mouse_x;
  		fmousey=mouse_y;

	}
	else {  //Mouse mode
		fmousex=fmousey=0;

		if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))fmousex += PAS;

		if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))fmousex -= PAS;

		if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))fmousey += PAS;

		if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))fmousey -= PAS;

		mouse_l=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
		mouse_r=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
       }

	if(mbL==0 && mouse_l){
		mbL=1;		
		retro_mouse_but0(1);
	}
	else if(mbL==1 && !mouse_l) {

 		
		retro_mouse_but0(0);
                mbL=0;
	}

	if(mbR==0 && mouse_r){
		mbR=1;
		retro_mouse_but1(1);
	}
	else if(mbR==1 && !mouse_r) {

 		retro_mouse_but1(0);
                mbR=0;
	}

	retro_mouse(fmousex, fmousey);

	if(STATUTON==1)Print_Statut();

}

int update_input_gui()
{
	int ret=0;	
	static int dskflag[7]={0,0,0,0,0,0,0};// UP -1 DW 1 A 2 B 3 LFT -10 RGT 10 X 4	

   	input_poll_cb();	
		
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) && dskflag[0]==0 ){
	    	dskflag[0]=1;
		ret= -1; 
	}
	else /*if (dskflag[0]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) ){
	   	dskflag[0]=0;
		ret= -1; 
	}*/dskflag[0]=0;

	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) && dskflag[1]==0 ){
	    	dskflag[1]=1;
		ret= 1;
	} 
	else/* if (dskflag[1]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) ){
	   	dskflag[1]=0;
		ret= 1; 
	}*/dskflag[1]=0;
		
	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) && dskflag[4]==0 )
	    	dskflag[4]=1;
	else if (dskflag[4]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ){
	   	dskflag[4]=0;
		ret= -10; 
	}

	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) && dskflag[5]==0 )
	    	dskflag[5]=1;
	else if (dskflag[5]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ){
	   	dskflag[5]=0;
		ret= 10; 
	}

	if ( ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) || \
		input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LCTRL) ) && dskflag[2]==0 ){
	    	dskflag[2]=1;
		
	}
	else if (dskflag[2]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A)\
		&& !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LCTRL) ){

	   	dskflag[2]=0;
		ret= 2;
	}

	if ( ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B) || \
		input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LALT) ) && dskflag[3]==0 ){
	    	dskflag[3]=1;
	}
	else if (dskflag[3]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B) &&\
		!input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LALT) ){

	   	dskflag[3]=0;
		ret= 3;
	}

	if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X) && dskflag[6]==0 )
	    	dskflag[6]=1;
	else if (dskflag[6]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X) ){
	   	dskflag[6]=0;
		ret= 4; 
	}

	return ret;

}

