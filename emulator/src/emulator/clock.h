#pragma once

#include <stddef.h>

struct clock;

typedef int (*ClockOnTickCallback)(struct clock* c, void* userdata);

struct clock {
    ClockOnTickCallback on_tick_callback;
    size_t tick_duration_ms;
    size_t last_tick_timestamp_ms;
    size_t tick_count;
};

void clock_init_with_duration_ms(struct clock* c, size_t tick_duration_ms, ClockOnTickCallback callback);
void clock_init_with_frequency_hz(struct clock* c, size_t frequency_speed_hz, ClockOnTickCallback callback);
void clock_start(struct clock* c);
void clock_update(struct clock* c);
void clock_free(struct clock* c);
