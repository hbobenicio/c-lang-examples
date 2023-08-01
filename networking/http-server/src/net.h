#pragma once

#include <stdint.h>

/**
 * Creates a tcp socket or exit if it fails.
 *
 * @return On success, 0 is returned
 */
int tcp_socket_or_exit(void);

/**
 * Creates a POSIX IPv4 address struct.
 *
 * @param ip A null-terminated string containing the IPv4 address (e.g. "127.0.0.1", "0.0.0.0", ...)
 * @param port The address port
 */
struct sockaddr_in ipv4_address_create(const char* ip, uint16_t port);

