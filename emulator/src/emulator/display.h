#pragma once

#include <stdint.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32 

struct display {
    uint8_t buffer[DISPLAY_WIDTH][DISPLAY_HEIGHT];
};

void display_init(struct display* d);
void display_free(struct display* d);
