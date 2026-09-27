# 1 "level/chunk.h"
#ifndef CHUNK_H
#define CHUNK_H 

#include "../phys/aabb.h"
#include <GL/gl.h>

typedef struct Level Level;

typedef struct {
    AABB aabb;
    Level* level;
    int x0, y0, z0;
    int x1, y1, z1;
    GLuint lists;
    bool dirty;
} Chunk;

extern int chunk_rebuilt_this_frame;
extern int chunk_updates;

Chunk chunk_create(Level* level, int x0, int y0, int z0, int x1, int y1, int z1);
void chunk_render(Chunk* chunk, int layer);
void chunk_set_dirty(Chunk* chunk);

#endif
