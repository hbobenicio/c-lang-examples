cmake_minimum_required(VERSION 3.24)
project(emulator)

# find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 sdl2)
message("Dependency Libraries found: ${SDL2_LIBRARIES}")

string(APPEND CMAKE_C_FLAGS_DEBUG " -Wall -Wextra -Wpedantic -Werror=vla -O0 -ggdb -fsanitize=address,undefined -fno-omit-frame-pointer")

add_executable(emulator
    "${CMAKE_SOURCE_DIR}/src/emulator/main.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/machine.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/opcode.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/display.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/disassembler.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/utils/fs.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/commons/buffer.c"
)

# TODO fix? Not working ATM... (-std=c17)
set_property(TARGET emulator PROPERTY C_STANDARD 17)

target_compile_definitions(emulator PRIVATE "${SDL2_CFLAGS_OTHER}")
target_include_directories(emulator PRIVATE "${CMAKE_SOURCE_DIR}/src" "${SDL2_INCLUDE_DIRS}")
target_link_directories(emulator PRIVATE "${SDL2_LIBRARY_DIRS}")
target_link_libraries(emulator "${SDL2_LIBRARIES}")
