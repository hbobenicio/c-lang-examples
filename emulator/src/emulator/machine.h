#pragma once

#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "register.h"
#include "display.h"

struct machine {
    uint8_t memory[MEMORY_SIZE];

    struct display display;

    //TODO Create CPU data structure and move registers and computation to there

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
void machine_free(struct machine* m);

void machine_load_rom(struct machine* m, const char* rom_file_path);
void machine_disassemble(struct machine* m, FILE* file);
void machine_run(struct machine* m);
