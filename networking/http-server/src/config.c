#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "str.h"
#include "log.h"
#define LOG_NAME "config"

#define DEFAULT_DEBUG false
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8080
#define DEFAULT_TCP_BACKLOG 16
#define DEFAULT_READ_TIMEOUT_MS 3000
#define DEFAULT_WRITE_TIMEOUT_MS 1
#define DEFAULT_CONNECTION_TIMEOUT_MS 9000

static void config_init_debug(struct config* c);
static void config_init_host(struct config* c);
static enum return_code config_init_port(struct config* c);
static enum return_code config_init_tcp_backlog(struct config* c);
static enum return_code config_init_read_timeout_ms(struct config* c);
static enum return_code config_init_write_timeout_ms(struct config* c);
static enum return_code config_init_connection_timeout_ms(struct config* c);

enum return_code config_init_from_env(struct config* c)
{
    assert(c != NULL);

    bool failed = false;

    config_init_debug(c);
    config_init_host(c);

    if (config_init_port(c) != RC_OK) {
        failed = true;
    }

    if (config_init_tcp_backlog(c) != RC_OK) {
        failed = true;
    }

    if (config_init_read_timeout_ms(c) != RC_OK) {
        failed = true;
    }

    if (config_init_write_timeout_ms(c) != RC_OK) {
        failed = true;
    }

    if (config_init_connection_timeout_ms(c) != RC_OK) {
        failed = true;
    }

    if (failed) {
        LOG_ERROR("failed to load configuration from environment variables");
        return RC_ERROR;
    }

    return RC_OK;
}

static void config_init_debug(struct config* c)
{
    if (getenv("HTTP_SERVER_DEBUG") == NULL) {
        c->debug = DEFAULT_DEBUG;
    } else {
        c->debug = true;
    }
}

static void config_init_host(struct config* c)
{
    const char* value = getenv("HTTP_SERVER_HOST");
    if (value == NULL) {
        c->server_host = DEFAULT_HOST;
    } else {
        c->server_host = value;
    }
}

static enum return_code config_init_port(struct config* c)
{
    const char* key = "HTTP_SERVER_PORT";
    const char* value = getenv(key);
    if (value == NULL) {
        c->server_port = DEFAULT_PORT;
        return RC_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != 0) {
        LOG_ERRORF("%s: bad value: Not a Number", key);
        return RC_ERROR;
    }
    if (parsed_value > UINT16_MAX) {
        LOG_ERRORF("%s: bad value: value is too big", key);
        return RC_ERROR;
    }

    c->server_port = (uint16_t) parsed_value;
    return RC_OK;
}

static enum return_code config_init_tcp_backlog(struct config* c)
{
    const char* key = "HTTP_SERVER_TCP_BACKLOG";
    const char* value = getenv(key);
    if (value == NULL) {
        c->tcp_backlog = DEFAULT_TCP_BACKLOG;
        return RC_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != 0) {
        LOG_ERRORF("%s: bad value: Not a Number", key);
        return RC_ERROR;
    }
    if (parsed_value > UINT16_MAX) {
        LOG_ERRORF("%s: bad value: value is too big", key);
        return RC_ERROR;
    }

    static_assert(UINT16_MAX < INT_MAX, "we store it as u16 but the listen posix API gets it as an int");
    c->tcp_backlog = (uint16_t) parsed_value;

    return RC_OK;
}

static enum return_code config_init_read_timeout_ms(struct config* c)
{
    const char* key = "HTTP_SERVER_READ_TIMEOUT_MS";
    const char* value = getenv(key);
    if (value == NULL) {
        c->read_timeout_ms = DEFAULT_READ_TIMEOUT_MS;
        return RC_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != 0) {
        LOG_ERRORF("%s: bad value: NaN", key);
        return RC_ERROR;
    }

    c->read_timeout_ms = (size_t) parsed_value;

    return RC_OK;
}

static enum return_code config_init_write_timeout_ms(struct config* c)
{
    const char* key = "HTTP_SERVER_WRITE_TIMEOUT_MS";
    const char* value = getenv(key);
    if (value == NULL) {
        c->write_timeout_ms = DEFAULT_WRITE_TIMEOUT_MS;
        return RC_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != 0) {
        LOG_ERRORF("%s: bad value: NaN", key);
        return RC_ERROR;
    }

    c->write_timeout_ms = (size_t) parsed_value;

    return RC_OK;
}

static enum return_code config_init_connection_timeout_ms(struct config* c)
{
    const char* key = "HTTP_SERVER_CONNECTION_TIMEOUT_MS";
    const char* value = getenv(key);
    if (value == NULL) {
        c->connection_timeout_ms = DEFAULT_CONNECTION_TIMEOUT_MS;
        return RC_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != 0) {
        LOG_ERRORF("%s: bad value: Not a Number", key);
        return RC_ERROR;
    }

    c->connection_timeout_ms = (size_t) parsed_value;

    return RC_OK;
}
