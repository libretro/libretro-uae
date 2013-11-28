#include <SDL.h>
#include "inputmode.h"

extern char launchDir [256];
extern SDL_Surface *prSDLScreen;

static SDL_Surface *ksur;
static SDL_Surface *inputMode[3];

int show_inputmode = 0;

int inputmode_init(void) {
	int i;
	char tmpchar[256];
	SDL_Surface* tmp;
	
	
	sprintf(tmpchar, "%s/images/joystick.bmp", launchDir);
	tmp = SDL_LoadBMP(tmpchar);
	if (tmp) {
		inputMode[0] = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
	}

	sprintf(tmpchar, "%s/images/mouse.bmp", launchDir);
	tmp = SDL_LoadBMP(tmpchar);
	if (tmp) {
		inputMode[1] = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
	}

	return 0;
}


void inputmode_redraw(void) {
	SDL_Rect r;
	SDL_Surface* surface = NULL;

	r.x=80;
	r.y=60;
	r.w=160;
	r.h=120;

	if (inputMode[0] && inputMode[1]) {
/*		if (gp2xMouseEmuOn) {
			surface = inputMode[1];
		} else {
			surface = inputMode[0];
		}*/

		SDL_BlitSurface(surface,NULL,prSDLScreen,&r);
	}
}
