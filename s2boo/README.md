# S2BOO - S-Record to Boot File Converter

A utility for converting Motorola S-record (.S19) files to 68HC11 boot loader format (.BOO).

## Overview

S2BOO is a command-line tool that reads Motorola S-record (S19) format files and generates boot files suitable for programming into 68HC11 microcontroller boot areas. It extracts the data records from S-record files and pads them to 256-byte boot blocks.

## Building

### Requirements

- CMake 3.10 or later
- C compiler (GCC, Clang, MSVC)

### Build Steps

```bash
cd s2boo
mkdir -p build
cd build
cmake ..
cmake --build .
```

This generates the executable: `s2boo` (or `s2boo.exe` on Windows)

## Usage

```bash
s2boo <S19_FILE> [<BOOT_FILE>]
```

### Arguments

- **S19_FILE** (required): Path to the input Motorola S-record file. If no extension is provided, `.S19` is assumed.
- **BOOT_FILE** (optional): Path to the output boot file. If not specified, uses the same filename as the input with `.BOO` extension.

### Examples

Convert `program.S19` to `program.BOO`:
```bash
./s2boo program.S19
```

Convert `program.S19` to a custom output file:
```bash
./s2boo program.S19 myboot.BOO
```

Let the utility add extensions:
```bash
./s2boo program
# Looks for program.S19, creates program.BOO
```

## How It Works

1. **Input**: Reads Motorola S-record file format (text format with S0, S1, S2, S3 record types)
2. **Processing**: 
   - Extracts data from S1/S2 data records
   - Validates checksums
   - Builds 256-byte boot blocks
3. **Output**: Writes binary boot file with padding to 256-byte boundaries

## File Formats

### S-Record (S19)
Standard Motorola S-record format with:
- S0 - Header record
- S1 - Data record with 16-bit address
- S2 - Data record with 24-bit address  
- S9 - Start address record

### Boot File (BOO)
Binary format optimized for 68HC11 boot area programming:
- 256 bytes per block
- Padded with 0xFF bytes
- Suitable for EEPROM/flash programming

## Dependencies

This utility depends on the 68HC11 support library for:
- Hexadecimal string parsing and conversion (`hexsubs`)
- File name default handling (`filesubs`)

## Author

Based on original code Copyright (c) 1992 By Mark D. Goodwin

## See Also

- [68HC11 Support Library](../support/README.md) - Core utilities used by this tool
