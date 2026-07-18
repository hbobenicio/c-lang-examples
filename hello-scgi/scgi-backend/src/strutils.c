#include "strutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

int cstr_parse_size_t(char* cstr, size_t* out_value) {
    assert(cstr != NULL);
    assert(out_value != NULL);
    
    char* endptr = NULL;
    errno = 0;
    unsigned long value = strtoul(cstr, &endptr, 10);
    if (errno != 0) {
        int error_code = errno;
        const char* error_reason = strerror(error_code);
        fprintf(stderr, "error: strtoul() failed: [%d] %s\n", error_code, error_reason);
        return 1;
    }
    if (value >= ULONG_MAX) {
        fprintf(stderr, "error: strtoul(): failed: unsigned long overflow\n");
        return 1;
    }
    *out_value = value;
    return 0;
}
