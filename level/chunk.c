# 1 "level/chunk.c"
#include "chunk.h"
#include "level.h"
#include "tile.h"
#include "../textures.h"

int chunk_rebuilt_this_frame = 0;
int chunk_updates = 0;
static GLuint g_terrain_texture = 0;
static Tesselator g_chunk_tesselator;

Chunk chunk_create(Level* level, int x0, int y0, int z0, int x1, int y1, int z1) {
    Chunk c;
    c.level = level;
    c.x0 = x0; c.y0 = y0; c.z0 = z0;
    c.x1 = x1; c.y1 = y1; c.z1 = z1;
    c.dirty = true;
    c.aabb = aabb_create((float)x0, (float)y0, (float)z0, (float)x1, (float)y1, (float)z1);
    c.lists = glGenLists(2);

    if (g_terrain_texture == 0) {
        g_terrain_texture = textures_load_texture("terrain.png", GL_NEAREST);
    }
    return c;
}

static void chunk_rebuild(Chunk* chunk, int layer) {
    if (chunk_rebuilt_this_frame == 2) return;
    chunk->dirty = false;
    chunk_updates++;
    chunk_rebuilt_this_frame++;

    glNewList(chunk->lists + layer, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_terrain_texture);

    tesselator_init(&g_chunk_tesselator);

    for (int x = chunk->x0; x < chunk->x1; x++) {
        for (int y = chunk->y0; y < chunk->y1; y++) {
            for (int z = chunk->z0; z < chunk->z1; z++) {
                if (level_is_tile(chunk->level, x, y, z)) {
                    int idx = (y * chunk->level->height + z) * chunk->level->width + x;
                    int block_type = chunk->level->blocks[idx];

                    if (block_type == 1) {
                        tile_render(&tile_grass, &g_chunk_tesselator, chunk->level, layer, x, y, z);
                    } else if (block_type == 2) {
                        tile_render(&tile_rock, &g_chunk_tesselator, chunk->level, layer, x, y, z);
                    } else if (block_type == 3) {
                        tile_render(&tile_dirt, &g_chunk_tesselator, chunk->level, layer, x, y, z);
                    }
                }
            }
        }
    }

    tesselator_flush(&g_chunk_tesselator);
    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void chunk_render(Chunk* chunk, int layer) {
    if (chunk->dirty) {
        chunk_rebuild(chunk, 0);
        chunk_rebuild(chunk, 1);
    }
    glCallList(chunk->lists + layer);
}

void chunk_set_dirty(Chunk* chunk) {
    chunk->dirty = true;
}
