#include <stdio.h>
#include <stdlib.h>

#include "scgi.h"

int main(void) {
    const char scgi_request[] =
        "70:"
        "CONTENT_LENGTH\0"
        "13\0"
        "SCGI\0"
        "1\0"
        ","
        "Hello, world!";

    struct scgi_parser parser = {0};
    scgi_parser_init(&parser, scgi_request, sizeof(scgi_request));

    int rc = scgi_parser_parse(&parser);
    fprintf(stderr, "info: rc=%d state=%d\n", rc, parser.state);
    
    return rc;
}
