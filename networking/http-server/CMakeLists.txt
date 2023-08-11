cmake_minimum_required(VERSION 3.26)
project(http-server LANGUAGES C)

option(SANITIZE "Enables/Disables the compiler sanitizer options" ON)
option(LTO "Enables/Disables the compiler LTO (Link Time Optimizations" OFF)
option(PERF "Enables/Disables compiler flags for perf analysis" OFF)

# used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(http-server
    "${CMAKE_SOURCE_DIR}/src/http-server/commons.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/str.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/io.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/stopwatch.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/scanner.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/net.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/http/method.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/http/parser.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/http/server.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/http.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/config.c"
    "${CMAKE_SOURCE_DIR}/src/http-server/main.c"
)

target_compile_options(http-server PRIVATE
    "-std=c18"
    "-Wall" "-Wextra" "-Wpedantic" "-Werror=vla"
    "-D_POSIX_C_SOURCE=200809" "-D_GNU_SOURCE"
)

target_include_directories(http-server PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

if(SANITIZE)
    target_compile_options(http-server PRIVATE "-fsanitize=address,undefined" "-fno-omit-frame-pointer")
    target_link_options(http-server PRIVATE "-fsanitize=address,undefined" "-fno-omit-frame-pointer")
endif()

if(LTO)
    target_compile_options(http-server PRIVATE "-flto")
    target_link_options(http-server PRIVATE "-flto")
endif()

if(PERF)
    target_compile_options(http-server PRIVATE "-fno-omit-frame-pointer")
    target_link_options(http-server PRIVATE "-fno-omit-frame-pointer")
endif()

