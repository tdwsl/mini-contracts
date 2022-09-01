#include "level.h"
#include "person.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

char map[MAP_MAX];
char zoneMap[MAP_MAX];
int mapW, mapH;
struct info info_arr[INFO_MAX];
int info_num;
struct exit exit_arr[EXIT_MAX];
int exit_num;
struct rule rule_arr[RULE_MAX];
int rule_num;
int player_x, player_y, player_outfit;
char player_d, player_state;

void
scanPath(FILE *fp, struct path *p)
{
    char buf[100];
    struct path_step *s;

    p->length = 0;
    p->current = 0;

    for(;;) {
        fscanf(fp, "%s", buf);
        if(!buf[0]) continue;

        s = &p->steps[p->length++];

        if(!strcmp(buf, "wait")) {
            s->ins = PATH_WAIT;
            fscanf(fp, "%d", &s->op1);
        } else if(!strcmp(buf, "sleep")) {
            s->ins = PATH_SLEEP;
            fscanf(fp, "%d", &s->op1);
        } else if(!strcmp(buf, "face")) {
            s->ins = PATH_FACE;
            fscanf(fp, "%d", &s->op1);
        } else if(!strcmp(buf, "turn")) {
            s->ins = PATH_TURN;
            fscanf(fp, "%d", &s->op1);
        } else if(!strcmp(buf, "goto")) {
            s->ins = PATH_GOTO;
            fscanf(fp, "%d%d", &s->op1, &s->op2);
        } else if(!strcmp(buf, "march")) {
            s->ins = PATH_MARCH;
        } else if(!strcmp(buf, "end")) {
            break;
        } else {
            printf("path - invalid step %s!\n", buf);
            exit(1);
        }
    }
}

void
scanInfo(FILE *fp, struct info *f)
{
    char buf[100];
    int i;

    fscanf(fp, "%d%d", &f->x, &f->y);

    f->text[0] = 0;

    for(;;) {
        fscanf(fp, "%s", buf);
        if(!strcmp(buf, "end-info")) {
            break;
        } else if(!strcmp(buf, "\\n")) {
            i = strlen(f->text);
            f->text[i] = '\n';
            f->text[i+1] = 0;
        } else {
            i = strlen(f->text);
            f->text[i] = ' ';
            strcpy(f->text+i+1, buf);
        }
    }
}

#define assertPerson()\
    if(person_num == 0) {\
        printf("%s - no person!\n", buf);\
        exit(1);\
    }

void
loadLevel(const char *filename)
{
    FILE *fp;
    char buf[100];
    int i;
    bool comment;

    fp = fopen(filename, "r");
    assert(fp);

    person_num = 0;
    info_num = 0;
    rule_num = 0;
    exit_num = 0;
    player_d = 0;
    player_state = PSTATE_NORMAL;
    player_outfit = 1;

    comment = 0;

    while(!feof(fp)) {
        fscanf(fp, "%s", buf);
        if(!buf[0]) continue;

        if(!strcmp(buf, "*/")) {
            if(comment) comment = 0;
            else {
                printf("*/ - no opening comment!\n");
                exit(1);
            }
            continue;
        } else if(comment) continue;

        if(!strcmp(buf, "map")) {
            fscanf(fp, "%d%d", &mapW, &mapH);
            for(i = 0; i < mapW*mapH; i++)
                fscanf(fp, "%d", &map[i]);
            for(i = 0; i < mapW*mapH; i++)
                fscanf(fp, "%d", &zoneMap[i]);
        } else if(!strcmp(buf, "path")) {
            assertPerson();
            scanPath(fp, &person_arr[person_num-1].path);
        } else if(!strcmp(buf, "person")) {
            fscanf(fp, "%d%d",
                    &person_arr[person_num].x, &person_arr[person_num].y);
            person_arr[person_num].head = 0;
            person_arr[person_num].outfit = 0;
            person_arr[person_num].path.length = 0;
            person_arr[person_num].state = PSTATE_NORMAL;
            person_arr[person_num].d = 0;
            person_num++;
        } else if(!strcmp(buf, "outfit")) {
            assertPerson();
            fscanf(fp, "%d", &person_arr[person_num-1].outfit);
        } else if(!strcmp(buf, "head")) {
            assertPerson();
            fscanf(fp, "%d", &person_arr[person_num-1].head);
        } else if(!strcmp(buf, "facing")) {
            assertPerson();
            fscanf(fp, "%d", &person_arr[person_num-1].d);
        } else if(!strcmp(buf, "hostile")) {
            assertPerson();
            person_arr[person_num-1].flags |= PFLAG_HOSTILE;
        } else if(!strcmp(buf, "target")) {
            assertPerson();
            person_arr[person_num-1].flags |= PFLAG_TARGET;
        } else if(!strcmp(buf, "can-gun")) {
            rule_arr[rule_num].rule = RULE_CANGUN;
            fscanf(fp, "%d", &rule_arr[rule_num].op1);
            rule_num++;
        } else if(!strcmp(buf, "can-knife")) {
            rule_arr[rule_num].rule = RULE_CANKNIFE;
            fscanf(fp, "%d", &rule_arr[rule_num].op1);
            rule_num++;
        } else if(!strcmp(buf, "can-shoot")) {
            rule_arr[rule_num].rule = RULE_CANSHOOT;
            fscanf(fp, "%d%d",
                    &rule_arr[rule_num].op1, &rule_arr[rule_num].op2);
            rule_num++;
        } else if(!strcmp(buf, "can-stab")) {
            rule_arr[rule_num].rule = RULE_CANSTAB;
            fscanf(fp, "%d%d",
                    &rule_arr[rule_num].op1, &rule_arr[rule_num].op2);
            rule_num++;
        } else if(!strcmp(buf, "access")) {
            rule_arr[rule_num].rule = RULE_ACCESS;
            fscanf(fp, "%d%d",
                    &rule_arr[rule_num].op1, &rule_arr[rule_num].op2);
            rule_num++;
        } else if(!strcmp(buf, "guard")) {
            rule_arr[rule_num].rule = RULE_GUARD;
            fscanf(fp, "%d%d",
                    &rule_arr[rule_num].op1, &rule_arr[rule_num].op2);
            rule_num++;
        } else if(!strcmp(buf, "exit")) {
            fscanf(fp, "%d%d",
                    &exit_arr[exit_num].x, &exit_arr[exit_num].y);
            exit_num++;
        } else if(!strcmp(buf, "info")) {
            scanInfo(fp, &info_arr[info_num++]);
        } else if(!strcmp(buf, "spawn")) {
            fscanf(fp, "%d%d", &player_x, &player_y);
        } else if(!strcmp(buf, "/*")) {
            comment = 1;
        } else {
            printf("%s - unknown statement!\n", buf);
            exit(1);
        }
    }
}
