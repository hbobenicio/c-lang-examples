#include <string.h>
#include <assert.h>
#include <errno.h>

#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define DYNAMIC_ARRAY_IMPLEMENTATION
#include "dynamic-array.h"

#include "error.h"
#include "str.h"
#include "io.h"
#include "stopwatch.h"
#include "net.h"
#include "http.h"
#include "config.h"
#include "http/server.h"

#include "log.h"
#define LOG_NAME "main"

int main(void)
{
    struct stopwatch elapsed_time;
    stopwatch_start(&elapsed_time);

    LOG_INFO("initializing...");

    if (config_init_from_env() != RESULT_OK) {
        LOG_ERROR("bad config");
        return 1;
    }

    const struct config* config = config_get();

    struct http_server server;

    // this will among other things...
    // 1. create the server socket
    // 2. bind it to the configured address (host+port)
    // 3. listen using the configured tcp backlog
    if (http_server_init(&server, config) != RESULT_OK) {
        LOG_ERROR("server initialization failed");
        return 1;
    }

    socklen_t client_address_len = sizeof(struct sockaddr_in);

    stopwatch_stop(&elapsed_time);

    LOG_INFOF("server is ready. url=\"" ANSI_HWHT "http://%s:%hu" ANSI_RESET "\" elapsed_time=%zuus",
              config->server_host,
              config->server_port,
              stopwatch_get_us(&elapsed_time));

    // the main server loop
    while (true) {
        struct sockaddr_in client_address = {0};

        // 4. accept
        int client_socket = accept(server.socket, (struct sockaddr*) &client_address, &client_address_len);
        if (client_socket < 0) {
            int error_code = errno;
            LOG_ERRORF("socket accept failed: %s", strerror(error_code));
            goto err;
        }

        LOG_DEBUG("new client connected.");

        // Read all http headers with a single recv call... it's not enough (aka \r\n\r\n not found) then
        // we respond BadRequest to it.

        ssize_t bytes_read;
        uint8_t unsafe_buffer[4096] = {0};
        const size_t unsafe_buffer_size = sizeof(unsafe_buffer);

        //NOTE this returns the raw unsafe data read from the client socket so it can contain `\0` and other bad bytes...
        if (recv_or_timeout(client_socket,
                            unsafe_buffer, unsafe_buffer_size,
                            config->read_timeout_ms,
                            &bytes_read) != RESULT_OK)
        {
            LOG_ERROR("failed to poll/read from the client connection");
            goto end_of_request;
        }
        if (bytes_read <= 0) {
            LOG_WARN("client disconnected or did not send http headers within read timeout");
            goto end_of_request;
        }

        LOG_DEBUGF("%ld bytes read", bytes_read);

        // Validate/sanitize unsafe input date here...

        size_t end_of_headers_offset = 0;
        if (http_headers_validate(unsafe_buffer, bytes_read, &end_of_headers_offset) != RESULT_OK) {
            //TODO write back bad request maybe?
            LOG_WARN("bad request headers");
            goto end_of_request;
        }

        //TODO implement http header parsing...

        LOG_DEBUG("parsed headers:");
        fwrite(unsafe_buffer, sizeof(uint8_t), end_of_headers_offset, stdout);
        fputc('\n', stdout);

        // Route and dispatch (each handler will judge if it makes sense to do more socket reading or not)
        
        //TODO implement router/routing/handling
        
        // No route matched. 404 it here...
        
        //TODO encapsulate/abstract sending static strings...
        const char response_body[] = "Server is under development... please be patient\n";
        const size_t response_body_len = sizeof(response_body) - 1;

        char response_buffer[4098] = {0};
        int bytes_printed = snprintf(response_buffer, sizeof(response_buffer),
                          "HTTP/1.1 %d Not Implemented\r\n"
                          "Server: http-server/0.0.0\r\n"
                          "Content-Length: %zu\r\n"
                          "Content-Type: text/plain; charset=utf-8\r\n"
                          "Connection: closed\r\n"
                          "\r\n"
                          "%s",
                          501,
                          response_body_len,
                          response_body
                          );
        assert(bytes_printed > 0);

        LOG_DEBUGF("sending %d bytes...", bytes_printed);
        fwrite(response_buffer, sizeof(char), bytes_printed, stdout);

        ssize_t bytes_sent = send(client_socket, response_buffer, bytes_printed, MSG_DONTWAIT);
        if (bytes_sent < 0) {
            int error_code = errno;
            LOG_ERRORF("failed to send full response (%d bytes): %s", bytes_printed, strerror(error_code));
            goto end_of_request;
        }
        if (bytes_sent < bytes_printed) {
            int error_code = errno;
            LOG_WARNF("failed to send full response (%d bytes): just sent %ld bytes: %s",
                      bytes_printed,
                      bytes_sent,
                      strerror(error_code));
            goto end_of_request;
        }

end_of_request:
        LOG_DEBUG("client socket: closing...");
        close(client_socket);
        LOG_DEBUG("client socket: closed.");
    }

    // Ensure socket close by using signals
    http_server_free(&server);
    return 0;

err:
    http_server_free(&server);
    return 1;
}

