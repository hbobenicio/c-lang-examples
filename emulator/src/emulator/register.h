#pragma once

#include <limits.h>
#include <assert.h>

enum {
    V0 = 0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    VA,
    VB,
    VC,
    VD,
    VE,
    VF,
    REGISTER_COUNT
};

typedef uint8_t Register;
static_assert(REGISTER_COUNT <= UINT8_MAX, "Register type must be big enough to represent all possible register values");

#define REGISTER_FMT "V%hu"
