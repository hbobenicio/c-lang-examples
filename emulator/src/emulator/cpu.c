#include "cpu.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <arpa/inet.h>

#include "utils/random.h"
#include "utils/bits.h"
#include "memory.h"
#include "opcode.h"
#include "disassembler.h"
#include "display.h"
#include "keyboard.h"
#include "timer.h"
#include "logging/logger.h"

// The logging tag (prefix) used by the logging functions for messages from this module
#define LOG_TAG "cpu"

#define FATAL_MSG_OPCODE_USING_VF_REGISTER "programs should not operate on VF register"

/**
 * @brief Fetches an Opcode from memory at the address where program counter (PC) points to.
 * 
 * @param cpu 
 * @return Word The fetched Opcode
 */
static Word fetch_opcode(struct cpu* cpu);

/**
 * @brief Advances the PC (Program Counter) register to point to the next instruction.
 * 
 * It considers the size of the Opcode to know how much bytes it needs to advance.
 * 
 * @param cpu 
 */
static void cpu_pc_advance(struct cpu* cpu);

/**
 * @brief Rewinds the PC (Program Counter) register to point to the previous instruction.
 * 
 * It considers the size of the Opcode to know how much bytes it needs to rewind.
 * This is useful for some instructions that "blocks" CPU processing (like those that use get_key()).
 * 
 * @param cpu 
 */
static void cpu_pc_advance_back(struct cpu* cpu);

/**
 * @brief Goes to the given address
 * 
 * @param cpu 
 * @param address A memory addreess where the CPU should go to.
 */
static void cpu_goto_address(struct cpu* cpu, Address address);

/**
 * @brief Pushes an address to the stack.
 * 
 * This is useful for instructions that perform a call to a subroutine that needs to return to the current instruction
 * 
 * @param cpu 
 * @param address The address that should be pushed
 */
static void cpu_stack_push(struct cpu* cpu, Address address);

/**
 * @brief Pops an address from the stack.
 * 
 * @param cpu 
 * @return Address The address poped from the stack where the CPU should return to
 */
static Address cpu_stack_pop(struct cpu* cpu);

/**
 * @brief Decodes and executes the given Opcode
 * 
 * @param cpu 
 * @param opcode The OpCode fetched from memory at the program counter address
 */
static void cpu_decode_and_exec_opcode(struct cpu* cpu, Word opcode);

/**
 * @defgroup cpu-impl-exec-opcode-group The Opcode implementation functions group
 * The group of functions that implements the executions of all opcodes
 */

