[bits 16]
[org 0x7C00]

;activate the A20 line
in  al, 0x92
or  al, 0000_0010b
and al, 1111_1110b
out 0x92, al

;Init segments
xor ax, ax
mov ds, ax
mov es, ax

;load kernel in Real Mode
mov ah, 0x02
mov al, 10
mov ch, 0
mov cl, 2
mov dh, 0
mov bx, 0x1000
int 0x13
jc disk_error

jmp load_gdt

; 4. GDT
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

; load GDT and start protect mode
load_gdt:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:MainMode     ;flush pipeline

disk_error:
    hlt

; enable Protected Mode
[bits 32]
MainMode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000

    jmp 0x08:0x1000     ;jump to kernel

times 510-($-$$) db 0
dw 0xAA55