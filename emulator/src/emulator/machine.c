#include "machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "utils/fs.h"
#include "commons/buffer.h"
#include "opcode.h"
#include "disassembler.h"

#define PROGRAM_MAX_SIZE (MEMORY_SIZE - MEMORY_PROGRAM_STARTING_ADDRESS)

#define UNIMPLEMENTED() \
    do { \
        fprintf(stderr, "unimplemented: %s\n", __func__); \
        exit(1); \
    } while (false);

static OpCode* machine_first_opcode(struct machine* m);
static OpCode machine_current_opcode(struct machine* m);
static void machine_run_step(struct machine* m);

static void run_display_clear(struct machine* m, OpCode opcode);
static void run_return(struct machine* m, OpCode opcode);
static void run_call(struct machine* m, OpCode opcode);
static void run_goto(struct machine* m, OpCode opcode);
static void run_call_subroutine(struct machine* m, OpCode opcode);
static void run_if_equals(struct machine* m, OpCode opcode);
static void run_if_not_equals(struct machine* m, OpCode opcode);
static void run_set_register(struct machine* m, OpCode opcode);
static void run_add_to_vx(struct machine* m, OpCode opcode);
static void run_set_vx_from_vy(struct machine* m, OpCode opcode);
static void run_set_vx_to_vx_or(struct machine* m, OpCode opcode);
static void run_set_vx_to_vx_and(struct machine* m, OpCode opcode);
static void run_set_vx_to_vx_xor(struct machine* m, OpCode opcode);
static void run_vx_plus_vy(struct machine* m, OpCode opcode);
static void run_vx_minus_vy(struct machine* m, OpCode opcode);
static void run_set_vx_rshift_one(struct machine* m, OpCode opcode);
static void run_set_vx_vy_minus_vx(struct machine* m, OpCode opcode);
static void run_set_vx_lshift_one(struct machine* m, OpCode opcode);
static void run_set_i_register(struct machine* m, OpCode opcode);
static void run_set_vx_rand_and(struct machine* m, OpCode opcode);
static void run_display_draw_sprite_at(struct machine* m, OpCode opcode);
static void run_if_key_equals_to_vx(struct machine* m, OpCode opcode);
static void run_if_key_not_equals_to_vx(struct machine* m, OpCode opcode);
static void run_set_vx_from_delay_timer(struct machine* m, OpCode opcode);
static void run_set_vx_from_key(struct machine* m, OpCode opcode);
static void run_set_delay_timer_from_vx(struct machine* m, OpCode opcode);
static void run_set_sound_timer(struct machine* m, OpCode opcode);
static void run_add_vx_to_i(struct machine* m, OpCode opcode);
static void run_set_i_to_sprite_addr(struct machine* m, OpCode opcode);
static void run_set_bcd(struct machine* m, OpCode opcode);
static void run_reg_dump(struct machine* m, OpCode opcode);
static void run_reg_load(struct machine* m, OpCode opcode);

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
}

void machine_run(struct machine* m)
{
    while (m->pc < MEMORY_SIZE && machine_current_opcode(m) != 0) {
        machine_run_step(m);
        m->pc++;
    }
}

static OpCode* machine_first_opcode(struct machine* m)
{
    return (OpCode*) &m->memory[MEMORY_PROGRAM_STARTING_ADDRESS];
}

static OpCode machine_current_opcode(struct machine* m)
{
    return *((OpCode*) &m->memory[m->pc]);
}

