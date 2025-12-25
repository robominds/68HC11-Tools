cmake_minimum_required(VERSION 3.10)
project(loader-chip)

# Set the paths to the preprocessor and assembler
set(AS11PP_EXE "${CMAKE_CURRENT_SOURCE_DIR}/../as11pp/build/as11pp")
set(AS11_EXE "/Users/markcastelluccio/projects/motorola-6800-assembler/bin/as11")
set(AS11PP_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../as11pp/build")
set(INCLUDES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../as11pp-includes/include")

# Create includes directory next to as11pp executable if it doesn't exist
file(MAKE_DIRECTORY "${AS11PP_BUILD_DIR}/includes")

# Copy include files to the includes directory
configure_file(
    "${INCLUDES_DIR}/rege1.inc"
    "${AS11PP_BUILD_DIR}/includes/rege1.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/regf1.inc"
    "${AS11PP_BUILD_DIR}/includes/regf1.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/regf1cln.inc"
    "${AS11PP_BUILD_DIR}/includes/regf1cln.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/bits.inc"
    "${AS11PP_BUILD_DIR}/includes/bits.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/genconst.inc"
    "${AS11PP_BUILD_DIR}/includes/genconst.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/intvec.inc"
    "${AS11PP_BUILD_DIR}/includes/intvec.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/sciio.inc"
    "${AS11PP_BUILD_DIR}/includes/sciio.inc"
    COPYONLY
)
configure_file(
    "${INCLUDES_DIR}/serio.inc"
    "${AS11PP_BUILD_DIR}/includes/serio.inc"
    COPYONLY
)

# Source assembly files to process
set(ASM_SOURCES
    prg_e1.asm
    prg_e2.asm
    prg_f1.asm
)

# Create output directory for preprocessed files
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/preprocessed")

# Create a custom target to build all assembly files
add_custom_target(loader-chip-asm ALL)

# Add a custom command for each assembly file
foreach(asm_file ${ASM_SOURCES})
    get_filename_component(base_name ${asm_file} NAME_WE)
    set(preprocessed_file "${CMAKE_CURRENT_BINARY_DIR}/preprocessed/${base_name}.i")
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/preprocessed/${base_name}.s19")
    
    # Step 1: Preprocess with as11pp
    # Run from the source directory so relative paths work
    add_custom_command(
        OUTPUT ${preprocessed_file}
        COMMAND ${AS11PP_EXE} "${asm_file}/V"
        COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_CURRENT_SOURCE_DIR}/temp.asm" ${preprocessed_file}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${asm_file}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Preprocessing ${asm_file}"
    )
    
    # Step 2: Assemble with as11
    # as11 creates .s19 files in the same directory as input
    add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${AS11_EXE} ${preprocessed_file}
        DEPENDS ${preprocessed_file}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/preprocessed"
        COMMENT "Assembling ${asm_file}"
    )
    
    # Add the output file to the custom target
    add_custom_target(${base_name}_asm ALL DEPENDS ${output_file})
    add_dependencies(loader-chip-asm ${base_name}_asm)
endforeach()
