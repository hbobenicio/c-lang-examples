#include "opcode.h"

Address opcode_decode_address(Word opcode)
{
    return opcode & 0x0FFF;
}

Register opcode_decode_register_x(Word opcode)
{
    return (opcode & 0x0F00) >> (2 * 4);
}

Register opcode_decode_register_y(Word opcode)
{
    return (opcode & 0x00F0) >> (1 * 4);
}

Const opcode_decode_const_8bit(Word opcode)
{
    return opcode & 0x00FF;
}

Const opcode_decode_const_4bit(Word opcode)
{
    return opcode & 0x000F;
}
