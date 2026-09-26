#ifndef TILE_H
#define TILE_H

#include "tesselator.h"

typedef struct Level Level;

typedef struct {
    int tex;
} Tile;

extern Tile tile_rock;
extern Tile tile_grass;

void tile_render(Tile* tile, Tesselator* t, Level* level, int layer, int x, int y, int z);
void tile_render_face(Tile* tile, Tesselator* t, int x, int y, int z, int face);

#endif