static void machine_run_step(struct machine* m)
{
    assert(m->pc < MEMORY_SIZE);
    OpCode opcode = machine_current_opcode(m);

    if (opcode == 0x00E0)            { run_display_clear(m, opcode); return; }
    if (opcode == 0x00EE)            { run_return(m, opcode); return; }
    if ((opcode & 0xF000) == 0x0000) { run_call(m, opcode); return; }
    if ((opcode & 0xF000) == 0x1000) { run_goto(m, opcode); return; }
    if ((opcode & 0xF000) == 0x2000) { run_call_subroutine(m, opcode); return; }
    if ((opcode & 0xF000) == 0x3000) { run_if_equals(m, opcode); return; }
    if ((opcode & 0xF000) == 0x4000) { run_if_not_equals(m, opcode); return; }
    if ((opcode & 0xF000) == 0x6000) { run_set_register(m, opcode); return; }
    if ((opcode & 0xF000) == 0x7000) { run_add_to_vx(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8000) { run_set_vx_from_vy(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8001) { run_set_vx_to_vx_or(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8002) { run_set_vx_to_vx_and(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8003) { run_set_vx_to_vx_xor(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8004) { run_vx_plus_vy(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8005) { run_vx_minus_vy(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8006) { run_set_vx_rshift_one(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x8007) { run_set_vx_vy_minus_vx(m, opcode); return; }
    if ((opcode & 0xF00F) == 0x800E) { run_set_vx_lshift_one(m, opcode); return; }
    if ((opcode & 0xF000) == 0xA000) { run_set_i_register(m, opcode); return; }
    if ((opcode & 0xF000) == 0xC000) { run_set_vx_rand_and(m, opcode); return; }
    if ((opcode & 0xF000) == 0xD000) { run_display_draw_sprite_at(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xE09E) { run_if_key_equals_to_vx(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xE0A1) { run_if_key_not_equals_to_vx(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF007) { run_set_vx_from_delay_timer(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF00A) { run_set_vx_from_key(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF015) { run_set_delay_timer_from_vx(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF018) { run_set_sound_timer(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF01E) { run_add_vx_to_i(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF029) { run_set_i_to_sprite_addr(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF033) { run_set_bcd(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF055) { run_reg_dump(m, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF065) { run_reg_load(m, opcode); return; }

    fprintf(stderr, "error: unsupported opcode. opcode=" OPCODE_FMT "\n", opcode);
}

static void run_display_clear(struct machine* m, OpCode opcode)
{
    (void) opcode;
    UNIMPLEMENTED();
}

static void run_return(struct machine* m, OpCode opcode)
{
    (void) opcode;
    // fprintf(stream, "Return from a subroutine\n");
    UNIMPLEMENTED();
}

static void run_call(struct machine* m, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    // fprintf(stream, "Call machine code at address " ADDRESS_FMT "\n", address);
    UNIMPLEMENTED();
}

static void run_goto(struct machine* m, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    // fprintf(stream, "goto " ADDRESS_FMT "\n", address);
    UNIMPLEMENTED();
}

static void run_call_subroutine(struct machine* m, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    // fprintf(stream, "Call subroutine at address " ADDRESS_FMT "\n", address);
    UNIMPLEMENTED();
}

static void run_if_equals(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    // fprintf(stream, "if (" REGISTER_FMT " == " CONST_FMT ")\n", x, value);
    UNIMPLEMENTED();
}

static void run_if_not_equals(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    // fprintf(stream, "if (" REGISTER_FMT " != " CONST_FMT ")\n", x, value);
    UNIMPLEMENTED();
}

static void run_set_register(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    m->registers[x] = value;
}

static void run_add_to_vx(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    m->registers[x] += value;
}

static void run_set_vx_from_vy(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] = m->registers[y];
}

static void run_set_vx_to_vx_or(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] |= m->registers[y];
}

static void run_set_vx_to_vx_and(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] &= m->registers[y];
}

static void run_set_vx_to_vx_xor(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] ^= m->registers[y];
}

static void run_vx_plus_vy(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] += m->registers[y];
}

static void run_vx_minus_vy(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] -= m->registers[y];
}

static void run_set_vx_rshift_one(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    m->registers[x] >>= 1;
}

static void run_set_vx_vy_minus_vx(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    m->registers[x] = m->registers[y] - m->registers[x];
}

static void run_set_vx_lshift_one(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    m->registers[x] <<= 1;
}


static void run_set_i_register(struct machine* m, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    m->i = address;
}

static void run_set_vx_rand_and(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    // fprintf(stream, REGISTER_FMT " = rand() & " CONST_FMT "\n", x, value);
    UNIMPLEMENTED();
}

static void run_display_draw_sprite_at(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);
    Const value = opcode_decode_const_4bit(opcode);

    // fprintf(stream, "draw(" REGISTER_FMT ", " REGISTER_FMT ", " CONST_FMT ")\n", x, y, value);
    UNIMPLEMENTED();
}

static void run_if_key_equals_to_vx(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "if (key() == " REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}

static void run_if_key_not_equals_to_vx(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "if (key() != " REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}

static void run_set_vx_from_delay_timer(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, REGISTER_FMT " = get_delay()\n", x);
    UNIMPLEMENTED();
}

static void run_set_vx_from_key(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, REGISTER_FMT " = get_key()\n", x);
    UNIMPLEMENTED();
}

static void run_set_delay_timer_from_vx(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "delay_timer(" REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}

static void run_set_sound_timer(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "sound_timer(" REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}
static void run_add_vx_to_i(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    m->i += m->registers[x];
}
static void run_set_i_to_sprite_addr(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "I = sprite_addr[" REGISTER_FMT "]\n", x);
    UNIMPLEMENTED();
}
static void run_set_bcd(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "set_bcd(" REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}
static void run_reg_dump(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "reg_dump(" REGISTER_FMT ", &I)", x);
    UNIMPLEMENTED();
}
static void run_reg_load(struct machine* m, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    // fprintf(stream, "reg_load(" REGISTER_FMT ", &I)", x);
    UNIMPLEMENTED();
}
