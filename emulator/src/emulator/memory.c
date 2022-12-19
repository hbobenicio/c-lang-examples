#include "memory.h"

#include <string.h>
#include <assert.h>

// The font sprite size in bytes
#define FONT_SPRITE_SIZE 5

// The starting memory address where the fonts will be loaded
#define FONT_MEM_ADDRESS 0x0000

// The total number of sprites
#define FONT_COUNT 16

static void load_font_sprites(uint8_t* mem);

void memory_init(uint8_t* mem)
{
    memset(mem, 0, MEMORY_SIZE);
    load_font_sprites(mem);
}

static void load_font_sprites(uint8_t* mem)
{
    static const uint8_t font_sprites[] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };
    assert(sizeof(font_sprites) == FONT_COUNT * FONT_SPRITE_SIZE);

    memcpy(mem + FONT_MEM_ADDRESS, font_sprites, sizeof(font_sprites));
}

Address memory_address_from_sprite_value(uint8_t sprite_value)
{
    return FONT_MEM_ADDRESS + (sprite_value * FONT_SPRITE_SIZE);
}
