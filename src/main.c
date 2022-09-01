#include "sdlutil.h"
#include "level.h"
#include "person.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define UPDATE_DELAY 150
#define CAMMOVE_RES 20
#define BGMOVE_RES 100

static struct {
    Uint32 up, down, left, right;
} controls = {
    SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
};

int g_bgX = 0, g_bgY = 0;
int g_camX = 0, g_camY = 0, g_dCamX = 0, g_dCamY = 0;
int updateCounter = 0;
bool player_canMove = 0;

void
draw()
{
    int w, h, x, y, dx, dy, xo, yo, i;
    struct person *p;
    SDL_Rect src, dst;

    getWindowSize(window, &w, &h);
    display = SDL_GetWindowSurface(window);

    src.w = 48;
    src.h = 32;
    src.x = 0;
    src.y = 16;

    /* bg */
    for(y = -32; y <= h+32; y += 32)
        for(x = -48; x <= w+48; x += 48) {
            dst.x = x+g_bgX/BGMOVE_RES;
            dst.y = y+g_bgY/BGMOVE_RES;
            blitSurface(s_ui, &src, display, &dst);
        }

    g_dCamX = (-player_x*16)*CAMMOVE_RES;
    g_dCamY = (-player_y*8)*CAMMOVE_RES;
    xo = g_camX/CAMMOVE_RES-8+w/2;
    yo = g_camY/CAMMOVE_RES-8+h/2;

    src.w = 16;
    src.h = 16;

    for(y = 0; y < mapH; y++) {
        for(x = 0; x < mapW; x++) {
            src.x = (map[y*mapW+x]%8)*16;
            src.y = (map[y*mapW+x]/8)*16;
            dst.x = x*16+xo;
            dst.y = y*8+yo;

            if(dst.x < -32 || dst.y < -32 || dst.x > w+16 || dst.y > h+16)
                continue;

            /* tiles */
            blitSurface(s_tileset, &src, display, &dst);

            /* zone borders */
            if(zoneMap[y*mapW+x] && !(map[y*mapW+x]%8)) {
                src.h = 8;
                src.y = zoneMap[y*mapW+x]*8;
                dst.y += 8;

                for(i = 0; i < 4; i++) {
                    dx = x+dirs[i*2];
                    dy = y+dirs[i*2+1];
                    if(!(dx < 0 || dy < 0 || dx >= mapH || dy >= mapH))
                        if(zoneMap[dy*mapW+dx] == zoneMap[y*mapW+x])
                            continue;
                    src.x = i*16;
                    blitSurface(s_zone, &src, display, &dst);
                }
                src.h = 16;
            }
        }

        /* player */
        if(player_y == y) {
            dst.x = player_x*16+xo;
            src = (SDL_Rect){player_d*16, player_outfit*16, 16, 16};
            blitSurface(s_outfit, &src, display, &dst);
            src = (SDL_Rect){player_d*16, 32, 16, 16};
            blitSurface(s_player, &src, display, &dst);
        }

        /* npc */
        for(i = 0; i < person_num; i++) {
            if(person_arr[i].y != y) continue;
            p = &person_arr[i];

            if(p->state == PSTATE_DEAD || p->state == PSTATE_SLEEP)
                dx = 64;
            else
                dx = p->d*16;

            dst.x = p->x*16+xo;
            src = (SDL_Rect){dx, p->outfit*16, 16, 16};
            blitSurface(s_outfit, &src, display, &dst);
            src = (SDL_Rect){dx, p->head*32, 16, 16};
            if(p->state == PSTATE_GUN)
                src.y += 16;
            blitSurface(s_head, &src, display, &dst);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void
updatePerson(struct person *p)
{
    int x, y;
    struct path_step step;

    if(p->state == PSTATE_NORMAL) {
        if(p->path.length) {
            /* follow path */
            step = p->path.steps[p->path.current];
            p->path.current = (p->path.current+1)%p->path.length;

            switch(step.ins) {
            case PATH_GOTO:
                p->tx = step.op1;
                p->ty = step.op2;
                p->state = PSTATE_GOTO;
                break;
            case PATH_WAIT:
                p->counter = step.op1;
                p->state = PSTATE_WAIT;
                break;
            case PATH_SLEEP:
                p->counter = step.op1;
                p->state = PSTATE_SLEEP;
                break;
            case PATH_FACE:
                p->d = step.op1;
                break;
            case PATH_TURN:
                p->d = (p->d+step.op1)%4;
                break;
            case PATH_MARCH:
                p->state = PSTATE_MARCH;
                break;
            }
        }
    } else if(p->state == PSTATE_GOTO) {
        navPerson(p, p->tx, p->ty);
        if(p->x == p->tx && p->y == p->ty)
            p->state = PSTATE_NORMAL;
    } else if(p->state == PSTATE_MARCH) {
        x = p->x;
        y = p->y;
        tryMove(&p->x, &p->y, &p->d,
                p->x+dirs[p->d*2], p->y+dirs[p->d*2+1]);
        if(p->x == x && p->y == y)
            p->state = PSTATE_NORMAL;
    } else if(p->state == PSTATE_WAIT || p->state == PSTATE_SLEEP) {
        p->counter--;
        if(p->counter <= 0)
            p->state = PSTATE_NORMAL;
    }
}

void
update(int diff)
{
    int dx, dy, i;
    char dd;

    g_bgX = (g_bgX+diff*2)%(48*BGMOVE_RES);
    g_bgY = (g_bgY+diff)%(32*BGMOVE_RES);

    for(i = 0; i < diff && (g_camX != g_dCamX || g_camY != g_dCamY); i++) {
        if(g_camX < g_dCamX) g_camX++;
        else if(g_camX > g_dCamX) g_camX--;
        if(g_camY < g_dCamY) g_camY++;
        else if(g_camY > g_dCamY) g_camY--;
    }

    updateCounter += diff;
    if(updateCounter < UPDATE_DELAY)
        return;
    updateCounter -= UPDATE_DELAY;

    player_canMove = 1;

    for(i = 0; i < person_num; i++)
        updatePerson(&person_arr[i]);
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

            case SDL_KEYDOWN:
                if(!player_canMove)
                    continue;

                if(ev.key.keysym.sym == controls.up)
                    tryMove(&player_x, &player_y, &player_d,
                            player_x, player_y-1);
                else if(ev.key.keysym.sym == controls.down)
                    tryMove(&player_x, &player_y, &player_d,
                            player_x, player_y+1);
                else if(ev.key.keysym.sym == controls.left)
                    tryMove(&player_x, &player_y, &player_d,
                            player_x-1, player_y);
                else if(ev.key.keysym.sym == controls.right)
                    tryMove(&player_x, &player_y, &player_d,
                            player_x+1, player_y);
                else
                    break;

                player_canMove = 0;

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
