cmake_minimum_required(VERSION 3.10)
project(s2boo C)

# Add the support library as a dependency (only if not already added by parent project)
if(NOT TARGET 68hc11support)
    add_subdirectory(../support support)
endif()

# Create the s2boo executable
add_executable(s2boo s2boo.c)

# Link against the support library
target_link_libraries(s2boo 68hc11support)

# Set include directories
target_include_directories(s2boo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../support/include
)

# Optional: Set C standard
set_property(TARGET s2boo PROPERTY C_STANDARD 99)
