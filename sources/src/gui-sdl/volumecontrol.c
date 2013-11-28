#include <SDL.h>
#include "volumecontrol.h"

extern SDL_Surface *prSDLScreen;

static SDL_Surface *ksur;

//int show_volumecontrol = 0;

int volumecontrol_init(void) {
	return 0;
}
	
// OSD volume
void volumecontrol_redraw(void) {
/*
	SDL_Rect r;
	SDL_Surface* surface;
	int i;

	Uint32 green = SDL_MapRGB(prSDLScreen->format, 250,250,0);

	r.x = 110;
	r.y = prSDLScreen->h-30;
	r.w = soundVolume;
	r.h = 15;

	// draw the blocks now
	SDL_FillRect(prSDLScreen, &r, green);

	SDL_BlitSurface(surface,NULL,prSDLScreen,&r);
*/
}
