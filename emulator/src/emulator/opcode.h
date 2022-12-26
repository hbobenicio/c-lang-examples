#pragma once

#include <stdint.h>
#include <limits.h>

#include "memory.h"
#include "register.h"

typedef uint16_t Word;
#define OPCODE_FMT "0x%04X"
#define WORD_MAX UINT16_MAX

typedef uint8_t Const;
#define CONST_FMT "0x%02X"

Address opcode_decode_address(Word opcode);
Register opcode_decode_register_x(Word opcode);
Register opcode_decode_register_y(Word opcode);
Const opcode_decode_const_8bit(Word opcode);
Const opcode_decode_const_4bit(Word opcode);
