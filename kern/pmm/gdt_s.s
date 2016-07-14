[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    ;kernel ds 索引号为 2
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;kernel cs 索引号为 1
    jmp 0x08:.flush

.flush
    ret
