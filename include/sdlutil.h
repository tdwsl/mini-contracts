#ifndef SDLUTIL_H
#define SDLUTIL_H

#include <SDL2/SDL.h>

extern SDL_Window *window;
extern SDL_Surface *display;
extern int g_scale;

extern SDL_Surface *s_head;
extern SDL_Surface *s_outfit;
extern SDL_Surface *s_player;
extern SDL_Surface *s_tileset;
extern SDL_Surface *s_zone;
extern SDL_Surface *s_font;
extern SDL_Surface *s_ui;

void initSDL();
void endSDL();
SDL_Surface *loadSurface(const char *filename);
void blitSurface(SDL_Surface *s1, SDL_Rect *r1, SDL_Surface *s2, SDL_Rect *r2);
void getWindowSize(SDL_Window *win, Uint32 *w, Uint32 *h);

#endif
