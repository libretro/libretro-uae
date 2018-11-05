#include "libretro.h"
#include "libretro-glue.h"
#include "keyboard.h"
#include "libretro-keymap.h"
#include "graph.h"
#include "vkbd.h"
#include "libretro-mapper.h"

#ifdef __CELLOS_LV2__
#include "sys/sys_time.h"
#include "sys/timer.h"
#include <sys/time.h>
#include <time.h>
#define usleep  sys_timer_usleep

void gettimeofday (struct timeval *tv, void *blah)
{
   int64_t time = sys_time_get_system_time();

   tv->tv_sec  = time / 1000000;
   tv->tv_usec = time - (tv->tv_sec * 1000000);  // implicit rounding will take care of this for us
}

#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

unsigned short int bmp[1024*1024];
unsigned short int savebmp[1024*1024];

int NPAGE=-1, KCOL=1, BKGCOLOR=0, MAXPAS=6;
int SHIFTON=-1,MOUSEMODE=-1,NUMJOY=0,SHOWKEY=-1,PAS=2,STATUTON=-1;

short signed int SNDBUF[1024*2];
int snd_sampler = 44100 / 50;
char RPATH[512];
char DSKNAME[512];

int gmx=320,gmy=240; //gui mouse

int al[2];//left analog1
int ar[2];//right analog1
int touch=-1; // gui mouse btn
int fmousex,fmousey; // emu mouse
int pauseg=0; //enter_gui
int SND=1; //SOUND ON/OFF
int NUMjoy=1;
int slowdown=0;

int controller_state[2][6]; // [port][property]
// 0 joy button a
// 1 joy button b
// 2 joy left/right
// 3 joy up/down
// 4 mouse a
// 5 mouse b

//hack
static int counter = 0;

