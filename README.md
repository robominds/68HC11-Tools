# <span style="color: red;">*** This repo is an updated archive of work from 1995 ***</span>
# <span style="color: red;">*** This repo is untested on H/W                      ***</span>

# 68HC11 Tools - Motorola 68HC11 Development Toolkit

A comprehensive set of tools and utilities for developing, assembling, and programming code for the Motorola 68HC11 microcontroller family.

## Overview

This repository provides a complete development environment for 68HC11 embedded systems, including:

- **Assembly Preprocessing** with macro support
- **Bootloader Programs** for different chip variants
- **Serial Communication Utilities** for programming and verification
- **File Format Conversion** (S-records to boot format)
- **Shared Support Libraries** for cross-platform development

## Project Structure

### Core Components

#### 1. **as11pp** - Assembly Preprocessor
A preprocessor for Motorola 6811 assembly language that adds modern programming features:
- `#include` directives for file inclusion
- `#header` directives for label substitution and macros
- Recursive file processing
- Fully portable across directories

**Build**: `mkdir -p build && cd build && cmake .. && make`

See: [as11pp/README.md](as11pp/README.md)

#### 2. **as11pp-includes** - Include Files Library
Register definition files for different 68HC11 variants:
- `rege1.inc` - 68HC11E1 register definitions
- `regf1.inc` - 68HC11F1 register definitions
- `regf1cln.inc` - 68HC11F1 clean register set
- Additional utility include files for interrupts, SCI I/O, and serial communication

#### 3. **loader-chip** - 68HC11 Bootloader Programs
Assembly language bootloader implementations for different chip variants:
- **prg_e1.asm** - Bootloader for 68HC11E1
- **prg_e2.asm** - Bootloader for 68HC11E2
- **prg_f1.asm** - Bootloader for 68HC11F1

Features:
- 9600 baud serial communication
- EEPROM programming with 10ms programming pulse
- Verification mode support
- Motorola S-record protocol handling

**Build**: `mkdir -p build && cd build && cmake .. && make`

See: [loader-chip/README.md](loader-chip/README.md)

#### 4. **loader-pc** - PC-Based File Loader
Cross-platform utility for downloading programs to 68HC11 microcontrollers:
- Serial port communication
- Program and verify modes
- Support for multiple S-record formats
- Cross-platform (Windows, macOS, Linux)

**Build**: `mkdir -p build && cd build && cmake .. && make`

See: [loader-pc/README.md](loader-pc/README.md)

#### 5. **s2s** - Motorola S-Record Restructure Utility
Utility for restructuring Motorola S-record (.S19) files:
- Configurable line lengths for different bootloader requirements
- Optional execution address specification
- Checksum validation and record regeneration
- Cross-platform file format conversion

**Build**: `mkdir -p build && cd build && cmake .. && make`

See: [s2s/README.md](s2s/README.md)

#### 6. **s2boo** - S-Record to Boot File Converter
Utility for converting Motorola S-record format to 68HC11 boot loader format:
- Reads .S19 S-record files
- Generates 256-byte boot blocks
- Suitable for direct EEPROM programming

**Build**: `mkdir -p build && cd build && cmake .. && make`

See: [s2boo/README.md](s2boo/README.md)

#### 7. **support** - Shared Utility Library
Cross-platform library providing common functionality:
- File handling utilities
- Hexadecimal conversion
- S-record processing
- Serial communication functions
- DOS/BIOS compatibility layer

See: [support/README.md](support/README.md)

## Quick Start

### Prerequisites

- **Build Tools**:
  - CMake 3.10 or later
  - C compiler (GCC, Clang, or equivalent)
  - Make or Ninja

- **Optional**:
  - External `as11` assembler (for assembly generation)
  - Serial port for programming 68HC11 hardware

### Building All Tools

```bash
# Build the preprocessor
cd as11pp
mkdir -p build && cd build && cmake .. && make
cd ../..

# Build the bootloader assembly
cd loader-chip
mkdir -p build && cd build && cmake .. && make
cd ../..

# Build the PC loader utility
cd loader-pc
mkdir -p build && cd build && cmake .. && make
cd ../..

# Build the S-record restructure utility (s2s)
cd s2s
mkdir -p build && cd build && cmake .. && make
cd ../..

# Build the S-record to boot converter (s2boo)
cd s2boo
mkdir -p build && cd build && cmake .. && make
cd ../..
```

**Or build everything at once** using the root-level CMake:

```bash
# From repository root
mkdir -p build
cd build
cmake ..
make build-and-test
```

This will:
1. Build all components (support library, executables)
2. Run automated tests (s2s on sample and bootloader files)
3. Display a summary of built executables and test outputs

For individual component builds, use `make build-all` or `make test-all`.

