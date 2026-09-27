# 1 "level/tesselator.h"
#ifndef TESSELATOR_H
#define TESSELATOR_H 

#include <stdbool.h>

#define MAX_VERTICES 100000

typedef struct {
    float vertex_buffer[MAX_VERTICES * 3];
    float tex_coord_buffer[MAX_VERTICES * 2];
    float color_buffer[MAX_VERTICES * 3];
    int vertices;

    float u, v;
    float r, g, b;
    bool has_color;
    bool has_texture;
} Tesselator;

void tesselator_init(Tesselator* t);
void tesselator_flush(Tesselator* t);
void tesselator_clear(Tesselator* t);
void tesselator_tex(Tesselator* t, float u, float v);
void tesselator_color(Tesselator* t, float r, float g, float b);
void tesselator_vertex(Tesselator* t, float x, float y, float z);

#endif
