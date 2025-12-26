cmake_minimum_required(VERSION 3.10)
project(s2s C)

# Add the support library as a dependency
add_subdirectory(../support support)

# Create the s2s executable
add_executable(s2s s2s.c)

# Link against the support library
target_link_libraries(s2s 68hc11support)

# Set include directories
target_include_directories(s2s PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../support/include
)

# Optional: Set C standard
set_property(TARGET s2s PROPERTY C_STANDARD 99)

# Convenience target: run s2s on a sample S19 produced by loader-chip
add_custom_target(run-sample
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/s2s
            ${CMAKE_CURRENT_SOURCE_DIR}/sample/min.S19
            ${CMAKE_CURRENT_BINARY_DIR}/sample_min.x19
    DEPENDS s2s
    COMMENT "Running s2s on sample/min.S19 to produce sample_min.x19"
)

# Convenience target: run s2s on a bootloader from loader-chip output
# Note: loader-chip must be built first (cd ../loader-chip && mkdir -p build && cd build && cmake .. && make)
get_filename_component(LOADER_CHIP_S19 
    "${CMAKE_CURRENT_SOURCE_DIR}/../loader-chip/build/preprocessed/prg_e1.s19"
    ABSOLUTE)
add_custom_target(run-loader-chip
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/s2s 
            ${LOADER_CHIP_S19}
            ${CMAKE_CURRENT_BINARY_DIR}/prg_e1.x19
    DEPENDS s2s
    COMMENT "Running s2s on loader-chip prg_e1.s19 to produce prg_e1.x19"
)
