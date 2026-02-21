# N_OS

A 64-bit operating system built from scratch with a custom bootloader, kernel, and basic shell interface.

## Features

- **64-bit Long Mode**: Full x86-64 architecture support
- **Custom Bootloader**: Two-stage bootloader with A20 line activation, GDT setup, and protected mode transition
- **VGA Text Mode Driver**: 80x25 character display with color support
- **PS/2 Keyboard Driver**: Polling-based keyboard input with scancode translation
- **Command Shell**: Basic shell with command parsing and execution
- **Modular Architecture**: Well-organized driver and library structure

## Project Structure

```
N_OS/
├── boot/
│   ├── bootloader.asm          # Stage 1 bootloader (512 bytes)
│   └── bootloaderstage2.asm    # Stage 2 bootloader (64-bit transition)
├── kernel/
│   ├── kernel.c                # Kernel entry point
│   ├── linker.ld               # Linker script
│   ├── drivers/
│   │   ├── terminal/
│   │   │   ├── terminal.c      # VGA text mode driver
│   │   │   └── terminal.h
│   │   └── userinput/
│   │       ├── keyboard/
│   │       │   ├── keyboard.c  # PS/2 keyboard driver
│   │       │   └── keyboard.h
│   │       ├── inputmanageur.c # Input event manager
│   │       ├── inputmanageur.h
│   │       ├── terminal_commande_manageur.c  # Command parser
│   │       └── terminal_commande_manageur.h
│   └── lib/
│       ├── strcmp.c            # String comparison utility
│       └── strcmp.h
├── .gitignore
└── README.md
```

## Building

### Prerequisites

- **NASM** (Netwide Assembler) for bootloader assembly
- **GCC** with 64-bit support for kernel compilation
- **LD** (GNU Linker)
- **QEMU** for testing (optional but recommended)

### Compilation Steps

1. **Assemble the bootloader (Stage 1)**
   ```bash
   nasm -f bin boot/bootloader.asm -o boot/bootloader.bin
   ```

2. **Assemble Stage 2 bootloader**
   ```bash
   nasm -f bin boot/bootloaderstage2.asm -o boot/bootloaderstage2.bin
   ```

3. **Compile kernel source files**
   ```bash
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/kernel.c -o kernel/kernel.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/lib/strcmp.c -o kernel/lib/strcmp.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/terminal/terminal.c -o kernel/drivers/terminal/terminal.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/userinput/keyboard/keyboard.c -o kernel/drivers/userinput/keyboard/keyboard.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/userinput/terminal_commande_manageur.c -o kernel/drivers/userinput/terminal_commande_manageur.o
   ```

4. **Link kernel**
   ```bash
   ld -m elf_x86_64 -T kernel/linker.ld -o kernel/kernel.elf kernel/kernel.o kernel/lib/strcmp.o kernel/drivers/terminal/terminal.o kernel/drivers/userinput/keyboard/keyboard.o kernel/drivers/userinput/terminal_commande_manageur.o
   ```

5. **Extract binary**
   ```bash
   objcopy -O binary kernel/kernel.elf kernel/kernel.bin
   ```

6. **Create OS image**
   ```bash
   cat boot/bootloader.bin boot/bootloaderstage2.bin kernel/kernel.bin > os.img
   ```

### Running

**With QEMU:**
```bash
qemu-system-x86_64 -drive format=raw,file=os.img
```

**With real hardware (advanced):**
```bash
dd if=os.img of=/dev/sdX bs=512
```
⚠️ **Warning**: Replace `/dev/sdX` with your USB drive. This will erase all data on the drive!

## Bootloader Architecture

### Stage 1 (512 bytes)
- Activates A20 line for extended memory access
- Initializes segment registers
- Loads Stage 2 from disk (sectors 2-6)
- Transfers control to Stage 2

### Stage 2 (2560 bytes)
- Loads kernel from disk (sectors 7+)
- Sets up GDT for 32-bit protected mode
- Creates page tables (PML4, PDPT, PD, PT)
- Enables PAE (Physical Address Extension)
- Activates Long Mode via EFER MSR
- Enables paging
- Loads 64-bit GDT
- Jumps to 64-bit kernel at 0x10000

## Kernel Features

### Terminal Driver
- **Resolution**: 80x25 characters
- **Memory**: VGA text buffer at 0xB8000
- **Functions**:
  - `print_char(char c)` - Print single character
  - `print_str(char *str)` - Print string
  - `clear_terminal()` - Clear screen

### Keyboard Driver
- **Protocol**: PS/2 polling mode
- **Ports**: 0x60 (data), 0x64 (status)
- **Scancode Set**: Set 1
- **Features**:
  - ASCII conversion
  - Shift key support (planned)
  - Special key handling (planned)

### Command Shell
- **Commands**:
  - `clear` - Clear the screen
  - (more commands coming soon)
- **Features**:
  - Command parsing with argument support
  - Unknown command error handling
  - Whitespace trimming

## Memory Map

```
0x00000 - 0x00500   BIOS Data Area
0x00500 - 0x07C00   Free
0x07C00 - 0x07E00   Stage 1 Bootloader (512 bytes)
0x07E00 - 0x08800   Stage 2 Bootloader (2560 bytes)
0x01000 - 0x02000   PML4 (Page Map Level 4)
0x02000 - 0x03000   PDPT (Page Directory Pointer Table)
0x03000 - 0x04000   PD (Page Directory)
0x04000 - 0x05000   PT (Page Table)
0x10000 - 0x?????   Kernel (loaded here)
0x90000             Stack pointer
0xB8000 - 0xB8FA0   VGA Text Buffer (4000 bytes)
```

## Development Roadmap

### Completed ✅
- [x] Stage 1 bootloader with A20 activation
- [x] Stage 2 bootloader with 64-bit transition
- [x] Basic VGA text output driver
- [x] PS/2 keyboard driver (polling)
- [x] Command parsing system
- [x] Basic shell with `clear` command

### In Progress 🚧
- [ ] Keyboard input integration with shell
- [ ] More shell commands (echo, help, etc.)
- [ ] Interrupt handling (IRQ1 for keyboard)

### Planned 📋
- [ ] Memory management (malloc/free)
- [ ] Process/task management
- [ ] File system (FAT32 or custom)
- [ ] System calls
- [ ] User mode applications
- [ ] Multitasking
- [ ] Network stack
- [ ] Graphics mode (VESA/GOP)

## Technical Details

### Bootloader
- **Architecture**: x86-64
- **Boot Mode**: Legacy BIOS (MBR)
- **Assembler**: NASM
- **Boot Signature**: 0xAA55

### Kernel
- **Language**: C
- **Mode**: 64-bit Long Mode
- **Compiler**: GCC with `-ffreestanding -nostdlib`
- **Entry Point**: `_start()` at 0x10000
- **No Standard Library**: All functions implemented from scratch

### Build System
- Manual compilation commands (Makefile planned)
- Binary output format for bootloader
- ELF format for kernel (converted to binary)

## Contributing

This is a learning project, but contributions are welcome! Areas where help is appreciated:
- Bug fixes
- Documentation improvements
- Driver development (disk, network, etc.)
- Feature implementation from the roadmap

## License

This project is open source and available for educational purposes.

## References

- [OSDev Wiki](https://wiki.osdev.org/)
- Intel 64 and IA-32 Architectures Software Developer's Manual
- [Writing a Simple Operating System from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)

## Author

Created as a learning project to understand operating system development from first principles.

---

**Status**: 🚧 Active Development

Last updated: February 2026
