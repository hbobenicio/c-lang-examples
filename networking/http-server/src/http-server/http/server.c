#include "server.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>

#include <http-server/config.h>
#include <http-server/net.h>
#include <http-server/log.h>
#define LOG_NAME "http/server"

static enum result http_server_bind(struct http_server* server);
static enum result http_server_listen(struct http_server* server);

enum result http_server_init(struct http_server* server, const struct config* config)
{
    assert(server != NULL);
    assert(config != NULL);

    server->config = config;
    server->socket = tcp_socket_or_exit();

    int opt = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0) {
        int error_code = errno;
        LOG_ERRORF("failed to set socket options: %s", strerror(error_code));
        goto err;
    }

    if (http_server_bind(server) != RESULT_OK) {
        goto err;
    }

    if (http_server_listen(server) != RESULT_OK) {
        goto err;
    }
   
    return RESULT_OK;

err:
    LOG_DEBUG("socket: closing...");
    close(server->socket);
    LOG_DEBUG("socket: closed.");

    return RESULT_ERR;
}

enum result http_server_bind(struct http_server* server)
{
    assert(server != NULL);
    struct sockaddr_in server_address = ipv4_address_create(server->config->server_host, server->config->server_port);

    LOG_DEBUGF("binding socket to address... addr=\"%s:%hu\"", server->config->server_host, server->config->server_port);

    if (bind(server->socket, (struct sockaddr*) &server_address, sizeof(server_address)) != 0) {
        int error_code = errno;
        LOG_ERRORF("socket binding failed: %s", strerror(error_code));
        return RESULT_ERR;
    }

    return RESULT_OK;
}

enum result http_server_listen(struct http_server* server)
{
    assert(server != NULL);

    LOG_DEBUGF("listening... tcp_backlog=%d", (int)server->config->tcp_backlog);

    if (listen(server->socket, (int) server->config->tcp_backlog) != 0) {
        int error_code = errno;
        LOG_ERRORF("socket listening failed: %s", strerror(error_code));
        return RESULT_ERR;
    }

    return RESULT_OK;
}

void http_server_free(struct http_server* server)
{
    LOG_DEBUG("socket: closing...");

    close(server->socket);

    LOG_DEBUG("socket: closed.");
}

