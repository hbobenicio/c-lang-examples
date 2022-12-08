#pragma once

#include <stdint.h>

#include <SDL2/SDL.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SCALE 10

struct display {
    //TODO will we ever use this?
    uint8_t buffer[DISPLAY_WIDTH][DISPLAY_HEIGHT];
    SDL_Window* window;
    SDL_Renderer* renderer;
};

void display_init(struct display* d);
void display_free(struct display* d);

void display_clear(struct display* d);
void display_render(struct display* d);
