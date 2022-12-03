#include "opcode.h"

#include <stdbool.h>

static void opcode_disassemble_display_clear(OpCode opcode, FILE* stream);
static void opcode_disassemble_return(OpCode opcode, FILE* stream);
static void opcode_disassemble_call(OpCode opcode, FILE* stream);
static void opcode_disassemble_goto(OpCode opcode, FILE* stream);
static void opcode_disassemble_call_subroutine(OpCode opcode, FILE* stream);
static void opcode_disassemble_if_equals(OpCode opcode, FILE* stream);
static void opcode_disassemble_if_not_equals(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_register(OpCode opcode, FILE* stream);
static void opcode_disassemble_add_to_vx(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_from_vy(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_to_vx_or(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_to_vx_and(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_to_vx_xor(OpCode opcode, FILE* stream);
static void opcode_disassemble_vx_plus_vy(OpCode opcode, FILE* stream);
static void opcode_disassemble_vx_minus_vy(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_rshift_one(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_vy_minus_vx(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_lshift_one(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_i_register(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_rand_and(OpCode opcode, FILE* stream);
static void opcode_disassemble_display_draw_sprite_at(OpCode opcode, FILE* stream);
static void opcode_disassemble_if_key_equals_to_vx(OpCode opcode, FILE* stream);
static void opcode_disassemble_if_key_not_equals_to_vx(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_from_delay_timer(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_vx_from_key(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_delay_timer_from_vx(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_sound_timer(OpCode opcode, FILE* stream);
static void opcode_disassemble_add_vx_to_i(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_i_to_sprite_addr(OpCode opcode, FILE* stream);
static void opcode_disassemble_set_bcd(OpCode opcode, FILE* stream);
static void opcode_disassemble_reg_dump(OpCode opcode, FILE* stream);
static void opcode_disassemble_reg_load(OpCode opcode, FILE* stream);

bool opcode_disassemble(OpCode opcode, FILE* stream)
{
    #define DISASSEMBLE_AND_RETURN(func) \
        do { \
            func(opcode, stream); \
            return true; \
        } while (false);

    if (opcode == 0x00E0)            DISASSEMBLE_AND_RETURN(opcode_disassemble_display_clear);
    if (opcode == 0x00EE)            DISASSEMBLE_AND_RETURN(opcode_disassemble_return);
    if ((opcode & 0xF000) == 0x0000) DISASSEMBLE_AND_RETURN(opcode_disassemble_call);
    if ((opcode & 0xF000) == 0x1000) DISASSEMBLE_AND_RETURN(opcode_disassemble_goto);
    if ((opcode & 0xF000) == 0x2000) DISASSEMBLE_AND_RETURN(opcode_disassemble_call_subroutine);
    if ((opcode & 0xF000) == 0x3000) DISASSEMBLE_AND_RETURN(opcode_disassemble_if_equals);
    if ((opcode & 0xF000) == 0x4000) DISASSEMBLE_AND_RETURN(opcode_disassemble_if_not_equals);
    if ((opcode & 0xF000) == 0x6000) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_register);
    if ((opcode & 0xF000) == 0x7000) DISASSEMBLE_AND_RETURN(opcode_disassemble_add_to_vx);
    if ((opcode & 0xF00F) == 0x8000) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_from_vy);
    if ((opcode & 0xF00F) == 0x8001) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_to_vx_or);
    if ((opcode & 0xF00F) == 0x8002) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_to_vx_and);
    if ((opcode & 0xF00F) == 0x8003) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_to_vx_xor);
    if ((opcode & 0xF00F) == 0x8004) DISASSEMBLE_AND_RETURN(opcode_disassemble_vx_plus_vy);
    if ((opcode & 0xF00F) == 0x8005) DISASSEMBLE_AND_RETURN(opcode_disassemble_vx_minus_vy);
    if ((opcode & 0xF00F) == 0x8006) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_rshift_one);
    if ((opcode & 0xF00F) == 0x8007) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_vy_minus_vx);
    if ((opcode & 0xF00F) == 0x800E) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_lshift_one);
    if ((opcode & 0xF000) == 0xA000) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_i_register);
    if ((opcode & 0xF000) == 0xC000) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_rand_and);
    if ((opcode & 0xF000) == 0xD000) DISASSEMBLE_AND_RETURN(opcode_disassemble_display_draw_sprite_at);
    if ((opcode & 0xF0FF) == 0xE09E) DISASSEMBLE_AND_RETURN(opcode_disassemble_if_key_equals_to_vx);
    if ((opcode & 0xF0FF) == 0xE0A1) DISASSEMBLE_AND_RETURN(opcode_disassemble_if_key_not_equals_to_vx);
    if ((opcode & 0xF0FF) == 0xF007) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_from_delay_timer);
    if ((opcode & 0xF0FF) == 0xF00A) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_vx_from_key);
    if ((opcode & 0xF0FF) == 0xF015) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_delay_timer_from_vx);
    if ((opcode & 0xF0FF) == 0xF018) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_sound_timer);
    if ((opcode & 0xF0FF) == 0xF01E) DISASSEMBLE_AND_RETURN(opcode_disassemble_add_vx_to_i);
    if ((opcode & 0xF0FF) == 0xF029) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_i_to_sprite_addr);
    if ((opcode & 0xF0FF) == 0xF033) DISASSEMBLE_AND_RETURN(opcode_disassemble_set_bcd);
    if ((opcode & 0xF0FF) == 0xF055) DISASSEMBLE_AND_RETURN(opcode_disassemble_reg_dump);
    if ((opcode & 0xF0FF) == 0xF065) DISASSEMBLE_AND_RETURN(opcode_disassemble_reg_load);

    fprintf(stream, "error: unsupported opcode. opcode=" OPCODE_FMT "\n", opcode);
    return false;
    
    #undef DISASSEMBLE_AND_RETURN
}

