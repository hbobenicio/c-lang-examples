#include "display.h"

#include <string.h>

#include <SDL2/SDL.h>

#include "memory.h"
#include "utils/bits.h"
#include "logging/logger.h"

#define LOG_TAG "display"

static void init_sdl(struct display* d);

void display_init(struct display* d)
{
    init_sdl(d);
    
    memset(d->buffer, 0, sizeof(d->buffer));
}

void display_free(struct display* d)
{
    SDL_DestroyRenderer(d->renderer);
    SDL_DestroyWindow(d->window);
    SDL_Quit();
}

void display_clear(struct display* d)
{
    SDL_SetRenderDrawColor(d->renderer, 0, 0, 0, 0);
    SDL_RenderClear(d->renderer);
}

void display_render(struct display* d)
{
    // Viewport rect for each pixel
    SDL_Rect r = {0};
    r.w = DISPLAY_SCALE;
    r.h = DISPLAY_SCALE;

    for (uint8_t i = 0; i < DISPLAY_WIDTH; i++) {
        for (uint8_t j = 0; j < DISPLAY_HEIGHT; j++) {
            Pixel pixel = d->buffer[i][j];

            r.x = i * DISPLAY_SCALE + 0 * DISPLAY_SCALE;
            r.y = j * DISPLAY_SCALE;

            if (pixel) {
                SDL_SetRenderDrawColor(d->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(d->renderer, &r);
            } else {
                SDL_SetRenderDrawColor(d->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(d->renderer, &r);
            }
        }
    }

    SDL_RenderPresent(d->renderer);
}

static void init_sdl(struct display* d)
{
    SDL_version compiled;
    SDL_VERSION(&compiled);

    SDL_version linked;
    SDL_GetVersion(&linked);

    log_infof("sdl: compiled version: %u.%u.%u", compiled.major, compiled.minor, compiled.patch);
    log_infof("sdl: linked version..: %u.%u.%u", linked.major, linked.minor, linked.patch);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "error: sdl2: init failed: %s\n", SDL_GetError());
        exit(1);
    }

    uint32_t window_flags = 0;
    int rc = SDL_CreateWindowAndRenderer(
        DISPLAY_WIDTH * DISPLAY_SCALE,
        DISPLAY_HEIGHT * DISPLAY_SCALE,
        window_flags,
        &d->window,
        &d->renderer
    );
    if  (rc != 0) {
        fprintf(stderr, "error: machine: failed to create SDL window and renderer: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    log_info("sdl: window and renderer created successfuly");

    display_clear(d);
}

bool display_draw_sprite(struct display* d, Register x, Register y, uint8_t* sprite, uint8_t sprite_len)
{
    uint8_t pixel_x = x % DISPLAY_WIDTH;
    uint8_t pixel_y = y % DISPLAY_HEIGHT;
    
    bool pixel_erased = false;

    for (uint8_t i = 0; i < sprite_len; i++, pixel_y = (pixel_y + 1) % DISPLAY_HEIGHT) {
        uint8_t sprite_byte = sprite[i];
        uint8_t offset_x = 0;

        for (int8_t bit = 7; bit >= 0; bit--, offset_x++) {
            Pixel new_pixel = bit_is_set(sprite_byte, bit);
            Pixel* current_pixel = &d->buffer[(pixel_x + offset_x) % DISPLAY_WIDTH][pixel_y];

            if (*current_pixel && new_pixel) {
                pixel_erased = true;
            }

            *current_pixel ^= new_pixel;
        }
    }

    return pixel_erased;
}
