#pragma once

#include <sys/types.h>
#include <arpa/inet.h>

#include <http-server/error.h>

struct config;

struct http_server {
    const struct config* config;
    int socket;
};

enum result http_server_init(struct http_server* server, const struct config* config);
void http_server_free(struct http_server* server);

