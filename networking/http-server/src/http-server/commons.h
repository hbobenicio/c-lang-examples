#pragma once

#include <regex.h>
#include "error.h"

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#define ARRAY_SIZE(XS) (sizeof(XS) / sizeof(XS[0]))

enum result parse_ull(const char* input, unsigned long long* output);
void regex_must_compile(regex_t* regex, const char* pattern, int flags);

