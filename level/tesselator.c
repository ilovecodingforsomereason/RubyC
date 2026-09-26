#include "tesselator.h"
#include <GL/gl.h>

void tesselator_clear(Tesselator* t) {
    t->vertices = 0;
}

void tesselator_init(Tesselator* t) {
    tesselator_clear(t);
    t->has_color = false;
    t->has_texture = false;
}

void tesselator_flush(Tesselator* t) {
    if (t->vertices == 0) return;

    glVertexPointer(3, GL_FLOAT, 0, t->vertex_buffer);
    if (t->has_texture) glTexCoordPointer(2, GL_FLOAT, 0, t->tex_coord_buffer);
    if (t->has_color) glColorPointer(3, GL_FLOAT, 0, t->color_buffer);

    glEnableClientState(GL_VERTEX_ARRAY);
    if (t->has_texture) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if (t->has_color) glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_QUADS, 0, t->vertices);

    glDisableClientState(GL_VERTEX_ARRAY);
    if (t->has_texture) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if (t->has_color) glDisableClientState(GL_COLOR_ARRAY);

    tesselator_clear(t);
}

void tesselator_tex(Tesselator* t, float u, float v) {
    t->has_texture = true;
    t->u = u;
    t->v = v;
}

void tesselator_color(Tesselator* t, float r, float g, float b) {
    t->has_color = true;
    t->r = r; t->g = g; t->b = b;
}

void tesselator_vertex(Tesselator* t, float x, float y, float z) {
    int v_idx = t->vertices * 3;
    t->vertex_buffer[v_idx + 0] = x;
    t->vertex_buffer[v_idx + 1] = y;
    t->vertex_buffer[v_idx + 2] = z;

    if (t->has_texture) {
        int t_idx = t->vertices * 2;
        t->tex_coord_buffer[t_idx + 0] = t->u;
        t->tex_coord_buffer[t_idx + 1] = t->v;
    }

    if (t->has_color) {
        t->color_buffer[v_idx + 0] = t->r;
        t->color_buffer[v_idx + 1] = t->g;
        t->color_buffer[v_idx + 2] = t->b;
    }

    t->vertices++;
    if (t->vertices == MAX_VERTICES) {
        tesselator_flush(t);
    }
}
