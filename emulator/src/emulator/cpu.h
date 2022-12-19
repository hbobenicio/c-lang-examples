#pragma once

#include <stddef.h>
#include <stdint.h>

#include "memory.h"
#include "register.h"

#define STACK_CAPACITY 12

struct display;
struct timer;

struct cpu {
    //NOTE until we check if it's worth implementing bus and/or a memory data structure, we have this...
    uint8_t* memory;
    struct display* display;
    struct timer* delay_timer;
    struct timer* sound_timer;

    /**
     * @brief General Purpose Registers (GPR's). The spec calls this 'V' (V0 to VF)
     */
    uint8_t registers[REGISTER_COUNT];
    /**
     * @brief The Program Counter register used to hold the next program instruction to be executed
     */
    Address pc;
    /**
     * @brief The 16-bit Address register, used to hold memory address values
     */
    Address i;
    /**
     * @brief The stack register used to keep the state of the call stack..
     */
    Address stack[STACK_CAPACITY];
    size_t stack_count;
};

void cpu_init(struct cpu* cpu, uint8_t* memory, struct display* display, struct timer* delay_timer, struct timer* sound_timer);

/**
 * @brief Executes a single instruction.
 */
void cpu_step(struct cpu* cpu);
