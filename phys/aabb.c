# 1 "phys/aabb.c"
#include "aabb.h"

AABB aabb_create(float x0, float y0, float z0, float x1, float y1, float z1) {
    AABB b;
    b.x0 = x0; b.y0 = y0; b.z0 = z0;
    b.x1 = x1; b.y1 = y1; b.z1 = z1;
    b.epsilon = 0.0f;
    return b;
}

AABB aabb_expand(const AABB* box, float xa, float ya, float za) {
    float _x0 = box->x0; float _y0 = box->y0; float _z0 = box->z0;
    float _x1 = box->x1; float _y1 = box->y1; float _z1 = box->z1;

    if (xa < 0.0f) _x0 += xa;
    if (xa > 0.0f) _x1 += xa;

    if (ya < 0.0f) _y0 += ya;
    if (ya > 0.0f) _y1 += ya;

    if (za < 0.0f) _z0 += za;
    if (za > 0.0f) _z1 += za;

    return aabb_create(_x0, _y0, _z0, _x1, _y1, _z1);
}

AABB aabb_grow(const AABB* box, float xa, float ya, float za) {
    return aabb_create(
        box->x0 - xa, box->y0 - ya, box->z0 - za,
        box->x1 + xa, box->y1 + ya, box->z1 + za
    );
}

float aabb_clip_x_collide(const AABB* box, const AABB* c, float xa) {
    if (c->y1 <= box->y0 || c->y0 >= box->y1) return xa;
    if (c->z1 <= box->z0 || c->z0 >= box->z1) return xa;

    if (xa > 0.0f && c->x1 <= box->x0) {
        float max = box->x0 - c->x1 - box->epsilon;
        if (max < xa) xa = max;
    }
    if (xa < 0.0f && c->x0 >= box->x1) {
        float max = box->x1 - c->x0 + box->epsilon;
        if (max > xa) xa = max;
    }
    return xa;
}

float aabb_clip_y_collide(const AABB* box, const AABB* c, float ya) {
    if (c->x1 <= box->x0 || c->x0 >= box->x1) return ya;
    if (c->z1 <= box->z0 || c->z0 >= box->z1) return ya;

    if (ya > 0.0f && c->y1 <= box->y0) {
        float max = box->y0 - c->y1 - box->epsilon;
        if (max < ya) ya = max;
    }
    if (ya < 0.0f && c->y0 >= box->y1) {
        float max = box->y1 - c->y0 + box->epsilon;
        if (max > ya) ya = max;
    }
    return ya;
}

float aabb_clip_z_collide(const AABB* box, const AABB* c, float za) {
    if (c->x1 <= box->x0 || c->x0 >= box->x1) return za;
    if (c->y1 <= box->y0 || c->y0 >= box->y1) return za;

    if (za > 0.0f && c->z1 <= box->z0) {
        float max = box->z0 - c->z1 - box->epsilon;
        if (max < za) za = max;
    }
    if (za < 0.0f && c->z0 >= box->z1) {
        float max = box->z1 - c->z0 + box->epsilon;
        if (max > za) za = max;
    }
    return za;
}

bool aabb_intersects(const AABB* box, const AABB* c) {
    if (c->x1 <= box->x0 || c->x0 >= box->x1) return false;
    if (c->y1 <= box->y0 || c->y0 >= box->y1) return false;
    if (c->z1 <= box->z0 || c->z0 >= box->z1) return false;
    return true;
}

void aabb_move(AABB* box, float xa, float ya, float za) {
    box->x0 += xa; box->y0 += ya; box->z0 += za;
    box->x1 += xa; box->y1 += ya; box->z1 += za;
}
