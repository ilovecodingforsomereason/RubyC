# 1 "level/tile.c"
#include "tile.h"
#include "level.h"

Tile tile_grass = { .tex = 0 };
Tile tile_rock = { .tex = 1 };
Tile tile_dirt = { .tex = 2 };

void tile_render(Tile* tile, Tesselator* t, Level* level, int layer, int x, int y, int z) {
    float u0 = (float)tile->tex / 16.0f;
    float u1 = u0 + 0.0625f;
    float v0 = 0.0f;
    float v1 = 0.0625f;

    float c1 = 1.0f;
    float c2 = 0.8f;
    float c3 = 0.6f;

    float x0 = (float)x; float x1 = (float)x + 1.0f;
    float y0 = (float)y; float y1 = (float)y + 1.0f;
    float z0 = (float)z; float z1 = (float)z + 1.0f;


    if (!level_is_solid_tile(level, x, y - 1, z)) {
        float br = level_get_brightness(level, x, y - 1, z) * c1;
        if (((br == c1) ^ (layer == 1)) != 0) {
            tesselator_color(t, br, br, br);
            tesselator_tex(t, u0, v0); tesselator_vertex(t, x0, y0, z1);
            tesselator_tex(t, u0, v1); tesselator_vertex(t, x0, y0, z0);
            tesselator_tex(t, u1, v1); tesselator_vertex(t, x1, y0, z0);
            tesselator_tex(t, u1, v0); tesselator_vertex(t, x1, y0, z1);
        }
    }


    if (!level_is_solid_tile(level, x, y + 1, z)) {
        float br = level_get_brightness(level, x, y, z) * c1;
        if (((br == c1) ^ (layer == 1)) != 0) {
            tesselator_color(t, br, br, br);
            tesselator_tex(t, u1, v1); tesselator_vertex(t, x1, y1, z1);
            tesselator_tex(t, u1, v0); tesselator_vertex(t, x1, y1, z0);
            tesselator_tex(t, u0, v0); tesselator_vertex(t, x0, y1, z0);
            tesselator_tex(t, u0, v1); tesselator_vertex(t, x0, y1, z1);
        }
    }


    if (!level_is_solid_tile(level, x, y, z - 1)) {
        float br = level_get_brightness(level, x, y, z - 1) * c2;
        if (((br == c2) ^ (layer == 1)) != 0) {
            tesselator_color(t, br, br, br);
            tesselator_tex(t, u1, v0); tesselator_vertex(t, x0, y1, z0);
            tesselator_tex(t, u0, v0); tesselator_vertex(t, x1, y1, z0);
            tesselator_tex(t, u0, v1); tesselator_vertex(t, x1, y0, z0);
            tesselator_tex(t, u1, v1); tesselator_vertex(t, x0, y0, z0);
        }
    }


    if (!level_is_solid_tile(level, x, y, z + 1)) {
        float br = level_get_brightness(level, x, y, z + 1) * c2;
        if (((br == c2) ^ (layer == 1)) != 0) {
            tesselator_color(t, br, br, br);
            tesselator_tex(t, u0, v0); tesselator_vertex(t, x0, y1, z1);
            tesselator_tex(t, u0, v1); tesselator_vertex(t, x0, y0, z1);
            tesselator_tex(t, u1, v1); tesselator_vertex(t, x1, y0, z1);
            tesselator_tex(t, u1, v0); tesselator_vertex(t, x1, y1, z1);
        }
    }


    if (!level_is_solid_tile(level, x - 1, y, z)) {
        float br = level_get_brightness(level, x - 1, y, z) * c3;
        if (((br == c3) ^ (layer == 1)) != 0) {
            tesselator_color(t, br, br, br);
            tesselator_tex(t, u1, v0); tesselator_vertex(t, x0, y1, z1);
            tesselator_tex(t, u0, v0); tesselator_vertex(t, x0, y1, z0);
            tesselator_tex(t, u0, v1); tesselator_vertex(t, x0, y0, z0);
            tesselator_tex(t, u1, v1); tesselator_vertex(t, x0, y0, z1);
        }
    }


    if (!level_is_solid_tile(level, x + 1, y, z)) {
        float br = level_get_brightness(level, x + 1, y, z) * c3;
        if (((br == c3) ^ (layer == 1)) != 0) {
            tesselator_color(t, br, br, br);
            tesselator_tex(t, u0, v0); tesselator_vertex(t, x1, y0, z1);
            tesselator_tex(t, u1, v0); tesselator_vertex(t, x1, y0, z0);
            tesselator_tex(t, u1, v1); tesselator_vertex(t, x1, y1, z0);
            tesselator_tex(t, u0, v1); tesselator_vertex(t, x1, y1, z1);
        }
    }
}

void tile_render_face(Tile* tile, Tesselator* t, int x, int y, int z, int face) {
    (void)tile;
    float x0 = (float)x; float x1 = (float)x + 1.0f;
    float y0 = (float)y; float y1 = (float)y + 1.0f;
    float z0 = (float)z; float z1 = (float)z + 1.0f;

    if (face == 0) {
        tesselator_vertex(t, x0, y0, z1); tesselator_vertex(t, x0, y0, z0);
        tesselator_vertex(t, x1, y0, z0); tesselator_vertex(t, x1, y0, z1);
    }
    if (face == 1) {
        tesselator_vertex(t, x1, y1, z1); tesselator_vertex(t, x1, y1, z0);
        tesselator_vertex(t, x0, y1, z0); tesselator_vertex(t, x0, y1, z1);
    }
    if (face == 2) {
        tesselator_vertex(t, x0, y1, z0); tesselator_vertex(t, x1, y1, z0);
        tesselator_vertex(t, x1, y0, z0); tesselator_vertex(t, x0, y0, z0);
    }
    if (face == 3) {
        tesselator_vertex(t, x0, y1, z1); tesselator_vertex(t, x0, y0, z1);
        tesselator_vertex(t, x1, y0, z1); tesselator_vertex(t, x1, y1, z1);
    }
    if (face == 4) {
        tesselator_vertex(t, x0, y1, z1); tesselator_vertex(t, x0, y1, z0);
        tesselator_vertex(t, x0, y0, z0); tesselator_vertex(t, x0, y0, z1);
    }
    if (face == 5) {
        tesselator_vertex(t, x1, y0, z1); tesselator_vertex(t, x1, y0, z0);
        tesselator_vertex(t, x1, y1, z0); tesselator_vertex(t, x1, y1, z1);
    }
}
