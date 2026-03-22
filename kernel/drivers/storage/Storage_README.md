kernel/drivers/storage
Low-level storage drivers for the kernel. Provides disk access via ATA PIO, FAT32 filesystem parsing, and a filesystem abstraction layer.
Structure
kernel/drivers/storage/
├── ata_pio/
│   ├── ata.h
│   └── ata.c          # ATA PIO driver (sector read/write)
├── fat32/
│   ├── fat32.h
│   └── fat32.c        # FAT32 parser (clusters, FAT chain, directories)
└── filesystem/
    ├── filesystem.h
    └── filesystem.c   # Abstraction layer (list, open, exec)
Modules
ata_pio — ATA Driver (Programmed I/O)
Direct disk communication via x86 I/O ports.
Function
Description
ata_init()
Detects the master drive on the primary bus (0x1F0)
ata_wait()
Polls the status register until BSY=0 and DRDY=1
ata_read_sector(lba, buf)
Reads 1 sector (512 bytes) at the given LBA address
ata_write_sector(lba, buf)
Writes 1 sector (512 bytes) at the given LBA address
Addressing mode: LBA28 (supports up to 128 GB)
Bus: Primary ATA — ports 0x1F0–0x1F7
fat32 — FAT32 Filesystem
Parses the BPB (BIOS Parameter Block) and navigates FAT32 structures.
Function
Description
fat32_init()
Reads sector 0 and extracts BPB metadata
fat32_read_cluster(cluster, buf)
Reads all sectors of a cluster into the buffer
fat32_next_cluster(cluster)
Returns the next entry in the FAT chain
fat32_list_dir(cluster, count)
Lists directory entries (prints 8.3 filenames)
fat32_read_file(dir, name, buf)
Finds a file in a directory and reads its content
Global variables available after fat32_init():
Variable
Description
bytes_per_sector
Sector size (typically 512)
sectors_per_cluster
Number of sectors per cluster
fat_start
First sector of the FAT
data_start
First sector of the data region
root_cluster
Root directory cluster number
filesystem — Abstraction Layer
High-level interface on top of FAT32. Maintains a current directory state.
Function
Description
list_files()
Prints files in the current directory with a file count
open_file(name)
Loads a file into memory and clears the terminal
exe_file(name)
Loads and executes a binary file from memory
State variable:
current_dir — cluster of the current directory (initialize with root_cluster after fat32_init())
Dependencies
Dependency
Path
Usage
outb / inb / inw / outw
arch/x86/io.h (assumed)
I/O port access
print_str / print_char / return_to_line
kernel/drivers/terminal/terminal.h
Terminal output
issamestr
lib/strcmp.h
Filename comparison
Initialization Order
ata_init();                    // 1. Detect and initialize the disk
fat32_init();                  // 2. Parse the BPB, compute fat_start and data_start
current_dir = root_cluster;    // 3. Set current directory to root
Known Limitations
ATA PIO only — no DMA support. Transfers block the CPU (busy-wait polling).
8.3 filenames only — LFN (long filename) entries are not handled.
Fixed cluster buffer — dir_buffer in filesystem.c is statically allocated for 128 sectors. Larger clusters will overflow.
No sector cache — every read triggers a direct disk access.
filesfound bug — *filesfound++ in fat32_list_dir increments the pointer, not the value. Should be (*filesfound)++.
exe_file / open_file bug — void fat32_read_file(...) is a prototype declaration, not a function call. Remove the leading void.