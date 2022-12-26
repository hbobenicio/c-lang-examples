#include "keyboard.h"

#include <stdint.h>
#include <assert.h>

#include <SDL2/SDL.h>

#define KEYBOARD_KEYS_COUNT 16

/**
 * @brief Maps the 16-key (8-bit) emulated keyboard values into SDL Scan (Keyboard) Codes.
 * 
 * Key Value -> SDL Scan Code.
 * 
 * @see http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.3
 * @see https://wiki.libsdl.org/SDL2/SDL_Scancode
 */
static const SDL_Scancode keymap[KEYBOARD_KEYS_COUNT] = {
    [0x1] = SDL_SCANCODE_1, [0x2] = SDL_SCANCODE_2, [0x3] = SDL_SCANCODE_3, [0xC] = SDL_SCANCODE_4,
    [0x4] = SDL_SCANCODE_Q, [0x5] = SDL_SCANCODE_W, [0x6] = SDL_SCANCODE_E, [0xD] = SDL_SCANCODE_R,
    [0x7] = SDL_SCANCODE_A, [0x8] = SDL_SCANCODE_S, [0x9] = SDL_SCANCODE_D, [0xE] = SDL_SCANCODE_F,
    [0xA] = SDL_SCANCODE_Z, [0x0] = SDL_SCANCODE_X, [0xB] = SDL_SCANCODE_C, [0xF] = SDL_SCANCODE_V,
};

bool keyboard_is_key_pressed(KeyValue kb_value)
{
    //https://wiki.libsdl.org/SDL2/SDL_GetKeyboardState
    const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

    SDL_Scancode key_scan_code = keymap[kb_value];

    // 1 means that the key is pressed and a value of 0 means that it is not
    return keyboard_state[key_scan_code] == 1;
}

bool keyboard_get_pressed_key(KeyValue* out_key_value)
{
    const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

    for (KeyValue k = 0; k < KEYBOARD_KEYS_COUNT; k++) {
        SDL_Scancode key_scan_code = keymap[k];
        uint8_t key_state = keyboard_state[key_scan_code];
        
        if (key_state == 1) {
            *out_key_value = k;
            return true;
        }
    }

    return false;
}
