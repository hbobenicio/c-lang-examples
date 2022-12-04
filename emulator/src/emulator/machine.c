#include "machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <arpa/inet.h>

#include "utils/fs.h"
#include "commons/buffer.h"
#include "opcode.h"
#include "disassembler.h"

#define PROGRAM_MAX_SIZE (MEMORY_SIZE - MEMORY_PROGRAM_STARTING_ADDRESS)

static OpCode* machine_first_opcode(struct machine* m);

void machine_init(struct machine* m)
{
    memory_init(m->memory);
    cpu_init(&m->cpu, m->memory);
    display_init(&m->display);
}

void machine_free(struct machine* m)
{
    display_free(&m->display);
}

void machine_load_rom(struct machine* m, const char* rom_file_path)
{
    struct buffer rom = file_read_contents(rom_file_path);

    // zero program memory area
    memset(&m->memory[MEMORY_PROGRAM_STARTING_ADDRESS], 0, PROGRAM_MAX_SIZE);

    assert(rom.capacity < PROGRAM_MAX_SIZE);
    memcpy(&m->memory[MEMORY_PROGRAM_STARTING_ADDRESS], rom.data, rom.capacity);

    // swap bytes order from big-endian to little-endian
    for (
        OpCode *opcode = machine_first_opcode(m);
        *opcode != 0 && ((uint8_t*)opcode) < (&m->memory[MEMORY_PROGRAM_STARTING_ADDRESS]) + PROGRAM_MAX_SIZE;
        opcode++
    ) {
        *opcode = ntohs(*opcode);
    }

    buffer_free(&rom);
}

void machine_disassemble(struct machine* m, FILE* file)
{
    struct disassembler disassembler = {
        .file = file,
    };

    bool unsupported_opcode_found = false;
    Address opcode_address = MEMORY_PROGRAM_STARTING_ADDRESS;
    for (
        OpCode *opcode = machine_first_opcode(m);
        *opcode != 0 && opcode_address < MEMORY_SIZE;
        opcode++, opcode_address += sizeof(*opcode)
    ) {
        fprintf(file, ADDRESS_FMT ": ", opcode_address);
        if (!disassembler_disassemble(&disassembler, *opcode)) {
            unsupported_opcode_found = true;
            break;
        }
    }

    if (unsupported_opcode_found) {
        fprintf(stderr, "error: some unsupported opcodes were found. disassembly failed.\n");
        exit(1);
    }
    
    fprintf(stderr, "info: machine: disassembling finished successfuly\n");
}

void machine_run(struct machine* m)
{
    cpu_run(&m->cpu);
}

static OpCode* machine_first_opcode(struct machine* m)
{
    return (OpCode*) &m->memory[MEMORY_PROGRAM_STARTING_ADDRESS];
}
