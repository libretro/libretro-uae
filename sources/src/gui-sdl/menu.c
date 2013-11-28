#include "SDL.h"
#include "SDL_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "gui.h"
#include "zfile.h"
#include "button_mappings.h"

#define SDL_UI_DEBUG 1

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
#endif /* USE_GL */

#define VIDEO_FLAGS SDL_HWSURFACE
SDL_Surface* tmpSDLScreen = NULL;

int selected_item = 0;
char yol[256];
char msg[50];
char msg_status[50];

char launchDir[256];

extern int dirz(int parametre);
//extern int tweakz(int parametre);
extern int prefz(int parametre);
int soundVolume = 100;
extern int flashLED;

// --- internal prototypes ---
void cocoa_gui_early_setup (void);

//
int gui_init (void) {
#if 0
	if (display == NULL) {
		SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
		display = SDL_SetVideoMode(320,240,16,VIDEO_FLAGS);
#if SDL_UI_DEBUG > 0
		write_log ("SDLUI: SDL_Init display init\n");
#endif
	} else {
#if SDL_UI_DEBUG > 0
		write_log ("SDLUI: SDL_Init display ready\n");
#endif
	}
#endif
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_JoystickOpen(0);
	SDL_ShowCursor(SDL_DISABLE);
  	TTF_Init();

	amiga_font = TTF_OpenFont("guidep/fonts/amiga4ever_pro2.ttf", 16);
	if (!amiga_font) {
	    printf("SDLUI: TTF_OpenFont failed: %s\n", TTF_GetError());
		abort();
	}
	text_color.r = 50;
	text_color.g = 50;
	text_color.b = 50;

	pMenu_Surface	= SDL_LoadBMP("guidep/images/menu.bmp");
	if (pMenu_Surface == NULL) {
		write_log ("SDLUI: Failed to load menu image\n");
		abort();
	}
	pMouse_Pointer	= SDL_LoadBMP("guidep/images/mousep.bmp");
	if (pMouse_Pointer == NULL) {
		write_log ("SDLUI: Failed to load mouse pointer image\n");
		abort();
	}
	SDL_SetColorKey(pMouse_Pointer, SDL_SRCCOLORKEY, SDL_MapRGB(pMouse_Pointer->format, 75, 155, 135));

	icon_expansion		= SDL_LoadBMP("guidep/images/icon-expansion.bmp");
	if (icon_expansion == NULL) {
		write_log ("SDLUI: Failed to load icon expansion\n");
		abort();
	}
	icon_preferences	= SDL_LoadBMP("guidep/images/icon-preferences.bmp");
	if (icon_preferences == NULL) {
		write_log ("SDLUI: Failed to load icon preferences\n");
		abort();
	}
	icon_keymaps		= SDL_LoadBMP("guidep/images/icon-keymaps.bmp");
	if (icon_keymaps == NULL) {
		write_log ("SDLUI: Failed to load icon keymaps\n");
		abort();
	}
	icon_floppy			= SDL_LoadBMP("guidep/images/icon-floppy.bmp");
	if (icon_floppy == NULL) {
		write_log ("SDLUI: Failed to load icon floppy\n");
		abort();
	}
	icon_reset			= SDL_LoadBMP("guidep/images/icon-reset.bmp");
	if (icon_reset == NULL) {
		write_log ("SDLUI: Failed to load icon reset\n");
		abort();
	}
	icon_storage		= SDL_LoadBMP("guidep/images/icon-storage.bmp");
	if (icon_storage == NULL) {
		write_log ("SDLUI: Failed to load icon storage\n");
		abort();
	}
	icon_run			= SDL_LoadBMP("guidep/images/icon-run.bmp");
	if (icon_run == NULL) {
		write_log ("SDLUI: Failed to load icon run\n");
		abort();
	}
	icon_exit			= SDL_LoadBMP("guidep/images/icon-exit.bmp");
	if (icon_exit == NULL) {
		write_log ("SDLUI: Failed to load icon exit\n");
		abort();
	}
//	icon_tweaks			= SDL_LoadBMP("guidep/images/icon-tweaks.bmp");

	return 1;
}

