#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DYNAMIC_ARRAY_IMPLEMENTATION
#include "dynamic-array.h"

#include "str.h"
#include "net.h"
#include "http.h"
#include "config.h"

int main(void)
{
    fprintf(stderr, "info: server: initializing...\n");

    struct config config;
    int rc = config_init_from_env(&config);
    if (rc != 0) {
        fprintf(stderr, "error: config: bad config.\n");
        exit(1);
    }

    // 1. socket
    int server_socket = tcp_socket_or_exit();

    struct sockaddr_in server_address = ipv4_address_create(config.server_host, config.server_port);

    // 2. bind
    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) != 0) {
        fprintf(stderr, "error: tcp: socket bind: failed: %s\n", strerror(errno));
        close(server_socket);
        exit(1);
    }

    // 3. listen
    int backlog = 10;
    if (listen(server_socket, backlog) != 0) {
        fprintf(stderr, "error: socket listening: failed: %s\n", strerror(errno));
        close(server_socket);
        exit(1);
    }

    // 4. accept
    struct sockaddr_in client_address = {0};
    socklen_t client_address_len = sizeof(client_address);
    fprintf(stderr, "info: server is ready for connections at %s:%hu\n", config.server_host, config.server_port);
    int client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_len);
    if (client_socket < 0) {
        fprintf(stderr, "error: socket accept: failed: %s\n", strerror(errno));
        close(server_socket);
        exit(1);
    }
    fprintf(stderr, "info: new client connected\n");

    while (true) {
        char request_buffer[4098] = {0};
        ssize_t bytes_received = read(client_socket, request_buffer, sizeof(request_buffer));
        if (bytes_received <= 0) {
            break;
        }
        fprintf(stderr, "debug: read %ld bytes\n", bytes_received);
        fwrite(request_buffer, 1, bytes_received, stdout);

        // _http_headers_parse(request_buffer, bytes_received);

        const char response_body[] = "It Works!\n";
        const size_t response_body_len = sizeof(response_body) - 1;

        char response_buffer[4098] = {0};
        int rc = snprintf(response_buffer, sizeof(response_buffer),
            "HTTP/1.1 200 OK\r\n"
            "Server: BadIdea/0.0.0\r\n"
            "Content-Length: %zu\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n"
            "Connection: Closed\r\n"
            "\r\n"
            "%s",
            response_body_len,
            response_body
        );
        assert(rc > 0);
        write(client_socket, response_buffer, sizeof(response_buffer));
    }
    close(client_socket);
    close(server_socket);
}

