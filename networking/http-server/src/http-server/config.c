#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "commons.h"
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

static void config_init_debug(void);
static void config_init_host(void);
static enum result config_init_port(void);
static enum result config_init_tcp_backlog(void);
static enum result config_init_read_timeout_ms(void);
static enum result config_init_write_timeout_ms(void);
static enum result config_init_connection_timeout_ms(void);

/**
 * @brief the global config instance
 */
static struct config config = {0};

const struct config* config_get(void)
{
    return &config;
}

enum result config_init_from_env(void)
{
    bool failed = false;

    config_init_debug();
    config_init_host();

    if (config_init_port() != RESULT_OK) {
        failed = true;
    }

    if (config_init_tcp_backlog() != RESULT_OK) {
        failed = true;
    }

    if (config_init_read_timeout_ms() != RESULT_OK) {
        failed = true;
    }

    if (config_init_write_timeout_ms() != RESULT_OK) {
        failed = true;
    }

    if (config_init_connection_timeout_ms() != RESULT_OK) {
        failed = true;
    }

    if (failed) {
        LOG_ERROR("failed to load configuration from environment variables");
        return RESULT_ERR;
    }

    return RESULT_OK;
}

static void config_init_debug(void)
{
    if (getenv("HTTP_SERVER_DEBUG") == NULL) {
        config.debug = DEFAULT_DEBUG;
    } else {
        config.debug = true;
    }
}

static void config_init_host(void)
{
    const char* value = getenv("HTTP_SERVER_HOST");
    if (value == NULL) {
        config.server_host = DEFAULT_HOST;
    } else {
        config.server_host = value;
    }
}

static enum result config_init_port(void)
{
    const char* key = "HTTP_SERVER_PORT";
    const char* value = getenv(key);
    if (value == NULL) {
        config.server_port = DEFAULT_PORT;
        return RESULT_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != RESULT_OK) {
        LOG_ERRORF("%s: bad value: Not a Number", key);
        return RESULT_ERR;
    }
    if (parsed_value > UINT16_MAX) {
        LOG_ERRORF("%s: bad value: value is too big", key);
        return RESULT_ERR;
    }

    config.server_port = (uint16_t) parsed_value;
    return RESULT_OK;
}

static enum result config_init_tcp_backlog(void)
{
    const char* key = "HTTP_SERVER_TCP_BACKLOG";
    const char* value = getenv(key);
    if (value == NULL) {
        config.tcp_backlog = DEFAULT_TCP_BACKLOG;
        return RESULT_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != RESULT_OK) {
        LOG_ERRORF("%s: bad value: Not a Number", key);
        return RESULT_ERR;
    }
    if (parsed_value > UINT16_MAX) {
        LOG_ERRORF("%s: bad value: value is too big", key);
        return RESULT_ERR;
    }

    static_assert(UINT16_MAX < INT_MAX, "we store it as u16 but the listen posix API gets it as an int");
    config.tcp_backlog = (uint16_t) parsed_value;

    return RESULT_OK;
}

static enum result config_init_read_timeout_ms(void)
{
    const char* key = "HTTP_SERVER_READ_TIMEOUT_MS";
    const char* value = getenv(key);
    if (value == NULL) {
        config.read_timeout_ms = DEFAULT_READ_TIMEOUT_MS;
        return RESULT_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != RESULT_OK) {
        LOG_ERRORF("%s: bad value: NaN", key);
        return RESULT_ERR;
    }
    if (parsed_value > INT_MAX) {
        LOG_ERRORF("%s: bad value: value is too big", key);
        return RESULT_ERR;
    }

    config.read_timeout_ms = (size_t) parsed_value;

    return RESULT_OK;
}

static enum result config_init_write_timeout_ms(void)
{
    const char* key = "HTTP_SERVER_WRITE_TIMEOUT_MS";
    const char* value = getenv(key);
    if (value == NULL) {
        config.write_timeout_ms = DEFAULT_WRITE_TIMEOUT_MS;
        return RESULT_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != RESULT_OK) {
        LOG_ERRORF("%s: bad value: NaN", key);
        return RESULT_ERR;
    }

    config.write_timeout_ms = (size_t) parsed_value;

    return RESULT_OK;
}

static enum result config_init_connection_timeout_ms(void)
{
    const char* key = "HTTP_SERVER_CONNECTION_TIMEOUT_MS";
    const char* value = getenv(key);
    if (value == NULL) {
        config.connection_timeout_ms = DEFAULT_CONNECTION_TIMEOUT_MS;
        return RESULT_OK;
    }

    unsigned long long parsed_value;
    if (parse_ull(value, &parsed_value) != RESULT_OK) {
        LOG_ERRORF("%s: bad value: Not a Number", key);
        return RESULT_ERR;
    }

    config.connection_timeout_ms = (size_t) parsed_value;

    return RESULT_OK;
}
