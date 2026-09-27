# 1 "phys/aabb.h"
#ifndef AABB_H
#define AABB_H 

#include <stdbool.h>

typedef struct {
    float x0, y0, z0;
    float x1, y1, z1;
    float epsilon;
} AABB;


AABB aabb_create(float x0, float y0, float z0, float x1, float y1, float z1);


AABB aabb_expand(const AABB* box, float xa, float ya, float za);
AABB aabb_grow(const AABB* box, float xa, float ya, float za);


float aabb_clip_x_collide(const AABB* box, const AABB* c, float xa);
float aabb_clip_y_collide(const AABB* box, const AABB* c, float ya);
float aabb_clip_z_collide(const AABB* box, const AABB* c, float za);


bool aabb_intersects(const AABB* box, const AABB* c);
void aabb_move(AABB* box, float xa, float ya, float za);

#endif
