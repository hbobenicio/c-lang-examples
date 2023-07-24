#pragma once

#include "str.h"

struct scanner {
    struct str_slice input;
};

int scanner_next_until_char(struct scanner* s, char c, struct str_slice* out);
int scanner_next_until_slice(struct scanner* s, struct str_slice pattern, struct str_slice* out);
int scanner_next_until_end(struct scanner* s, struct str_slice* out);

