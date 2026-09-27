# 1 "level/level.h"
#ifndef LEVEL_H
#define LEVEL_H 

#include <stdbool.h>
#include "../phys/aabb.h"
#include "level_listener.h"

#define MAX_LISTENERS 8

typedef struct Level {
    int width;
    int height;
    int depth;
    unsigned char* blocks;
    int* light_depths;
    LevelListener listeners[MAX_LISTENERS];
    int listener_count;
} Level;

Level* level_create(int w, int h, int d);
void level_destroy(Level* level);
void level_load(Level* level);
void level_save(Level* level);
void level_calc_light_depths(Level* level, int x0, int z0, int x1, int z1);
void level_add_listener(Level* level, LevelListener listener);
void level_remove_listener(Level* level, LevelListener listener);

bool level_is_tile(Level* level, int x, int y, int z);
bool level_is_solid_tile(Level* level, int x, int y, int z);
bool level_is_light_blocker(Level* level, int x, int y, int z);

int level_get_cubes(Level* level, AABB box, AABB* out_boxes, int max_count);
float level_get_brightness(Level* level, int x, int y, int z);
void level_set_tile(Level* level, int x, int y, int z, int type);


static inline float level_get_width(Level* l) { return (float)l->width; }
static inline float level_get_height(Level* l) { return (float)l->height; }
static inline float level_get_depth(Level* l) { return (float)l->depth; }

#endif
