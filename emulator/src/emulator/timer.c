#include "timer.h"

#include <SDL2/SDL_timer.h>

#include "logging/logger.h"

#define LOG_TAG "timer"

#define CLOCK_FREQUENCY_HZ 60
#define CLOCK_TIMEUP_DURATION_MS (1 * 1000 / CLOCK_FREQUENCY_HZ)

void timer_init(struct timer* t)
{
    t->last_tick_timestamp_ms = 0;
    t->value = 0;
}

void timer_start(struct timer* t)
{
    t->last_tick_timestamp_ms = SDL_GetTicks64();
}

void timer_update(struct timer* t)
{
    if (!timer_is_active(t)) {
        //noop
        return;
    }

    uint64_t now_timestamp_ms = SDL_GetTicks64();
    uint64_t elapsed_time_ms = now_timestamp_ms - t->last_tick_timestamp_ms;
    if (elapsed_time_ms < CLOCK_TIMEUP_DURATION_MS) {
        // time is not up yet
        return;
    }

    // time's up! should tick

    uint64_t ticks_count = elapsed_time_ms / CLOCK_TIMEUP_DURATION_MS;

#ifdef EXIT_AT_MULTIPLE_TICKS
    log_fatal("delays between updates greater than 2 ticks are not handled ATM")
#else
    while (ticks_count--) {
        timer_tick(t);
    }
#endif
}

bool timer_is_active(const struct timer* t)
{
    return t->value > 0;
}

void timer_tick(struct timer* t)
{
    if (t->value > 0) {
        t->value--;
    }
}

Register timer_get_value(const struct timer* t)
{
    return t->value;
}

void timer_set_value(struct timer* t, Register value)
{
    //TODO should I restart/reset the timer?
    t->value = value;
}
