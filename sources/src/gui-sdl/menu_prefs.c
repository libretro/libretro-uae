#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "SDL.h"
#include "button_mappings.h"
#include <stdlib.h>

int prefz (int parameter);

extern void write_text(int x, int y, char* txt);
extern void blit_image(SDL_Surface* img, int x, int y);
extern SDL_Surface *display;
#ifdef USE_GL
#define NO_SDL_GLEXT
# include <SDL_opengl.h>
/* These are not defined in the current version of SDL_opengl.h. */
# ifndef GL_TEXTURE_STORAGE_HINT_APPLE
#  define GL_TEXTURE_STORAGE_HINT_APPLE 0x85BC
#  endif
# ifndef GL_STORAGE_SHARED_APPLE
#  define GL_STORAGE_SHARED_APPLE 0x85BF
# endif
extern struct gl_buffer_t glbuffer;
extern void render_gl_buffer (const struct gl_buffer_t *buffer, int first_line, int last_line);
extern void flush_gl_buffer (const struct gl_buffer_t *buffer, int first_line, int last_line);
#endif
extern SDL_Surface* tmpSDLScreen;
extern SDL_Surface* pMenu_Surface;
extern SDL_Color text_color;
extern char msg[50];
extern char msg_status[50];
#define TITLE_X 52
#define TITLE_Y 9
#define STATUS_X 30 
#define STATUS_Y 460

int prefz (int parameter) {
	SDL_Event event;

	pMenu_Surface = SDL_LoadBMP("guidep/images/menu_tweak.bmp");
	if (pMenu_Surface == NULL) {
		write_log ("SDLUI: Failed to load menu image\n");
		abort();
	}
	int prefsloopdone = 0;
	int kup = 0;
	int kdown = 0;
	int kleft = 0;
	int kright = 0;
	int selected_item = 0;
	int deger;
	int q;
	int w;

	char* prefs[]	= {	"CPU",
						"CPU Speed",
						"Chipset",
						"Chip",
						"Fast",
						"Bogo",
						"Sound",
						"Frame Skip",
						"Floppy Speed" };

	char* p_cpu[]	= {"68000", "68010", "68020", "68020/68881", "68ec020", "68ec020/68881"};	//5
	char* p_speed[]	= {"max","real"};								//20
	char* p_chip[]	= {"OCS", "ECS (Agnus)", "ECS (Denise)", "ECS", "AGA"};				//4
	char* p_sound[]	= {"Off", "Off (emulated)", "On", "On (perfect)"};				//3
	char* p_frame[]	= {"0","1","2","3"};								//3
	char* p_ram[]	= {"0","512","1024"};								//2
	char* p_floppy[]= {"0","100","200","300"};							//3
	int defaults[]	= {0,0,0,0,0,0,0,0};

	defaults[0] = currprefs.cpu_model;
	if (currprefs.address_space_24 != 0) {
		if (currprefs.cpu_model == 2) { defaults[0] = 4; }
		if (currprefs.cpu_model == 3) { defaults[0] = 5; }
	}
	defaults[1] = 0;
//	defaults[1] = currprefs.m68k_speed;
	defaults[2] = currprefs.chipset_mask;
	defaults[3] = currprefs.chipmem_size;
	defaults[4] = currprefs.fastmem_size;
	defaults[5] = currprefs.bogomem_size;
	defaults[6] = currprefs.produce_sound;
	defaults[7] = currprefs.gfx_framerate;
	defaults[8] = currprefs.floppy_speed;

	char *tmp;
	tmp=(char*)malloc(6);

	while (!prefsloopdone) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				prefsloopdone = 1;
			}
			if (event.type == SDL_JOYBUTTONDOWN) {
				switch (event.jbutton.button) {
					case PLATFORM_BUTTON_UP: selected_item--; break;
					case PLATFORM_BUTTON_DOWN: selected_item++; break;
					case PLATFORM_BUTTON_LEFT: kleft = 1; break;
					case PLATFORM_BUTTON_RIGHT: kright = 1; break;
					case PLATFORM_BUTTON_SELECT: prefsloopdone = 1; break;
					case PLATFORM_BUTTON_B: prefsloopdone = 1; break;
				}
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:	prefsloopdone = 1; break;
				 	case SDLK_UP:		selected_item--; break;
					case SDLK_DOWN:		selected_item++; break;
					case SDLK_LEFT:		kleft = 1; break;
					case SDLK_RIGHT:	kright = 1; break;
					case SDLK_b:		prefsloopdone = 1; break;
					default: break;
				}
			}
		}

		if (kleft == 1) {
			defaults[selected_item]--;
			kleft = 0;

			if (selected_item == 1) { 
				//cpu_speed_change = 1; 
			}
			if (selected_item == 6) { 
				//snd_change = 1; 
			}
			if (selected_item == 7) { 
				//gfx_frameskip_change = 1; 
			}
		}

		if (kright == 1) {
			defaults[selected_item]++;
			kright = 0;

			if (selected_item == 1) { 
				//cpu_speed_change = 1; 
			}
			if (selected_item == 6) { 
				//snd_change = 1; 
			}
			if (selected_item == 7) { 
				//gfx_frameskip_change = 1; 
			}
		}

		if (defaults[0] < 0) defaults[0] = 5;	//cpu
		if (defaults[1] < 0) defaults[1] = 1; //speed
		if (defaults[2] < 0) defaults[2] = 4;	//chipset
		if (defaults[3] < 0) defaults[3] = 3;	//chip
		if (defaults[4] < 0) defaults[4] = 3;	//slow
		if (defaults[5] < 0) defaults[5] = 3;	//fast
		if (defaults[6] < 0) defaults[6] = 3;	//sound
		if (defaults[7] < 0) defaults[7] = 3;	//frameskip
		if (defaults[8] < 0) defaults[8] = 3;	//floppy

		if (defaults[0] > 5) defaults[0] = 0;	//cpu
		if (defaults[1] > 1) defaults[1] = 0;	//speed
		if (defaults[2] > 4) defaults[2] = 0;	//chipset
		if (defaults[3] > 3) defaults[3] = 0;	//chip
		if (defaults[4] > 3) defaults[4] = 0;	//slow
		if (defaults[5] > 3) defaults[5] = 0;	//fast
		if (defaults[6] > 3) defaults[6] = 0;	//sound
		if (defaults[7] > 3) defaults[7] = 0;	//frameskip
		if (defaults[8] > 3) defaults[8] = 0;	//floppy

		if (selected_item < 0) { selected_item = 8; }
		if (selected_item > 8) { selected_item = 0; }
	// background
		SDL_BlitSurface (pMenu_Surface, NULL, tmpSDLScreen, NULL);