void gui_exit (void){
#if 0
	SDL_FreeSurface(tmpSDLScreen);

	SDL_FreeSurface(pMenu_Surface);
	SDL_FreeSurface(pMouse_Pointer);

	SDL_FreeSurface(icon_expansion);
	SDL_FreeSurface(icon_preferences);
	SDL_FreeSurface(icon_keymaps);
	SDL_FreeSurface(icon_floppy);
	SDL_FreeSurface(icon_reset);
	SDL_FreeSurface(icon_storage);
	SDL_FreeSurface(icon_run);
	SDL_FreeSurface(icon_exit);
//	SDL_FreeSurface(icon_tweaks);
#endif
	SDL_Quit;
}

void gui_display (int shortcut){
	if (tmpSDLScreen == NULL) {
		tmpSDLScreen = SDL_CreateRGBSurface(display->flags, display->w, display->h, display->format->BitsPerPixel,
						display->format->Rmask, display->format->Gmask, display->format->Bmask, display->format->Amask);
		if (tmpSDLScreen == NULL) {
			write_log ("SDLUI: Failed to create temp screen\n");
			abort();
		} else {
			write_log ("SDLUI: Created temp screen %dx%dx%d\n", display->w, display->h, display->format->BitsPerPixel);
		}
	}
	SDL_Event event;

	int menu_exitcode = -1;
	int mainloopdone = 0;
	int mouse_x = 30;
	int mouse_y = 40;
	int kup = 0;
	int kdown = 0;
	int kleft = 0;
	int kright = 0;
	int ksel = 0;
	int iconpos_x = 0;
	int iconpos_y = 0;

	if (getcwd (launchDir, 256)) {
		strcpy (yol, launchDir);
		write_log ("SDLUI: current dir: %s\n", launchDir);
	} else {
		write_log("getcwd failed with errno %d\n", errno);
		return;
	}

	while (!mainloopdone) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				mainloopdone = 1;
			}
			if (event.type == SDL_JOYBUTTONDOWN) {
				switch (event.jbutton.button) {
					case PLATFORM_BUTTON_R: break;
					case PLATFORM_BUTTON_L: break;
					case PLATFORM_BUTTON_UP: kup = 1; break;
					case PLATFORM_BUTTON_DOWN: kdown = 1; break;
					case PLATFORM_BUTTON_LEFT: kleft = 1; break;
					case PLATFORM_BUTTON_RIGHT: kright = 1; break;
					case PLATFORM_BUTTON_CLICK: ksel = 1; break;
					case PLATFORM_BUTTON_B: ksel = 1; break;
					case PLATFORM_BUTTON_Y: break;
					case PLATFORM_BUTTON_START: mainloopdone = 1; break;
				}
			}
			if (event.type == SDL_KEYDOWN) {
    			switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:	mainloopdone = 1; break;
				 	case SDLK_UP:		kup = 1; break;
					case SDLK_DOWN:		kdown = 1; break;
					case SDLK_LEFT:		kleft = 1; break;
					case SDLK_RIGHT:	kright = 1; break;
					case SDLK_b:		ksel = 1; break;
					default: break;
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				mouse_x += event.motion.xrel;
				mouse_y += event.motion.yrel;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (selected_item == 0) {
					if (mouse_x >= 0 && mouse_x <= 20) {
						if (mouse_y >= 0 && mouse_y <= 20) {
							mainloopdone = 1;
						}
					}
				} else {
					ksel = 1; break;
				}
			}
		}
		if (ksel == 1) {
			if (selected_item == menu_sel_expansion) {
				sprintf (msg, "%s", "Select KickStart ROM");
				sprintf (msg_status, "%s", "EXIT: Back/ESC");
				sprintf (yol, "%s/roms", launchDir);
				dirz(1);
			}
			if (selected_item == menu_sel_floppy) {
				sprintf (msg, "%s", "Select Disk Image");
				sprintf (msg_status, "%s", "DF0: B  DF1: A");
				sprintf (yol, "%s/disks", launchDir);
				dirz(0);
			}
			if (selected_item == menu_sel_prefs) {
				sprintf (msg, "%s", "Emulation Configuration");
				sprintf (msg_status, "%s", "EXIT: Back/ESC");
				prefz(0);
			}
			if (selected_item == menu_sel_reset) {
				//reset amiga
				menu_exitcode = 2;
				mainloopdone = 1;
			}
			if (selected_item == menu_sel_keymaps) {
			}
/*			if (selected_item == menu_sel_tweaks) {
				sprintf(msg,"%s","Tweaks");
				sprintf(msg_status,"%s","L/R = -/+  B: Apply");
				tweakz(0);
			}*/
			if (selected_item == menu_sel_storage) {

			}
			if (selected_item == menu_sel_run) {
				menu_exitcode = 1;
				mainloopdone = 1;
			}
			if (selected_item == menu_sel_exit) {
				SDL_Quit();
				exit(0);
			}
			ksel = 0;
		}
	// background
		SDL_BlitSurface (pMenu_Surface, NULL, tmpSDLScreen, NULL);

	// icons
        	iconpos_x = 10;
	        iconpos_y = 33;

        	selected_hilite (iconpos_x, iconpos_y, mouse_x, mouse_y, icon_floppy, menu_sel_floppy);
	        blit_image (icon_floppy, iconpos_x, iconpos_y);

	        iconpos_x += iconsizex + bosluk;
        	selected_hilite (iconpos_x, iconpos_y, mouse_x, mouse_y, icon_preferences, menu_sel_prefs);
	        blit_image (icon_preferences, iconpos_x, iconpos_y);

