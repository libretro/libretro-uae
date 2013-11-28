#include "SDL.h"
#include "gp2x.h"
#include <stdlib.h>
#include "gp2x-cpuctrl/cpuctrl.h"

extern void write_text(int x, int y, char* txt);
extern void blit_image(SDL_Surface* img, int x, int y);
extern SDL_Surface* display;
extern SDL_Surface* tmpSDLScreen;
extern SDL_Surface* pMenu_Surface;
extern SDL_Color text_color;
extern char msg[50];
extern char msg_status[50];

int tweakz (int parametre) {
	SDL_Event event;

    	pMenu_Surface = SDL_LoadBMP("images/menu_tweak.bmp");
	int tweakloopdone = 0;
	int kup = 0;
	int kdown = 0;
	int kleft = 0;
	int kright = 0;
	int kb = 0;
	int seciliolan = 0;
	int q;

	char* tweaks[]		= {"CPU Mhz","tRC","tRAS","tWR","tMRD","tRFC","tRP","tRCD","PLL","Timing","Default (200mhz)","Default (266mhz)","Evil Dragon (266mhz)"};
	int defaults[]		= {200,8,16,3,8,8,8,8,1,1};
	int def_slow_tweak[]	= {200,8,16,3,8,8,8,8,1,1};
	int def_fast_tweak[]	= {266,8,16,3,8,8,8,8,1,1};
	int evil_tweak[]	= {266,6,4,1,1,1,2,2,1,1};
	char *tmp;
	tmp=(char*)malloc(5);

#ifdef GP2X
	unsigned sysfreq=0;
	int cpufreq;
	sysfreq	= get_freq_920_CLK();
	sysfreq*= get_920_Div()+1;
	cpufreq	= sysfreq/1000000;

	defaults[0] = cpufreq;
	defaults[1] = get_tRC();
	defaults[2] = get_tRAS();
	defaults[3] = get_tWR();
	defaults[4] = get_tMRD();
	defaults[5] = get_tRFC();
	defaults[6] = get_tRP();
	defaults[7] = get_tRCD();
#endif

	while (!tweakloopdone) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { tweakloopdone = 1; }
			if (event.type == SDL_JOYBUTTONDOWN) {
             			switch (event.jbutton.button) {
					case GP2X_BUTTON_UP: seciliolan--; break;
					case GP2X_BUTTON_DOWN: seciliolan++; break;
					case GP2X_BUTTON_LEFT: kleft = 1; break;
					case GP2X_BUTTON_RIGHT: kright = 1; break;
					case GP2X_BUTTON_SELECT: tweakloopdone = 1; break;
					case GP2X_BUTTON_B: kb =1; break;
				}
			}
      			if (event.type == SDL_KEYDOWN) {
    				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:	tweakloopdone = 1; break;
				 	case SDLK_UP:		seciliolan--; break;
					case SDLK_DOWN:		seciliolan++; break;
					case SDLK_LEFT:		kleft = 1; break;
					case SDLK_RIGHT:	kright = 1; break;
					case SDLK_b:		kb = 1; break;
					default: break;
				}
			}
		}
		if (kb == 1) {
			if (seciliolan == 10) {
				for (q=0; q<10;q++) { defaults[q] = def_slow_tweak[q]; }
			}
			if (seciliolan == 11) {
				for (q=0; q<10;q++) { defaults[q] = def_fast_tweak[q]; }
			}
			if (seciliolan == 12) {
				for (q=0; q<10;q++) { defaults[q] = evil_tweak[q]; }
			}
			if (seciliolan < 10) {
				//apply
				//printf("FLCK: %d",0);			set_CAS(0);
				printf("FLCK: %d",defaults[0]);		set_FCLK(defaults[0]);
				printf("tRC : %d",defaults[1]);		set_tRC(defaults[1]-1);
				printf("tRAS: %d",defaults[2]);		set_tRAS(defaults[2]-1);
				printf("tWR : %d",defaults[3]);		set_tWR(defaults[3]-1);
				printf("tMRD: %d",defaults[4]);		set_tMRD(defaults[4]-1);
				printf("tRFC: %d",defaults[5]);		set_tRFC(defaults[5]-1);
				printf("tRP : %d",defaults[6]);		set_tRP(defaults[6]-1);
				printf("tRCD: %d",defaults[7]);		set_tRCD(defaults[7]-1);
				if (defaults[8] == 0) {
					printf("FLCD: %d",defaults[9]);	set_add_FLCDCLK(defaults[9]-1);
				} else {
					printf("ULCD: %d",defaults[9]);	set_add_ULCDCLK(defaults[9]-1);
				}
				tweakloopdone = 1;
			}
			kb = 0;
		}
		if (kleft == 1) {
			if (seciliolan < 10) { defaults[seciliolan]--; }
			kleft = 0;
		}
		if (kright == 1) {
			if (seciliolan < 10) { defaults[seciliolan]++; }
			kright = 0;
		}

		if (defaults[0] == 199) defaults[0] = 200; //mhz
		if (defaults[1] == 17) defaults[1] = 1;	//trc
		if (defaults[2] == 17) defaults[2] = 1; //tras
		if (defaults[3] == 17) defaults[3] = 1;	//twr
		if (defaults[4] == 17) defaults[4] = 1; //tmrd
		if (defaults[5] == 17) defaults[5] = 1;	//trfc
		if (defaults[6] == 17) defaults[6] = 1; //trp
		if (defaults[7] == 17) defaults[7] = 1; //trcd
		if (defaults[8] == -1) defaults[8] = 1; //lcd
		if (defaults[9] == -7) defaults[9] = 10; //timing

		if (defaults[0] == 316) defaults[0] = 315; //mhz
		if (defaults[1] == 0) defaults[1] = 16; //trc
		if (defaults[2] == 0) defaults[2] = 16; //tras
		if (defaults[3] == 0) defaults[3] = 16; //twr
		if (defaults[4] == 0) defaults[4] = 16; //tmrd
		if (defaults[5] == 0) defaults[5] = 16; //trfc
		if (defaults[6] == 0) defaults[6] = 16; //trp
		if (defaults[7] == 0) defaults[7] = 16; //trcd
		if (defaults[8] == 11) defaults[7] = -6; //timing

		if (seciliolan < 0) { seciliolan = 12; }
		if (seciliolan > 12) { seciliolan = 0; }

	// background
		SDL_BlitSurface (pMenu_Surface,NULL,tmpSDLScreen,NULL);

	// texts
		int sira = 0;
		int skipper = 0;
		for (q=0; q<13; q++) {
			if (q == 10) { skipper = 30; }
			if (seciliolan == q) {
				text_color.r = 255; text_color.g = 100; text_color.b = 100;
			}
			write_text (10,skipper+25+(sira*10),tweaks[q]); //
			if (q <10) {
				if (q == 8) {
					if (defaults[8] == 0) {	
						sprintf(tmp,"%s","FPLL");
					} else {		
						sprintf(tmp,"%s","UPLL"); 
					}
				} else {
					sprintf(tmp,"%d",defaults[q]);
				}
				write_text (100,skipper+25+(sira*10),tmp);
			}
			if (seciliolan == q) {
				text_color.r = 0; text_color.g = 0; text_color.b = 0;
			}
			sira++;
		}

		write_text (25,3,msg);
		write_text (15,228,msg_status);
		SDL_BlitSurface (tmpSDLScreen,NULL,display,NULL);
#ifdef USE_GL
		flush_gl_buffer (&glbuffer, 0, display->h - 1);
		render_gl_buffer (&glbuffer, 0, display->h - 1);
        glFlush ();
        SDL_GL_SwapBuffers ();
#else
		SDL_Flip (display);
#endif
	} //while done

    	pMenu_Surface = SDL_LoadBMP("images/menu.bmp");
	return 0;
}
