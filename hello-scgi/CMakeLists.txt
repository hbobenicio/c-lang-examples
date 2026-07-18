cmake_minimum_required(VERSION 3.30)
project(scgi-backend LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(STATIC "Build Static Executables" ON)

add_executable(scgi-backend
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/mem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/strutils.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/net.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/scgi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/main.c"
)
set_target_properties(scgi-backend PROPERTIES
    C_STANDARD "23"
)
target_compile_options(scgi-backend 
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)
target_link_options(scgi-backend
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

if(${STATIC})
    target_compile_options(scgi-backend PRIVATE "-static")
    target_link_options   (scgi-backend PRIVATE "-static")
endif()

add_executable(scgi-backend-test
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/net.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/scgi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/scgi-backend/src/test.c"
)
set_target_properties(scgi-backend-test PROPERTIES
    C_STANDARD "23"
)
