# Loader-PC - 68HC11 PC-based File Loader

A PC-based utility for loading and verifying program files onto 68HC11 microcontrollers via serial port communication.

## Overview

Loader-PC is a command-line tool that communicates with a 68HC11 microcontroller via serial port to load executable program files and optionally verify them. It supports two modes of operation:

- **Program Mode** (/P): Downloads program data to the microcontroller
- **Verify Mode** (/V): Reads back and verifies previously programmed data

## Building

### Requirements

- CMake 3.10 or later
- C compiler (GCC, Clang, MSVC)

### Build Steps

```bash
cd loader-pc
mkdir -p build
cd build
cmake ..
cmake --build .
```

This generates the executable: `loader-pc` (or `loader-pc.exe` on Windows)

## Usage

```bash
loader-pc <FILE> [<BOOT_FILE>] [/P|/V] [/2]
```

### Arguments

- **FILE** (required): Path to the program file to load or verify (S19 format expected)
- **BOOT_FILE** (optional): Path to the boot loader file. If not specified, uses default path
- **/P** (optional): Program mode - downloads code to the device (default is verify mode)
- **/V** (optional): Verify mode - reads and verifies without programming (default)
- **/2** (optional): Use COM2 instead of COM1 for serial communication

### Examples

Verify a program file using COM1:
```bash
./loader-pc program.S19 /V
```

Program a device using COM2:
```bash
./loader-pc program.S19 bootfile.BOO /P /2
```

Program using default boot file:
```bash
./loader-pc program
```

## How It Works

1. **Initialization**: Opens serial port and configures for 9600 baud, 8 data bits, no parity
2. **Boot Loading**: Sends the boot loader code to the microcontroller
3. **Program Transfer**: Uploads the program file in S-record format
4. **Verification**: If in verify mode, reads back and validates the programmed data

## Serial Communication

- **Default Speed**: 9600 baud
- **Data Format**: 8 data bits, no parity, 1 stop bit
- **Ports**: COM1 (default) or COM2

## Dependencies

This utility depends on the 68HC11 support library for:
- S-record file parsing and upload (`s19subs`)
- Serial communication (`serial`, `sersubs`)

## Platform Notes

**Note**: This utility includes stub implementations for serial communication functions to allow cross-platform compilation. On systems without actual serial hardware, the application will compile and run but cannot perform actual hardware communication.

For actual 68HC11 programming, native serial port implementations would need to be provided for your specific platform.

## Author

Based on original code Copyright (c) 1992 By Mark D. Goodwin

## See Also

- [68HC11 Support Library](../support/README.md) - Core utilities used by this tool
- [S2BOO](../s2boo/README.md) - S-record to boot file converter
