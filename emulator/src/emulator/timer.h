#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "register.h"

// Chip-8 provides 2 timers, a delay timer and a sound timer.
// 
// The delay timer is active whenever the delay timer register (DT) is non-zero.
// This timer does nothing more than subtract 1 from the value of DT at a rate of 60Hz.
// When DT reaches 0, it deactivates.
// 
// The sound timer is active whenever the sound timer register (ST) is non-zero.
// This timer also decrements at a rate of 60Hz, however, as long as ST's value is greater than zero,
// the Chip-8 buzzer will sound.
// When ST reaches zero, the sound timer deactivates.

struct timer {
    uint64_t last_tick_timestamp_ms;
    Register value;
};

void timer_init(struct timer* t);
void timer_start(struct timer* t);
void timer_update(struct timer* t);
bool timer_is_active(const struct timer* t);
void timer_tick(struct timer* t);
Register timer_get_value(const struct timer* t);
void timer_set_value(struct timer* t, Register value);
