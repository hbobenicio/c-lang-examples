#include "keyboard.h"

#include <stdint.h>

#include <SDL2/SDL.h>

/**
 * @brief Maps the 16-bit emulated keyboard values into SDL Scan (Keyboard) Codes.
 * @see http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.3
 * @see https://wiki.libsdl.org/SDL2/SDL_Scancode
 */
static const uint8_t keymap[16] = {
    [0x1] = SDL_SCANCODE_1, [0x2] = SDL_SCANCODE_2, [0x3] = SDL_SCANCODE_3, [0xC] = SDL_SCANCODE_4,
    [0x4] = SDL_SCANCODE_Q, [0x5] = SDL_SCANCODE_W, [0x6] = SDL_SCANCODE_E, [0xD] = SDL_SCANCODE_R,
    [0x7] = SDL_SCANCODE_A, [0x8] = SDL_SCANCODE_S, [0x9] = SDL_SCANCODE_D, [0xE] = SDL_SCANCODE_F,
    [0xA] = SDL_SCANCODE_Z, [0x0] = SDL_SCANCODE_X, [0xB] = SDL_SCANCODE_C, [0xF] = SDL_SCANCODE_V,
};

bool keyboard_is_key_pressed(Register kb_value)
{
    //https://wiki.libsdl.org/SDL2/SDL_GetKeyboardState
    const uint8_t* sdl_keys = SDL_GetKeyboardState(NULL);

    Register sdl_kb_scan_code = keymap[kb_value];

    return sdl_keys[sdl_kb_scan_code] == 1;
}
