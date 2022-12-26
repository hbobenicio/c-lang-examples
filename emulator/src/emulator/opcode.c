#include "opcode.h"

#include "logging/logger.h"

#define LOG_TAG "opcode"

Address opcode_decode_address(Word opcode)
{
    return opcode & 0x0FFF;
}

Register opcode_decode_register_x(Word opcode)
{
    Register reg = (opcode & 0x0F00) >> (2 * 4);
    return reg;
}

Register opcode_decode_register_y(Word opcode)
{
    Register reg = (opcode & 0x00F0) >> (1 * 4);
    return reg;
}

Const opcode_decode_const_8bit(Word opcode)
{
    return opcode & 0x00FF;
}

Const opcode_decode_const_4bit(Word opcode)
{
    return opcode & 0x000F;
}
