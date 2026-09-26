#include "frustum.h"
#include <GL/gl.h>
#include <math.h>

static Frustum g_frustum_singleton;

static void normalize_plane(float frustum[6][4], int side) {
    float magnitude = (float)sqrt(frustum[side][0] * frustum[side][0] + 
                                 frustum[side][1] * frustum[side][1] + 
                                 frustum[side][2] * frustum[side][2]);
    if (magnitude == 0.0f) magnitude = 1.0f;
    frustum[side][0] /= magnitude;
    frustum[side][1] /= magnitude;
    frustum[side][2] /= magnitude;
    frustum[side][3] /= magnitude;
}

Frustum* frustum_get_frustum(void) {
    float proj[16];
    float modl[16];
    float clip[16];

    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    clip[0]  = modl[0]*proj[0]  + modl[1]*proj[4]  + modl[2]*proj[8]  + modl[3]*proj[12];
    clip[1]  = modl[0]*proj[1]  + modl[1]*proj[5]  + modl[2]*proj[9]  + modl[3]*proj[13];
    clip[2]  = modl[0]*proj[2]  + modl[1]*proj[6]  + modl[2]*proj[10] + modl[3]*proj[14];
    clip[3]  = modl[0]*proj[3]  + modl[1]*proj[7]  + modl[2]*proj[11] + modl[3]*proj[15];

    clip[4]  = modl[4]*proj[0]  + modl[5]*proj[4]  + modl[6]*proj[8]  + modl[7]*proj[12];
    clip[5]  = modl[4]*proj[1]  + modl[5]*proj[5]  + modl[6]*proj[9]  + modl[7]*proj[13];
    clip[6]  = modl[4]*proj[2]  + modl[5]*proj[6]  + modl[6]*proj[10] + modl[7]*proj[14];
    clip[7]  = modl[4]*proj[3]  + modl[5]*proj[7]  + modl[6]*proj[11] + modl[7]*proj[15];

    clip[8]  = modl[8]*proj[0]  + modl[9]*proj[4]  + modl[10]*proj[8]  + modl[11]*proj[12];
    clip[9]  = modl[8]*proj[1]  + modl[9]*proj[5]  + modl[10]*proj[9]  + modl[11]*proj[13];
    clip[10] = modl[8]*proj[2]  + modl[9]*proj[6]  + modl[10]*proj[10] + modl[11]*proj[14];
    clip[11] = modl[8]*proj[3]  + modl[9]*proj[7]  + modl[10]*proj[11] + modl[11]*proj[15];

    clip[12] = modl[12]*proj[0] + modl[13]*proj[4] + modl[14]*proj[8]  + modl[15]*proj[12];
    clip[13] = modl[12]*proj[1] + modl[13]*proj[5] + modl[14]*proj[9]  + modl[15]*proj[13];
    clip[14] = modl[12]*proj[2] + modl[13]*proj[6] + modl[14]*proj[10] + modl[15]*proj[14];
    clip[15] = modl[12]*proj[3] + modl[13]*proj[7] + modl[14]*proj[11] + modl[15]*proj[15];

    float (*f)[4] = g_frustum_singleton.m_Frustum;

    f[0][0] = clip[3] - clip[0]; f[0][1] = clip[7] - clip[4]; f[0][2] = clip[11] - clip[8]; f[0][3] = clip[15] - clip[12];
    normalize_plane(g_frustum_singleton.m_Frustum, 0);

    f[1][0] = clip[3] + clip[0]; f[1][1] = clip[7] + clip[4]; f[1][2] = clip[11] + clip[8]; f[1][3] = clip[15] + clip[12];
    normalize_plane(g_frustum_singleton.m_Frustum, 1);

    f[2][0] = clip[3] + clip[1]; f[2][1] = clip[7] + clip[5]; f[2][2] = clip[11] + clip[9]; f[2][3] = clip[15] + clip[13];
    normalize_plane(g_frustum_singleton.m_Frustum, 2);

    f[3][0] = clip[3] - clip[1]; f[3][1] = clip[7] - clip[5]; f[3][2] = clip[11] - clip[9]; f[3][3] = clip[15] - clip[13];
    normalize_plane(g_frustum_singleton.m_Frustum, 3);

    f[4][0] = clip[3] - clip[2]; f[4][1] = clip[7] - clip[6]; f[4][2] = clip[11] - clip[10]; f[4][3] = clip[15] - clip[14];
    normalize_plane(g_frustum_singleton.m_Frustum, 4);

    f[5][0] = clip[3] + clip[2]; f[5][1] = clip[7] + clip[6]; f[5][2] = clip[11] + clip[10]; f[5][3] = clip[15] + clip[14];
    normalize_plane(g_frustum_singleton.m_Frustum, 5);

    return &g_frustum_singleton;
}

bool frustum_cube_in_frustum(Frustum* f, const AABB* aabb) {
    for (int i = 0; i < 6; i++) {
        if (f->m_Frustum[i][0] * aabb->x0 + f->m_Frustum[i][1] * aabb->y0 + f->m_Frustum[i][2] * aabb->z0 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x1 + f->m_Frustum[i][1] * aabb->y0 + f->m_Frustum[i][2] * aabb->z0 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x0 + f->m_Frustum[i][1] * aabb->y1 + f->m_Frustum[i][2] * aabb->z0 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x1 + f->m_Frustum[i][1] * aabb->y1 + f->m_Frustum[i][2] * aabb->z0 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x0 + f->m_Frustum[i][1] * aabb->y0 + f->m_Frustum[i][2] * aabb->z1 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x1 + f->m_Frustum[i][1] * aabb->y0 + f->m_Frustum[i][2] * aabb->z1 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x0 + f->m_Frustum[i][1] * aabb->y1 + f->m_Frustum[i][2] * aabb->z1 + f->m_Frustum[i][3] > 0.0f) continue;
        if (f->m_Frustum[i][0] * aabb->x1 + f->m_Frustum[i][1] * aabb->y1 + f->m_Frustum[i][2] * aabb->z1 + f->m_Frustum[i][3] > 0.0f) continue;
        return false;
    }
    return true;
}
