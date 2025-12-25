cmake_minimum_required(VERSION 3.10)
project(loader-pc C)

# Add the support library as a dependency
add_subdirectory(../support support)

# Create the loader-pc executable
add_executable(loader-pc loader.c)

# Link against the support library
target_link_libraries(loader-pc 68hc11support)

# Set include directories
target_include_directories(loader-pc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../support/include
)

# Optional: Set C standard
set_property(TARGET loader-pc PROPERTY C_STANDARD 99)
