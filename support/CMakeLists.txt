cmake_minimum_required(VERSION 3.10)
project(68HC11Support C)

# Create library from source files
add_library(68hc11support STATIC
    src/filesubs.c
    src/hexsubs.c
    src/s19subs.c
    src/sersubs.c
    src/serial_stubs.c
)

# Set include directory for the library
target_include_directories(68hc11support PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optional: Set C standard
set_property(TARGET 68hc11support PROPERTY C_STANDARD 99)
