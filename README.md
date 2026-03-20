# N_OS

A 64-bit operating system built from scratch with a custom bootloader, kernel, memory management, storage drivers, and a basic shell interface.

## Features

- **64-bit Long Mode**: Full x86-64 architecture support
- **Custom Bootloader**: Two-stage bootloader with A20 line activation, GDT setup, E820 memory detection, and protected/long mode transition
- **Physical Memory Manager**: Bitmap-based page frame allocator using E820 memory map
- **Virtual Memory Manager**: 4-level paging with identity mapping
- **Dynamic Allocator**: Custom `malloc`/`free` implementation (no stdlib)
- **ATA PIO Driver**: LBA28 disk I/O on primary bus
- **FAT32 Driver**: BPB parsing, cluster chaining, 8.3 filename support
- **Filesystem Abstraction**: Unified VFS-style interface over FAT32
- **VGA Text Mode Driver**: 80×25 character display with color support
- **PS/2 Keyboard Driver**: Polling-based input with scancode translation
- **Command Shell**: Basic shell with command parsing and execution
- **App Manager**: Application lifecycle and input routing layer
- **Modular Architecture**: Well-organized driver, memory, and library structure

## Project Structure

```
N_OS/
├── boot/
│   ├── bootloader.asm              # Stage 1 bootloader (512 bytes)
│   └── bootloaderstage2.asm        # Stage 2 bootloader (64-bit transition)
├── kernel/
│   ├── kernel.c                    # Kernel entry point
│   ├── linker.ld                   # Linker script
│   ├── Startup_management.c        # Boot sequence orchestration
│   ├── Startup_management.h
│   ├── apps/
│   │   ├── app_manageur.c          # Application lifecycle manager
│   │   └── app_manageur.h
│   ├── drivers/
│   │   ├── memory/
│   │   │   ├── Physical_memory_manager.c  # Bitmap-based PMM
│   │   │   ├── Physical_memory_manager.h
│   │   │   ├── Virtual_memory_manageur.c  # VMM / paging
│   │   │   └── Virtual_memory_manageur.h
│   │   ├── storage/
│   │   │   ├── ata_pio/
│   │   │   │   ├── ata.c           # ATA PIO driver (LBA28)
│   │   │   │   └── ata.h
│   │   │   ├── fat32/
│   │   │   │   ├── fat32.c         # FAT32 parser
│   │   │   │   └── fat32.h
│   │   │   └── filesysteme/
│   │   │       ├── filesysteme.c   # Filesystem abstraction layer
│   │   │       └── filesysteme.h
│   │   ├── terminal/
│   │   │   ├── terminal.c          # VGA text mode driver
│   │   │   ├── terminal.h
│   │   │   ├── terminal_commande_manageur.c  # Command parser
│   │   │   └── terminal_commande_manageur.h
│   │   └── userinput/
│   │       ├── keybord/
│   │       │   ├── keybord.c       # PS/2 keyboard driver
│   │       │   └── keybord.h
│   │       ├── inputmanageur.c     # Input event manager
│   │       └── inputmanageur.h
│   └── lib/
│       ├── malloc.c                # Dynamic memory allocator
│       ├── malloc.h
│       ├── strcmp.c                # String comparison
│       ├── strcmp.h
│       ├── types.h                 # Fixed-width integer types (uint8_t, etc.)
│       ├── utils.c                 # Misc utilities
│       └── utils.h
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
   # Kernel core
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/kernel.c -o kernel/kernel.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/Startup_management.c -o kernel/Startup_management.o

   # Apps
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/apps/app_manageur.c -o kernel/apps/app_manageur.o

   # Memory drivers
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/memory/Physical_memory_manager.c -o kernel/drivers/memory/Physical_memory_manager.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/memory/Virtual_memory_manageur.c -o kernel/drivers/memory/Virtual_memory_manageur.o

   # Storage drivers
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/storage/ata_pio/ata.c -o kernel/drivers/storage/ata_pio/ata.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/storage/fat32/fat32.c -o kernel/drivers/storage/fat32/fat32.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/storage/filesysteme/filesysteme.c -o kernel/drivers/storage/filesysteme/filesysteme.o

   # Terminal & input drivers
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/terminal/terminal.c -o kernel/drivers/terminal/terminal.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/terminal/terminal_commande_manageur.c -o kernel/drivers/terminal/terminal_commande_manageur.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/userinput/keybord/keybord.c -o kernel/drivers/userinput/keybord/keybord.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/drivers/userinput/inputmanageur.c -o kernel/drivers/userinput/inputmanageur.o

   # Libraries
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/lib/malloc.c -o kernel/lib/malloc.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/lib/strcmp.c -o kernel/lib/strcmp.o
   gcc -m64 -ffreestanding -nostdlib -nostdinc -c kernel/lib/utils.c -o kernel/lib/utils.o
   ```

