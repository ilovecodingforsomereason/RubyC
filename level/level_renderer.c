#include "level_renderer.h"
#include "frustum.h"
#include "tile.h"
#include <stdlib.h>
#include <math.h>

static void wrapper_tile_changed(void* inst, int x, int y, int z) {
    level_renderer_set_dirty((LevelRenderer*)inst, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}
static void wrapper_light_changed(void* inst, int x, int z, int y0, int y1) {
    level_renderer_set_dirty((LevelRenderer*)inst, x - 1, y0 - 1, z - 1, x + 1, y1 + 1, z + 1);
}
static void wrapper_all_changed(void* inst) {
    LevelRenderer* lr = (LevelRenderer*)inst;
    level_renderer_set_dirty(lr, 0, 0, 0, lr->level->width, lr->level->depth, lr->level->height);
}

LevelRenderer* level_renderer_create(Level* level) {
    LevelRenderer* lr = (LevelRenderer*)malloc(sizeof(LevelRenderer));
    lr->level = level;
    tesselator_init(&lr->t);

    lr->x_chunks = level->width / 16;
    lr->y_chunks = level->depth / 16;
    lr->z_chunks = level->height / 16;
    lr->chunk_count = lr->x_chunks * lr->y_chunks * lr->z_chunks;
    lr->chunks = (Chunk*)malloc(sizeof(Chunk) * lr->chunk_count);

    for (int x = 0; x < lr->x_chunks; x++) {
        for (int y = 0; y < lr->y_chunks; y++) {
            for (int z = 0; z < lr->z_chunks; z++) {
                int x0 = x * 16; int y0 = y * 16; int z0 = z * 16;
                int x1 = (x + 1) * 16; int y1 = (y + 1) * 16; int z1 = (z + 1) * 16;
                
                if (x1 > level->width) x1 = level->width;
                if (y1 > level->depth) y1 = level->depth;
                if (z1 > level->height) z1 = level->height;

                int idx = (x + y * lr->x_chunks) * lr->z_chunks + z;
                lr->chunks[idx] = chunk_create(level, x0, y0, z0, x1, y1, z1);
            }
        }
    }

    LevelListener listener = {
        .tile_changed = wrapper_tile_changed,
        .light_column_changed = wrapper_light_changed,
        .all_changed = wrapper_all_changed,
        .instance = lr
    };
    level_add_listener(level, listener);
    return lr;
}

void level_renderer_destroy(LevelRenderer* lr) {
    if (!lr) return;
    free(lr->chunks);
    free(lr);
}

void level_renderer_pick(LevelRenderer* lr, Player* player) {
    float r = 3.0f;
    AABB box = aabb_grow(&player->bb, r, r, r);
    int x0 = (int)box.x0; int x1 = (int)(box.x1 + 1.0f);
    int y0 = (int)box.y0; int y1 = (int)(box.y1 + 1.0f);
    int z0 = (int)box.z0; int z1 = (int)(box.z1 + 1.0f);

    glInitNames();
    for (int x = x0; x < x1; x++) {
        glPushName(x);
        for (int y = y0; y < y1; y++) {
            glPushName(y);
            for (int z = z0; z < z1; z++) {
                glPushName(z);
                if (level_is_solid_tile(lr->level, x, y, z)) {
                    glPushName(0);
                    for (int i = 0; i < 6; i++) {
                        glPushName(i);
                        tesselator_init(&lr->t);
                        tile_render_face(&tile_rock, &lr->t, x, y, z, i);
                        tesselator_flush(&lr->t);
                        glPopName();
                    }
                    glPopName();
                }
                glPopName();
            }
            glPopName();
        }
        glPopName();
    }
}

void level_renderer_render(LevelRenderer* lr, Player* player, int layer) {
    (void)player;
    chunk_rebuilt_this_frame = 0;
    Frustum* f = frustum_get_frustum();
    for (int i = 0; i < lr->chunk_count; i++) {
        if (frustum_cube_in_frustum(f, &lr->chunks[i].aabb)) {
            chunk_render(&lr->chunks[i], layer);
        }
    }
}

void level_renderer_render_hit(LevelRenderer* lr, HitResult* h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    static float s_ticks = 0.0f; s_ticks += 0.05f;
    glColor4f(1.0f, 1.0f, 1.0f, (float)sin(s_ticks) * 0.15f + 0.35f);
    
    
    
    glPushMatrix();
    float offset = 0.002f;
    if (h->f == 0) glTranslatef(0.0f, -offset, 0.0f);
    if (h->f == 1) glTranslatef(0.0f, offset, 0.0f);
    if (h->f == 2) glTranslatef(0.0f, 0.0f, -offset);
    if (h->f == 3) glTranslatef(0.0f, 0.0f, offset);
    if (h->f == 4) glTranslatef(-offset, 0.0f, 0.0f);
    if (h->f == 5) glTranslatef(offset, 0.0f, 0.0f);

    tesselator_init(&lr->t);
    tile_render_face(&tile_rock, &lr->t, h->x, h->y, h->z, h->f);
    tesselator_flush(&lr->t);
    
    glPopMatrix();
    glDisable(GL_BLEND);
}

void level_renderer_set_dirty(LevelRenderer* lr, int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= 16; x1 /= 16; y0 /= 16; y1 /= 16; z0 /= 16; z1 /= 16;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 >= lr->x_chunks) x1 = lr->x_chunks - 1;
    if (y1 >= lr->y_chunks) y1 = lr->y_chunks - 1;
    if (z1 >= lr->z_chunks) z1 = lr->z_chunks - 1;

    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                int idx = (x + y * lr->x_chunks) * lr->z_chunks + z;
                chunk_set_dirty(&lr->chunks[idx]);
            }
        }
    }
}
