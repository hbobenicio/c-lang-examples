#pragma once

#include <stdint.h>

struct config {
    const char* server_host;
    uint16_t server_port;
};

int config_init_from_env(struct config* c);

