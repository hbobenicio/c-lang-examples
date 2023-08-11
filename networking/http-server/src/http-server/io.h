#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "error.h"

enum result recv_or_timeout(int fd, uint8_t* buffer, size_t buffer_size, size_t timeout_ms, ssize_t* out_bytes_read);

