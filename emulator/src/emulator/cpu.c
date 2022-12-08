#include "cpu.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils/random.h"
#include "memory.h"
#include "opcode.h"
#include "disassembler.h"
#include "display.h"
#include "keyboard.h"

#define UNIMPLEMENTED() \
    do { \
        fprintf(stderr, "TODO: unimplemented function %s at:\n  %s:%d\n", __func__, __FILE__, __LINE__); \
        exit(1); \
    } while (false);

static OpCode fetch_opcode(struct cpu* cpu);
static void cpu_pc_advance(struct cpu* cpu);
static void cpu_goto_address(struct cpu* cpu, Address address);
static void cpu_stack_push(struct cpu* cpu, Address address);
static Address cpu_stack_pop(struct cpu* cpu);
/**
 * @brief Decodes and executes the given Opcode
 * @param cpu 
 * @param opcode 
 */
static void cpu_decode_and_exec_opcode(struct cpu* cpu, OpCode opcode);

static void exec_display_clear(struct cpu* cpu, OpCode opcode);
static void exec_return(struct cpu* cpu, OpCode opcode);
static void exec_call(struct cpu* cpu, OpCode opcode);
static void exec_goto(struct cpu* cpu, OpCode opcode);
static void exec_call_subroutine(struct cpu* cpu, OpCode opcode);
static void exec_if_equals(struct cpu* cpu, OpCode opcode);
static void exec_if_not_equals(struct cpu* cpu, OpCode opcode);
static void exec_set_register(struct cpu* cpu, OpCode opcode);
static void exec_add_to_vx(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_from_vy(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_to_vx_or(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_to_vx_and(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_to_vx_xor(struct cpu* cpu, OpCode opcode);
static void exec_vx_plus_vy(struct cpu* cpu, OpCode opcode);
static void exec_vx_minus_vy(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_rshift_one(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_vy_minus_vx(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_lshift_one(struct cpu* cpu, OpCode opcode);
static void exec_set_i_register(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_rand_and(struct cpu* cpu, OpCode opcode);
static void exec_display_draw_sprite_at(struct cpu* cpu, OpCode opcode);
static void exec_if_key_equals_to_vx(struct cpu* cpu, OpCode opcode);
static void exec_if_key_not_equals_to_vx(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_from_delay_timer(struct cpu* cpu, OpCode opcode);
static void exec_set_vx_from_key(struct cpu* cpu, OpCode opcode);
static void exec_set_delay_timer_from_vx(struct cpu* cpu, OpCode opcode);
static void exec_set_sound_timer(struct cpu* cpu, OpCode opcode);
static void exec_add_vx_to_i(struct cpu* cpu, OpCode opcode);
static void exec_set_i_to_sprite_addr(struct cpu* cpu, OpCode opcode);
static void exec_set_bcd(struct cpu* cpu, OpCode opcode);
static void exec_reg_dump(struct cpu* cpu, OpCode opcode);
static void exec_reg_load(struct cpu* cpu, OpCode opcode);

void cpu_init(struct cpu* cpu, uint8_t* memory, struct display* display)
{
    cpu->memory = memory;
    cpu->display = display;

    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->stack, 0, sizeof(cpu->stack));
    cpu->pc = MEMORY_PROGRAM_STARTING_ADDRESS;
    cpu->i = MEMORY_PROGRAM_STARTING_ADDRESS;
    cpu->stack_count = 0;
}

void cpu_run(struct cpu* cpu)
{
    // assert(cpu->pc == MEMORY_PROGRAM_STARTING_ADDRESS);

    //NOTE this is just temporarely until we find a better way to debug execution...
    struct disassembler disassembler = { .file = stderr };

    // while (true) {
        if (cpu->pc >= MEMORY_SIZE) {
            fprintf(stderr, "error: program counter (PC) overflow\n");
            exit(1);
        }

        // Fetch
        OpCode opcode = fetch_opcode(cpu);
        assert(opcode != 0);

        cpu_pc_advance(cpu);

        disassembler_disassemble(&disassembler, opcode);

        // Decode and Execute
        cpu_decode_and_exec_opcode(cpu, opcode);
    // }
}

static void cpu_decode_and_exec_opcode(struct cpu* cpu, OpCode opcode)
{
    if (opcode == 0x00E0)            { exec_display_clear(cpu, opcode); return; }
    if (opcode == 0x00EE)            { exec_return(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x0000) { exec_call(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x1000) { exec_goto(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x2000) { exec_call_subroutine(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x3000) { exec_if_equals(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x4000) { exec_if_not_equals(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x6000) { exec_set_register(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x7000) { exec_add_to_vx(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8000) { exec_set_vx_from_vy(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8001) { exec_set_vx_to_vx_or(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8002) { exec_set_vx_to_vx_and(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8003) { exec_set_vx_to_vx_xor(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8004) { exec_vx_plus_vy(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8005) { exec_vx_minus_vy(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8006) { exec_set_vx_rshift_one(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x8007) { exec_set_vx_vy_minus_vx(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x800E) { exec_set_vx_lshift_one(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0xA000) { exec_set_i_register(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0xC000) { exec_set_vx_rand_and(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0xD000) { exec_display_draw_sprite_at(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xE09E) { exec_if_key_equals_to_vx(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xE0A1) { exec_if_key_not_equals_to_vx(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF007) { exec_set_vx_from_delay_timer(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF00A) { exec_set_vx_from_key(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF015) { exec_set_delay_timer_from_vx(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF018) { exec_set_sound_timer(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF01E) { exec_add_vx_to_i(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF029) { exec_set_i_to_sprite_addr(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF033) { exec_set_bcd(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF055) { exec_reg_dump(cpu, opcode); return; }
    if ((opcode & 0xF0FF) == 0xF065) { exec_reg_load(cpu, opcode); return; }

    fprintf(stderr, "error: cpu: unsupported opcode. opcode=" OPCODE_FMT "\n", opcode);
}

static OpCode fetch_opcode(struct cpu* cpu)
{
    return *((OpCode*) &cpu->memory[cpu->pc]);
}

static void cpu_pc_advance(struct cpu* cpu)
{
    cpu->pc += sizeof(OpCode);
}

static void cpu_goto_address(struct cpu* cpu, Address address)
{
    cpu->pc = address;
}

static void cpu_stack_push(struct cpu* cpu, Address address)
{
    if (cpu->stack_count >= STACK_CAPACITY) {
        fprintf(stderr, "error: cpu: runtime: stack overflow\n");
        exit(1);
    }
    cpu->stack[cpu->stack_count] = address;
    cpu->stack_count++;
}

static Address cpu_stack_pop(struct cpu* cpu)
{
    if (cpu->stack_count == 0) {
        fprintf(stderr, "error: cpu: runtime: stack underflow\n");
        exit(1);
    }
    cpu->stack_count--;
    return cpu->stack[cpu->stack_count];
}

static void exec_display_clear(struct cpu* cpu, OpCode opcode)
{
    (void) opcode;
    display_clear(cpu->display);
}

static void exec_return(struct cpu* cpu, OpCode opcode)
{
    (void) opcode;

    Address returning_address = cpu_stack_pop(cpu);
    cpu_goto_address(cpu, returning_address);
}

static void exec_call(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Address address = opcode_decode_address(opcode);
    (void) address;

    // fprintf(stream, "Call machine code at address " ADDRESS_FMT "\n", address);
    UNIMPLEMENTED();
}

static void exec_goto(struct cpu* cpu, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);
    cpu_goto_address(cpu, address);
}

static void exec_call_subroutine(struct cpu* cpu, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);
    cpu_stack_push(cpu, cpu->pc);
    cpu_goto_address(cpu, address);
}

static void exec_if_equals(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    if (cpu->registers[x] == value) {
        cpu_pc_advance(cpu);
    }
}

static void exec_if_not_equals(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    if (cpu->registers[x] != value) {
        cpu_pc_advance(cpu);
    }
}

static void exec_set_register(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    cpu->registers[x] = value;
}

static void exec_add_to_vx(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    cpu->registers[x] += value;
}

static void exec_set_vx_from_vy(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] = cpu->registers[y];
}

static void exec_set_vx_to_vx_or(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] |= cpu->registers[y];
}

static void exec_set_vx_to_vx_and(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] &= cpu->registers[y];
}

static void exec_set_vx_to_vx_xor(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] ^= cpu->registers[y];
}

static void exec_vx_plus_vy(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] += cpu->registers[y];
}

static void exec_vx_minus_vy(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] -= cpu->registers[y];
}

static void exec_set_vx_rshift_one(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->registers[x] >>= 1;
}

static void exec_set_vx_vy_minus_vx(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] = cpu->registers[y] - cpu->registers[x];
}

static void exec_set_vx_lshift_one(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->registers[x] <<= 1;
}


static void exec_set_i_register(struct cpu* cpu, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    cpu->i = address;
}

static void exec_set_vx_rand_and(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);
    
    cpu->registers[x] = random_u8() & value;
}

static void exec_display_draw_sprite_at(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);
    Const value = opcode_decode_const_4bit(opcode);
    (void) x;
    (void) y;
    (void) value;

    // fprintf(stream, "draw(" REGISTER_FMT ", " REGISTER_FMT ", " CONST_FMT ")\n", x, y, value);
    UNIMPLEMENTED();
}

static void exec_if_key_equals_to_vx(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    if (keyboard_is_key_pressed(cpu->registers[x])) {
        cpu_pc_advance(cpu);
    }
}

static void exec_if_key_not_equals_to_vx(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    if (!keyboard_is_key_pressed(cpu->registers[x])) {
        cpu_pc_advance(cpu);
    }
}

static void exec_set_vx_from_delay_timer(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, REGISTER_FMT " = get_delay()\n", x);
    UNIMPLEMENTED();
}

static void exec_set_vx_from_key(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, REGISTER_FMT " = get_key()\n", x);
    UNIMPLEMENTED();
}

static void exec_set_delay_timer_from_vx(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, "delay_timer(" REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}

static void exec_set_sound_timer(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, "sound_timer(" REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}

static void exec_add_vx_to_i(struct cpu* cpu, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->i += cpu->registers[x];
}

static void exec_set_i_to_sprite_addr(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, "I = sprite_addr[" REGISTER_FMT "]\n", x);
    UNIMPLEMENTED();
}

static void exec_set_bcd(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, "set_bcd(" REGISTER_FMT ")\n", x);
    UNIMPLEMENTED();
}

static void exec_reg_dump(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, "reg_dump(" REGISTER_FMT ", &I)", x);
    UNIMPLEMENTED();
}

static void exec_reg_load(struct cpu* cpu, OpCode opcode)
{
    (void) cpu;
    Register x = opcode_decode_register_x(opcode);
    (void) x;

    // fprintf(stream, "reg_load(" REGISTER_FMT ", &I)", x);
    UNIMPLEMENTED();
}
