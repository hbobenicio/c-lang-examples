#pragma once

#include <stdint.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int tcp_socket_or_exit(void);

struct sockaddr_in ipv4_address_create(const char* ip, uint16_t port);

