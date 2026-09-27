# 1 "level/frustum.h"
#ifndef FRUSTUM_H
#define FRUSTUM_H 

#include "../phys/aabb.h"

typedef struct {
    float m_Frustum[6][4];
} Frustum;

Frustum* frustum_get_frustum(void);
bool frustum_cube_in_frustum(Frustum* f, const AABB* aabb);

#endif
