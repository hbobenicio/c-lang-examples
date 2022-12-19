#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "register.h"

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SCALE 10

/**
 * @brief Monochromatic pixel. 0 => Black (OFF); 1 => White (ON)
 */
typedef bool Pixel;

struct display {
    Pixel buffer[DISPLAY_WIDTH][DISPLAY_HEIGHT];
    SDL_Window* window;
    SDL_Renderer* renderer;
};

void display_init(struct display* d);
void display_free(struct display* d);

void display_clear(struct display* d);
void display_render(struct display* d);

bool display_draw_sprite(struct display* d, Register x, Register y, uint8_t* sprite, uint8_t sprite_len);
