#include "http.h"

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include <poll.h>
#include <sys/socket.h>

#include "config.h"
#include "scanner.h"

#include "log.h"
#define LOG_NAME "http"

static bool http_headers_is_valid_byte(uint8_t unsafe_byte);

enum result http_headers_validate(uint8_t* unsafe_cstr_buffer,
                                  size_t unsafe_cstr_buffer_size,
                                  size_t* out_end_of_headers_offset)
{
    assert(unsafe_cstr_buffer != NULL);
    assert(unsafe_cstr_buffer_size > 1);

    //not really sure what to do here ATM...
    // ideas...
    // - prevent '\0' before \r\n\r\n
    // - check if there is \r\n\r\n
    // - we cant really safely call memmem here because we dont know if the input has a bad \0 inside it
    //   (but maybe we could just check for no '\0' and then use memmem to check for end-of-request and its position...
    int state = 0;
    for (size_t i = 0; i < unsafe_cstr_buffer_size; i++) {
        uint8_t unsafe_byte = unsafe_cstr_buffer[i];
        // LOG_DEBUGF("req[%zu] = 0x%02X , state = %d", i, unsafe_byte, state);

        if (!http_headers_is_valid_byte(unsafe_byte)) {
            LOG_WARNF("bad byte in request header: 0x%02X", (int) unsafe_byte);
            return RESULT_ERR;
        }

        if (state == 0 && unsafe_byte == '\r') {
            state = 1;
            continue;
        }

        if (state == 1 && unsafe_byte == '\r') {
            continue;
        }

        if (state == 1 && unsafe_byte == '\n') {
            state = 2;
            continue;
        }

        if (state == 2 && unsafe_byte == '\r') {
            state = 3;
            continue;
        }

        if (state == 3 && unsafe_byte == '\n') {

            // remove the trailing \r\n which just separates headers from body.
            //NOTE remember to skip it when parsing the request body
            *out_end_of_headers_offset = i - 2;

            return RESULT_OK;
        }

        state = 0;
    }

    LOG_WARN("end of request headers (aka `\\r\\n\\r\\n`) was not found");
    return RESULT_ERR;
}

enum result http_request_poll_and_read(struct http_request* req)
{
    //TODO refactor this to use the function in io module

    const struct config* config = config_get();

    // we'll only poll the client socket for input events (dont forget we still writes the response to it)
    //TODO check if we also should implement write timeout...
    struct pollfd pollfd = {
        .fd = req->conn,
        .events = POLLIN,
    };

    //size_t total_bytes_read = 0;
    bool request_completed = false;
    bool first_chunk = true;

    // connection socket polling/reading loop
    while (true) {

        // General Algorithm:
        // 1. poll socket for data or timeout.
        //   1.1 if timed out, just close the socket and continue
        // 2. recv data.
        //   2.1 if data is 0, then client disconnected. just close the socket and continue
        // 3. 

        //TODO graceful shutdown check if we really want/need to block signals when polling.
        // Is blocking all signals for the current thread required(?) for POSIX poll, already encapsulated if using linux ppoll
        int poll_ready_count = poll(&pollfd, (nfds_t) 1, (int) config->read_timeout_ms);
        if (poll_ready_count < 0) {
            int error_code = errno;
            LOG_ERRORF("client socket: read polling failed: %s", strerror(error_code));
            return RESULT_ERR;
        }

        // timed out
        if (poll_ready_count == 0) {
            LOG_WARNF("client socket: read polling timeout after %zu ms.", config->read_timeout_ms);
            return RESULT_ERR;
        }

        // We are only checking POLLIN events here
        assert(pollfd.revents & POLLIN);

        uint8_t buffer[4096] = {0};

        //NOTE MSG_WAITALL may be problematic with Keep-Alive connections? (I don't know for sure...)
        ssize_t bytes_received = recv(req->conn, buffer, sizeof(buffer), MSG_DONTWAIT);
        if (bytes_received <= 0) {
            if (!request_completed) {
                LOG_WARN("client disconnected before finishing reading");
                return RESULT_ERR;
            }
            break;
        }
        // total_bytes_read += bytes_received;
        LOG_DEBUGF("read %ld bytes (max buffer size = %zu)", bytes_received, sizeof(buffer));

        //TODO init this with enough data so this never allocate for almost every case
        int rc = dynamic_array_push_back(&req->data, buffer, bytes_received);
        if (rc != 0) {
            return RESULT_ERR;
        }

        if (first_chunk) {
            first_chunk = false;

            // TODO parse headers for Content-Length (not reliable though). let's just require it for now...
            struct scanner scanner = {
                .input = str_slice_from_buffer((const char*) req->data.items, req->data.length),
            };

            struct str_slice line;
            rc = scanner_next_until_slice(&scanner, str_slice_from_cstr_trusted("\r\n"), &line);
            if (rc != 0) {
                LOG_WARN("request has no more lines");
                return RESULT_ERR;
            }
            //TODO add this to the API skip the pattern
            scanner.input.len += sizeof("\r\n") - 1;
            str_slice_println(line, stdout);

            //TODO parse lines for headers, get content-length and maybe parse the body if any
        }
    }

    // debug Dumping request to stdout
    fwrite(req->data.items, sizeof(uint8_t), req->data.length, stdout);

    return RESULT_OK;
}

enum result http_method_parse(struct str_slice input, struct str_slice* out)
{
    assert(input.ptr != NULL);
    assert(out != NULL);

    if (input.len > HTTP_REQUEST_METHOD_MAX_SIZE) {
        return RESULT_ERR;
    }

    if (memcmp(input.ptr, "CONNECT", sizeof("CONNECT") - 1) == 0) {
        *out = (struct str_slice) {
            .ptr = input.ptr,
            .len = sizeof("CONNECT") - 1,
        };
        return RESULT_OK;
    }

    return RESULT_ERR;
}

static bool http_headers_is_valid_byte(uint8_t unsafe_byte)
{
    if (unsafe_byte == '\0') {
        return false;
    }

    // only 2 ascii control chars are permitted in request headers: \r and \n
    if (iscntrl((int) unsafe_byte) && unsafe_byte != '\r' && unsafe_byte != '\n') {
        return false;
    }

    return true;
}

