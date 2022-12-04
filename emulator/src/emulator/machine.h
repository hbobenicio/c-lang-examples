#pragma once

#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "register.h"
#include "cpu.h"
#include "display.h"

#define STACK_CAPACITY 12

struct machine {
    uint8_t memory[MEMORY_SIZE];
    struct cpu cpu;
    struct display display;
};

void machine_init(struct machine* m);
void machine_free(struct machine* m);

void machine_load_rom(struct machine* m, const char* rom_file_path);
void machine_disassemble(struct machine* m, FILE* file);
void machine_run(struct machine* m);
