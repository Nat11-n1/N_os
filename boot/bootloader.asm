[bits 16]
[org 0x7C00]

;activate the A20 line
in  al, 0x92
or  al, 0000_0010b
and al, 1111_1110b
out 0x92, al

;init segments
xor ax, ax
mov ds, ax
mov es, ax

;load stage 2 from disk
mov ah, 0x02        ;function: read sectors
mov al, 5           ;number of sectors to read (stage 2)
mov ch, 0           ;cylinder 0
mov cl, 2           ;sector 2 (right after bootloader)
mov dh, 0           ;head 0
mov bx, 0x7E00      ;load stage 2 right after bootloader
int 0x13
jc disk_error

;jump to stage 2
jmp 0x0000:0x7E00

disk_error:
    mov si, error_msg
    call print_string
    hlt

;print string function
print_string:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

error_msg db "Disk error!", 0

times 510-($-$$) db 0
dw 0xAA55