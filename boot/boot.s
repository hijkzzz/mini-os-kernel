; MultiBoot 常量
MBOOT_HEADER_MAGIC  equ  0x1BADB002
MBOOT_PAGE_ALIGN    equ  1 << 0
MBOOT_MEM_INFO      equ  1 << 1
MBOOT_HEADER_FLAGS  equ  MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHEKSUM       equ  -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32]
section .text

; MultiBoot 头
dd MBOOT_HEADER_MAGIC;
dd MBOOT_HEADER_FLAGS;
dd MBOOT_CHEKSUM;

[GLOBAL start]
[GLOBAL glb_mboot_ptr]
[EXTERN kern_entry]              ; C函数入口

start:
    cli
    mov esp, STACK_TOP
    mov ebp, 0
    and esp, 0FFFFFFF0H         ; 栈对齐双字，提升储存效率
    mov [glb_mboot_ptr], ebx
    call kern_entry

stop:
    hlt
    jmp stop


section .bss
; 内核栈
stack:
    resb 32768
glb_mboot_ptr:
    resb 4

STACK_TOP equ $ - stack - 1     ; 内核栈栈顶
