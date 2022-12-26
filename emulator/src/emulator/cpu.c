#include "cpu.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <arpa/inet.h>

#include "utils/random.h"
#include "memory.h"
#include "opcode.h"
#include "disassembler.h"
#include "display.h"
#include "keyboard.h"
#include "timer.h"
#include "logging/logger.h"

#define LOG_TAG "cpu"

// #define UNIMPLEMENTED() \
//     log_fatalf("TODO: unimplemented function %s at:\n>>> %s:%d", __func__, __FILE__, __LINE__)

static Word fetch_opcode(struct cpu* cpu);
static void cpu_pc_advance(struct cpu* cpu);
static void cpu_pc_advance_back(struct cpu* cpu);
static void cpu_goto_address(struct cpu* cpu, Address address);
static void cpu_stack_push(struct cpu* cpu, Address address);
static Address cpu_stack_pop(struct cpu* cpu);
/**
 * @brief Decodes and executes the given Opcode
 * @param cpu 
 * @param opcode 
 */
static void cpu_decode_and_exec_opcode(struct cpu* cpu, Word opcode);

static void exec_display_clear(struct cpu* cpu, Word opcode);
static void exec_return(struct cpu* cpu, Word opcode);
static void exec_call(struct cpu* cpu, Word opcode);
static void exec_goto(struct cpu* cpu, Word opcode);
static void exec_call_subroutine(struct cpu* cpu, Word opcode);
static void exec_if_equals(struct cpu* cpu, Word opcode);
static void exec_if_not_equals(struct cpu* cpu, Word opcode);
static void exec_skip_next_if_vx_equals_vy(struct cpu* cpu, Word opcode);
static void exec_set_register(struct cpu* cpu, Word opcode);
static void exec_add_to_vx(struct cpu* cpu, Word opcode);
static void exec_set_vx_from_vy(struct cpu* cpu, Word opcode);
static void exec_set_vx_to_vx_or(struct cpu* cpu, Word opcode);
static void exec_set_vx_to_vx_and(struct cpu* cpu, Word opcode);
static void exec_set_vx_to_vx_xor(struct cpu* cpu, Word opcode);
static void exec_vx_plus_vy(struct cpu* cpu, Word opcode);
static void exec_vx_minus_vy(struct cpu* cpu, Word opcode);
static void exec_set_vx_rshift_one(struct cpu* cpu, Word opcode);
static void exec_set_vx_vy_minus_vx(struct cpu* cpu, Word opcode);
static void exec_set_vx_lshift_one(struct cpu* cpu, Word opcode);
static void exec_if_vx_not_equals_to_vy(struct cpu* cpu, Word opcode);
static void exec_set_i_register(struct cpu* cpu, Word opcode);
static void exec_jump_to_addr_plus_v0(struct cpu* cpu, Word opcode);
static void exec_set_vx_rand_and(struct cpu* cpu, Word opcode);
static void exec_display_draw_sprite_at(struct cpu* cpu, Word opcode);
static void exec_if_key_equals_to_vx(struct cpu* cpu, Word opcode);
static void exec_if_key_not_equals_to_vx(struct cpu* cpu, Word opcode);
static void exec_set_vx_from_delay_timer(struct cpu* cpu, Word opcode);
static void exec_set_vx_from_key(struct cpu* cpu, Word opcode);
static void exec_set_delay_timer_from_vx(struct cpu* cpu, Word opcode);
static void exec_set_sound_timer(struct cpu* cpu, Word opcode);
static void exec_add_vx_to_i(struct cpu* cpu, Word opcode);
static void exec_set_i_to_sprite_addr(struct cpu* cpu, Word opcode);
static void exec_set_bcd(struct cpu* cpu, Word opcode);
static void exec_reg_dump(struct cpu* cpu, Word opcode);
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
    //NOTE this is just temporarely until we find a better way to debug execution...
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

    fprintf(stderr, "error: cpu: unsupported opcode. opcode=" OPCODE_FMT "\n", opcode);
    exit(1);
}

static Word fetch_opcode(struct cpu* cpu)
{
    // return *((OpCode*) &cpu->memory[cpu->pc]);
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

    Address returning_address = cpu_stack_pop(cpu);
    cpu_goto_address(cpu, returning_address);
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
    // Skip next instruction if Vx = Vy.
    //
    // The interpreter compares register Vx to register Vy, and if they are equal,
    // increments the program counter by 2.
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

    cpu->registers[x] += cpu->registers[y];
}

static void exec_vx_minus_vy(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] -= cpu->registers[y];
}

static void exec_set_vx_rshift_one(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->registers[x] >>= 1;
}

static void exec_set_vx_vy_minus_vx(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    cpu->registers[x] = cpu->registers[y] - cpu->registers[x];
}

static void exec_set_vx_lshift_one(struct cpu* cpu, Word opcode)
{
    Register x = opcode_decode_register_x(opcode);

    cpu->registers[x] <<= 1;
}

static void exec_if_vx_not_equals_to_vy(struct cpu* cpu, Word opcode)
{
    // Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block); 
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
    // The interpreter reads n bytes from memory, starting at the address stored in I. [OK]
    //
    // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
    // Sprites are XORed onto the existing screen.
    //
    // If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
    //
    // If the sprite is positioned so part of it is outside the coordinates of the display,
    // it wraps around to the opposite side of the screen. [OK]
    //
    // See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more
    // information on the Chip-8 screen and sprites. [OK]
    //
    // These sprites are 5 bytes long, or 8 (bits of each byte) x 5 (height, byte count) pixels
    //
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);
    Const value = opcode_decode_const_4bit(opcode);
    // fprintf(stream, "draw(" REGISTER_FMT ", " REGISTER_FMT ", " CONST_FMT ")\n", x, y, value);
    // draw(V0, V1, 0x4)
    //
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SDL_RenderDrawPoint(renderer, i, i);
    //
    // You can retrieve the color/value of a specific pixel in an SDL_Surface by using the SDL_GetRGB
    // https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
    //
    // https://wiki.libsdl.org/SDL2/SDL_CreateRGBSurface

    // fprintf(stderr, "debug: draw(" REGISTER_FMT "=0x%X, " REGISTER_FMT "=0x%X, [", x, cpu->registers[x], y, cpu->registers[y]);
    // for (uint8_t i = 0; i < value; i++) {
    //     if (i > 0) fprintf(stderr, ", ");
    //     fprintf(stderr, "0x%X", cpu->memory[cpu->i + i]);
    // }
    // fprintf(stderr, "])\n");

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