static void opcode_disassemble_display_clear(OpCode opcode, FILE* stream)
{
    (void) opcode;
    fprintf(stream, "Clear the screen\n");
}

static void opcode_disassemble_return(OpCode opcode, FILE* stream)
{
    (void) opcode;
    fprintf(stream, "Return from a subroutine\n");
}

static void opcode_disassemble_call(OpCode opcode, FILE* stream)
{
    Address address = opcode_decode_address(opcode);

    fprintf(stream, "Call machine code at address " ADDRESS_FMT "\n", address);
}

static void opcode_disassemble_goto(OpCode opcode, FILE* stream)
{
    Address address = opcode_decode_address(opcode);

    fprintf(stream, "goto " ADDRESS_FMT "\n", address);
}

static void opcode_disassemble_call_subroutine(OpCode opcode, FILE* stream)
{
    Address address = opcode_decode_address(opcode);

    fprintf(stream, "Call subroutine at address " ADDRESS_FMT "\n", address);
}

static void opcode_disassemble_if_equals(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(stream, "if (" REGISTER_FMT " == " CONST_FMT ")\n", x, value);
}

static void opcode_disassemble_if_not_equals(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(stream, "if (" REGISTER_FMT " != " CONST_FMT ")\n", x, value);
}

static void opcode_disassemble_set_register(OpCode opcode, FILE* stream)
{
    Register reg = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(stream, REGISTER_FMT" = " CONST_FMT "\n", reg, value);
}

static void opcode_disassemble_add_to_vx(OpCode opcode, FILE* stream)
{
    Register reg = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(stream, REGISTER_FMT" += " CONST_FMT "\n", reg, value);
}

static void opcode_disassemble_set_vx_from_vy(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " = " REGISTER_FMT "\n", x, y);
}

static void opcode_disassemble_set_vx_to_vx_or(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " |= " REGISTER_FMT "\n", x, y);
}

static void opcode_disassemble_set_vx_to_vx_and(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " &= " REGISTER_FMT "\n", x, y);
}

static void opcode_disassemble_set_vx_to_vx_xor(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " ^= " REGISTER_FMT "\n", x, y);
}

static void opcode_disassemble_vx_plus_vy(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " += " REGISTER_FMT "\n", x, y);
}

static void opcode_disassemble_vx_minus_vy(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " -= " REGISTER_FMT "\n", x, y);
}

static void opcode_disassemble_set_vx_rshift_one(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, REGISTER_FMT " >>= 1\n", x);
}

static void opcode_disassemble_set_vx_vy_minus_vx(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(stream, REGISTER_FMT " = " REGISTER_FMT " - " REGISTER_FMT "\n", x, y, x);
}

static void opcode_disassemble_set_vx_lshift_one(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, REGISTER_FMT " <<= 1\n", x);
}


static void opcode_disassemble_set_i_register(OpCode opcode, FILE* stream)
{
    Address address = opcode_decode_address(opcode);

    fprintf(stream, "I = " ADDRESS_FMT "\n", address);
}

static void opcode_disassemble_set_vx_rand_and(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(stream, REGISTER_FMT " = rand() & " CONST_FMT "\n", x, value);
}

static void opcode_disassemble_display_draw_sprite_at(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);
    Const value = opcode_decode_const_4bit(opcode);

    fprintf(stream, "draw(" REGISTER_FMT ", " REGISTER_FMT ", " CONST_FMT ")\n", x, y, value);
}

static void opcode_disassemble_if_key_equals_to_vx(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "if (key() == " REGISTER_FMT ")\n", x);
}

static void opcode_disassemble_if_key_not_equals_to_vx(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "if (key() != " REGISTER_FMT ")\n", x);
}

static void opcode_disassemble_set_vx_from_delay_timer(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, REGISTER_FMT " = get_delay()\n", x);
}

static void opcode_disassemble_set_vx_from_key(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, REGISTER_FMT " = get_key()\n", x);
}

static void opcode_disassemble_set_delay_timer_from_vx(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "delay_timer(" REGISTER_FMT ")\n", x);
}

static void opcode_disassemble_set_sound_timer(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "sound_timer(" REGISTER_FMT ")\n", x);
}
static void opcode_disassemble_add_vx_to_i(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "I += " REGISTER_FMT "\n", x);
}
static void opcode_disassemble_set_i_to_sprite_addr(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "I = sprite_addr[" REGISTER_FMT "]\n", x);
}
static void opcode_disassemble_set_bcd(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "set_bcd(" REGISTER_FMT ")\n", x);
}
static void opcode_disassemble_reg_dump(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "reg_dump(" REGISTER_FMT ", &I)", x);
}
static void opcode_disassemble_reg_load(OpCode opcode, FILE* stream)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(stream, "reg_load(" REGISTER_FMT ", &I)", x);
}

Address opcode_decode_address(OpCode opcode)
{
    return opcode & 0x0FFF;
}

Register opcode_decode_register_x(OpCode opcode)
{
    return (opcode & 0x0F00) >> (2 * 4);
}

Register opcode_decode_register_y(OpCode opcode)
{
    return (opcode & 0x00F0) >> (1 * 4);
}

Const opcode_decode_const_8bit(OpCode opcode)
{
    return opcode & 0x00FF;
}

Const opcode_decode_const_4bit(OpCode opcode)
{
    return opcode & 0x000F;
}
