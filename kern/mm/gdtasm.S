[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    ; 更新数据段
    ; 内核数据段，索引号为 2
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 更新代码段
    ; 内核代码段，索引号为 1
    jmp 0x08:.flush

.flush
    ret

[GLOBAL tss_flush]    ; Allows our C code to call tss_flush().
tss_flush:
   mov ax, 0x2B      ; Load the index of our TSS structure - The index is
                     ; 0x28, as it is the 5th selector and each is 8 bytes
                     ; long, but we set the bottom two bits (making 0x2B)
                     ; so that it has an RPL of 3, not zero.
   ltr ax            ; Load 0x2B into the task state register.
   ret
