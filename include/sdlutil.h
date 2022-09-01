#ifndef SDLUTIL_H
#define SDLUTIL_H

#include <SDL2/SDL.h>

extern SDL_Window *window;
extern SDL_Surface *display;

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

#endif
