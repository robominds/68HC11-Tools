# S2S - Motorola S-Record Restructure Utility

A utility for restructuring Motorola S-record (.S19) files with customizable line lengths and optional execution address specification.

## Overview

S2S is a command-line tool that reads Motorola S-record (S19) format files and regenerates them with:

- **Configurable line lengths** - Adjust the number of data bytes per record
- **Optional execution address** - Specify a custom go-address for program execution
- **Checksum validation** - Verifies checksums on input records
- **Format conversion** - Converts between different S-record representations

This is useful for adapting S-record files to different bootloader requirements or system specifications.

## Building

### Requirements

- CMake 3.10 or later
- C compiler (GCC, Clang, MSVC)

### Build Steps

```bash
cd s2s
mkdir -p build
cd build
cmake ..
cmake --build .
```

This generates the executable: `s2s` (or `s2s.exe` on Windows)

## Usage

### Basic Syntax

```bash
./s2s <input_file> [<output_file>] [<line_length>] [<go_address>]
```

### Parameters

- **input_file** - Input Motorola S19 file (required)
  - Default extension: `.S19` (added if not specified)

- **output_file** - Output S19 file (optional)
  - If not specified, uses input filename with `.x19` extension
  - Default extension: `.x19`

- **line_length** - Number of data bytes per record (optional)
  - Default: 32 bytes
  - Valid range: 1-254

- **go_address** - Execution start address in decimal (optional)
  - Default: Uses address from input file
  - Format: Decimal number or hex with `0x` prefix

### Examples

```bash
# Convert with default settings
./s2s program.S19

# Specify output filename
./s2s input.S19 output.S19

# Use custom line length (16 bytes per record)
./s2s program.S19 result.S19 16

# Set custom execution address
./s2s program.S19 result.S19 32 0x1000

# Output with specific line length
./s2s firmware.S19 firmware.x19 24
```

## Quick Test

Two convenience CMake targets are available for testing:

### Test with Sample File

```bash
cd s2s/build
cmake ..
make run-sample
ls -lh sample_min.x19
```

This runs `s2s` on the minimal sample S19 file (`sample/min.S19`) and produces `sample_min.x19`.

### Test with Bootloader Output

If you've also built the `loader-chip` component, you can test with real bootloader output:

```bash
cd s2s/build
cmake ..
make run-loader-chip
ls -lh prg_e1.x19
```

This runs `s2s` on the assembled bootloader (`../loader-chip/build/preprocessed/prg_e1.s19`) and produces `prg_e1.x19`.

## Sample

A minimal sample S19 file is included at `sample/min.S19`:

```
S0030000FC
S107000001020304EE
S9030000FC
```

The sample contains:
- **S0** - Header record
- **S1** - Data record with 4 bytes at address 0x0000
- **S9** - Termination record with return address 0x0000

## S-Record Format

S2S works with Motorola S-record format which uses the following record types:

| Type | Description |
|------|-------------|
| **S0** | Header record (ignored) |
| **S1** | 16-bit address data record |
| **S2** | 24-bit address data record |
| **S3** | 32-bit address data record |
| **S4** | Count record (not supported) |
| **S5** | Count record for S1 data |
| **S6** | Count record for S2 data |
| **S7** | 32-bit address termination record |
| **S8** | 24-bit address termination record |
| **S9** | 16-bit address termination record |

## Implementation Details

### Checksum Calculation

S2S validates input records using the Motorola S-record checksum algorithm:

```
checksum = 256 - ((sum of all data bytes) mod 256)
```

Each record's checksum is verified, and an error is reported if validation fails.

### Record Processing

- **S0 records** - Header records are skipped
- **S1/S2/S3 records** - Data records are processed and written to output
- **S5/S6 records** - Count records are skipped (regenerated if needed)
- **S7/S8/S9 records** - Termination records are used for execution address

### Output Format

- Records are written with the specified line length
- Each record includes proper checksum calculation
- Output file starts with 0xFF marker byte for boot compatibility
- Records are padded to 256-byte blocks if necessary

## File Handling

### File Extension Handling

- Input files automatically receive `.S19` extension if none is provided
- Output files automatically receive `.x19` extension if none is provided
- The tool displays both input and output filenames on startup

### Output File

The generated `.x19` file contains:

1. Initial 0xFF marker byte
2. Restructured S-records with adjusted line lengths
3. Proper checksums for each record
4. Termination record with execution address

## Error Handling

S2S provides error checking for:

- **File access errors** - Cannot open input or output files
- **Checksum errors** - Mismatched checksum in input records
- **Format errors** - Non-S-record lines in input file
- **Illegal record types** - Unsupported record types (S4, S5, S6)

## Use Cases

### Bootloader Compatibility

Different bootloaders may require specific record line lengths. S2S allows adjustment to match bootloader specifications:

```bash
# For bootloader expecting 16-byte records
./s2s firmware.S19 bootload.S19 16
```

### Address Remapping

Set execution address for code that needs to run at a specific memory location:

```bash
# Execute at 0x1000 after loading
./s2s code.S19 relocated.S19 32 0x1000
```

### File Format Conversion

Convert between different S-record representations:

```bash
# Create version with longer records for storage
./s2s input.S19 storage.S19 64
```

## Related Tools

- `s2boo` - Convert S-records to boot loader format
- `loader-pc` - Download S-records to 68HC11 via serial port
- `as11pp` - Assembly preprocessor producing S-record output

## References

- Motorola S-Record Format Specification
- 68HC11 Bootloader Documentation
- S-Record Checksum Algorithm

## License

See the root project LICENSE file for license information.
