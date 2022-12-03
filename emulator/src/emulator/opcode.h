#pragma once

#include <stdint.h>

#include "memory.h"
#include "register.h"

typedef uint16_t OpCode;
#define OPCODE_FMT "0x%X"

typedef uint8_t Const;
#define CONST_FMT "0x%X"

Address opcode_decode_address(OpCode opcode);
Register opcode_decode_register_x(OpCode opcode);
Register opcode_decode_register_y(OpCode opcode);
Const opcode_decode_const_8bit(OpCode opcode);
Const opcode_decode_const_4bit(OpCode opcode);
