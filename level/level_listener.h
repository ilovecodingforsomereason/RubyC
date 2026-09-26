#ifndef LEVEL_LISTENER_H
#define LEVEL_LISTENER_H

typedef struct LevelListener LevelListener;

struct LevelListener {
    void (*tile_changed)(void* instance, int x, int y, int z);
    void (*light_column_changed)(void* instance, int x, int z, int y0, int y1);
    void (*all_changed)(void* instance);
    void* instance;
};

#endif