## Typical Workflow

1. **Write Assembly Code**
   - Create `.asm` files using 68HC11 assembly syntax
   - Use `#include` directives for register definitions

2. **Preprocess**
   ```bash
   as11pp your_program.asm/V
   ```
   - Generates `temp.asm` with all macros expanded

3. **Assemble**
   ```bash
   as11 temp.asm
   ```
   - Generates `.s19` S-record output

4. **Convert (Optional)**
   ```bash
   # Restructure S-records for specific bootloader (e.g., 16-byte records)
   s2s your_program.s19 restructured.s19 16
   
   # Or convert to boot loader format
   s2boo your_program.s19 -o your_program.boo
   ```
   - Restructures records or converts to boot loader format as needed

5. **Program Device**
   ```bash
   loader-pc /dev/ttyUSB0 -p your_program.s19
   ```
   - Downloads to 68HC11 via serial port

## Project Features

### Multi-Variant Support
Tools support multiple 68HC11 variants:
- 68HC11E1 (standard variant)
- 68HC11E2
- 68HC11F1 (with expanded chip select)

### Cross-Platform
- Runs on Windows, macOS, and Linux
- Portable code using standard C99
- CMake for consistent builds

### Modular Design
- Reusable support library
- Loosely coupled components
- Easy to extend with new tools

### Complete Documentation
- Individual README files for each component
- Code comments and examples
- Register definition files with documentation

## File Organization

```
68HC11-Tools/
├── README.md                 (This file)
├── .gitignore               (Git configuration)
│
├── as11pp/                  (Preprocessor)
│   ├── as11pp.c
│   ├── CMakeLists.txt
│   └── README.md
│
├── as11pp-includes/         (Include files)
│   ├── README.md
│   └── include/
│       ├── rege1.inc
│       ├── regf1.inc
│       ├── regf1cln.inc
│       ├── bits.inc
│       ├── genconst.inc
│       ├── intvec.inc
│       ├── sciio.inc
│       └── serio.inc
│
├── loader-chip/             (Bootloader assembly)
│   ├── CMakeLists.txt
│   ├── README.md
│   ├── prg_e1.asm
│   ├── prg_e2.asm
│   ├── prg_f1.asm
│   └── build/               (generated)
│
├── loader-pc/               (PC download utility)
│   ├── loader.c
│   ├── CMakeLists.txt
│   ├── README.md
│   └── build/               (generated)
│
├── s2s/                     (S-record restructure utility)
│   ├── s2s.c
│   ├── CMakeLists.txt
│   ├── README.md
│   └── build/               (generated)
│
├── s2boo/                   (S-record converter)
│   ├── s2boo.c
│   ├── CMakeLists.txt
│   ├── README.md
│   └── build/               (generated)
│
└── support/                 (Shared library)
    ├── CMakeLists.txt
    ├── README.md
    ├── include/
    │   ├── dos_compat.h
    │   ├── filesubs.h
    │   ├── hexsubs.h
    │   ├── s19subs.h
    │   ├── serial.h
    │   ├── sercpp.h
    │   └── sersubs.h
    └── src/
        ├── filesubs.c
        ├── hexsubs.c
        ├── s19subs.c
        ├── serial_stubs.c
        └── sersubs.c
```

## Building Documentation

Each component has detailed documentation in its README:

- [as11pp](as11pp/README.md) - Preprocessor usage and features
- [as11pp-includes](as11pp-includes/README.md) - Include file reference
- [loader-chip](loader-chip/README.md) - Bootloader specifications
- [loader-pc](loader-pc/README.md) - PC utility usage
- [s2s](s2s/README.md) - S-record restructure utility
- [s2boo](s2boo/README.md) - File conversion tool
- [support](support/README.md) - Library reference

## Common Tasks

### Assemble a Simple Program

```bash
cd as11pp/build
./as11pp ../path/to/program.asm/V
```

### Program a 68HC11 Device

```bash
cd loader-pc/build
./loader-pc /dev/ttyUSB0 -p /path/to/program.s19
```

### Verify Programmed Data

```bash
./loader-pc /dev/ttyUSB0 -v /path/to/program.s19
```

## References

- Motorola 68HC11 Reference Manual
- 68HC11 EEPROM Programming Specifications
- Motorola S-Record Format Documentation
- Serial Communication Standards (RS-232)

## Contributing

This is a historical embedded systems development toolkit. Contributions for bug fixes, documentation improvements, or additional variants are welcome.

## License

See individual component LICENSE files for licensing information.

## History

This toolkit was developed for embedded systems education and professional 68HC11 development. It represents best practices from the 1990s-2000s embedded systems era.

---

**For detailed information on any component, see its individual README file.**
