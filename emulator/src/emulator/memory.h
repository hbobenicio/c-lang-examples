#pragma once

#include <stdint.h>
#include <limits.h>
#include <assert.h>

// 4KB Memory
#define MEMORY_SIZE (4 * 1024)

// Address where programs should be loaded
#define MEMORY_PROGRAM_STARTING_ADDRESS 0x0200

typedef uint16_t Address;
static_assert(MEMORY_SIZE <= UINT16_MAX, "Address type must be big enough to represent all possible memory values");

#define ADDRESS_FMT "0x%X"

// The uppermost 256 bytes (0xF00-0xFFF) are reserved for display refresh,
// and the 96 bytes below that (0xEA0-0xEFF) were reserved for the call stack,
// internal use, and other variables
