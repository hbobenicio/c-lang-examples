#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef uint16_t OpCode;
#define OPCODE_FMT "0x%X"

bool opcode_disassemble(OpCode opcode, FILE* stream);
