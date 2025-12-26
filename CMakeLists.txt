cmake_minimum_required(VERSION 3.10)
project(68HC11-Tools C)

# Set default C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build shared support library first
add_subdirectory(support)

# Build core components
add_subdirectory(as11pp)
add_subdirectory(loader-chip)
add_subdirectory(loader-pc)
add_subdirectory(s2s)
add_subdirectory(s2boo)

# Convenience target: build everything
add_custom_target(build-all
    COMMAND ${CMAKE_COMMAND} -E echo "=== All components built successfully ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Executables:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - as11pp:    ${CMAKE_CURRENT_BINARY_DIR}/as11pp/as11pp"
    COMMAND ${CMAKE_COMMAND} -E echo "  - loader-pc: ${CMAKE_CURRENT_BINARY_DIR}/loader-pc/loader-pc"
    COMMAND ${CMAKE_COMMAND} -E echo "  - s2s:       ${CMAKE_CURRENT_BINARY_DIR}/s2s/s2s"
    COMMAND ${CMAKE_COMMAND} -E echo "  - s2boo:     ${CMAKE_CURRENT_BINARY_DIR}/s2boo/s2boo"
)
add_dependencies(build-all as11pp loader-pc s2s s2boo)

# Convenience target: run all tests
add_custom_target(test-all
    COMMENT "Running all component tests"
)

# Add sample tests from s2s
add_dependencies(test-all run-sample run-loader-chip)

# Convenience target: build and test everything
add_custom_target(build-and-test
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "=== Build and Test Complete ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Test outputs:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - S2S sample test:        ${CMAKE_CURRENT_BINARY_DIR}/s2s/sample_min.x19"
    COMMAND ${CMAKE_COMMAND} -E echo "  - S2S loader-chip test:   ${CMAKE_CURRENT_BINARY_DIR}/s2s/prg_e1.x19"
)
add_dependencies(build-and-test build-all test-all)

