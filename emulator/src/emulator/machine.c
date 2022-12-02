#include "machine.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "utils/fs.h"
#include "commons/buffer.h"
#include "opcode.h"

#define PROGRAM_MAX_SIZE (MEMORY_SIZE - MEMORY_PROGRAM_STARTING_ADDRESS)

static OpCode* machine_first_opcode(struct machine* m);

void machine_init(struct machine* m)
{
    memset(m->memory, 0, sizeof(m->memory));
    memset(m->registers, 0, sizeof(m->registers));
    m->pc = MEMORY_PROGRAM_STARTING_ADDRESS;
    m->i = MEMORY_PROGRAM_STARTING_ADDRESS;
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

void machine_disassemble(struct machine* m, FILE* stream)
{
    bool unsupported_opcode_found = false;
    Address opcode_address = MEMORY_PROGRAM_STARTING_ADDRESS;
    for (
        OpCode *opcode = machine_first_opcode(m);
        *opcode != 0 && opcode_address < MEMORY_SIZE;
        opcode++, opcode_address += sizeof(*opcode)
    ) {
        fprintf(stream, ADDRESS_FMT ": ", opcode_address);
        if (!opcode_disassemble(*opcode, stream)) {
            unsupported_opcode_found = true;
            break;
        }
    }

    if (unsupported_opcode_found) {
        fprintf(stderr, "error: some unsupported opcodes were found. disassembly failed.\n");
        exit(1);
    }
}

static OpCode* machine_first_opcode(struct machine* m)
{
    return (OpCode*) &m->memory[MEMORY_PROGRAM_STARTING_ADDRESS];
}
