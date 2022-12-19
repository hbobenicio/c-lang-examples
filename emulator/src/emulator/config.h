#pragma once

#include <stdint.h>

#include "logging/level.h"

struct config {
    enum log_level log_level;
    uint32_t cpu_clock_speed_hz;
};

/**
 * @brief Retrieves a reference to the (static) global config object
 */
const struct config* config(void);

/**
 * @brief Initializes the global config object from environment variable definitions
 */
void config_init_from_env(void);
