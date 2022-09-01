#ifndef LEVEL_H
#define LEVEL_H

#define MAP_MAX 400*400
#define INFO_MAX 40
#define INFOTEXT_MAX 400
#define RULE_MAX 20
#define EXIT_MAX 8

enum {
    RULE_CANGUN,
    RULE_CANKNIFE,
    RULE_CANSHOOT,
    RULE_CANSTAB,
    RULE_GUARD,
    RULE_ACCESS,
};

struct info {
    int x, y;
    char text[INFOTEXT_MAX];
};

struct rule {
    char rule;
    int op1, op2;
};

struct exit {
    int x, y;
};

extern char map[];
extern char zoneMap[];
extern int mapW, mapH;
extern struct info info_arr[];
extern int info_num;
extern struct rule rule_arr[];
extern int rule_num;
extern struct exit exit_arr[];
extern int exit_num;
extern int player_x, player_y, player_outfit;
extern char player_d, player_state;

void loadLevel(const char *filename);

#endif
