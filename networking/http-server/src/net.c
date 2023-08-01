#include "net.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"

#define LOG_NAME "net"

int tcp_socket_or_exit(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        int error_code = errno;
        LOG_ERRORF("failed to create tcp socket: %s", strerror(error_code));
        exit(1);
    }
    return sock;
}

struct sockaddr_in ipv4_address_create(const char* ip, uint16_t port)
{
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);
    return address;
}

