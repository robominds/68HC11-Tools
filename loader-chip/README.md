# Loader Chip - Motorola 68HC11 Bootloader Programs

This directory contains bootloader/loader programs for the Motorola 68HC11 microcontroller family. These programs are designed to run on different chip variants and handle program loading and verification over serial communication.

## Overview

The loader chip programs implement a serial bootloader that:

- **Receives S-records** over a 9600 baud serial interface (8 data bits, no parity)
- **Programs EEPROM** on the 68HC11 microcontroller
- **Supports verification** mode to read back and verify programmed data
- **Uses control codes** to switch between program ('P') and verify ('V') modes

## Program Variants

The project includes loader programs for different 68HC11 variants:

### prg_e1.asm
- **Target**: 68HC11E1 variant
- **Registers**: Uses `rege1.inc` register definitions
- **Features**: Standard loader for the E1 variant
- **Memory**: Programs internal EEPROM with 8ms programming pulse

### prg_f1.asm
- **Target**: 68HC11F1 variant
- **Registers**: Uses `regf1cln.inc` register definitions (clean variant)
- **Features**: Enhanced loader with expanded chip select support
- **Special Features**: 
  - Port G control for expanded mode
  - Chip select address/size configuration
  - Programs from $0000 to $f800 (excludes config register)

### prg_e2.asm
- **Target**: 68HC11E2 variant
- **Registers**: Uses `regf1cln.inc` register definitions
- **Similar to**: prg_f1.asm, adapted for E2 variant
- **Memory**: Programs from $0000 to $f800

## Assembly Files

- **prgf1v10.asm** - Original unmodified version 1.0 for F1
- **prgf1v11.asm** - Version 1.1 with improvements for F1
- **prgf1v11.s19** - Pre-assembled S19 output of v1.1

## Building

### Prerequisites

- `as11pp` - Motorola 6811 Assembly Preprocessor
- `as11` - Motorola 6811 Assembler (https://github.com/JimInCA/motorola-6800-assembler)
- CMake 3.10 or later
- C compiler (for building tools)

### Build Instructions

From the loader-chip directory:

```bash
mkdir -p build
cd build
cmake ..
make
```

This will:

1. **Preprocess** each .asm file using `as11pp` to handle:
   - `#include` directives for register definitions
   - `#header` directives for label substitution
   - Macro text substitution with parameters

2. **Assemble** the preprocessed files using `as11` to generate S19 (Motorola S-record) format binaries

### Output

After building, the generated S19 files are located in:

```
build/preprocessed/
├── prg_e1.s19   (511 bytes)
├── prg_e2.s19   (551 bytes)
└── prg_f1.s19   (551 bytes)
```

These S19 files can be programmed directly into the 68HC11 EEPROM.

## Protocol

### Serial Communication

- **Baud Rate**: 9600
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1

### Sequence

1. **Control Byte**: Send 'P' (Program) or 'V' (Verify)
2. **S-Records**: Send Motorola S-record data
3. **Byte-by-byte handling**: Each data byte is either:
   - **Program mode**: Written to EEPROM with 10ms programming pulse
   - **Verify mode**: Read back and echo'd to host for verification

### Special Addresses

- **$FE00 and above**: Treated as protected (config register area)
- **$F800 and above (F1/E2)**: Only config register programming allowed
- **$FFFF**: Standard entry point for jump commands

## Register Definitions

The assembly files use register definition files located in the parent `as11pp-includes/include/` directory:

- `rege1.inc` - Register definitions for 68HC11E1 variant
- `regf1cln.inc` - Clean register definitions for 68HC11F1 variant

These are automatically copied during the CMake configuration step.

## Timing

The programs use timing constants configured for 8 MHz crystal:

- **ms10** (10ms): Programming pulse duration
- **us500** (500μs): Data polling interval

These can be adjusted in the assembly source if using different clock frequencies.

## Implementation Details

### Programming Process

The loader implements standard Motorola 68HC11 EEPROM programming:

1. Enable internal address/data latches
2. Write byte to target address
3. Enable internal programming voltage (10ms)
4. Disable programming voltage
5. Poll data to confirm programming completion

### Verification

In verify mode, the loader:

1. Reads the byte at the current address
2. Echoes it back to the host
3. Allows comparison with original data

## Files

```
loader-chip/
├── CMakeLists.txt           (Build configuration)
├── README.md               (This file)
├── prg_e1.asm             (E1 variant source)
├── prg_e2.asm             (E2 variant source)
├── prg_f1.asm             (F1 variant source)
├── prgf1v10.asm           (F1 v1.0 original)
├── prgf1v11.asm           (F1 v1.1 modified)
├── prgf1v11.s19           (Pre-assembled F1 v1.1)
├── reg_e1.inc             (E1 register definitions - local copy)
├── reg_f1.inc             (F1 register definitions - local copy)
├── reg_f1cln.inc          (F1 clean register definitions - local copy)
└── build/                 (Generated during build)
    └── preprocessed/
        ├── prg_e1.i       (Preprocessed E1)
        ├── prg_e2.i       (Preprocessed E2)
        ├── prg_f1.i       (Preprocessed F1)
        ├── prg_e1.s19     (Assembled E1)
        ├── prg_e2.s19     (Assembled E2)
        └── prg_f1.s19     (Assembled F1)
```

## Related Projects

- `as11pp` - Preprocessor for these assembly files
- `as11pp-includes` - Include files with register definitions
- `loader-pc` - PC-side loader utility for downloading S-records
- `support` - Shared utility functions and libraries

## References

- Motorola 68HC11 Reference Manual
- EEPROM Programming Specifications
- Serial Communication Protocol Documentation

## License

See the root project LICENSE file for license information.
