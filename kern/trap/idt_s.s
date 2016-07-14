%macro ISR_NOERRCODE 1    ;指定参数数量
[GLOBAL isr%1]

isr%1:
    cli
    push 0     ;无效错误代码
    push %1    ;中断号
    jmp isr_common_stub
%endmacro


%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli
    push %1
    jmp isr_common_stub
%endmacro

; 定义中断处理函数
ISR_NOERRCODE  0 	; 0 #DE 除 0 异常
ISR_NOERRCODE  1 	; 1 #DB 调试异常
ISR_NOERRCODE  2 	; 2 NMI
ISR_NOERRCODE  3 	; 3 BP 断点异常
ISR_NOERRCODE  4 	; 4 #OF 溢出
ISR_NOERRCODE  5 	; 5 #BR 对数组的引用超出边界
ISR_NOERRCODE  6 	; 6 #UD 无效或未定义的操作码
ISR_NOERRCODE  7 	; 7 #NM 设备不可用(无数学协处理器)
ISR_ERRCODE    8 	; 8 #DF 双重故障(有错误代码)
ISR_NOERRCODE  9 	; 9 协处理器跨段操作
ISR_ERRCODE   10 	; 10 #TS 无效TSS(有错误代码)
ISR_ERRCODE   11 	; 11 #NP 段不存在(有错误代码)
ISR_ERRCODE   12 	; 12 #SS 栈错误(有错误代码)
ISR_ERRCODE   13 	; 13 #GP 常规保护(有错误代码)
ISR_ERRCODE   14 	; 14 #PF 页故障(有错误代码)
ISR_NOERRCODE 15 	; 15 CPU 保留
ISR_NOERRCODE 16 	; 16 #MF 浮点处理单元错误
ISR_ERRCODE   17 	; 17 #AC 对齐检查
ISR_NOERRCODE 18 	; 18 #MC 机器检查
ISR_NOERRCODE 19 	; 19 #XM SIMD(单指令多数据)浮点异常

; 20~31 Intel 保留
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
; 32～255 用户自定义
ISR_NOERRCODE 255

[EXTERN isr_handler]
[GLOBAL isr_common_stub]
isr_common_stub:
    pusha       ;通用寄存器
    mov ax, ds
    push eax    ;数据段描述符

    mov ax, 0x10 ;切换到内核数据段
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push esp    ;传入 pt_regs 结构体
    call isr_handler ;调用中断服务例程
    add esp, 4  ;释放参数

    pop ebx     ;恢复用户数据段
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx

    popa  ;弹出通用寄存器
    add esp, 8 ;弹出 ISR 号, Error Code
    iret  ;弹出 EIP EFLAGS ESP SS
.end

[GLOBAL idt_flush]
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret  ;弹出 EIP
.end
