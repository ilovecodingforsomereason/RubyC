# 1 "level/level_renderer.h"
#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H 

#include "level.h"
#include "chunk.h"
#include "tesselator.h"
#include "../player.h"
#include "../hit_result.h"

typedef struct LevelRenderer {
    Level* level;
    Chunk* chunks;
    int x_chunks;
    int y_chunks;
    int z_chunks;
    int chunk_count;
    Tesselator t;
} LevelRenderer;

LevelRenderer* level_renderer_create(Level* level);
void level_renderer_destroy(LevelRenderer* lr);
void level_renderer_pick(LevelRenderer* lr, Player* player);
void level_renderer_render(LevelRenderer* lr, Player* player, int layer);
void level_renderer_render_hit(LevelRenderer* lr, HitResult* h);
void level_renderer_set_dirty(LevelRenderer* lr, int x0, int y0, int z0, int x1, int y1, int z1);

#endif
