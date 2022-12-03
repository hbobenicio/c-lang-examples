#include "display.h"

#include <string.h>

#include <SDL2/SDL.h>

static void init_sdl(void);

void display_init(struct display* d)
{
    init_sdl();
    
    // can't do it with memset, right?
    // memset(d->buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(d->buffer[0]));
    for (size_t i = 0; i < DISPLAY_WIDTH; i++) {
        for (size_t j = 0; j < DISPLAY_HEIGHT; j++) {
            d->buffer[i][j] = 0;
        }
    }
}

void display_free(struct display* d)
{
    (void) d;
    SDL_Quit();
}

static void init_sdl(void)
{
    SDL_version compiled;
    SDL_VERSION(&compiled);

    SDL_version linked;
    SDL_GetVersion(&linked);

    SDL_Log("SDL compiled version.: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch);
    SDL_Log("SDL linked version...: %u.%u.%u\n", linked.major, linked.minor, linked.patch);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "error: sdl2: init failed: %s\n", SDL_GetError());
        exit(1);
    }
}
