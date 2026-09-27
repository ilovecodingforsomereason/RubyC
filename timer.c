# 1 "timer.c"
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include "timer.h"
#include <time.h>

static uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

void timer_init(Timer* timer, float ticks_per_second) {
    timer->ticks_per_second = ticks_per_second;
    timer->last_time = get_time_ns();
    timer->ticks = 0;
    timer->a = 0.0f;
    timer->time_scale = 1.0f;
    timer->fps = 0.0f;
    timer->passed_time = 0.0f;
}

void timer_advance_time(Timer* timer) {
    uint64_t now = get_time_ns();
    int64_t passed_ns = (int64_t)(now - timer->last_time);
    timer->last_time = now;

    if (passed_ns < 0) passed_ns = 0;
    if (passed_ns > 1000000000L) passed_ns = 1000000000L;

    if (passed_ns > 0) {
        timer->fps = 1000000000.0f / (float)passed_ns;
    }

    timer->passed_time += (float)passed_ns * timer->time_scale * timer->ticks_per_second / 1000000000.0f;
    timer->ticks = (int)timer->passed_time;

    if (timer->ticks > 100) timer->ticks = 100;

    timer->passed_time -= timer->ticks;
    timer->a = timer->passed_time;
}
