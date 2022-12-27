#include "machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <arpa/inet.h>

#include <SDL2/SDL.h>

#include "config.h"
#include "utils/fs.h"
#include "commons/buffer.h"
#include "opcode.h"
#include "disassembler.h"
#include "logging/logger.h"

#define PROGRAM_MAX_SIZE (MEMORY_SIZE - MEMORY_PROGRAM_STARTING_ADDRESS)

#define LOG_TAG "machine"

static Word machine_first_opcode(struct machine* m);

void machine_init(struct machine* m)
{
    log_info("initializing...");

    //This is needed for rand() which is needed by some CPU instructions
    srand(time(NULL));

    timer_init(&m->delay_timer);
    timer_init(&m->sound_timer);

    memory_init(m->memory);

    display_init(&m->display);

    cpu_init(&m->cpu, m->memory, &m->display, &m->delay_timer, &m->sound_timer);

    log_info("initialization succeeded");
}

void machine_free(struct machine* m)
{
    display_free(&m->display);
}

void machine_load_rom(struct machine* m, const char* rom_file_path)
{
    log_infof("loading rom \"%s\"", rom_file_path);

    struct buffer rom = file_read_contents(rom_file_path);
    if (rom.capacity > PROGRAM_MAX_SIZE) {
        log_errorf("failed to load program rom: program size (%zu) is too big", rom.capacity);
        buffer_free(&rom);
        exit(1);
    }
    log_debugf("rom size = %zu", rom.capacity);

    // zero program memory area
    memset(&m->memory[MEMORY_PROGRAM_STARTING_ADDRESS], 0, PROGRAM_MAX_SIZE);

    // copy program rom bytes to memory area beginning at the correct starting offset
    memcpy(&m->memory[MEMORY_PROGRAM_STARTING_ADDRESS], rom.data, rom.capacity);

    buffer_free(&rom);
}

void machine_disassemble(struct machine* m, FILE* file)
{
    struct disassembler disassembler = {
        .file = file,
    };

    bool unsupported_opcode_found = false;
    Address addr = MEMORY_PROGRAM_STARTING_ADDRESS;
    for (
        Word opcode = machine_first_opcode(m);
        opcode != 0 && addr < MEMORY_SIZE;
        addr += sizeof(opcode), opcode = (m->memory[addr] << 8) | (m->memory[addr+1])
    ) {
        fprintf(file, ADDRESS_FMT ": OPCODE[" OPCODE_FMT "]: ", addr, opcode);
        if (!disassembler_disassemble(&disassembler, opcode)) {
            unsupported_opcode_found = true;
            break;
        }
    }

    if (unsupported_opcode_found) {
        fprintf(stderr, "error: some unsupported opcodes were found. disassembly failed.\n");
        exit(1);
    }
    
    fprintf(stderr, "info: machine: disassembling finished successfuly\n");
}

void machine_run(struct machine* m)
{
    timer_start(&m->delay_timer);
    timer_start(&m->sound_timer);

    // Emulation Main Loop
    // uint64_t cpu_timer_start = SDL_GetTicks64();
    while (true)
    {
        // uint64_t fps_timer_start = SDL_GetTicks64();

        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        goto loop_exit;

                    case SDL_KEYDOWN: {
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            goto loop_exit;
                        }
                    } break;

                    default:
                        break;
                }
            }
        }
        
        // Update Timers
        timer_update(&m->delay_timer);
        timer_update(&m->sound_timer);

        // Update CPU: Executes a single instruction (step) and performs clock speed control
        // Tentativa 1
        uint64_t cpu_run_start_time_moment = SDL_GetPerformanceCounter();
        cpu_step(&m->cpu);
        uint64_t cpu_run_end_time_moment = SDL_GetPerformanceCounter();
        uint64_t cpu_run_elapsed_time = cpu_run_end_time_moment - cpu_run_start_time_moment;
        double cpu_run_elapsed_time_s = (double) cpu_run_elapsed_time / SDL_GetPerformanceFrequency();
        double cpu_run_elapsed_time_ms = cpu_run_elapsed_time_s * 1000;
        double cpu_run_elapsed_time_us = cpu_run_elapsed_time_s * 1000 * 1000;

        uint32_t cpu_run_ideal_duration_ms = 1000 / config()->cpu_clock_speed_hz;
        if (cpu_run_elapsed_time_ms < (double) cpu_run_ideal_duration_ms) {
            uint32_t sleep_duration_ms = cpu_run_ideal_duration_ms - cpu_run_elapsed_time_ms;
            SDL_Delay(sleep_duration_ms);
        }
        uint64_t cpu_run_wallclock_end_time_moment = SDL_GetPerformanceCounter();
        uint64_t cpu_run_wallclock_elapsed_time = cpu_run_wallclock_end_time_moment - cpu_run_start_time_moment;
        double cpu_run_wallclock_elapsed_time_ms = ((double) cpu_run_wallclock_elapsed_time / SDL_GetPerformanceFrequency()) * 1000;

        log_tracef(
            "cpu: clock control: work time: actual=%.2lfμs simulated=%.2lfms",
            cpu_run_elapsed_time_us,
            cpu_run_wallclock_elapsed_time_ms
        );

        // Tentativa 2
        // uint64_t cpu_timer_end = SDL_GetTicks64();
        // uint64_t cpu_timer_elapsed = cpu_timer_end - cpu_timer_start;
        // uint64_t cpu_run_config_duration_ms = 1000 / config()->cpu_clock_speed_hz;
        // if (cpu_timer_elapsed >= cpu_run_config_duration_ms) {
            // cpu_step(&m->cpu);
        //     cpu_timer_start = SDL_GetTicks64();
        // }

        // Render the full display
        display_render(&m->display);

        // Finish rendering and flush the back buffer into screen
        // uint64_t fps_timer_end = SDL_GetTicks64();
        // uint64_t fps_timer_elapsed_ms = fps_timer_end - fps_timer_start;
        // if (fps_timer_elapsed_ms < (1000/60)) {
        //     uint32_t sleep_duration_ms = (1000/60) - fps_timer_elapsed_ms;
        //     SDL_Delay(sleep_duration_ms);
        // }
        display_render_flush(&m->display);
    }

loop_exit:
    log_info("finishing emulation...");
}

static Word machine_first_opcode(struct machine* m)
{
    Address addr = MEMORY_PROGRAM_STARTING_ADDRESS;
    Word opcode = (m->memory[addr] << 8) | (m->memory[addr + 1]) ;
    return opcode;
}
