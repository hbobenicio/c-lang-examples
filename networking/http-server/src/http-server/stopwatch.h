#pragma once

#include <time.h>

struct stopwatch {
    struct timespec start_ts;
    struct timespec end_ts;
};

void stopwatch_start(struct stopwatch* sw);
void stopwatch_stop(struct stopwatch* sw);
void stopwatch_reset(struct stopwatch* sw);

size_t stopwatch_get_ns(struct stopwatch* sw);
size_t stopwatch_get_us(struct stopwatch* sw);