/**
 * @brief Clears the display.
 * 
 * @param cpu 
 * @param opcode 00E0 - CLS
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_display_clear(struct cpu* cpu, Word opcode);

/**
 * @brief Return from a subroutine.
 *
 * The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer.
 * 
 * @param cpu 
 * @param opcode 00EE - RET
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_return(struct cpu* cpu, Word opcode);

/**
 * @brief Jumps to a machine code routine at nnn.
 *
 * This instruction is only used on the old computers on which Chip-8 was originally implemented.
 * It is ignored by modern interpreters.
 * 
 * @param cpu 
 * @param opcode 0nnn - SYS addr
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_call(struct cpu* cpu, Word opcode);

/**
 * @brief Jumps to location NNN (goto).
 *
 * The interpreter sets the program counter to NNN.
 * 
 * @param cpu 
 * @param opcode 1nnn - JP addr
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_goto(struct cpu* cpu, Word opcode);

/**
 * @brief Calls the subroutine at address NNN.
 * 
 * The interpreter increments the stack pointer, then puts the current PC on the top of the stack.
 * The PC is then set to nnn.
 * 
 * @param cpu 
 * @param opcode 2nnn - CALL addr
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_call_subroutine(struct cpu* cpu, Word opcode);

/**
 * @brief Skip next instruction if Vx = kk.
 *
 * The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
 * 
 * @param cpu 
 * @param opcode 3xkk - SE Vx, byte
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_if_equals(struct cpu* cpu, Word opcode);

/**
 * @brief Skip next instruction if Vx != kk.
 *
 * The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
 * 
 * @param cpu 
 * @param opcode 4xkk - SNE Vx, byte
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_if_not_equals(struct cpu* cpu, Word opcode);

/**
 * @brief Skip next instruction if Vx = Vy.
 * 
 * The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
 * 
 * @param cpu 
 * @param opcode 5xy0 - SE Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_skip_next_if_vx_equals_vy(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = kk.
 *
 * The interpreter puts the value kk into register Vx.
 * 
 * @param cpu 
 * @param opcode 6xkk - LD Vx, byte
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_register(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vx + kk.
 *
 * Adds the value kk to the value of register Vx, then stores the result in Vx. 
 * 
 * @param cpu 
 * @param opcode 7xkk - ADD Vx, byte
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_add_to_vx(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vy.
 *
 * Stores the value of register Vy in register Vx.
 * 
 * @param cpu 
 * @param opcode 8xy0 - LD Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_from_vy(struct cpu* cpu, Word opcode);

/**
 * @brief Set Vx = Vx OR Vy.
 *
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
 * A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
 * then the same bit in the result is also 1. Otherwise, it is 0. 
 * 
 * @param cpu 
 * @param opcode 8xy1 - OR Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_to_vx_or(struct cpu* cpu, Word opcode);

/**
 * @brief Set Vx = Vx AND Vy.
 *
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
 * A bitwise AND compares the corrseponding bits from two values, and if both bits are 1,
 * then the same bit in the result is also 1. Otherwise, it is 0. 
 * 
 * @param cpu 
 * @param opcode 8xy2 - AND Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_to_vx_and(struct cpu* cpu, Word opcode);

/**
 * @brief Set Vx = Vx XOR Vy.
 * 
 * Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
 * An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same,
 * then the corresponding bit in the result is set to 1. Otherwise, it is 0.
 * 
 * @param cpu 
 * @param opcode 8xy3 - XOR Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_to_vx_xor(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vx + Vy, set VF = carry.
 * 
 * The values of Vx and Vy are added together.
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 * 
 * @param cpu 
 * @param opcode 8xy4 - ADD Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_vx_plus_vy(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vx - Vy, set VF = NOT borrow.
 *
 * If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
 * 
 * @param cpu 
 * @param opcode 8xy5 - SUB Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_vx_minus_vy(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vx SHR 1.
 *
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
 * 
 * @param cpu 
 * @param opcode 8xy6 - SHR Vx {, Vy}
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_rshift_one(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vy - Vx, set VF = NOT borrow.
 *
 * If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
 * 
 * @param cpu 
 * @param opcode 8xy7 - SUBN Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_vy_minus_vx(struct cpu* cpu, Word opcode);

/**
 * @brief Sets Vx = Vx SHL 1.
 *
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
 * 
 * @param cpu 
 * @param opcode 8xyE - SHL Vx {, Vy}
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_lshift_one(struct cpu* cpu, Word opcode);

/**
 * @brief Skips next instruction if Vx != Vy.
 *
 * The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
 * 
 * @param cpu 
 * @param opcode 9xy0 - SNE Vx, Vy
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_if_vx_not_equals_to_vy(struct cpu* cpu, Word opcode);

/**
 * @brief Sets I = nnn.
 *
 * The value of register I is set to nnn.
 * 
 * @param cpu 
 * @param opcode Annn - LD I, addr
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_i_register(struct cpu* cpu, Word opcode);

/**
 * @brief Jumps to location nnn + V0.
 *
 * The program counter is set to nnn plus the value of V0.
 * 
 * @param cpu 
 * @param opcode Bnnn - JP V0, addr
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_jump_to_addr_plus_v0(struct cpu* cpu, Word opcode);

/**
 * @brief Set Vx = random byte AND kk.
 *
 * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
 * The results are stored in Vx. See instruction 8xy2 for more information on AND.
 * 
 * @param cpu 
 * @param opcode Cxkk - RND Vx, byte
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_rand_and(struct cpu* cpu, Word opcode);

/**
 * @brief Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 *
 * The interpreter reads n bytes from memory, starting at the address stored in I.
 * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen.
 * If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the display,
 * it wraps around to the opposite side of the screen.
 * 
 * @param cpu 
 * @param opcode Dxyn - DRW Vx, Vy, nibble
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_display_draw_sprite_at(struct cpu* cpu, Word opcode);

/**
 * @brief Skip next instruction if key with the value of Vx is pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position,
 * PC is increased by 2.
 * 
 * @param cpu 
 * @param opcode Ex9E - SKP Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_if_key_equals_to_vx(struct cpu* cpu, Word opcode);

/**
 * @brief Skip next instruction if key with the value of Vx is not pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
 * 
 * @param cpu 
 * @param opcode ExA1 - SKNP Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_if_key_not_equals_to_vx(struct cpu* cpu, Word opcode);

/**
 * @brief Set Vx = delay timer value.
 *
 * The value of DT is placed into Vx.
 * 
 * @param cpu 
 * @param opcode Fx07 - LD Vx, DT
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_from_delay_timer(struct cpu* cpu, Word opcode);

/**
 * @brief Wait for a key press, store the value of the key in Vx.
 *
 * All execution stops until a key is pressed, then the value of that key is stored in Vx.
 * 
 * @param cpu 
 * @param opcode Fx0A - LD Vx, K
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_vx_from_key(struct cpu* cpu, Word opcode);

/**
 * @brief Set delay timer = Vx.
 *
 * DT is set equal to the value of Vx.
 * 
 * @param cpu 
 * @param opcode Fx15 - LD DT, Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_delay_timer_from_vx(struct cpu* cpu, Word opcode);

/**
 * @brief Set sound timer = Vx.
 *
 * ST is set equal to the value of Vx.
 * 
 * @param cpu 
 * @param opcode Fx18 - LD ST, Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_sound_timer(struct cpu* cpu, Word opcode);

/**
 * @brief Set I = I + Vx.
 *
 * The values of I and Vx are added, and the results are stored in I.
 * 
 * @param cpu 
 * @param opcode Fx1E - ADD I, Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_add_vx_to_i(struct cpu* cpu, Word opcode);

/**
 * @brief Set I = location of sprite for digit Vx.
 *
 * The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
 * 
 * @param cpu 
 * @param opcode Fx29 - LD F, Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_i_to_sprite_addr(struct cpu* cpu, Word opcode);

/**
 * @brief Store BCD representation of Vx in memory locations I, I+1, and I+2.
 *
 * The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
 * the tens digit at location I+1, and the ones digit at location I+2.
 * 
 * @param cpu 
 * @param opcode Fx33 - LD B, Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_set_bcd(struct cpu* cpu, Word opcode);

/**
 * @brief Store registers V0 through Vx in memory starting at location I.
 *
 * The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
 * 
 * @param cpu 
 * @param opcode Fx55 - LD [I], Vx
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_reg_dump(struct cpu* cpu, Word opcode);

/**
 * @brief Read registers V0 through Vx from memory starting at location I.
 * 
 * The interpreter reads values from memory starting at location I into registers V0 through Vx.
 * 
 * @param cpu 
 * @param opcode Fx65 - LD Vx, [I]
 * @ingroup cpu-impl-exec-opcode-group
 */
