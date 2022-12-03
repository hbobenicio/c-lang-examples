#include <stdio.h>

#include "machine.h"
#include "opcode.h"

#include <stdio.h>
#include <stdlib.h>
#include "utils/fs.h"

int main(int argc, const char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "error: ROM_PATH argument not specified\n");
        printf("Usage: emulator <ROM_PATH>\n");
        exit(1);
    }

    const char* rom_file_path = argv[1];

    struct machine machine;
    machine_init(&machine);
    machine_load_rom(&machine, rom_file_path);
    // machine_disassemble(&machine, stdout);
    machine_run(&machine);
}
