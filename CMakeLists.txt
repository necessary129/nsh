cmake_minimum_required(VERSION 3.12)


project(nsh VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "Create indexing for clangd")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS_DEBUG_INIT "-Wall -Wextra -Wpedantic -g")
set(CMAKE_C_FLAGS_RELEASE_INIT "-Wall -Wextra -Wpedantic  -O2")


add_subdirectory(src)
