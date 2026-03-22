# Memory Subsystem — N_OS

Located in `kernel/drivers/memory/`.  
This subsystem handles all physical and virtual memory management for the N_OS kernel.

---

## Files

| File | Role |
|---|---|
| `Physical_memory_manager.c/.h` | Physical page allocator (PMM) |
| `Virtual_memory_manageur.c/.h` | Virtual address space mapper (VMM) |

---

## Physical Memory Manager (`PMM`)

### Overview

The PMM tracks the state of every physical 4 KiB page in RAM using a **bitmap** — one bit per page (0 = free, 1 = used).

The bitmap is placed immediately after `_kernel_end` in physical memory.

### Initialization — `PMM_init()`

Reads the **E820 memory map** left by the bootloader:

| Address | Content |
|---|---|
| `0x5FF8` | `uint16_t` — number of E820 entries |
| `0x6000` | Array of 24-byte E820 entries (`base`, `size`, `type`) |

**Steps performed:**

1. Walks the E820 map to determine total RAM (`RAM_Total`).
2. Computes `total_pages = RAM_Total / 4096` and `bitmap_size`.
3. Initializes the entire bitmap to `0xFF` (all pages marked used).
4. Marks E820 type-1 (usable) regions as free.
5. Re-marks the following regions as **protected**:

| Protected region | Reason |
|---|---|
| `0x0 – 0x100000` | BIOS, IVT, legacy hardware |
| `0x10000 – _kernel_end` | Kernel image |
| `_kernel_end – _kernel_end + bitmap` | PMM bitmap itself |

### API

```c
// Initialize the PMM. Must be called once at kernel startup, before any allocation.
void PMM_init(void);

// Allocate one physical page (4 KiB). Returns the physical address.
// Returns 0xFFFFFFFFFFFFFFFF if out of memory.
uint64_t PMM_alloc_page(void);

// Free a previously allocated page. The address must be 4 KiB-aligned.
// Returns 1 on success, 0 if the address is not page-aligned.
int PMM_free_page(uint64_t physical_address);
```

### Memory Layout

```
0x00000000  ┌────────────────────────────┐
            │  BIOS / Legacy (protected) │  0x0 – 0xFFFFF
0x00001000  ├────────────────────────────┤
            │  Page Tables (bootloader)  │  0x1000 – 0x4FFF
0x00010000  ├────────────────────────────┤
            │  Kernel Image (protected)  │  0x10000 – _kernel_end
            ├────────────────────────────┤
            │  PMM Bitmap (protected)    │  _kernel_end – _kernel_end + bitmap_size
            ├────────────────────────────┤
            │  Free usable RAM           │
            └────────────────────────────┘
```

---

## Virtual Memory Manager (`VMM`)

### Overview

The VMM walks the **x86-64 4-level page table hierarchy** (PML4 → PDPT → PD → PT) to map and unmap virtual addresses to physical addresses.

Tables that don't exist yet are allocated on-demand via `PMM_alloc_page()`.

All page table entries are created with flags `Present | Writable` (`0x03`).

### API

```c
// Map a virtual address to a physical address.
// Intermediate page tables are allocated automatically if missing.
// Returns 1 on success.
int VMM_map_page(uint64_t virtual_address, uint64_t physical_address);

// Unmap a virtual address and invalidate the TLB entry (INVLPG).
// Does NOT free intermediate page tables.
void VMM_unmap_page(uint64_t virtual_address);
```

### Address Decoding (x86-64)

```
Bits 63–48  Sign extension
Bits 47–39  PML4 index   (9 bits)
Bits 38–30  PDPT index   (9 bits)
Bits 29–21  PD index     (9 bits)
Bits 20–12  PT index     (9 bits)
Bits 11–0   Page offset  (12 bits)
```

---

## Dependencies

| Dependency | Used by |
|---|---|
| `kernel/lib/types.h` | Fixed-width integer types (`uint8_t`, `uint64_t`, …) |
| `kernel/lib/utils.h` | `invlpg()` helper for TLB invalidation |
| `PMM` | Called by VMM to allocate intermediate page tables |

---

## Usage Example

```c
// At kernel startup
PMM_init();

// Allocate a physical page and map it to a virtual address
uint64_t phys = PMM_alloc_page();
VMM_map_page(0xFFFF800000000000, phys);

// Later: unmap and free
VMM_unmap_page(0xFFFF800000000000);
PMM_free_page(phys);
```

---

## Known Limitations

- `VMM_unmap_page()` does not check whether intermediate tables are present before dereferencing — a page fault will occur if called on a virtual address that was never mapped.
- `VMM_unmap_page()` does not free empty intermediate page tables (PDPT, PD, PT).
- `PMM_alloc_page()` returns a raw physical address cast as a pointer; this is only safe because the bootloader sets up an identity mapping covering all of RAM.
- Page table entries use `Present | Writable` only — no user/supervisor, no NX bit.