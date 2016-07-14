[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    ;内核数据段索引号为 2
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;内核代码段索引号为 1
    jmp 0x08:.flush

.flush
    ret
