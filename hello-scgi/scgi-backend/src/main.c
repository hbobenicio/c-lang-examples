#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "mem.h"
#include "strutils.h"
#include "net.h"
#include "scgi.h"

void connection_handle(struct scgi_context* ctx);

int main(void)
{
    // Configuration
    const char* server_host = "0.0.0.0";
    uint16_t server_port = 8081;
    int server_tcp_backlog = 16;

    // socket + setsockopt + bind + listen
    int server_socket_fd = tcp_socket_create(server_host, server_port, server_tcp_backlog);
    if (server_socket_fd == -1) {
        return 1;
    }

    // Main server loop
    while (true)
    {
        struct sockaddr_in client_address = {0};
        socklen_t client_address_len = sizeof(client_address);

        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, (socklen_t *) &client_address_len);
        if (client_socket_fd == -1) {
            perror("error: accept failed");
            continue;
        }

        fprintf(stderr, "info: new client connection accepted\n");

        unsigned char request_buffer[SCGI_REQ_MAX_BUFFER_SIZE] = {0};
        struct scgi_context ctx = {
            .conn = {
                .client_socket_fd = client_socket_fd,
            },
            .allocator = fixed_buffer_allocator_from_buffer(request_buffer, ARRAY_SIZE(request_buffer)),
        };

        connection_handle(&ctx);

        fprintf(stderr, "debug: closing client connection\n");
        close(client_socket_fd);
    }

    close(server_socket_fd);

    return 0;
}

void connection_handle(struct scgi_context* ctx)
{
    assert(ctx != NULL);

    size_t digits_count = 0;
    const size_t max_digits = 9;
    char headers_size_digits[16] = {0};

    // read headers size digits until ':' is found
    while (true) {
        char c;
        ssize_t nread = recv_char(ctx->conn.client_socket_fd, &c);
        if (nread <= 0) {
            return;
        }
        fprintf(stderr, "debug: scgi: io: got '%c' (%d)\n", c, c);
        if (c == ':') {
            fprintf(stderr, "debug: scgi: io: done reading headers size digits\n");
            break;
        }
        if (!isdigit(c)) {
            fprintf(stderr, "error: scgi: parsing: non digit received before ':'. got=%d\n", c);
            return;
        }
        if (digits_count >= max_digits) {
            fprintf(stderr, "error: scgi: parsing: headers size digits overflow\n");
            return;
        }
        headers_size_digits[digits_count++] = c;
    }

    // parses headers size from char* to size_t
    size_t headers_size = 0;
    if (cstr_parse_size_t(headers_size_digits, &headers_size) != 0) {
        fprintf(stderr, "error: scgi: parsing: bad headers size digits\n");
        return;
    }
    fprintf(stderr, "debug: headers size: %zu\n", headers_size);
    if (headers_size >= SCGI_HEADERS_MAX_SIZE) {
        fprintf(stderr, "error: scgi: headers size is too big. headers_size=%zu max=%d\n", headers_size, SCGI_HEADERS_MAX_SIZE);
        return;
    }

    // try to read all the headers from the given headers size
    ssize_t bytes_read = recv(ctx->conn.client_socket_fd, ctx->req.input_buffer, headers_size, MSG_WAITALL);
    if (bytes_read < 0) {
        fprintf(stderr, "error: scgi: io: failed to read headers\n");
        return;
    }
    if ((size_t) bytes_read != headers_size) {
        fprintf(stderr,
            "error: scgi: io: bad headers size. expected=%zu read=%ld errno=%d reason=\"%s\"\n",
            headers_size, bytes_read, errno, strerror(errno));
        return;
    }
    fprintf(stderr, "debug: scgi: io: headers=\"");
    fwrite(ctx->req.input_buffer, headers_size, 1, stderr);
    fprintf(stderr, "\"\n");

    // read the ',' into the input_buffer (for the parser to know when headers are done)
    {
        char dummy;
        bytes_read = recv(ctx->conn.client_socket_fd, &dummy, sizeof(dummy), 0);
        if (bytes_read < 0) {
            fprintf(stderr, "error: scgi: io: failed to read headers\n");
            return;
        }
        if (dummy != ',') {
            fprintf(stderr, "error: scgi: parsing, expected=',' got=%d\n", dummy);
            return;
        }
        ctx->req.input_buffer[headers_size] = dummy;
    }

    struct scgi_parser parser = {0};
    scgi_parser_init(&parser, ctx->req.input_buffer, headers_size + 1 /* ',' */);
    int rc = scgi_parser_parse(&parser);
    fprintf(stderr, "info: scgi parser: parsing finished. rc=%d state=%d\n", rc, parser.state);

    // Send a simple string response
    int response_status_code = 200;
    const char* response_body = "Hello, World!\n";
    size_t response_body_len = strlen(response_body);
    if (scgi_send_text(&ctx->conn, response_status_code, response_body, response_body_len) != 0) {
        return;
    }
}

// #include <uuid/uuid.h>
// uuid_t binuuid;
// uuid_generate_random(binuuid);
// char uuid[UUID_STR_LEN] = {0};
// uuid_unparse_lower(binuuid, uuid);
// puts(uuid);
