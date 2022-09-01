#include "sdlutil.h"
#include <SDL2/SDL.h>
#include <assert.h>

SDL_Window *window;
SDL_Surface *display;

SDL_Surface *s_head;
SDL_Surface *s_outfit;
SDL_Surface *s_player;
SDL_Surface *s_tileset;
SDL_Surface *s_zone;
SDL_Surface *s_font;
SDL_Surface *s_ui;

void
initSDL()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) >= 0);
    window = SDL_CreateWindow("Mini Contracts",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480, SDL_WINDOW_RESIZABLE);
    assert(window);
    display = SDL_GetWindowSurface(window);

    s_head = loadSurface("img/head.bmp");
    s_outfit = loadSurface("img/outfit.bmp");
    s_player = loadSurface("img/player.bmp");
    s_tileset = loadSurface("img/tileset.bmp");
    s_zone = loadSurface("img/zone.bmp");
    s_font = loadSurface("img/font.bmp");
    s_ui = loadSurface("img/ui.bmp");
}

void
endSDL()
{
    SDL_FreeSurface(s_ui);
    SDL_FreeSurface(s_font);
    SDL_FreeSurface(s_zone);
    SDL_FreeSurface(s_tileset);
    SDL_FreeSurface(s_player);
    SDL_FreeSurface(s_outfit);
    SDL_FreeSurface(s_head);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Surface*
loadSurface(const char *filename)
{
    SDL_Surface *orig;
    SDL_Surface *newSurf;

    orig = SDL_LoadBMP(filename);
    assert(orig);

    newSurf = SDL_ConvertSurface(orig, display->format, 0);
    assert(newSurf);
    SDL_FreeSurface(orig);
    SDL_SetColorKey(newSurf, SDL_TRUE, SDL_MapRGB(newSurf->format,
            0xff, 0x00, 0xff));
    return newSurf;
}
