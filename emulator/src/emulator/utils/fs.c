#include "fs.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <emulator/logging/logger.h>

#define LOG_TAG "fs"

struct buffer file_read_contents(const char* file_path)
{
    log_debugf("opening file '%s'...", file_path);
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        log_errorf("failed to open file '%s': %s", file_path, strerror(errno));
        return buffer_empty();
    }

    log_debug("calculating file size...");

    int rc = fseek(file, 0, SEEK_END);
    if (rc != 0) {
        log_errorf("failed to seek to the end of file '%s': %s", file_path, strerror(errno));
        fclose(file);
        exit(1);
    }

    size_t file_size = (size_t) ftell(file);
    if (file_size == 0) {
        log_warnf("file '%s' is empty", file_path);
        fclose(file);
        return buffer_empty();
    }

    rewind(file);

    log_debugf("file '%s' has size = %zu", file_path, file_size);

    struct buffer buffer = buffer_new(file_size);

    size_t bytes_read = fread(buffer.data, sizeof(buffer.data[0]), file_size, file);
    if (bytes_read != file_size) {
        log_errorf(
            "failed to read all bytes from file '%s'. file_size=%zu bytes_read=%zu",
            file_path, file_size, bytes_read
        );
        fclose(file);
        exit(1);
    }
    
    fclose(file);

    return buffer;
}