static void exec_reg_load(struct cpu* cpu, Word opcode);

void cpu_init(struct cpu* cpu, uint8_t* memory, struct display* display, struct timer* delay_timer, struct timer* sound_timer)
{
    cpu->memory = memory;
    cpu->display = display;
    cpu->delay_timer = delay_timer;
    cpu->sound_timer = sound_timer;

    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->stack, 0, sizeof(cpu->stack));
    cpu->pc = MEMORY_PROGRAM_STARTING_ADDRESS;
    cpu->i = MEMORY_PROGRAM_STARTING_ADDRESS; //TODO is this correct? Shouldn't it start at zero?
    cpu->stack_count = 0;
}

void cpu_step(struct cpu* cpu)
{
    //NOTE this is just temporary until we find a better way to debug execution...
    struct disassembler disassembler = { .file = stderr };

    if (cpu->pc >= MEMORY_SIZE) {
        fprintf(stderr, "error: program counter (PC) overflow\n");
        exit(1);
    }

    // Fetch
    Word opcode = fetch_opcode(cpu);
    fprintf(stderr, ADDRESS_FMT ": OPCODE[" OPCODE_FMT "]: ", cpu->pc, opcode);
    assert(opcode != 0);

    cpu_pc_advance(cpu);

    disassembler_disassemble(&disassembler, opcode);

    // Decode and Execute
    cpu_decode_and_exec_opcode(cpu, opcode);
}