4. **Link kernel**
   ```bash
   ld -m elf_x86_64 -T kernel/linker.ld -o kernel/kernel.elf \
     kernel/kernel.o \
     kernel/Startup_management.o \
     kernel/apps/app_manageur.o \
     kernel/drivers/memory/Physical_memory_manager.o \
     kernel/drivers/memory/Virtual_memory_manageur.o \
     kernel/drivers/storage/ata_pio/ata.o \
     kernel/drivers/storage/fat32/fat32.o \
     kernel/drivers/storage/filesysteme/filesysteme.o \
     kernel/drivers/terminal/terminal.o \
     kernel/drivers/terminal/terminal_commande_manageur.o \
     kernel/drivers/userinput/keybord/keybord.o \
     kernel/drivers/userinput/inputmanageur.o \
     kernel/lib/malloc.o \
     kernel/lib/strcmp.o \
     kernel/lib/utils.o
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
⚠️ **Warning**: Replace `/dev/sdX` with your actual USB drive device. This will erase all data on it!

## Bootloader Architecture

### Stage 1 (512 bytes)
- Activates A20 line for extended memory access
- Initializes segment registers
- Loads Stage 2 from disk (sectors 2–6)
- Transfers control to Stage 2

### Stage 2 (2560 bytes)
- Detects physical memory map via BIOS INT 0x15 / E820 (entry count at `0x5FF8`, table at `0x6000`)
- Loads kernel from disk (sectors 7+)
- Sets up GDT for 32-bit protected mode
- Creates 4-level page tables (PML4, PDPT, PD, PT) — identity mapping
- Enables PAE (Physical Address Extension)
- Activates Long Mode via EFER MSR
- Enables paging
- Loads 64-bit GDT
- Jumps to 64-bit kernel at `0x10000`

## Kernel Features

### Physical Memory Manager (PMM)
- **Algorithm**: Bitmap-based page frame allocator
- **Page size**: 4096 bytes
- **Source**: E820 memory map from Stage 2
- **Functions**: page allocation, deallocation, free zone marking

### Virtual Memory Manager (VMM)
- **Paging**: 4-level (PML4 → PDPT → PD → PT)
- **Mapping**: Identity mapping during early boot
- **Integration**: Works with PMM for physical frame allocation

### Dynamic Allocator
- Custom `malloc` / `free` — no standard library dependency
- Defined in `kernel/lib/malloc.c`

### ATA PIO Driver
- **Mode**: PIO (Programmed I/O), LBA28
- **Bus**: Primary (ports `0x1F0`–`0x1F7`)
- **Operations**: sector read / write

### FAT32 Driver
- BPB (BIOS Parameter Block) parsing
- Cluster chain traversal (FAT table)
- 8.3 short filename support

### Filesystem Abstraction
- Unified interface over FAT32
- Defined in `kernel/drivers/storage/filesysteme/`

### Terminal Driver
- **Resolution**: 80×25 characters
- **Memory**: VGA text buffer at `0xB8000`
- **Functions**: `print_char`, `print_str`, `clear_terminal`

### Keyboard Driver
- **Protocol**: PS/2 polling mode
- **Ports**: `0x60` (data), `0x64` (status)
- **Scancode Set**: Set 1
- ASCII conversion, shift key support

### Command Shell
- **Commands**: `clear` (more coming)
- Command parsing with argument support
- Unknown command error handling

### App Manager
- Application lifecycle management
- Input routing between shell and apps

## Memory Map

```
0x00000 - 0x00500   BIOS Data Area
0x00500 - 0x05FF7   Free
0x05FF8              E820 entry count
0x06000 - 0x06???   E820 memory map (24-byte entries)
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
- [x] Stage 2 bootloader with 64-bit long mode transition
- [x] E820 physical memory map detection
- [x] Basic VGA text output driver
- [x] PS/2 keyboard driver (polling)
- [x] Command parsing system
- [x] Basic shell with `clear` command
- [x] ATA PIO disk driver (LBA28)
- [x] FAT32 filesystem parser
- [x] Filesystem abstraction layer
- [x] Custom `malloc` / `free` allocator
- [x] Fixed-width types (`types.h`)

### In Progress 🚧
- [ ] A malloc system
- [ ] App manager — input handling & lifecycle

### Planned 📋
- [ ] Interrupt handling (IDT, IRQ1 for keyboard)
- [ ] More shell commands (`echo`, `help`, `ls`, etc.)
- [ ] Process / task management
- [ ] System calls
- [ ] User mode applications
- [ ] Multitasking / scheduler
- [ ] Network stack
- [ ] Graphics mode (VESA/GOP)
- [ ] Makefile build system
- [ ] Physical Memory Manager — bitmap init & E820 free zone marking
- [ ] Virtual Memory Manager — full page mapping API

## Technical Details

### Bootloader
- **Architecture**: x86-64
- **Boot Mode**: Legacy BIOS (MBR)
- **Assembler**: NASM
- **Boot Signature**: `0xAA55`

### Kernel
- **Language**: C
- **Mode**: 64-bit Long Mode
- **Compiler**: GCC with `-ffreestanding -nostdlib -nostdinc`
- **Entry Point**: `_start()` at `0x10000`
- **No Standard Library**: all types and utilities implemented from scratch

### Build System
- Manual compilation commands (Makefile planned)
- Binary output for bootloader stages
- ELF format for kernel, converted to flat binary via `objcopy`

## Contributing

This is a learning project, but contributions are welcome! Areas where help is appreciated:
- Bug fixes and code review
- Documentation improvements
- Driver development (network, graphics, etc.)
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

Last updated: March 2026