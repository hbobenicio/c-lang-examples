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
#define CONFIG_HTTP_SERVER_TCP_BACKLOG_DEFAULT 16

enum return_code config_init_from_env(struct config* c)
{
    assert(c != NULL);

    const char* key;
    const char* value;

    key = "HTTP_SERVER_DEBUG";
    value = getenv(key);
    if (value == NULL) {
        c->debug = false;
    } else {
        c->debug = true;
    }

    key = "HTTP_SERVER_HOST";
    value = getenv(key);
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
        unsigned long long parsed_value;
        if (parse_ull(value, &parsed_value) != 0) {
            LOG_ERRORF("%s: bad value: Not a Number", key);
            return RC_ERROR;
        } else if (parsed_value > UINT16_MAX) {
            LOG_ERRORF("%s: bad value: value is too big", key);
            return RC_ERROR;
        } else {
            c->server_port = (uint16_t) parsed_value;
        }
    }

    key = "HTTP_SERVER_TCP_BACKLOG";
    value = getenv(key);
    if (value == NULL) {
        c->tcp_backlog = CONFIG_HTTP_SERVER_TCP_BACKLOG_DEFAULT;
    } else {
        unsigned long long parsed_value;
        if (parse_ull(value, &parsed_value) != 0) {
            LOG_ERRORF("%s: bad value: Not a Number", key);
            return RC_ERROR;
        } else if (parsed_value > UINT16_MAX) {
            LOG_ERRORF("%s: bad value: value is too big", key);
            return RC_ERROR;
        } else {
            static_assert(UINT16_MAX < INT_MAX, "we store it as u16 but the listen posix API gets it as an int");
            c->tcp_backlog = (uint16_t) parsed_value;
        }
    }

    return RC_OK;
}

