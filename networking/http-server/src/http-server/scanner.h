#pragma once

#include "str.h"
#include "error.h"

struct scanner {
    struct str_slice input;
};

enum result scanner_next_until_char(struct scanner* s, char c, struct str_slice* out);
enum result scanner_next_until_slice(struct scanner* s, struct str_slice pattern, struct str_slice* out);
enum result scanner_next_until_end(struct scanner* s, struct str_slice* out);

