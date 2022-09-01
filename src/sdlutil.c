#include "sdlutil.h"
#include <SDL2/SDL.h>
#include <assert.h>

SDL_Window *window;
SDL_Surface *display;
int g_scale = 2;

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
    SDL_Surface *surf;
    SDL_Surface *scaled;
    SDL_Rect r;

    orig = SDL_LoadBMP(filename);
    assert(orig);

    surf = SDL_ConvertSurface(orig, display->format, 0);
    assert(surf);
    SDL_FreeSurface(orig);

    if(g_scale != 1) {
        r = (SDL_Rect){0, 0, surf->w*g_scale, surf->h*g_scale};
        scaled = SDL_CreateRGBSurface(surf->flags,
                surf->w*g_scale, surf->h*g_scale, 32,
                surf->format->Rmask,surf->format->Gmask,surf->format->Bmask,
                surf->format->Amask);
        assert(scaled);
        SDL_BlitScaled(surf, 0, scaled, &r);
        SDL_FreeSurface(surf);
        surf = scaled;
    }

    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format,
            0xff, 0x00, 0xff));

    return surf;
}

void
blitSurface(SDL_Surface *s1, SDL_Rect *r1, SDL_Surface *s2, SDL_Rect *r2)
{
    SDL_Rect src, dst;

    if(g_scale == 1)
        SDL_BlitSurface(s1, r1, s2, r2);
    else {
        dst = *r2;
        dst.x *= g_scale;
        dst.y *= g_scale;

        if(r1) {
            src = *r1;
            src.w *= g_scale;
            src.h *= g_scale;
            src.x *= g_scale;
            src.y *= g_scale;
            SDL_BlitSurface(s1, &src, s2, &dst);
        } else
            SDL_BlitSurface(s1, 0, s2, &dst);
    }
}

void
getWindowSize(SDL_Window *win, Uint32 *w, Uint32 *h)
{
    SDL_GetWindowSize(win, w, h);
    if(w) *w /= g_scale;
    if(h) *h /= g_scale;
}
