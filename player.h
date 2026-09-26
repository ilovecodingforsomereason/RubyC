#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "phys/aabb.h"

typedef struct Level Level;

typedef struct {
    Level* level;
    float xo, yo, zo;
    float x, y, z;
    float xd, yd, zd;
    float yRot, xRot;
    AABB bb;
    bool on_ground;
} Player;

struct GLFWwindow;

void player_init(Player* player, Level* level);
void player_tick(Player* player, struct GLFWwindow* window);
void player_turn(Player* player, float xo, float yo);
void player_move(Player* player, float xa, float ya, float za);
void player_move_relative(Player* player, float xa, float za, float speed);
void player_reset_pos(Player* player);

#endif
