# 1 "level/level.c"
#include "level.h"
#include "tile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STB_PERLIN_IMPLEMENTATION 
#include "stb_perlin.h"

Level* level_create(int w, int h, int d) {
    Level* level = (Level*)malloc(sizeof(Level));
    level->width = w;
    level->height = h;
    level->depth = d;

    level->blocks = (unsigned char*)malloc((size_t)w * h * d * sizeof(unsigned char));
    level->light_depths = (int*)malloc((size_t)w * h * sizeof(int));
    level->listener_count = 0;

    float base_frequency = 0.007f;
    float base_amplitude = 22.0f;
    float sea_level = 24.0f;

    float cave_frequency = 0.05f;
    float cave_range = 0.08f;

    float seed_offset_x = (float)(rand() % 50000);
    float seed_offset_y = (float)(rand() % 50000);
    float seed_offset_z = (float)(rand() % 50000);

    for (int x = 0; x < w; x++) {
        for (int z = 0; z < h; z++) {
            float total_noise = 0.0f;
            float freq = base_frequency;
            float amp = base_amplitude;

            for (int octave = 0; octave < 3; octave++) {
                float sample_x = ((float)x + seed_offset_x) * freq;
                float sample_z = ((float)z + seed_offset_z) * freq;
                total_noise += stb_perlin_noise3(sample_x, 0.0f, sample_z, 0, 0, 0) * amp;
                freq *= 2.0f;
                amp *= 0.4f;
            }

            int surface_y = (int)(total_noise + sea_level);
            if (surface_y < 1) surface_y = 1;
            if (surface_y >= d) surface_y = d - 1;

            for (int y = 0; y < d; y++) {
                int idx = (y * level->height + z) * level->width + x;

                if (y > surface_y) {
                    level->blocks[idx] = 0;
                    continue;
                }

                float c_sample_x = ((float)x + seed_offset_x) * cave_frequency;
                float c_sample_y = ((float)y + seed_offset_y) * cave_frequency * 1.3f;
                float c_sample_z = ((float)z + seed_offset_z) * cave_frequency;

                float cave_noise = stb_perlin_noise3(c_sample_x, c_sample_y, c_sample_z, 0, 0, 0);


                bool is_border = (x == 0 || x == w - 1 || z == 0 || z == h - 1);

                if (y > 2 && y < surface_y - 4 && fabs(cave_noise) < cave_range && !is_border) {
                    level->blocks[idx] = 0;
                } else {
                    if (y == surface_y) {
                        level->blocks[idx] = 1;
                    } else if (y < surface_y && y >= surface_y - 3) {
                        level->blocks[idx] = 3;
                    } else {
                        level->blocks[idx] = 2;
                    }
                }
            }
        }
    }

    memset(level->light_depths, 0, (size_t)w * h * sizeof(int));

    level_calc_light_depths(level, 0, 0, w, h);
    level_load(level);
    return level;
}

void level_destroy(Level* level) {
    if (!level) return;
    free(level->blocks);
    free(level->light_depths);
    free(level);
}

void level_load(Level* level) {
    FILE* f = fopen("level.dat", "rb");
    if (!f) return;

    size_t size = (size_t)level->width * level->height * level->depth;
    size_t read_bytes = fread(level->blocks, 1, size, f);
    fclose(f);

    if (read_bytes == size) {
        level_calc_light_depths(level, 0, 0, level->width, level->height);
        for (int i = 0; i < level->listener_count; i++) {
            if (level->listeners[i].all_changed) {
                level->listeners[i].all_changed(level->listeners[i].instance);
            }
        }
    }
}

void level_save(Level* level) {
    FILE* f = fopen("level.dat", "wb");
    if (!f) return;

    size_t size = (size_t)level->width * level->height * level->depth;
    fwrite(level->blocks, 1, size, f);
    fclose(f);
}

void level_calc_light_depths(Level* level, int x0, int z0, int x1, int z1) {
    for (int x = x0; x < x0 + x1; x++) {
        for (int z = z0; z < z0 + z1; z++) {
            if (x < 0 || x >= level->width || z < 0 || z >= level->height) continue;

            int l_idx = x + z * level->width;
            int old_depth = level->light_depths[l_idx];
            int y = level->depth - 1;

            while (y > 0 && !level_is_light_blocker(level, x, y, z)) {
                y--;
            }
            level->light_depths[l_idx] = y;

            if (old_depth != y) {
                int yl0 = (old_depth < y) ? old_depth : y;
                int yl1 = (old_depth > y) ? old_depth : y;
                for (int i = 0; i < level->listener_count; i++) {
                    if (level->listeners[i].light_column_changed) {
                        level->listeners[i].light_column_changed(level->listeners[i].instance, x, z, yl0, yl1);
                    }
                }
            }
        }
    }
}

void level_add_listener(Level* level, LevelListener listener) {
    if (level->listener_count < MAX_LISTENERS) {
        level->listeners[level->listener_count++] = listener;
    }
}

void level_remove_listener(Level* level, LevelListener listener) {
    for (int i = 0; i < level->listener_count; i++) {
        if (level->listeners[i].instance == listener.instance) {
            for (int j = i; j < level->listener_count - 1; j++) {
                level->listeners[j] = level->listeners[j + 1];
            }
            level->listener_count--;
            break;
        }
    }
}

bool level_is_tile(Level* level, int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return false;
    int idx = (y * level->height + z) * level->width + x;
    return (level->blocks[idx] != 0);
}

bool level_is_solid_tile(Level* level, int x, int y, int z) {
    return level_is_tile(level, x, y, z);
}

bool level_is_light_blocker(Level* level, int x, int y, int z) {
    return level_is_solid_tile(level, x, y, z);
}

int level_get_cubes(Level* level, AABB box, AABB* out_boxes, int max_count) {
    int count = 0;
    int x0 = (int)box.x0; int x1 = (int)(box.x1 + 1.0f);
    int y0 = (int)box.y0; int y1 = (int)(box.y1 + 1.0f);
    int z0 = (int)box.z0; int z1 = (int)(box.z1 + 1.0f);

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 > level->width) x1 = level->width;
    if (y1 > level->depth) y1 = level->depth;
    if (z1 > level->height) z1 = level->height;

    for (int x = x0; x < x1; x++) {
        for (int y = y0; y < y1; y++) {
            for (int z = z0; z < z1; z++) {
                if (level_is_solid_tile(level, x, y, z)) {
                    if (count < max_count) {
                        out_boxes[count++] = aabb_create((float)x, (float)y, (float)z, (float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f);
                    }
                }
            }
        }
    }
    return count;
}

float level_get_brightness(Level* level, int x, int y, int z) {
    float dark = 0.8f; float light = 1.0f;
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return light;
    return level_is_tile(level, x, y, z) ? dark : light;
}

void level_set_tile(Level* level, int x, int y, int z, int type) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return;

    level->blocks[(y * level->height + z) * level->width + x] = (unsigned char)type;
    level_calc_light_depths(level, x, z, 1, 1);

    for (int i = 0; i < level->listener_count; i++) {
        if (level->listeners[i].tile_changed) {
            level->listeners[i].tile_changed(level->listeners[i].instance, x, y, z);
        }
    }
}