static int mbt[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

long frame=0;
unsigned long  Ktime=0 , LastFPSTime=0;

int BOXDEC= 32+2;
int STAT_BASEY;

extern char key_state[512];
extern char key_state2[512];

extern bool opt_analog;

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


void enter_gui(void)
{	
   enter_options();
}

#ifdef WIIU
#include <features_cpu.h>
#endif

/* in milliseconds */
long GetTicks(void)
{
#ifdef _ANDROID_
   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   return (now.tv_sec*1000000 + now.tv_nsec/1000)/1000;
#elif defined(WIIU)
return (cpu_features_get_time_usec())/1000;
#else
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return (tv.tv_sec*1000000 + tv.tv_usec)/1000;
#endif

} 

void gui_poll_events(void)
{
   //NO SURE FIND BETTER WAY TO COME BACK IN MAIN THREAD IN HATARI GUI

   Ktime = GetTicks();

   if(Ktime - LastFPSTime >= 1000/50)
   {
      slowdown=0;
      frame++; 
      LastFPSTime = Ktime;		
      co_switch(mainThread);
      //retro_run();
   }
}

void Print_Statut(void)
{
   STAT_BASEY=CROP_HEIGHT;

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
   L2  show/hide status
   R2  [UNUSED]
   L   mouse speed down
   R   mouse speed up
   SEL toggle mouse/joy mode
   STR show/hide virtual keyboard (vkbd)
   A   joystick fire 1/mouse 1/valid key in vkbd
   B   joystick fire 2/mouse 2
   X   [UNUSED]
   Y   [UNUSED]
   */

void Screen_SetFullUpdate(void)
{
   reset_drawing();
}

void Process_key(void)
{
   int i;
   for(i=0;i<320;i++)
   {
      key_state[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i)?0x80:0;

      if(keyboard_translation[i]==0x60/*AK_LSH*/ )
      {  //SHIFT CASE

         if( key_state[i] && key_state2[i]==0 )
         {
            if(SHIFTON == 1)
               retro_key_up(	keyboard_translation[i] );					
            else if(SHIFTON == -1) 
               retro_key_down(keyboard_translation[i] );

            SHIFTON=-SHIFTON;

            key_state2[i]=1;

         }
         else if (!key_state[i] && key_state2[i]==1)
            key_state2[i]=0;

      }
      else
      {

         if(key_state[i] && keyboard_translation[i]!=-1  && key_state2[i] == 0)
         {
            retro_key_down(keyboard_translation[i]);		
            key_state2[i]=1;
         }
         else if ( !key_state[i] && keyboard_translation[i]!=-1 && key_state2[i]==1 )
         {
            retro_key_up(keyboard_translation[i]);
            key_state2[i]=0;

         }

      }
   }
}

void update_input(void)
{
   int i;	
   //   RETRO      B    Y    SLT  STA  UP   DWN  LEFT RGT  A    X    L    R    L2   R2   L3   R3
   //   INDEX      0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
   static int oldi=-1;
   static int vkx=0,vky=0;

   if(oldi!=-1)
   {
      retro_key_up(oldi);
      oldi=-1;
   }

   input_poll_cb();
   Process_key();

   i=RETRO_DEVICE_ID_JOYPAD_START;//show vkey toggle
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
      mbt[i]=1;
   else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) )
   {
      mbt[i]=0;
      SHOWKEY=-SHOWKEY;
      Screen_SetFullUpdate();
   }

   i=RETRO_DEVICE_ID_JOYPAD_SELECT;//mouse/joy toggle
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
      mbt[i]=1;
   else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) )
   {
      mbt[i]=0;
      MOUSEMODE=-MOUSEMODE;
      Screen_SetFullUpdate();
      counter = -1;
   }
   
   i=RETRO_DEVICE_ID_JOYPAD_L;//mouse speed down
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
      mbt[i]=1;
   else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) )
   {
      mbt[i]=0;
      PAS--;if(PAS<1)PAS=1;
   }

   i=RETRO_DEVICE_ID_JOYPAD_R;//mouse speed up
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
      mbt[i]=1;
   else if ( mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) )
   {
      mbt[i]=0;
      PAS++;if(PAS>MAXPAS)PAS=MAXPAS;
   }

   i=RETRO_DEVICE_ID_JOYPAD_L2;//show/hide status
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i) && mbt[i]==0 )
      mbt[i]=1;
   else if (mbt[i]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i))
   {
      mbt[i]=0;
      STATUTON=-STATUTON;
      Screen_SetFullUpdate();
   }


   if(SHOWKEY==1)
   {
      static int vkflag[5]={0,0,0,0,0};		

      if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) && vkflag[0]==0 )
         vkflag[0]=1;
      else if (vkflag[0]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) )
      {
         vkflag[0]=0;
         vky -= 1; 
      }

      if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) && vkflag[1]==0 )
         vkflag[1]=1;
      else if (vkflag[1]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) )
      {
         vkflag[1]=0;
         vky += 1; 
      }

      if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) && vkflag[2]==0 )
         vkflag[2]=1;
      else if (vkflag[2]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) )
      {
         vkflag[2]=0;
         vkx -= 1;
      }

      if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) && vkflag[3]==0 )
         vkflag[3]=1;
      else if (vkflag[3]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) )
      {
         vkflag[3]=0;
         vkx += 1;
      }

      if(vkx < 0)
         vkx=9;
      if(vkx > 9)
         vkx=0;
      if(vky < 0)
         vky=4;
      if(vky > 4)
         vky=0;

      virtual_kdb(bmp,vkx,vky);

      i=8;
      if(input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i)  && vkflag[4]==0) 	
         vkflag[4]=1;
      else if( !input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i)  && vkflag[4]==1)
      {

         vkflag[4]=0;
         i=check_vkey2(vkx,vky);

         if(i==-2)
         {
            NPAGE=-NPAGE;oldi=-1;
            //Clear interface zone					
            Screen_SetFullUpdate();
         }
         else if(i==-1)
            oldi=-1;
         else if(i==-3)
         {//KDB bgcolor
            Screen_SetFullUpdate();
            KCOL=-KCOL;
            oldi=-1;
         }
         else if(i==-4)
         {//VKbd show/hide 			
            oldi=-1;
            Screen_SetFullUpdate();
            SHOWKEY=-SHOWKEY;
         }
         else if(i==-5)
         {//Change Joy number
            NUMjoy=-NUMjoy;
            oldi=-1;
         }
         else
         {
            if(i==0x60/*AK_LSH*/)
            {
               if(SHIFTON == 1)
                  retro_key_up(i);
               else retro_key_down(i);

               SHIFTON=-SHIFTON;

               Screen_SetFullUpdate();

               oldi=-1;
            }
            else
            {
               oldi=i;
               retro_key_down(i);
            }
         }				
      }

      if(STATUTON==1)
         Print_Statut();

      return;
   }

   static int mbL=0,mbR=0;
   int mouse_l;
   int mouse_r;
   int16_t mouse_x;
   int16_t mouse_y;

   if(MOUSEMODE==-1)
   { //Joy mode

      //dirty hack to register buttons
      if (counter < 8)
      {
        do_hack();
      }
      else
      {
        update_joystick_input(0);
        update_joystick_input(1);
      }
      
      mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
      mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
      mouse_l = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
      mouse_r = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);

      fmousex=mouse_x;
      fmousey=mouse_y;
   }
   else
   {  //Mouse mode
      fmousex=fmousey=0;

      //ANALOG RIGHT
      ar[0] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
      ar[1] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));

      if(ar[0]<=-1024)
         fmousex-=(-ar[0])/1024;
      if(ar[0]>= 1024)
         fmousex+=( ar[0])/1024;
      if(ar[1]<=-1024)
         fmousey-=(-ar[1])/1024;
      if(ar[1]>= 1024)
         fmousey+=( ar[1])/1024;

      //PAD
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
         fmousex += PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
         fmousex -= PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
         fmousey += PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
         fmousey -= PAS;

      mouse_l=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
      mouse_r=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);  
   }

   if(mbL==0 && mouse_l)
   {
      mbL=1;		
      retro_mouse_but0(1);
   }
   else if(mbL==1 && !mouse_l)
   {
      retro_mouse_but0(0);
      mbL=0;
   }

   if(mbR==0 && mouse_r)
   {
      mbR=1;
      retro_mouse_but1(1);
   }
   else if(mbR==1 && !mouse_r)
   {
      retro_mouse_but1(0);
      mbR=0;
   }

   retro_mouse(fmousex, fmousey);

   if(STATUTON==1)
      Print_Statut();
   
}

