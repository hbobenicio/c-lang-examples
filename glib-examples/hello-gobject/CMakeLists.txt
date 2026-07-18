cmake_minimum_required(VERSION 3.30)
project(hello-gobject LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GOBJECT REQUIRED IMPORTED_TARGET gobject-2.0)

add_executable(hello-gobject
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app-configuration.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
)

target_link_libraries(hello-gobject PkgConfig::GOBJECT)
