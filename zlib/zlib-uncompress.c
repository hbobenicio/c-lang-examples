#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include <zlib.h>

#define GZIP_MAGIC_NUMBER "\x1f\x8b"

struct buffer {
    unsigned char* data;
    size_t size;
};

struct buffer buffer_alloc(size_t size, bool zero_values)
{
    struct buffer buffer = {0};

    if (zero_values) {
        buffer.data = calloc(1, size);
    } else {
        buffer.data = malloc(size);
    }

    if (buffer.data == NULL) {
        fprintf(stderr, "error: allocation of %zu bytes failed: %s\n", size, strerror(errno));
        exit(1);
    }

    buffer.size = size;

    return buffer;
}

void buffer_free(struct buffer* buffer)
{
    assert(buffer != NULL);
    assert(buffer->data != NULL);
    assert(buffer->size > 0);

    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
}

struct uncompressing_params {
    const char* input_file_path;
    const char* output_file_path;
    size_t buffer_size;
};

void zlib_print_version(void);

int hbmo_uncompress(const struct uncompressing_params* params);

int main(int argc, char** argv)
{
    zlib_print_version();

    for (size_t i = 1; i < argc; i++) {
        const char* arg = argv[i];
        size_t arg_len = strlen(arg);
        puts(arg + arg_len - 1 - 2);
    }
    return 1;

    struct uncompressing_params params = {
        .input_file_path = "/tmp/FOO.gz",
        .output_file_path = "/tmp/FOO",
        .buffer_size = 4 * 1024,
    };

    if (hbmo_uncompress(&params) != 0) {
        fprintf(stderr, "error: uncompressing file failed.\n");
        return 1;
    }

    return 0;
}

void zlib_print_version(void)
{
    fprintf(stderr, "zlib header version: %s\n", ZLIB_VERSION);
    fprintf(stderr, "zlib    lib version: %s\n", zlibVersion());
}

int hbmo_uncompress(const struct uncompressing_params* params)
{
    if (params->buffer_size < 2) {
        fprintf(stderr, "error: buffer size is too low.\n");
        goto err;
    }

    struct buffer chunk = buffer_alloc(params->buffer_size, false);

    gzFile input_file = gzopen(params->input_file_path, "rb");
    if (input_file == NULL) {
        int err_code = errno;
        const char* err_cause = strerror(err_code);
        fprintf(
            stderr,
            "error: failed to open file \"%s\" (mode=\"rb\"): [%d] %s\n",
            params->input_file_path, err_code, err_cause
        );
        goto err_buffer_free;
    }

    FILE* output_file = fopen(params->output_file_path, "wb");
    if (output_file == NULL) {
        int err_code = errno;
        const char* err_cause = strerror(err_code);
        fprintf(
            stderr,
            "error: failed to open file \"%s\" (mode=\"wb\"): [%d] %s\n",
            params->input_file_path, err_code, err_cause
        );
        goto err_close_input_file;
    }

    while (true) {
        int rc = gzread(input_file, chunk.data, chunk.size);
        if (rc == -1) {
            fprintf(
                stderr,
                "error: failed to read from input file \"%s\": %s\n",
                params->input_file_path, gzerror(input_file, &rc)
            );
            goto err_close_output_file;
        }
        int uncompressed_bytes_read = rc;
        if (uncompressed_bytes_read == 0) {
            break;
        }

        size_t bytes_written = fwrite(chunk.data, sizeof(chunk.data[0]), uncompressed_bytes_read, output_file);
        if (ferror(output_file) || bytes_written == 0) {
            int err_code = errno;
            const char* err_cause = strerror(err_code);
            fprintf(
                stderr,
                "error: failed to write on output file \"%s\": [%d] %s\n",
                params->output_file_path, err_code, err_cause
            );
            goto err_close_output_file;
        }

        fprintf(stderr, "info: chunk uncompressed.\n");
    }

    fprintf(stderr, "info: done.\n");

    fclose(output_file);
    gzclose(input_file);
    buffer_free(&chunk);
    return 0;

err_close_output_file:
    //TODO this can optionally remove the corrupted file
    fclose(output_file);

err_close_input_file:
    gzclose(input_file);

err_buffer_free:
    buffer_free(&chunk);

err:
    return 1;
}