static void cpu_decode_and_exec_opcode(struct cpu* cpu, Word opcode)
{
    if (opcode == 0x00E0)            { exec_display_clear(cpu, opcode); return; }
    if (opcode == 0x00EE)            { exec_return(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x0000) { exec_call(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x1000) { exec_goto(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x2000) { exec_call_subroutine(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x3000) { exec_if_equals(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0x4000) { exec_if_not_equals(cpu, opcode); return; }
    if ((opcode & 0xF00F) == 0x5000) { exec_skip_next_if_vx_equals_vy(cpu, opcode); return; }
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
    if ((opcode & 0xF00F) == 0x9000) { exec_if_vx_not_equals_to_vy(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0xA000) { exec_set_i_register(cpu, opcode); return; }
    if ((opcode & 0xF000) == 0xB000) { exec_jump_to_addr_plus_v0(cpu, opcode); return; }
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

    log_fatalf("unsupported opcode. opcode=" OPCODE_FMT, opcode);
}

static Word fetch_opcode(struct cpu* cpu)
{
    // Chip-8 instructions are stored as 16-bit unsigned integers
    // It uses big-endian, so the most significant bytes are stored first (lower address) and
    // less significant bytes are stored last (higher address).
    // So we get here a word (2 bytes) and OR them in order to get the opcode from memory bytes
    Word opcode = (cpu->memory[cpu->pc] << 8) | (cpu->memory[cpu->pc + 1]);

    return opcode;
}

static void cpu_pc_advance(struct cpu* cpu)
{
    cpu->pc += sizeof(Word);
}

static void cpu_pc_advance_back(struct cpu* cpu)
{
    cpu->pc -= sizeof(Word);
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

static void exec_display_clear(struct cpu* cpu, Word opcode)
{
    (void) opcode;
    display_clear(cpu->display);
}

static void exec_return(struct cpu* cpu, Word opcode)
{
    (void) opcode;

    Address ret_addr = cpu_stack_pop(cpu);
    cpu_goto_address(cpu, ret_addr);
}

static void exec_call(struct cpu* cpu, Word opcode)
{
    log_warn("obsolete instruction NATIVE CALL");
    Address address = opcode_decode_address(opcode);
    
    cpu_stack_push(cpu, cpu->pc);
    cpu_goto_address(cpu, address);
}

static void exec_goto(struct cpu* cpu, Word opcode)
{
    Address address = opcode_decode_address(opcode);
    cpu_goto_address(cpu, address);
}

static void exec_call_subroutine(struct cpu* cpu, Word opcode)
{
    Address address = opcode_decode_address(opcode);
    cpu_stack_push(cpu, cpu->pc);
    cpu_goto_address(cpu, address);
}

static void exec_if_equals(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    if (cpu->registers[x] == value) {
        cpu_pc_advance(cpu);
    }
}

static void exec_if_not_equals(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    if (cpu->registers[x] != value) {
        cpu_pc_advance(cpu);
    }
}

static void exec_skip_next_if_vx_equals_vy(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    if (cpu->registers[x] == cpu->registers[y]) {
        cpu_pc_advance(cpu);
    }
}

static void exec_set_register(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    cpu->registers[x] = value;
}

static void exec_add_to_vx(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    cpu->registers[x] += value;
}

static void exec_set_vx_from_vy(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] = cpu->registers[y];
}

static void exec_set_vx_to_vx_or(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] |= cpu->registers[y];
}

static void exec_set_vx_to_vx_and(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] &= cpu->registers[y];
}

static void exec_set_vx_to_vx_xor(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] ^= cpu->registers[y];
}

static void exec_vx_plus_vy(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    if (x == VF || y == VF) {
        log_fatal(FATAL_MSG_OPCODE_USING_VF_REGISTER);
    }

    size_t sum = cpu->registers[x] + cpu->registers[y];

    // if the sum overflows, then set the carry flag in the VF register. Otherwise clears it
    if (sum > REGISTER_MAX) {
        cpu->registers[VF] = 0x01;
    } else {
        cpu->registers[VF] = 0x00;
    }

    cpu->registers[x] = (Register) sum;
}

static void exec_vx_minus_vy(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    if (x == VF || y == VF) {
        log_fatal(FATAL_MSG_OPCODE_USING_VF_REGISTER);
    }

    if (cpu->registers[x] > cpu->registers[y]) {
        cpu->registers[VF] = 0x01;
    } else {
        cpu->registers[VF] = 0x00;
    }

    cpu->registers[x] -= cpu->registers[y];
}

static void exec_set_vx_rshift_one(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    if (x == VF) {
        log_fatal(FATAL_MSG_OPCODE_USING_VF_REGISTER);
    }

    if (bit_is_set(cpu->registers[x], 0)) {
        cpu->registers[VF] = 0x01;
    } else {
        cpu->registers[VF] = 0x00;
    }

    cpu->registers[x] >>= 1;
}

static void exec_set_vx_vy_minus_vx(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    if (x == VF || y == VF) {
        log_fatal(FATAL_MSG_OPCODE_USING_VF_REGISTER);
    }

    if (cpu->registers[y] > cpu->registers[x]) {
        cpu->registers[VF] = 0x01;
    } else {
        cpu->registers[VF] = 0x00;
    }

    cpu->registers[x] = cpu->registers[y] - cpu->registers[x];
}

static void exec_set_vx_lshift_one(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    if (x == VF) {
        log_fatal(FATAL_MSG_OPCODE_USING_VF_REGISTER);
    }

    static_assert(sizeof(Register) == 1, "implementation depends that register size equals 1");
    if (bit_is_set(cpu->registers[x], 7)) {
        cpu->registers[VF] = 0x01;
    } else {
        cpu->registers[VF] = 0x00;
    }

    cpu->registers[x] <<= 1;
}

static void exec_if_vx_not_equals_to_vy(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    if (cpu->registers[x] != cpu->registers[y]) {
        cpu_pc_advance(cpu);
    }
}

static void exec_set_i_register(struct cpu* cpu, Word opcode)
{
    Address address = opcode_decode_address(opcode);

    cpu->i = address;
}

static void exec_jump_to_addr_plus_v0(struct cpu* cpu, Word opcode)
{
    Address address = opcode_decode_address(opcode);
    cpu_goto_address(cpu, address + cpu->registers[V0]);
}

static void exec_set_vx_rand_and(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);
    
    cpu->registers[x] = random_u8() & value;
}

static void exec_display_draw_sprite_at(struct cpu* cpu, Word opcode)
{
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    //
    // The interpreter reads n bytes from memory, starting at the address stored in I.
    //
    // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
    // Sprites are XORed onto the existing screen.
    //
    // If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
    //
    // If the sprite is positioned so part of it is outside the coordinates of the display,
    // it wraps around to the opposite side of the screen.
    //
    // See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more
    // information on the Chip-8 screen and sprites.
    //
    // These sprites are 5 bytes long, or 8 (bits of each byte) x 5 (height, byte count) pixels
    //
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);
    Const value = opcode_decode_const_4bit(opcode);

    // You can retrieve the color/value of a specific pixel in an SDL_Surface by using the SDL_GetRGB
    // https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
    //
    // https://wiki.libsdl.org/SDL2/SDL_CreateRGBSurface

    bool pixel_erased = display_draw_sprite(
        cpu->display,
        cpu->registers[x],
        cpu->registers[y],
        &cpu->memory[cpu->i],
        value
    );
    cpu->registers[VF] = pixel_erased ? 1 : 0;
}

static void exec_if_key_equals_to_vx(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    if (keyboard_is_key_pressed(cpu->registers[x])) {
        cpu_pc_advance(cpu);
    }
}

static void exec_if_key_not_equals_to_vx(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    if (!keyboard_is_key_pressed(cpu->registers[x])) {
        cpu_pc_advance(cpu);
    }
}

static void exec_set_vx_from_delay_timer(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->registers[x] = timer_get_value(cpu->delay_timer);
}

static void exec_set_vx_from_key(struct cpu* cpu, Word opcode)
{
    // Wait for a key press, store the value of the key in Vx.
    // All execution stops until a key is pressed, then the value of that key is stored in Vx.
    //
    Register x = opcode_decode_register_x(opcode);
    KeyValue key;
    if (keyboard_get_pressed_key(&key)) {
        cpu->registers[x] = key;
    } else {
        cpu_pc_advance_back(cpu);
    }
}

static void exec_set_delay_timer_from_vx(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    timer_set_value(cpu->delay_timer, cpu->registers[x]);
}

static void exec_set_sound_timer(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    timer_set_value(cpu->sound_timer, cpu->registers[x]);
}

static void exec_add_vx_to_i(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->i += cpu->registers[x];
}

static void exec_set_i_to_sprite_addr(struct cpu* cpu, Word opcode)
{
    // Set I = location of sprite for digit Vx.
    // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
    // See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    Register x = opcode_decode_register_x(opcode);
    Const sprite_value = cpu->registers[x];
    Address sprite_addr = memory_address_from_sprite_value(sprite_value);
    cpu->i = sprite_addr;
}

static void exec_set_bcd(struct cpu* cpu, Word opcode)
{
    // Store BCD representation of Vx in memory locations I, I+1, and I+2.
    // 
    // The interpreter takes the decimal value of Vx,
    // and places the hundreds digit in memory at location in I,
    // the tens digit at location I+1,
    // and the ones digit at location I+2.
    //
    // Vx = hex(0xFE) = decimal(254)
    //              +-> MEM[I  ] = 2
    //              +-> MEM[I+1] = 5
    //              +-> MEM[I+2] = 4
    Register x = opcode_decode_register_x(opcode);
    Register hundreds = (cpu->registers[x] % 1000) / 100;  // 2
    Register tens     = (cpu->registers[x] %  100) /  10;  // 5
    Register ones     = (cpu->registers[x] %   10) /   1;  // 4
    cpu->memory[cpu->i    ] = hundreds;
    cpu->memory[cpu->i + 1] = tens;
    cpu->memory[cpu->i + 2] = ones;
}

static void exec_reg_dump(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    Address addr = cpu->i;
    for (Register i = 0; i <= x; i++, addr++) {
        cpu->memory[addr] = cpu->registers[i];
    }
}

static void exec_reg_load(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    Address addr = cpu->i;
    for (Register i = 0; i <= x; i++, addr++) {
        cpu->registers[i] = cpu->memory[addr];
    }
}
