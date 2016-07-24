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
