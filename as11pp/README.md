# AS11PP - Motorola 6811 Assembly Preprocessor

A preprocessor for Motorola 6811 assembly language files that extends the AS11 assembler with additional features like file inclusion and macro text substitution.

## Features

- **File Inclusion**: Process `#include` directives to incorporate external assembly files
- **Header Macros**: Support `#header` directives for label substitution and parameterization
- **Recursive Processing**: Automatically handles nested includes
- **Portable**: Works from any directory by locating includes relative to the executable

## Building

### Using GCC directly

```bash
gcc -Wall -Wextra as11pp.c -o as11pp
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

To install:

```bash
sudo make install
```

## Usage

```bash
./as11pp <asmfile> [/option]
```

### Options

- `/V` or `/v` - Verbose mode (default)
- `/P` or `/p` - Parameter mode

### Examples

```bash
./as11pp myprogram.asm
./as11pp myprogram.asm/V
```

## Preprocessor Directives

### #include

Include another assembly file with optional parameters:

```assembly
#include("filename.inc", param1, param2, param3)
```

### #header

Define label substitutions for macro-like text replacement:

```assembly
#header("filename.inc", label1, label2, label3)
```

## Directory Structure

The preprocessor expects include files to be located in an `includes/` directory relative to the executable location. This allows the program to be run from any directory while still finding the necessary include files.

## Output

The preprocessor generates a `temp.asm` file in the current working directory containing the expanded assembly code with all includes processed and substitutions applied.

## Requirements

- C compiler with C99 support
- POSIX-compliant system (macOS, Linux, Unix)

## Notes

- Maximum line length: 256 characters
- Maximum parameters: 10 per include/header directive
- Assembly file names limited to 256 characters (command-line argument)
