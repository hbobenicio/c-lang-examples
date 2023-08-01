#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "str.h"
#include "log.h"
#define LOG_NAME "config"

#define CONFIG_HTTP_SERVER_PORT_DEFAULT 8080

int config_init_from_env(struct config* c)
{
    assert(c != NULL);

    const char* key = "HTTP_SERVER_HOST";
    const char* value = getenv(key);
    if (value == NULL) {
        c->server_host = "127.0.0.1";
    } else {
        c->server_host = value;
    }

    key = "HTTP_SERVER_PORT";
    value = getenv(key);
    if (value == NULL) {
        c->server_port = CONFIG_HTTP_SERVER_PORT_DEFAULT;
    } else {
        unsigned long parsed_value;
        if (parse_ul(value, &parsed_value) != 0) {
            LOG_WARNF("%s: bad value. ignoring it and using default value: %d", key, CONFIG_HTTP_SERVER_PORT_DEFAULT);
            c->server_port = CONFIG_HTTP_SERVER_PORT_DEFAULT;
        } else {
            c->server_port = (uint16_t) parsed_value;
        }
    }

    return 0;
}

