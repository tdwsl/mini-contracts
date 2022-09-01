#include "person.h"
#include "level.h"

struct person person_arr[PERSON_MAX];
int person_num;

char
getDir(int x, int y)
{
    int i;

    for(i = 0; i < 4; i++)
        if(dirs[i*2] == x && dirs[i*2+1] == y)
            return i;

    return 0;
}

void
tryMove(int *x, int *y, char *d, int dx, int dy)
{
    char dd;

    dd = getDir(dx-*x, dy-*y);
    if(*d != dd) {
        *d = dd;
        return;
    }

    if(spaceFree(dx, dy)) {
        *x = dx;
        *y = dy;
    }
}

void
navPerson(struct person *p, int tx, int ty)
{
    int i, j, g, dx, dy;

    if(p->x == tx && p->y == ty)
        return;

    /*if((p->x == tx && p->y >= ty-1 && p->y <= ty+1) ||
            (p->y == ty && p->x >= tx-1 && p->x <= tx+1)) {
        tryMove(&p->x, &p->y, &p->d, tx, ty);
        return;
    }*/

    for(i = 0; i < mapW*mapH; i++)
        pathMap[i] = (!spaceFree(i%mapW, i/mapW)||map[i]%8==6)*-1;

    pathMap[p->y*mapW+p->x] = 0;
    if(map[player_y*mapW+player_x]%8 != 6)
        pathMap[player_y*mapW+player_x] = 0;
    pathMap[ty*mapW+tx] = 1;

    for(g = 0; g < 30 && !pathMap[p->y*mapW+p->x]; g++) {
        for(i = 0; i < mapW*mapH; i++) {
            if(pathMap[i] != 1) continue;

            for(j = 0; j < 4; j++) {
                dx = i%mapW+dirs[j*2];
                dy = i/mapW+dirs[j*2+1];
                if(dx < 0 || dy < 0 || dx >= mapW || dy >= mapH)
                    continue;
                if(pathMap[dy*mapW+dx] == 0)
                    pathMap[dy*mapW+dx] = 3;
            }
        }

        for(i = 0; i < mapW*mapH; i++) {
            if(pathMap[i] == 3) pathMap[i] = 1;
            else if(pathMap[i] == 1) pathMap[i] = 2;
        }
    }

    /*for(i = 0; i < mapW*mapH; i++) {
        printf("%2d ", pathMap[i]);
        if((i+1)%mapW == 0)
            printf("\n");
    }*/

    if(pathMap[p->y*mapW+p->x]) {
        for(i = 0; i < 4; i++) {
            dx = p->x+dirs[i*2];
            dy = p->y+dirs[i*2+1];
            if(dx < 0 || dy < 0 || dx >= mapW || dy >= mapH)
                continue;
            if(pathMap[dy*mapW+dx] > 0) {
                if(!spaceFree(dx, dy)) break;
                tryMove(&p->x, &p->y, &p->d, dx, dy);
                return;
            }
        }
    }

    for(i = 0; i < 4; i++)
        if(spaceFree(p->x+dirs[i*2], p->y+dirs[i*2+1])) {
            tryMove(&p->x, &p->y, &p->d,
                    p->x+dirs[i*2], p->y+dirs[i*2+1]);
            return;
        }
}
