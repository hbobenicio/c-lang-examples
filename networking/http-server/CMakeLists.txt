cmake_minimum_required(VERSION 3.26)
project(http-server LANGUAGES C)

option(SANITIZE "Enables/Disables the compiler sanitizer options" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(http-server
    "${CMAKE_SOURCE_DIR}/src/str.c"
    "${CMAKE_SOURCE_DIR}/src/scanner.c"
    "${CMAKE_SOURCE_DIR}/src/net.c"
    "${CMAKE_SOURCE_DIR}/src/http.c"
    "${CMAKE_SOURCE_DIR}/src/config.c"
    "${CMAKE_SOURCE_DIR}/src/main.c"
)

target_compile_options(http-server PRIVATE
    "-std=c18"
    "-Wall" "-Wextra" "-Wpedantic" "-Werror=vla"
    "-D_POSIX_C_SOURCE=200809" "-D_GNU_SOURCE"
)

if(SANITIZE)
    target_compile_options(http-server PRIVATE "-fsanitize=address,undefined" "-fno-omit-frame-pointer")
    target_link_options(http-server PRIVATE "-fsanitize=address,undefined" "-fno-omit-frame-pointer")
endif()