// joystick buttons don't register correctly upon init or mouse/toggle until
// they've been pressed once 
void do_hack(void)
{
    
        if (counter == 0 && controller_state[0][0] != 1)
        {
            setjoybuttonstate (0, 0, 1);
            controller_state[0][0] = 1;
        }
        else if (counter == 1 && controller_state[0][0] != 0)
        {
            setjoybuttonstate (0, 0, 0);
            controller_state[0][0] = 0;
        }
        else if (counter == 2 && controller_state[0][1] != 1)
        {
            setjoybuttonstate (0, 1, 1);
            controller_state[0][1] = 1;
        }
        else if (counter == 3 && controller_state[0][1] != 0)
        {
            setjoybuttonstate (0, 1, 0);
            controller_state[0][1] = 0;
        }
        else if (counter == 4 && controller_state[1][0] != 1)
        {
            setjoybuttonstate (1, 0, 1);
            controller_state[1][0] = 1;
        }
        else if (counter == 5 && controller_state[1][0] != 0)
        {
            setjoybuttonstate (1, 0, 0);
            controller_state[1][0] = 0;
        }
        else if (counter == 6 && controller_state[1][1] != 1)
        {
            setjoybuttonstate (1, 1, 1);
            controller_state[1][1] = 1;
        }
        else if (counter == 7 && controller_state[1][1] != 0)
        {
            setjoybuttonstate (1, 1, 0);
            controller_state[1][1] = 0;
        }
        
        counter++;     
}

