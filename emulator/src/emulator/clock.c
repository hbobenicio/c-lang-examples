#include "clock.h"

#include <SDL2/SDL_timer.h>

void clock_init_with_duration_ms(struct clock* c, size_t tick_duration_ms, ClockOnTickCallback callback)
{
    c->on_tick_callback = callback;
    c->last_tick_timestamp_ms = 0;
    c->tick_count = 0;
    c->tick_duration_ms = tick_duration_ms;
}

void clock_init_with_frequency_hz(struct clock* c, size_t frequency_speed_hz, ClockOnTickCallback callback)
{
    c->on_tick_callback = callback;
    c->last_tick_timestamp_ms = 0;
    c->tick_count = 0;
    c->tick_duration_ms= frequency_speed_hz * 1000;
}

void clock_start(struct clock* c)
{
    c->last_tick_timestamp_ms = SDL_GetTicks64();
}

void clock_free(struct clock* c)
{

}
