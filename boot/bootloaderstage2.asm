[bits 16]
[org 0x7E00]

;load kernel from disk
mov ah, 0x02
mov al, 10
mov ch, 0
mov cl, 7
mov dh, 0
mov bx, 0x10000 ;load kernel at 0x10000
int 0x13
jc disk_error

jmp setup_gdt

;GDT for 32-bit protected mode
gdt_start:
    dq 0x0

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

;setup GDT and enter protected mode
setup_gdt:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

[bits 32]
protected_mode:
    ;init segments for 32-bit
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000

    ;create page tables for 64-bit
    ;clear memory from 0x1000 to 0x5000
    mov edi, 0x1000
    mov ecx, 0x1000
    xor eax, eax
    rep stosd

    ;PML4 at 0x1000 points to PDPT
    mov dword [0x1000], 0x2003

    ;PDPT at 0x2000 points to PD
    mov dword [0x2000], 0x3003

    ;PD at 0x3000 points to PT
    mov dword [0x3000], 0x4003

    ;PT at 0x4000 maps first 2MB identity
    mov edi, 0x4000
    mov ebx, 0x00000003
    mov ecx, 512
.fill_pt:
    mov dword [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .fill_pt

    ;load CR3 with PML4 address
    mov eax, 0x1000
    mov cr3, eax

    ;enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ;enable long mode via EFER register
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ;enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ;load 64-bit GDT
    lgdt [gdt64_descriptor]
    jmp 0x08:long_mode

;GDT for 64-bit long mode
gdt64_start:
    dq 0x0000000000000000

gdt64_code:
    dq 0x00209A0000000000

gdt64_data:
    dq 0x0000920000000000

gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dd gdt64_start

[bits 64]
long_mode:
    ;init segments for 64-bit
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x90000

    ;jump to kernel
    jmp 0x10000

disk_error:
    hlt

;fill to exactly 5 sectors (2560 bytes)
times 2560-($-$$) db 0