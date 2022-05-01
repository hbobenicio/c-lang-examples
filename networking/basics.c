#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

// POSIX
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ifaddrs.h>

#include "utils/cpu.h"

typedef struct {
    size_t tid;
} thread_context;

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    struct ifaddrs* interfaces = NULL;
    if (getifaddrs(&interfaces) == -1) {
        int err_code = errno;
        const char* err_msg = strerror(err_code);
        fprintf(stderr, "error: getifaddrs failed. errno=%d strerror=%s\n", err_code, err_msg);
        return EXIT_FAILURE;
    }

    fputs("[DEBUG] interfaces:\n", stderr);
    {
        size_t i = 0;
        for (struct ifaddrs* interface = interfaces; interface != NULL; interface = interface->ifa_next, i++) {
            fprintf(stderr, "[DEBUG]\t%zu.\t%s", i + 1, interface->ifa_name);

            struct sockaddr* address = interface->ifa_addr;

            switch (address->sa_family) {
            case AF_INET: {
                struct sockaddr_in *sock_addr_inet = (struct sockaddr_in *) address;

                char ip[50] = {0};
                if (inet_ntop(AF_INET, &sock_addr_inet->sin_addr, ip, sizeof(ip)) == NULL) {
                    // TODO improve this error handling
                    fprintf(stderr, "\n[ERROR] failed to get ipv4 from AF_INET interface address.\n");
                    exit(1);
                }

                fprintf(stderr, "\t\t%s\n", ip);

            } break;

            case AF_INET6: {
                struct sockaddr_in6 *sock_addr_inet6 = (struct sockaddr_in6 *) address;

                char ip[255] = {0};
                if (inet_ntop(AF_INET6, &sock_addr_inet6->sin6_addr, ip, sizeof(ip)) == NULL) {
                    // TODO improve this error handling
                    fprintf(stderr, "\n[ERROR] failed to get ipv6 from AF_INET6 interfaces address.\n");
                    exit(1);
                }

                fprintf(stderr, "\t\t%s\n", ip);
            }
            
            default:
                fprintf(stderr, "\t\t<unknown family %hu>\n", address->sa_family);
                break;
            }
        }
    }
    // struct ifaddrs  *ifa_next;    /* Next item in list */
    // char            *ifa_name;    /* Name of interface */
    // unsigned int     ifa_flags;   /* Flags from SIOCGIFFLAGS */
    // struct sockaddr *ifa_addr;    /* Address of interface */


    const size_t num_cores = checkports_utils_get_num_cores();
    fprintf(stderr, "[DEBUG] number of cores = %zu\n", num_cores);

    // thrd_t threads[num_cores];
    // thread_context threads_context[num_cores];

    // partition ports from x to y to num_cores

    return 0;
}
