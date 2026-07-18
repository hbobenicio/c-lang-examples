#include "net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int tcp_socket_create(const char* server_host, uint16_t server_port, int backlog)
{
    assert(server_host != NULL);
    assert(server_port > 0);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("error: failed to create tcp socket");
        return -1;
    }

    int opt = 1;
    int rc = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (rc != 0) {
        perror("error: failed to set tcp socket options");
        close(socket_fd);
        return -1;
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        // .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(server_port),
    };
    if (inet_pton(AF_INET, server_host, &server_address.sin_addr.s_addr) != 1) {
        perror("error: inet_pton() failed\n");
        close(socket_fd);
        return -1;
    }
    socklen_t addrlen = sizeof(server_address);

    if (bind(socket_fd, (struct sockaddr *) &server_address, addrlen) != 0) {
        perror("error: address binding failed");
        close(socket_fd);
        return -1;
    }

    if (listen(socket_fd, backlog) != 0) {
        perror("error: tcp listen failed");
        close(socket_fd);
        return -1;
    }

    fprintf(stderr, "info: server listening on port %"PRIu16"...\n", server_port);

    return socket_fd;
}

ssize_t recv_char(int fd, char *out_value)
{
    assert(fd != -1);
    assert(out_value != NULL);

    char c;
    ssize_t bytes_received = recv(fd, &c, sizeof(c), 0);
    if (bytes_received < 0) {
        int error_code = errno;
        const char* error_reason = strerror(error_code);
        fprintf(stderr, "error: %s(): [%d] %s\n", __func__, error_code, error_reason);
        return bytes_received;
    }
    if (bytes_received == 0) {
        fprintf(stderr, "error: %s(): eof\n", __func__);
        return bytes_received;
    }
    
    *out_value = c;
    return bytes_received;
}
