#pragma once

#include <stdlib.h>
#include <stdint.h>

int tcp_socket_create(const char* server_host, uint16_t server_port, int backlog);
ssize_t recv_char(int fd, char *out_value);
