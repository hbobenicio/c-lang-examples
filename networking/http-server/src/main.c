#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <signal.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DYNAMIC_ARRAY_IMPLEMENTATION
#include "dynamic-array.h"

#include "error.h"
#include "str.h"
#include "net.h"
#include "http.h"
#include "config.h"
#include "log.h"

#define LOG_NAME "http-server"

int main(void)
{
    struct timespec start_time = {0};
    struct timespec end_time   = {0};

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &start_time) != 0) {
        int error_code = errno;
        LOG_ERRORF("failed to get monotonic clock time: %s", strerror(error_code));
        exit(1);
    }

    LOG_INFO("initializing...");

    //TODO implement graceful shutdown with signal handlers
    //     https://wiki.sei.cmu.edu/confluence/display/c/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers
    //     volatile sig_atomic_t for flaging the main loop that it needs to stop? what about the timeout?
    // Create a signal set to include all signals
    sigset_t all_signals;
    sigfillset(&all_signals);

    struct config config = {0};
    if (config_init_from_env(&config) != RC_OK) {
        LOG_ERROR("bad config");
        exit(1);
    }

    //TODO add it to the config
    const int read_timeout_ms = 5000;

    // 1. socket
    int server_socket = tcp_socket_or_exit();

    // Set socket options
    //TODO search for more useful options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        int error_code = errno;
        LOG_ERRORF("failed to set socket options: %s", strerror(error_code));
        close(server_socket);
        exit(1);
    }
    
    struct sockaddr_in server_address = ipv4_address_create(config.server_host, config.server_port);

    // 2. bind
    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) != 0) {
        int error_code = errno;
        LOG_ERRORF("socket binding failed: %s", strerror(error_code));
        close(server_socket);
        exit(1);
    }

    // 3. listen
    if (listen(server_socket, (int) config.tcp_backlog) != 0) {
        int error_code = errno;
        LOG_ERRORF("socket listening failed: %s", strerror(error_code));
        close(server_socket);
        exit(1);
    }

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &end_time) != 0) {
        int error_code = errno;
        LOG_ERRORF("failed to get monotonic clock time: %s", strerror(error_code));
        close(server_socket);
        exit(1);
    }

    size_t elapsed_time_nsec = (end_time.tv_sec - start_time.tv_sec) * 1000 * 1000 * 1000;
    elapsed_time_nsec += end_time.tv_nsec - start_time.tv_nsec;

    LOG_INFOF("server is ready for connections at " ANSI_HWHT "http://%s:%hu" ANSI_RESET " after %zu us",
              config.server_host,
              config.server_port,
              elapsed_time_nsec / 1000);

    // Main loop
    while (true) {
        struct sockaddr_in client_address = {0};
        socklen_t client_address_len = sizeof(client_address);

        // 4. accept
        //NOTE this will block until a new client connection is stablished

        int client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_len);
        if (client_socket < 0) {
            fprintf(stderr, "error: socket accept: failed: %s\n", strerror(errno));
            close(server_socket);
            exit(1);
        }

        //TODO generate a unique client connection id
        LOG_DEBUG("new client connected.");

        // we'll only poll the client socket for input events (dont forget we still writes the response to it)
        //TODO check if we also should implement write timeout...
        struct pollfd client_socket_pollfd = {
            .fd = client_socket,
            .events = POLLIN,
        };

        // Socket polling/reading loop
        while (true) {

            //TODO check if we really want/need to block signals when polling. what happens when CTRL-C/Kill while polling?
            //     should we not terminate???
            // Block all signals for the current thread. required(?) for POSIX poll, already encapsulated if using linux ppoll
            pthread_sigmask(SIG_BLOCK, &all_signals, NULL);
            int poll_ready_count = poll(&client_socket_pollfd, (nfds_t) 1, read_timeout_ms);
            pthread_sigmask(SIG_UNBLOCK, &all_signals, NULL);

            if (poll_ready_count < 0) {
                int error_code = errno;
                LOG_ERRORF("client socket: read polling failed: %s", strerror(error_code));
                break;
            }

            // timed out
            if (poll_ready_count == 0) {
                LOG_WARNF("client socket: read polling timeout after %d ms.", read_timeout_ms);
                break;
            }

            // We are only checking POLLIN events here
            assert(client_socket_pollfd.revents & POLLIN);

            char request_buffer[4098] = {0};

            //TODO replace read by recv
            //NOTE DO NOT assume this buffer contains a null-terminated c-string. It's just an opaque array of bytes ATM
            ssize_t bytes_received = read(client_socket, request_buffer, sizeof(request_buffer));
            if (bytes_received <= 0) {
                LOG_DEBUG("client disconnected");
                break;
            }
            LOG_DEBUGF("read %ld bytes (max buffer size = %zu)", bytes_received, sizeof(request_buffer));

            // Debug Dumping request to stdout
            //FIXME something odd is happening here in this write when the request comes from a browser...
            fwrite(request_buffer, 1, bytes_received, stdout);

            //TODO parse the request
            //TODO routing
            // _http_headers_parse(request_buffer, bytes_received);

            const char response_body[] = "It Works!\n";

            //NOTE sizeof on arrays counts the trailing '\0'
            const size_t response_body_len = sizeof(response_body) - 1;

            char response_buffer[4098] = {0};
            int rc = snprintf(response_buffer, sizeof(response_buffer),
                              "HTTP/1.1 %d OK\r\n"
                              "Server: http-server/0.0.0\r\n"
                              "Content-Length: %zu\r\n"
                              "Content-Type: text/plain; charset=utf-8\r\n"
                              "Connection: Closed\r\n"
                              "\r\n"
                              "%s",
                              200,
                              response_body_len,
                              response_body
                              );
            assert(rc > 0);
            // release builds strip off assert calls so the compiler still warns about rc not being used.
            (void) rc;

            write(client_socket, response_buffer, sizeof(response_buffer));

        } // Reading loop

        LOG_DEBUG("client socket: closing...");
        close(client_socket);
        LOG_DEBUG("client socket: closed.");

    } // Main loop

    // Ensure socket close by using signals too
    LOG_INFO("server socket: closing...");
    close(server_socket);
    LOG_INFO("server socket: closed.");

    return 0;
}

