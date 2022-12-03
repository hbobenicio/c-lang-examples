#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "opcode.h"

struct disassembler {
    FILE* file;
};

bool disassembler_disassemble(struct disassembler* d, OpCode opcode);
