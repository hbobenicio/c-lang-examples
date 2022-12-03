#pragma once

#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "register.h"

struct machine {
    uint8_t memory[MEMORY_SIZE];
    uint8_t registers[REGISTER_COUNT];
    /**
     * @brief The Program Counter register used to hold the next program instruction to be executed
     */
    Address pc;
    /**
     * @brief 16-bit register used to hold memory address values
     */
    Address i;
};

void machine_init(struct machine* m);
#define machine_reset machine_init

void machine_load_rom(struct machine* m, const char* rom_file_path);
void machine_disassemble(struct machine* m, FILE* file);
void machine_run(struct machine* m);
