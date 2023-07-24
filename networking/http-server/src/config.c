#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#define CONFIG_HTTP_SERVER_PORT_DEFAULT 8080

int config_init_from_env(struct config* c)
{
    assert(c != NULL);
    const char* env_value;

    env_value = getenv("HTTP_SERVER_HOST");
    if (env_value == NULL) {
        c->server_host = "127.0.0.1";
    } else {
        c->server_host = env_value;
    }

    env_value = getenv("HTTP_SERVER_PORT");
    if (env_value == NULL) {
        c->server_port = CONFIG_HTTP_SERVER_PORT_DEFAULT;
    } else {
        char* endptr = NULL;
        errno = 0;
        unsigned long value = strtoul(env_value, &endptr, 10);
        if (errno != 0 || value >= ULONG_MAX) {
            fprintf(stderr, "warn: config: HTTP_SERVER_PORT: bad value. ignoring it and using default value\n");
            c->server_port = CONFIG_HTTP_SERVER_PORT_DEFAULT;
        } else {
            c->server_port = value;
        }
    }

    return 0;
}