//	        iconpos_x += iconsizex + bosluk;
//        	selected_hilite (iconpos_x, iconpos_y, mouse_x, mouse_y, icon_tweaks, menu_sel_tweaks);
//	        blit_image (icon_tweaks, iconpos_x, iconpos_y);

        	iconpos_x += iconsizex + bosluk;
	        selected_hilite (iconpos_x, iconpos_y, mouse_x, mouse_y, icon_keymaps, menu_sel_keymaps);
        	blit_image (icon_keymaps, iconpos_x, iconpos_y);

	        iconpos_x += iconsizex + bosluk;
	        selected_hilite (iconpos_x, iconpos_y, mouse_x, mouse_y, icon_expansion, menu_sel_expansion);
        	blit_image (icon_expansion, iconpos_x, iconpos_y);

        	iconpos_x = 10;
	        iconpos_y = iconpos_y + iconsizey + bosluk;

        	selected_hilite (iconpos_x,iconpos_y,mouse_x,mouse_y,icon_storage, menu_sel_storage);
	        blit_image (icon_storage, iconpos_x, iconpos_y);

	        iconpos_x += iconsizex + bosluk;
	        selected_hilite (iconpos_x,iconpos_y,mouse_x,mouse_y, icon_reset, menu_sel_reset);
        	blit_image (icon_reset, iconpos_x, iconpos_y);

	        iconpos_x += iconsizex + bosluk;
        	selected_hilite (iconpos_x,iconpos_y,mouse_x,mouse_y, icon_run, menu_sel_run);
	        blit_image (icon_run, iconpos_x, iconpos_y);

        	iconpos_x += iconsizex + bosluk;
	        selected_hilite (iconpos_x,iconpos_y,mouse_x,mouse_y, icon_exit, menu_sel_exit);
        	blit_image (icon_exit, iconpos_x, iconpos_y);
	// texts
		write_text (TITLE_X, TITLE_Y, "PUAE //GnoStiC");

	// mouse pointer ------------------------------
		if (kleft == 1) {
	                mouse_x -= (iconsizex + bosluk);
        	        kleft = 0;
		}
		if (kright == 1) {
	                mouse_x += (iconsizex + bosluk);
	                kright = 0;
		}
	        if (kup == 1) {
	                mouse_y -= (iconsizey + bosluk);
	                kup = 0;
		}
		if (kdown == 1) {
        	        kdown = 0;
	                mouse_y += (iconsizey + bosluk);
		}

#define _MENU_X 640
#define _MENU_Y 480

		if (mouse_x < 1) { mouse_x = 1; }
		if (mouse_y < 1) { mouse_y = 1; }
		if (mouse_x > _MENU_X) { mouse_x = _MENU_X; }
		if (mouse_y > _MENU_Y) { mouse_y = _MENU_Y; }
		rect.x = mouse_x;
		rect.y = mouse_y;
		//rect.w = pMouse_Pointer->w;
		//rect.h = pMouse_Pointer->h;
		SDL_BlitSurface (pMouse_Pointer, NULL, tmpSDLScreen, &rect);
		// mouse pointer-end

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

//	return menu_exitcode;
}

