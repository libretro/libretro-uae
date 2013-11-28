#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "button_mappings.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int dirz (int parameter);

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

#define MAX_FILES 1024
#define TITLE_X 52
#define TITLE_Y 9
#define STATUS_X 30 
#define STATUS_Y 460

extern char launchDir[];
extern char yol[];
extern char msg[];
extern char msg_status[];

int dirz (int parameter) {
	SDL_Event event;
	int getdir = 1;
	int loadloopdone = 0;
	int num_of_files = 0;
	int selected_item = 0;
	int q;
	int bas = 0;
	int ka = 0;
	int kb = 0;
	char **filez = (char **)malloc(MAX_FILES*sizeof(char *));
	int i;
	int paging = 18;

	pMenu_Surface = SDL_LoadBMP("guidep/images/menu_load.bmp");
	if (pMenu_Surface == NULL) {
		write_log ("SDLUI: Failed to load menu image\n");
		abort();
	}
	DIR *d;
	d = opendir(yol);
	struct dirent *ep;

	if (d == NULL) {
		write_log ("SDL_UI: opendir %s failed, trying current path\n", yol);
		strcpy(yol, "./");
		d = opendir(yol);
	}
	if (d == NULL) {
		write_log ("SDL_UI: opendir %s failed\n", yol);
	} else {
		for(i=0; i<MAX_FILES; i++) {
			ep = readdir(d);
			if (ep == NULL) {
				write_log ("SDL_UI: readdir %s failed\n", yol);
				break;
			} else {
				//if ((!strcmp(ep->d_name,".")) || (!strcmp(ep->d_name,"..")) || (!strcmp(ep->d_name,"uae"))) {

					struct stat sstat;
					char *tmp=(char *)calloc(1,256);
					strcpy(tmp,launchDir);
					strcat(tmp,"/");
					strcat(tmp,ep->d_name);

					//if (!stat(tmp, &sstat)) {
				        //	if (S_ISDIR(sstat.st_mode)) {
					//		//folder EKLENECEK
					//	} else {
							filez[i]=(char*)malloc(64);
							strncpy(filez[i],ep->d_name,64);
							num_of_files++;
					//	}
					//}
					xfree(tmp);
				//}
			}
		}
		closedir(d);
	}
	if (num_of_files<18) {
		paging = num_of_files;
	}

	while (!loadloopdone) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				loadloopdone = 1;
			}
			if (event.type == SDL_JOYBUTTONDOWN) {
				switch (event.jbutton.button) {
					case PLATFORM_BUTTON_UP: selected_item -= 1; break;
					case PLATFORM_BUTTON_DOWN: selected_item += 1; break;
					case PLATFORM_BUTTON_A: ka = 1; break;
					case PLATFORM_BUTTON_B: kb = 1; break;
					case PLATFORM_BUTTON_SELECT: loadloopdone = 1; break;
				}
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:	loadloopdone = 1; break;
				 	case SDLK_UP:		selected_item -= 1; break;
					case SDLK_DOWN:		selected_item += 1; break;
					case SDLK_a:		ka = 1; break;
					case SDLK_b:		kb = 1; break;
					default: break;
				}
			}
		}

		if (ka == 1) {	//df1
			if (parameter == 0) {
				char *tmp=(char *)calloc(1,256);
				strcpy(tmp,launchDir);
				strcat(tmp,"/roms/");
				strcat(tmp,filez[selected_item]);
				strcpy(currprefs.floppyslots[1].df,tmp);
				xfree(tmp);

				loadloopdone = 1;
			}
			ka = 0;
		}
		if (kb == 1) {  //df0;
			if (parameter == 0) {
				char *tmp=(char *)calloc(1,256);
				strcpy(tmp,launchDir);
				strcat(tmp,"/disks/");
				strcat(tmp,filez[selected_item]);
				strcpy(currprefs.floppyslots[0].df,tmp);
				xfree(tmp);

				loadloopdone = 1;
			} else {
				char *tmp=(char *)calloc(1,256);
				strcpy(tmp,launchDir);
				strcat(tmp,"/roms/");
				strcat(tmp,filez[selected_item]);
				strcpy(currprefs.romfile,tmp);
				xfree(tmp);

				loadloopdone = 1; 
			}
			kb = 0;		
		}
		if (selected_item < 0) { selected_item = 0; }
		if (selected_item >= num_of_files) { selected_item = num_of_files-1; }
		if (selected_item > (bas + paging -1)) { bas += 1; }
		if (selected_item < bas) { bas -= 1; }
		if ((bas+paging) > num_of_files) { bas = (num_of_files - paging); }

	// background
		SDL_BlitSurface (pMenu_Surface,NULL,tmpSDLScreen,NULL);

	// texts
		int sira = 0;
		for (q=bas; q < (bas + paging); q++) {
			if (selected_item == q) {
				text_color.r = 255;
				text_color.g = 100;
				text_color.b = 100;
			}
			write_text (20, 50 + (sira * 20),filez[q]); //
			if (selected_item == q) {
				text_color.r = 0;
				text_color.g = 0;
				text_color.b = 0;
			}
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

	xfree(filez);
    pMenu_Surface = SDL_LoadBMP("guidep/images/menu.bmp");

	return 0;
}
