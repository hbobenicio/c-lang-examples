#include "fs.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

struct buffer file_read_contents(const char* file_path)
{
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "error: could not open file '%s': %s\n", file_path, strerror(errno));
        return buffer_empty();
    }

    assert(fseek(file, 0, SEEK_END) != -1);
    size_t file_size = (size_t) ftell(file);
    assert(file_size > 0);
    rewind(file);

    struct buffer buffer = buffer_new(file_size);

    assert(fread(buffer.data, sizeof(buffer.data[0]), file_size, file) == file_size);
    fclose(file);

    return buffer;
}
