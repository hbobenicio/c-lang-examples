#include "io.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>


#include "log.h"
#define LOG_NAME "io"

enum result recv_or_timeout(int fd, uint8_t* buffer, size_t buffer_size, size_t timeout_ms, ssize_t* out_bytes_read)
{
    struct pollfd pollfd = { .fd = fd, .events = POLLIN };

    int poll_ready_count = poll(&pollfd, (nfds_t) 1, (int) timeout_ms);
    if (poll_ready_count < 0) {
        int error_code = errno;
        LOG_ERRORF("recv polling failed: %s", strerror(error_code));
        return RESULT_ERR;
    }

    if (poll_ready_count == 0) {
        LOG_WARNF("read timeout triggered after %zu ms.", timeout_ms);
        *out_bytes_read = 0;
        return RESULT_OK;
    }

    // We are only checking POLLIN events here
    assert(pollfd.revents & POLLIN);

    memset(buffer, '\0', buffer_size);

    //NOTE -1 may not be necessary here but this makes it a null-terminated string for convenience
    assert(buffer_size > 1);
    *out_bytes_read = recv(fd, buffer, buffer_size - 1, MSG_DONTWAIT);

    return RESULT_OK;
}

