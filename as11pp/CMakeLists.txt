cmake_minimum_required(VERSION 3.10)
project(as11pp C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra)
endif()

# Create executable
add_executable(as11pp as11pp.c)

# Install target
install(TARGETS as11pp DESTINATION bin)
