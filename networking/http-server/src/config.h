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
 * Initializes the configuration from environment variables values.
 *
 * @param c The configuration to be loaded, where data should be saved
 */
enum return_code config_init_from_env(struct config* c);
