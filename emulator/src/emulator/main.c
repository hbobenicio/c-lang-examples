//POSIX.1-2008: strnlen
#define _POSIX_C_SOURCE 200809L
// #define _FORTIFY_SOURCE 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "machine.h"
#include "opcode.h"
#include "utils/fs.h"
#include "utils/math.h"

#include "logging/logger.h"

#define LOG_TAG "main"

enum cli_command {
    CLI_COMMAND_DISASSEMBLE,
    CLI_COMMAND_RUN,
};

static const char usage[] =
    "\n"
    "Usage: emulator <COMMAND> <ROM_PATH>\n"
    "\n"
    "COMMAND:\n"
    "    run\n"
    "        Load and run the Chip-8 program rom at ROM_PATH\n"
    "    disassemble\n"
    "        Load and disassemble the Chip-8 program rom at ROM_PATH\n"
    "\n"
    "ROM_PATH:\n"
    "    File path for a Chip-8 Program ROM\n"
;

int cli_parse_command(const char* arg, enum cli_command* out_command);

int main(int argc, const char** argv)
{
    config_init_from_env();
    
    if (argc != 3) {
        // fprintf(stderr, "error: ROM_PATH argument not specified\n");
        log_error("ROM_PATH argument not specified");
        puts(usage);
        exit(1);
    }

    enum cli_command command;
    if (cli_parse_command(argv[1], &command) != 0) {
        log_errorf("cli: invalid command. argv[1]=\"%s\"", argv[1]);
        puts(usage);
        exit(1);
    }

    const char* rom_file_path = argv[2];

    struct machine machine;
    machine_init(&machine);
    machine_load_rom(&machine, rom_file_path);

    switch (command)
    {
        case CLI_COMMAND_DISASSEMBLE:
            machine_disassemble(&machine, stdout);
            break;

        case CLI_COMMAND_RUN:
            machine_run(&machine);
            break;
        
        default: assert(false && "unreachable: unknown command");
    }

    machine_free(&machine);
}

int cli_parse_command(const char* arg, enum cli_command* out_command)
{
    static const char cmd_disassemble[] = "disassemble";
    static const size_t cmd_disassemble_size = sizeof(cmd_disassemble); // this is size, not length

    static const char cmd_run[] = "run";
    static const size_t cmd_run_size = sizeof(cmd_run); // this is size, not length

    static const size_t max_arg_size = MAX(cmd_disassemble_size, cmd_run_size);
    
    size_t arg_len = strnlen(arg, max_arg_size);
    if (arg_len >= max_arg_size) {
        fprintf(stderr, "error: cli: argument parsing: argument is too big. argv=\"%s\"\n", arg);
        return 1;
    }

    if (strncmp(arg, cmd_disassemble, cmd_disassemble_size) == 0) {
        *out_command = CLI_COMMAND_DISASSEMBLE;
        return 0;
    }

    if (strncmp(arg, cmd_run, cmd_run_size) == 0) {
        *out_command = CLI_COMMAND_RUN;
        return 0;
    }

    return 1;
}
