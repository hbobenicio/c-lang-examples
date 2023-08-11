#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "error.h"

/**
 * The application configuration
 */
struct config {
    bool debug;
    const char* server_host;
    uint16_t server_port;
    uint16_t tcp_backlog;
    size_t read_timeout_ms;
    size_t write_timeout_ms;
    size_t connection_timeout_ms;
};

/**
 * @brief returns the global instance of the configuration
 */
const struct config* config_get(void);

/**
 * @brief Initializes the configuration from environment variables values.
 */
enum result config_init_from_env(void);
