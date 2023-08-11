#include "commons.h"

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

enum result parse_ull(const char* input, unsigned long long* output)
{
    assert(input != NULL);
    assert(output != NULL);

    char* endptr = NULL;
    errno = 0;
    unsigned long long value = strtoull(input, &endptr, 10);
    if (errno != 0 || value == ULLONG_MAX) {
        return RESULT_ERR;
    }

    *output = value;
    return RESULT_OK;
}

void regex_must_compile(regex_t* regex, const char* pattern, int flags)
{
    int rc = regcomp(regex, pattern, flags);
    if (rc != 0) {
        // When we call regerror with 0, 0 as last parameters, it returns the error msg size
        // (already considering the '\0')
        const size_t error_msg_size = regerror(rc, regex, 0, 0);
        char* error_msg = calloc(error_msg_size, sizeof(char));
        if (!error_msg) {
            fprintf(stderr, "error: regex: compilation failed. pattern=\"%s\"\n", pattern);
            exit(EXIT_FAILURE);
        }

        // We call it once again, now with the error_msg buffer to be set
        regerror(rc, regex, error_msg, error_msg_size);

        fprintf(stderr, "error: regex: compilation failed: %s. pattern=\"%s\"\n", error_msg, pattern);
        
        free(error_msg);
        exit(EXIT_FAILURE);
    }
}