void write_text (int x, int y, char* txt) {
	SDL_Surface* pText_Surface = TTF_RenderText_Solid(amiga_font, txt, text_color);

	rect.x = x;
	rect.y = y;
	rect.w = pText_Surface->w;
	rect.h = pText_Surface->h;

	SDL_BlitSurface (pText_Surface,NULL,tmpSDLScreen,&rect);
	SDL_FreeSurface(pText_Surface);
}

void blit_image (SDL_Surface* img, int x, int y) {
	SDL_Rect dest;
   	dest.x = x;
   	dest.y = y;
   	SDL_BlitSurface(img, 0, tmpSDLScreen, &dest);
}

void selected_hilite (int ix, int iy, int mx, int my, SDL_Surface* img, int hangi) {
        int secili = 0;
        if (mx >= ix && mx <= ix + iconsizex) {
                if (my >= iy && my <= iy + iconsizey) {
                    secili = 1;
                }
        }
        if (secili == 1) {
            SDL_SetAlpha(img, SDL_SRCALPHA, 100);
            selected_item = hangi;
        } else {
            SDL_SetAlpha(img, SDL_SRCALPHA, 255);
	}
}
//
void gui_fps (int fps, int idle, int color)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

void gui_flicker_led (int led, int unitnum, int status)
{
}

void gui_led (int led, int on)
{
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
}

int gui_update (void)
{
	return 0;
}

void gui_message (const char *format,...)
{
       char msg[2048];
       va_list parms;

       va_start (parms,format);
       vsprintf ( msg, format, parms);
       va_end (parms);

       write_log (msg);
}

void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected) {}
void gui_lock (void) {}
void gui_unlock (void) {}

static int guijoybutton[MAX_JPORTS];
static int guijoyaxis[MAX_JPORTS][4];
static bool guijoychange;

void gui_gameport_button_change (int port, int button, int onoff)
{
        //write_log ("%d %d %d\n", port, button, onoff);
#ifdef RETROPLATFORM
        int mask = 0;
        if (button == JOYBUTTON_CD32_PLAY)
                mask = RP_JOYSTICK_BUTTON5;
        if (button == JOYBUTTON_CD32_RWD)
                mask = RP_JOYSTICK_BUTTON6;
        if (button == JOYBUTTON_CD32_FFW)
                mask = RP_JOYSTICK_BUTTON7;
        if (button == JOYBUTTON_CD32_GREEN)
                mask = RP_JOYSTICK_BUTTON4;
        if (button == JOYBUTTON_3 || button == JOYBUTTON_CD32_YELLOW)
                mask = RP_JOYSTICK_BUTTON3;
        if (button == JOYBUTTON_1 || button == JOYBUTTON_CD32_RED)
                mask = RP_JOYSTICK_BUTTON1;
        if (button == JOYBUTTON_2 || button == JOYBUTTON_CD32_BLUE)
                mask = RP_JOYSTICK_BUTTON2;
        rp_update_gameport (port, mask, onoff);
#endif
        if (onoff)
                guijoybutton[port] |= 1 << button;
        else
                guijoybutton[port] &= ~(1 << button);
        guijoychange = true;
}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
        int onoff = state ? 100 : 0;
        if (axis < 0 || axis > 3)
                return;
        if (max < 0) {
                if (guijoyaxis[port][axis] == 0)
                        return;
                if (guijoyaxis[port][axis] > 0)
                        guijoyaxis[port][axis]--;
        } else {
                if (state > max)
                        state = max;
                if (state < 0)
                        state = 0;
                guijoyaxis[port][axis] = max ? state * 127 / max : onoff;
#ifdef RETROPLATFORM
                if (axis == DIR_LEFT_BIT)
                        rp_update_gameport (port, RP_JOYSTICK_LEFT, onoff);
                if (axis == DIR_RIGHT_BIT)
                        rp_update_gameport (port, DIR_RIGHT_BIT, onoff);
                if (axis == DIR_UP_BIT)
                        rp_update_gameport (port, DIR_UP_BIT, onoff);
                if (axis == DIR_DOWN_BIT)
                        rp_update_gameport (port, DIR_DOWN_BIT, onoff);
#endif
        }
        guijoychange = true;
}

void cocoa_gui_early_setup (void) {
//it's easier to put this here than adding a ifdef SDL_UI in main.m
}
