# 68HC11 Support Library

A support library providing utility functions for 68HC11 embedded systems development tools.

## Contents

### Source Files (src/)

- **filesubs.c** - File handling utilities for working with file operations
- **hexsubs.c** - Hexadecimal conversion and validation utilities
- **s19subs.c** - Motorola S-record file format handling and upload functionality
- **sersubs.c** - Serial communication timing and polling functions

### Header Files (include/)

- **filesubs.h** - File utility function declarations
- **hexsubs.h** - Hex conversion macros and functions (`hex2nib`, `nib2hex`, `hex2uint`, etc.)
- **s19subs.h** - S-record processing and file upload declarations
- **sersubs.h** - Serial timeout and character input declarations
- **serial.h** - Core serial port communication interface
- **sercpp.h** - C++ wrapper classes for serial communication
- **dos_compat.h** - DOS/BIOS compatibility layer for cross-platform support

## Building

### Requirements

- CMake 3.10 or later
- C compiler (GCC, Clang, MSVC)

### Build Steps

```bash
cd support
mkdir build
cd build
cmake ..
cmake --build .
```

This generates a static library: `lib68hc11support.a` (or `.lib` on Windows)

### Using the Library

Include the library in your project's CMakeLists.txt:

```cmake
add_subdirectory(support)
target_link_libraries(your_target 68hc11support)
target_include_directories(your_target PRIVATE support/include)
```

## Features

- **Serial Communication**: Portable serial port handling with timeouts and flow control
- **S-Record Support**: Parse and upload Motorola S19 format files
- **Hex Conversion**: Efficient hexadecimal string to integer conversions
- **File I/O**: Utilities for file operations and default filename handling
- **Cross-Platform**: DOS compatibility layer allows building on Unix/Linux/macOS systems

## Compatibility

This library was originally developed for DOS/embedded environments. A compatibility layer (`dos_compat.h`) provides stubs for legacy DOS-specific functions on modern platforms, allowing the code to compile on Unix-like systems while maintaining the original API.

### Platform-Specific Notes

- **Windows**: Native DOS/BIOS functions available if needed
- **Unix/Linux/macOS**: DOS functionality stubbed/emulated through the compatibility layer

## License

Based on original code Copyright (c) 1992 By Mark D. Goodwin
