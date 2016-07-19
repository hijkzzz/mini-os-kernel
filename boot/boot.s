; 1. CS 指向基地址为 0x00000000, 限长为 4G – 1 的代码段描述符
; 2. DS ,SS ,ES ,FS ,GS 指向基地址为0x00000000, 限长为 4G – 1 的数据段描述符
; 3. A20 地址线已经打开
; 4. 页机制被禁止
; 5. 中断被禁止
; 6. EAX = 0x2BADB002 MBOOT_HEADER_MAGIC  equ  0x1BADB002
; 7. 系统信息和启动信息块的线性地址保存在 EBX中

MBOOT_HEADER_MAGIC  equ  0x1BADB002
MBOOT_PAGE_ALIGN    equ  1 << 0
MBOOT_MEM_INFO      equ  1 << 1
MBOOT_HEADER_FLAGS  equ  MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHEKSUM       equ  -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[BITS 32]
section .init.text

; 在代码段的起始位置设置符合 Multiboot 规范的标记
dd MBOOT_HEADER_MAGIC;
dd MBOOT_HEADER_FLAGS;
dd MBOOT_CHEKSUM;

[GLOBAL start]
[GLOBAL tmp_mboot_ptr]          ; 声明临时 multiboot 结构体指针
[EXTERN kern_entry]             ; C 代码的入口函数

start:
    cli
    mov esp, STACK_TOP
    mov ebp, 0
    and esp, 0FFFFFFF0H         ; 栈顶对齐 16 字节，提升储存效率
    mov [tmp_mboot_ptr], ebx    ; 保存 multiboot 结构体指针
    call kern_entry

; 开启分页前的临时数据段
section .init.data
stack:
    times 1024 db 0             ; 临时内核栈大小 1024B
STACK_TOP equ $ - stack - 1     ; 临时内核栈栈顶

tmp_mboot_ptr:
    dd 0

