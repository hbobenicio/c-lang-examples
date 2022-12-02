project(emulator)
cmake_minimum_required(VERSION 3.24)

add_executable(emulator
    "${CMAKE_SOURCE_DIR}/src/emulator/main.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/machine.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/opcode.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/cpu.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/utils/fs.c"
    "${CMAKE_SOURCE_DIR}/src/emulator/commons/buffer.c"
)
target_include_directories(emulator PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)
