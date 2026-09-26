#ifndef HIT_RESULT_H
#define HIT_RESULT_H

#include <stdlib.h>

typedef struct {
    int x;
    int y;
    int z;
    int o; 
    int f; 
} HitResult;

static inline HitResult* hit_result_create(int x, int y, int z, int o, int f) {
    HitResult* hr = (HitResult*)malloc(sizeof(HitResult));
    if (hr) {
        hr->x = x; hr->y = y; hr->z = z;
        hr->o = o; hr->f = f;
    }
    return hr;
}

#endif
