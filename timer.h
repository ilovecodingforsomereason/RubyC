#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct {
    float ticks_per_second;
    uint64_t last_time;
    int ticks;
    float a;
    float time_scale;
    float fps;
    float passed_time;
} Timer;

void timer_init(Timer* timer, float ticks_per_second);
void timer_advance_time(Timer* timer);

#endif