void update_joystick_input(int port)
{   
    int state;
    
    //primary fire button
    state = input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
    if (state != controller_state[port][0])
    {
        setjoybuttonstate (port, 0, state);
        controller_state[port][0] = state;
    }
    
    //secondary fire button
    state = input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B); 
    if (state != controller_state[port][1])
    {
        setjoybuttonstate (port, 1, state);
        controller_state[port][1] = state;
    }
    
    //update directions
    if(opt_analog)
    {
        state = input_state_cb(port, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
        if (state != controller_state[port][2])
        {
            setjoystickstate (port, 0, state, 32767);
            controller_state[port][2];
        }
        
        state = input_state_cb(port, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
        if (state != controller_state[port][3])
        {
            setjoystickstate (port, 1, state, 32767);
            controller_state[port][3];
        } 
    }
    else
    {
        // dpad
        state = 0;
        state += input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) ? -1 : 0;
        state += input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) ? 1 : 0;
        if (state != controller_state[port][3]) {
           setjoystickstate  (port, 1, state, 1);   
           controller_state[port][3] = state;
        }

        state = 0;
        state += input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ? -1 : 0;
        state += input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? 1 : 0;
        if (state != controller_state[port][2]) {
           setjoystickstate  (port, 0, state, 1);
           controller_state[port][2] = state;
        }
    }
    
    
}

int input_gui(void)
{
   int SAVPAS=PAS;	
   int ret=0;

   input_poll_cb();

   int mouse_l;
   int mouse_r;
   int16_t mouse_x,mouse_y;
   mouse_x=mouse_y=0;

   //mouse/joy toggle
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, 2) && mbt[2]==0 )
      mbt[2]=1;
   else if ( mbt[2]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, 2) )
   {
      mbt[2]=0;
      MOUSEMODE=-MOUSEMODE;
   }
   
   if(slowdown>0)return 0;
   
   if(MOUSEMODE==1)
   {

      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))mouse_x += PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))mouse_x -= PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))mouse_y += PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))mouse_y -= PAS;
      mouse_l=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
      mouse_r=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);

      PAS=SAVPAS;
   }
   else
   {
      mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
      mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
      mouse_l    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
      mouse_r    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
   }
   
   slowdown=1;
   static int mmbL=0,mmbR=0;

   if(mmbL==0 && mouse_l)
   {
      mmbL=1;		
      touch=1;

   }
   else if(mmbL==1 && !mouse_l)
   {
      mmbL=0;
      touch=-1;
   }

   if(mmbR==0 && mouse_r)
      mmbR=1;		
   else if(mmbR==1 && !mouse_r)
      mmbR=0;

   gmx+=mouse_x;
   gmy+=mouse_y;
   if(gmx<0)
      gmx=0;
   if(gmx>retrow-1)
      gmx=retrow-1;
   if(gmy<0)
      gmy=0;
   if(gmy>retroh-1)
      gmy=retroh-1;
   
   return 0;
}

int update_input_gui(void)
{
   int ret=0;	
   static int dskflag[7]={0,0,0,0,0,0,0};// UP -1 DW 1 A 2 B 3 LFT -10 RGT 10 X 4	

   input_poll_cb();	

   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) && dskflag[0]==0 )
   {
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
         else if (dskflag[4]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) )
         {
            dskflag[4]=0;
            ret= -10; 
         }

   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) && dskflag[5]==0 )
      dskflag[5]=1;
   else if (dskflag[5]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) )
   {
      dskflag[5]=0;
      ret= 10; 
   }

   if ( ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) || \
            input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LCTRL) ) && dskflag[2]==0 ){
      dskflag[2]=1;

   }
   else if (dskflag[2]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A)\
         && !input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LCTRL) )
   {
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
   else if (dskflag[6]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X) )
   {
      dskflag[6]=0;
      ret= 4; 
   }

   return ret;

}


