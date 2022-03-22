#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

void* panic_if_null(void* ptr)
{
    if (ptr == NULL) {
        fputs("error: aqls: parser: out of memory", stderr);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

StringView file_read_to_string_view_or_panic(const char* file_path)
{
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        int error_code = errno;
        fprintf(stderr, "error: io: fopen failed: %s. file_path=\"%s\"\n", strerror(error_code), file_path);
        exit(EXIT_FAILURE);
    }

    int rc = fseek(file, 0L, SEEK_END);
    if (rc != 0) {
        int error_code = errno;
        fprintf(stderr, "error: io: fseek failed: %s. file_path=\"%s\"\n", strerror(error_code), file_path);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        int error_code = errno;
        fprintf(stderr, "error: io: ftell failed: %s. file_path=\"%s\"\n", strerror(error_code), file_path);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    size_t content_len = (size_t) file_size;
    size_t content_capacity = content_len + 1;

    char* content = calloc(1, content_capacity);
    if (content == NULL) {
        fprintf(stderr, "error: out of memory. file_path=\"%s\"\n", file_path);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    rc = fseek(file, 0, SEEK_SET);
    if (rc != 0) {
        int error_code = errno;
        fprintf(stderr, "error: io: fseek failed: %s. file_path=\"%s\"\n", strerror(error_code), file_path);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = fread(content, sizeof(char), content_len, file);
    if (bytes_read != content_len) {
        int error_code = errno;
        fprintf(stderr, "error: io: fread: not every byte from file could be read: %s. bytes_read=%zu file_path=\"%s\"\n", strerror(error_code), bytes_read, file_path);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    content[content_len] = '\0';

    fclose(file);

    return (StringView) {
        .str = content,
        .len = content_len,
    };
}

char* file_read_or_panic(const char* file_path)
{
    StringView content = file_read_to_string_view_or_panic(file_path);
    return (char*) content.str;
}
