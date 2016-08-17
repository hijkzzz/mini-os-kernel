; 中断处理函数生成宏
%macro ISR_NOERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli
    push 0     ; 无效错误代码
    push %1    ; 中断号
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
ISR_NOERRCODE  0     ; 0 #DE 除 0 异常
ISR_NOERRCODE  1     ; 1 #DB 调试异常
ISR_NOERRCODE  2     ; 2 NMI
ISR_NOERRCODE  3     ; 3 BP 断点异常
ISR_NOERRCODE  4     ; 4 #OF 溢出
ISR_NOERRCODE  5     ; 5 #BR 对数组的引用超出边界
ISR_NOERRCODE  6     ; 6 #UD 无效或未定义的操作码
ISR_NOERRCODE  7     ; 7 #NM 设备不可用(无数学协处理器)
ISR_ERRCODE    8     ; 8 #DF 双重故障(有错误代码)
ISR_NOERRCODE  9     ; 9 协处理器跨段操作
ISR_ERRCODE   10     ; 10 #TS 无效TSS(有错误代码)
ISR_ERRCODE   11     ; 11 #NP 段不存在(有错误代码)
ISR_ERRCODE   12     ; 12 #SS 栈错误(有错误代码)
ISR_ERRCODE   13     ; 13 #GP 常规保护(有错误代码)
ISR_ERRCODE   14     ; 14 #PF 页故障(有错误代码)
ISR_NOERRCODE 15     ; 15 CPU 保留
ISR_NOERRCODE 16     ; 16 #MF 浮点处理单元错误
ISR_ERRCODE   17     ; 17 #AC 对齐检查
ISR_NOERRCODE 18     ; 18 #MC 机器检查
ISR_NOERRCODE 19     ; 19 #XM SIMD(单指令多数据)浮点异常

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

; 32～255 用户自定义，硬件中断
ISR_NOERRCODE 32     ; 0 电脑系统计时器
ISR_NOERRCODE 33     ; 1 键盘
ISR_NOERRCODE 34     ; 2 与 IRQ9 相接，MPU-401 MD 使用
ISR_NOERRCODE 35     ; 3 串口设备
ISR_NOERRCODE 36     ; 4 串口设备
ISR_NOERRCODE 37     ; 5 建议声卡使用
ISR_NOERRCODE 38     ; 6 软驱传输控制使用
ISR_NOERRCODE 39     ; 7 打印机传输控制使用
ISR_NOERRCODE 40     ; 8 即时时钟
ISR_NOERRCODE 41     ; 9 与 IRQ2 相接，可设定给其他硬件
ISR_NOERRCODE 42     ; 10 建议网卡使用
ISR_NOERRCODE 43     ; 11 建议 AGP 显卡使用
ISR_NOERRCODE 44     ; 12 接 PS/2 鼠标，也可设定给其他硬件
ISR_NOERRCODE 45     ; 13 协处理器使用
ISR_NOERRCODE 46     ; 14 IDE0 传输控制使用
ISR_NOERRCODE 47     ; 15 IDE1 传输控制使用

ISR_NOERRCODE 255    ; 系统调用

; 保存，恢复现场
[EXTERN isr_handler]
[GLOBAL isr_common_stub]
isr_common_stub:
    pusha               ; 保存通用寄存器
    mov ax, ds
    push eax            ; 保存数据段选择子

    mov ax, 0x10        ; 加载内核数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push esp            ; 传入 pt_regs 结构体
    call isr_handler    ; 调用中断服务例程
    add esp, 4          ; 清除传入参数

    pop ebx             ; 恢复原来的数据段选择子
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx

    popa                ; 通用寄存器 出栈
    add esp, 8          ; 中断号, Error Code 出栈
    iret                ; EIP，CS，EFLAGS，ESP，SS 出栈
.end

; 加载中断描述符表
[GLOBAL idt_flush]
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret                 ; EIP 出栈
.end


; 8259 端口号
INT_M_CTL	equ	0x20	; I/O port for interrupt controller         <Master>
INT_M_CTLMASK	equ	0x21	; setting bits in this port disables ints   <Master>
INT_S_CTL	equ	0xA0	; I/O port for second interrupt controller  <Slave>
INT_S_CTLMASK	equ	0xA1	; setting bits in this port disables ints   <Slave>

; 关中断
disable_irq:
	mov	ecx, [esp + 4]		; irq
	pushf
	cli
	mov	ah, 1
	rol	ah, cl			; ah = (1 << (irq % 8))
	cmp	cl, 8
	jae	disable_8		; disable irq >= 8 at the slave 8259
disable_0:
	in	al, INT_M_CTLMASK
	test	al, ah
	jnz	dis_already		; already disabled?
	or	al, ah
	out	INT_M_CTLMASK, al	; set bit at master 8259
	popf
	mov	eax, 1			; disabled by this function
	ret
disable_8:
	in	al, INT_S_CTLMASK
	test	al, ah
	jnz	dis_already		; already disabled?
	or	al, ah
	out	INT_S_CTLMASK, al	; set bit at slave 8259
	popf
	mov	eax, 1			; disabled by this function
	ret
dis_already:
	popf
	xor	eax, eax		; already disabled
	ret

; 开中断
enable_irq:
	mov	ecx, [esp + 4]		; irq
	pushf
	cli
	mov	ah, ~1
	rol	ah, cl			; ah = ~(1 << (irq % 8))
	cmp	cl, 8
	jae	enable_8		; enable irq >= 8 at the slave 8259
enable_0:
	in	al, INT_M_CTLMASK
	and	al, ah
	out	INT_M_CTLMASK, al	; clear bit at master 8259
	popf
	ret
enable_8:
	in	al, INT_S_CTLMASK
	and	al, ah
	out	INT_S_CTLMASK, al	; clear bit at slave 8259
	popf
	ret
