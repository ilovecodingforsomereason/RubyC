# 1 "player.c"
#include "player.h"
#include "level/level.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void player_set_pos(Player* player, float x, float y, float z) {
    player->x = x; player->y = y; player->z = z;
    float w = 0.3f; float h = 0.9f;
    player->bb.x0 = x - w; player->bb.y0 = y - h; player->bb.z0 = z - w;
    player->bb.x1 = x + w; player->bb.y1 = y + h; player->bb.z1 = z + w;
}

void player_reset_pos(Player* player) {
    float rx = ((float)rand() / (float)RAND_MAX) * level_get_width(player->level);
    float ry = level_get_depth(player->level) + 10.0f;
    float rz = ((float)rand() / (float)RAND_MAX) * level_get_height(player->level);
    player_set_pos(player, rx, ry, rz);
}

void player_init(Player* player, Level* level) {
    player->level = level;
    player->xd = 0.0f; player->yd = 0.0f; player->zd = 0.0f;
    player->xRot = 0.0f; player->yRot = 0.0f;
    player->on_ground = false;
    player_reset_pos(player);
}

void player_turn(Player* player, float xo, float yo) {
    player->yRot = (float)(player->yRot + xo * 0.15);
    player->xRot = (float)(player->xRot - yo * 0.15);
    if (player->xRot < -90.0f) player->xRot = -90.0f;
    if (player->xRot > 90.0f) player->xRot = 90.0f;
}

void player_tick(Player* player, GLFWwindow* window) {
    player->xo = player->x; player->yo = player->y; player->zo = player->z;
    float xa = 0.0f; float ya = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) player_reset_pos(player);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ya--;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ya++;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) xa--;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) xa++;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (player->on_ground) player->yd = 0.12f;
    }

    player_move_relative(player, xa, ya, player->on_ground ? 0.02f : 0.005f);
    player->yd = (float)(player->yd - 0.005);
    player_move(player, player->xd, player->yd, player->zd);

    player->xd *= 0.91f; player->yd *= 0.98f; player->zd *= 0.91f;
    if (player->on_ground) {
        player->xd *= 0.8f;
        player->zd *= 0.8f;
    }
}

void player_move(Player* player, float xa, float ya, float za) {
    float xaOrg = xa; float yaOrg = ya; float zaOrg = za;

    AABB query_box = aabb_expand(&player->bb, xa, ya, za);

    AABB cubes[512];
    int cube_count = level_get_cubes(player->level, query_box, cubes, 512);

    for (int i = 0; i < cube_count; i++) ya = aabb_clip_y_collide(&cubes[i], &player->bb, ya);
    aabb_move(&player->bb, 0.0f, ya, 0.0f);

    for (int i = 0; i < cube_count; i++) xa = aabb_clip_x_collide(&cubes[i], &player->bb, xa);
    aabb_move(&player->bb, xa, 0.0f, 0.0f);

    for (int i = 0; i < cube_count; i++) za = aabb_clip_z_collide(&cubes[i], &player->bb, za);
    aabb_move(&player->bb, 0.0f, 0.0f, za);

    player->on_ground = (yaOrg != ya && yaOrg < 0.0f);

    if (xaOrg != xa) player->xd = 0.0f;
    if (yaOrg != ya) player->yd = 0.0f;
    if (zaOrg != za) player->zd = 0.0f;

    player->x = (player->bb.x0 + player->bb.x1) / 2.0f;
    player->y = player->bb.y0 + 1.62f;
    player->z = (player->bb.z0 + player->bb.z1) / 2.0f;
}

void player_move_relative(Player* player, float xa, float za, float speed) {
    float dist = xa * xa + za * za;
    if (dist < 0.01f) return;
    dist = speed / (float)sqrt(dist);
    xa *= dist; za *= dist;

    float rad = (float)(player->yRot * M_PI / 180.0);
    float sin_r = (float)sin(rad);
    float cos_r = (float)cos(rad);

    player->xd += xa * cos_r - za * sin_r;
    player->zd += za * cos_r + xa * sin_r;
}
