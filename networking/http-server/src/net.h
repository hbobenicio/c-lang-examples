#pragma once

#include <stdint.h>

int tcp_socket_or_exit(void);

struct sockaddr_in ipv4_address_create(const char* ip, uint16_t port);

