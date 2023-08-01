#pragma once

#include <stdint.h>

/**
 * The application configuration
 */
struct config {
    const char* server_host;
    uint16_t server_port;
};

/**
 * Initializes the configuration from environment variables values.
 *
 * @param c The configuration to be loaded, where data should be saved
 * @return On success, 0 is returned
 */
int config_init_from_env(struct config* c);

