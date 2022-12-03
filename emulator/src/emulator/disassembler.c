#include "disassembler.h"

static void disassemble_display_clear(struct disassembler* d, OpCode opcode);
static void disassemble_return(struct disassembler* d, OpCode opcode);
static void disassemble_call(struct disassembler* d, OpCode opcode);
static void disassemble_goto(struct disassembler* d, OpCode opcode);
static void disassemble_call_subroutine(struct disassembler* d, OpCode opcode);
static void disassemble_if_equals(struct disassembler* d, OpCode opcode);
static void disassemble_if_not_equals(struct disassembler* d, OpCode opcode);
static void disassemble_set_register(struct disassembler* d, OpCode opcode);
static void disassemble_add_to_vx(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_from_vy(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_to_vx_or(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_to_vx_and(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_to_vx_xor(struct disassembler* d, OpCode opcode);
static void disassemble_vx_plus_vy(struct disassembler* d, OpCode opcode);
static void disassemble_vx_minus_vy(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_rshift_one(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_vy_minus_vx(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_lshift_one(struct disassembler* d, OpCode opcode);
static void disassemble_set_i_register(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_rand_and(struct disassembler* d, OpCode opcode);
static void disassemble_display_draw_sprite_at(struct disassembler* d, OpCode opcode);
static void disassemble_if_key_equals_to_vx(struct disassembler* d, OpCode opcode);
static void disassemble_if_key_not_equals_to_vx(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_from_delay_timer(struct disassembler* d, OpCode opcode);
static void disassemble_set_vx_from_key(struct disassembler* d, OpCode opcode);
static void disassemble_set_delay_timer_from_vx(struct disassembler* d, OpCode opcode);
static void disassemble_set_sound_timer(struct disassembler* d, OpCode opcode);
static void disassemble_add_vx_to_i(struct disassembler* d, OpCode opcode);
static void disassemble_set_i_to_sprite_addr(struct disassembler* d, OpCode opcode);
static void disassemble_set_bcd(struct disassembler* d, OpCode opcode);
static void disassemble_reg_dump(struct disassembler* d, OpCode opcode);
static void disassemble_reg_load(struct disassembler* d, OpCode opcode);

bool disassembler_disassemble(struct disassembler* d, OpCode opcode)
{
    if (opcode == 0x00E0)            { disassemble_display_clear(d, opcode); return true; }
    if (opcode == 0x00EE)            { disassemble_return(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x0000) { disassemble_call(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x1000) { disassemble_goto(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x2000) { disassemble_call_subroutine(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x3000) { disassemble_if_equals(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x4000) { disassemble_if_not_equals(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x6000) { disassemble_set_register(d, opcode); return true; }
    if ((opcode & 0xF000) == 0x7000) { disassemble_add_to_vx(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8000) { disassemble_set_vx_from_vy(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8001) { disassemble_set_vx_to_vx_or(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8002) { disassemble_set_vx_to_vx_and(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8003) { disassemble_set_vx_to_vx_xor(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8004) { disassemble_vx_plus_vy(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8005) { disassemble_vx_minus_vy(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8006) { disassemble_set_vx_rshift_one(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x8007) { disassemble_set_vx_vy_minus_vx(d, opcode); return true; }
    if ((opcode & 0xF00F) == 0x800E) { disassemble_set_vx_lshift_one(d, opcode); return true; }
    if ((opcode & 0xF000) == 0xA000) { disassemble_set_i_register(d, opcode); return true; }
    if ((opcode & 0xF000) == 0xC000) { disassemble_set_vx_rand_and(d, opcode); return true; }
    if ((opcode & 0xF000) == 0xD000) { disassemble_display_draw_sprite_at(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xE09E) { disassemble_if_key_equals_to_vx(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xE0A1) { disassemble_if_key_not_equals_to_vx(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF007) { disassemble_set_vx_from_delay_timer(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF00A) { disassemble_set_vx_from_key(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF015) { disassemble_set_delay_timer_from_vx(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF018) { disassemble_set_sound_timer(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF01E) { disassemble_add_vx_to_i(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF029) { disassemble_set_i_to_sprite_addr(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF033) { disassemble_set_bcd(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF055) { disassemble_reg_dump(d, opcode); return true; }
    if ((opcode & 0xF0FF) == 0xF065) { disassemble_reg_load(d, opcode); return true; }

    fprintf(d->file, "error: unsupported opcode. opcode=" OPCODE_FMT "\n", opcode);
    return false;
}

static void disassemble_display_clear(struct disassembler* d, OpCode opcode)
{
    (void) opcode;
    fprintf(d->file, "Clear the screen\n");
}

static void disassemble_return(struct disassembler* d, OpCode opcode)
{
    (void) opcode;
    fprintf(d->file, "Return from a subroutine\n");
}

static void disassemble_call(struct disassembler* d, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    fprintf(d->file, "Call machine code at address " ADDRESS_FMT "\n", address);
}

static void disassemble_goto(struct disassembler* d, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    fprintf(d->file, "goto " ADDRESS_FMT "\n", address);
}

static void disassemble_call_subroutine(struct disassembler* d, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    fprintf(d->file, "Call subroutine at address " ADDRESS_FMT "\n", address);
}

static void disassemble_if_equals(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(d->file, "if (" REGISTER_FMT " == " CONST_FMT ")\n", x, value);
}

static void disassemble_if_not_equals(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(d->file, "if (" REGISTER_FMT " != " CONST_FMT ")\n", x, value);
}

static void disassemble_set_register(struct disassembler* d, OpCode opcode)
{
    Register reg = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(d->file, REGISTER_FMT " = " CONST_FMT "\n", reg, value);
}

static void disassemble_add_to_vx(struct disassembler* d, OpCode opcode)
{
    Register reg = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(d->file, REGISTER_FMT " += " CONST_FMT "\n", reg, value);
}

static void disassemble_set_vx_from_vy(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " = " REGISTER_FMT "\n", x, y);
}

static void disassemble_set_vx_to_vx_or(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " |= " REGISTER_FMT "\n", x, y);
}

static void disassemble_set_vx_to_vx_and(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " &= " REGISTER_FMT "\n", x, y);
}

static void disassemble_set_vx_to_vx_xor(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " ^= " REGISTER_FMT "\n", x, y);
}

static void disassemble_vx_plus_vy(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " += " REGISTER_FMT "\n", x, y);
}

static void disassemble_vx_minus_vy(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " -= " REGISTER_FMT "\n", x, y);
}

static void disassemble_set_vx_rshift_one(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, REGISTER_FMT " >>= 1\n", x);
}

static void disassemble_set_vx_vy_minus_vx(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);

    fprintf(d->file, REGISTER_FMT " = " REGISTER_FMT " - " REGISTER_FMT "\n", x, y, x);
}

static void disassemble_set_vx_lshift_one(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, REGISTER_FMT " <<= 1\n", x);
}


static void disassemble_set_i_register(struct disassembler* d, OpCode opcode)
{
    Address address = opcode_decode_address(opcode);

    fprintf(d->file, "I = " ADDRESS_FMT "\n", address);
}

static void disassemble_set_vx_rand_and(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Const value = opcode_decode_const_8bit(opcode);

    fprintf(d->file, REGISTER_FMT " = rand() & " CONST_FMT "\n", x, value);
}

static void disassemble_display_draw_sprite_at(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);
    Register y = opcode_decode_register_y(opcode);
    Const value = opcode_decode_const_4bit(opcode);

    fprintf(d->file, "draw(" REGISTER_FMT ", " REGISTER_FMT ", " CONST_FMT ")\n", x, y, value);
}

static void disassemble_if_key_equals_to_vx(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "if (key() == " REGISTER_FMT ")\n", x);
}

static void disassemble_if_key_not_equals_to_vx(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "if (key() != " REGISTER_FMT ")\n", x);
}

static void disassemble_set_vx_from_delay_timer(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, REGISTER_FMT " = get_delay()\n", x);
}

static void disassemble_set_vx_from_key(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, REGISTER_FMT " = get_key()\n", x);
}

static void disassemble_set_delay_timer_from_vx(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "delay_timer(" REGISTER_FMT ")\n", x);
}

static void disassemble_set_sound_timer(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "sound_timer(" REGISTER_FMT ")\n", x);
}

static void disassemble_add_vx_to_i(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "I += " REGISTER_FMT "\n", x);
}

static void disassemble_set_i_to_sprite_addr(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "I = sprite_addr[" REGISTER_FMT "]\n", x);
}

static void disassemble_set_bcd(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "set_bcd(" REGISTER_FMT ")\n", x);
}

static void disassemble_reg_dump(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "reg_dump(" REGISTER_FMT ", &I)\n", x);
}

static void disassemble_reg_load(struct disassembler* d, OpCode opcode)
{
    Register x = opcode_decode_register_x(opcode);

    fprintf(d->file, "reg_load(" REGISTER_FMT ", &I)\n", x);
}