#define OPTIONS_Y 200
	// texts
		int sira = 0;
		int pos = 0;
		for (q=0; q<9; q++) {
			if (selected_item == q) {
				text_color.r = 150;
				text_color.g = 50;
				text_color.b = 50;
			}

			pos = 50 + (sira * 20);
			write_text (20, pos, prefs[q]); //

			if (q == 0) {
				write_text (OPTIONS_Y, pos, p_cpu[defaults[q]]);
			}
			if (q == 1) {
				sprintf(tmp, "%d", p_speed[defaults[q]]);
				write_text (OPTIONS_Y, pos, tmp); 
			}
			if (q == 2) {
				write_text (OPTIONS_Y, pos, p_chip[defaults[q]]);
			}
			if (q > 2 && q < 6) {
				if (defaults[q] == 0) { deger = 0; }
				if (defaults[q] == 1) { deger = 512; }
				if (defaults[q] == 2) { deger = 1024; }
				if (defaults[q] == 3) { deger = 2048; }

				sprintf(tmp,"%d",deger);
				write_text (OPTIONS_Y, pos, tmp);
			}
			if (q == 6) {
				write_text (OPTIONS_Y, pos, p_sound[defaults[q]]);
			}
			if (q == 7) {
				write_text (OPTIONS_Y, pos, p_frame[defaults[q]]);
			}
			text_color.r = 0;
			text_color.g = 0;
			text_color.b = 0;
			sira++;
		}

		write_text (TITLE_X, TITLE_Y, msg);
		write_text (STATUS_X, STATUS_Y, msg_status);

		SDL_BlitSurface (tmpSDLScreen, NULL, display, NULL);
#ifdef USE_GL
		flush_gl_buffer (&glbuffer, 0, display->h - 1);
		render_gl_buffer (&glbuffer, 0, display->h - 1);
        glFlush ();
        SDL_GL_SwapBuffers ();
#else
		SDL_Flip (display);
#endif
	} //while done
/*
	if (defaults[0] == 4) { }
	if (defaults[0] == 5) { }
	defaults[1]--;
*/
    pMenu_Surface = SDL_LoadBMP("guidep/images/menu.bmp");
	return 0;
}
