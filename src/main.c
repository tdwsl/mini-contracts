#include "sdlutil.h"
#include "level.h"
#include "person.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define UPDATE_DELAY 25

int g_bgX = 0, g_bgY = 0;

void
draw()
{
    int w, h, x, y, xo, yo, i;
    SDL_Rect src, dst;

    SDL_GetWindowSize(window, &w, &h);
    display = SDL_GetWindowSurface(window);

    src.w = 48;
    src.h = 32;
    src.x = 0;
    src.y = 16;

    for(y = -32; y <= h+32; y += 32)
        for(x = -48; x <= w+48; x += 48) {
            dst.x = x+g_bgX/100;
            dst.y = y+g_bgY/100;
            SDL_BlitSurface(s_ui, &src, display, &dst);
        }

    xo = -player_x*16 + w/2;
    yo = -player_y*8 + h/2;

    src.w = 16;
    src.h = 16;

    for(y = 0; y < mapH; y++) {
        for(x = 0; x < mapW; x++) {
            src.x = (map[y*mapW+x]%8)*16;
            src.y = (map[y*mapW+x]/8)*16;
            dst.x = x*16+xo;
            dst.y = y*8+yo;
            SDL_BlitSurface(s_tileset, &src, display, &dst);
        }

        if(player_y == y) {
            dst.x = player_x*16+xo;
            dst.y = player_y*8+yo;
            src = (SDL_Rect){player_d*16, player_outfit*16, 16, 16};
            SDL_BlitSurface(s_outfit, &src, display, &dst);
            src = (SDL_Rect){player_d*16, 32, 16, 16};
            SDL_BlitSurface(s_player, &src, display, &dst);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void
update(int diff)
{
    g_bgX = (g_bgX+diff*2)%4800;
    g_bgY = (g_bgY+diff)%3200;
}

int
main()
{
    bool quit;
    SDL_Event ev;
    int lastUpdate, currentTime;

    loadLevel("lvl/prison.txt");
    initSDL();

    quit = 0;
    lastUpdate = SDL_GetTicks();

    while(!quit) {
        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            }
        }

        currentTime = SDL_GetTicks();
        if(currentTime < lastUpdate)
            lastUpdate = currentTime;
        if(lastUpdate != currentTime) {
            update(currentTime-lastUpdate);
            lastUpdate = currentTime;
        }

        draw();
    }

    endSDL();
    return 0;
}
