#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

#include "types.h"

void init_idt();

typedef
struct idt_entry_t {
    uint16_t base_low;
    uint16_t sel;    // 目标代码的段选择子
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
}__attribute__((packed)) idt_entry_t;

typedef
struct idt_ptr_t {
    uint16_t limit;
    uint32_t base;
}__attribute__((packed)) idt_ptr_t;

typedef
struct pt_regs_t {
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;    //中断号
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} pt_regs_t;


// 中断处理函数指针
typedef void (*interrupt_handler_t)(pt_regs_t *);

// 注册中断处理函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h);

// 调用中断处理
void isr_handler(pt_regs_t *regs);

// 声明中断处理函数
// 0-19 属于 CPU 的异常中断
void isr0();         // 0 #DE 除 0 异常
void isr1();         // 1 #DB 调试异常
void isr2();         // 2 NMI
void isr3();         // 3 BP 断点异常
void isr4();         // 4 #OF 溢出
void isr5();         // 5 #BR 对数组的引用超出边界
void isr6();         // 6 #UD 无效或未定义的操作码
void isr7();         // 7 #NM 设备不可用(无数学协处理器)
void isr8();         // 8 #DF 双重故障(有错误代码)
void isr9();         // 9 协处理器跨段操作
void isr10();         // 10 #TS 无效TSS(有错误代码)
void isr11();         // 11 #NP 段不存在(有错误代码)
void isr12();         // 12 #SS 栈错误(有错误代码)
void isr13();         // 13 #GP 常规保护(有错误代码)
void isr14();         // 14 #PF 页故障(有错误代码)
void isr15();         // 15 CPU 保留
void isr16();         // 16 #MF 浮点处理单元错误
void isr17();         // 17 #AC 对齐检查
void isr18();         // 18 #MC 机器检查
void isr19();         // 19 #XM SIMD(单指令多数据)浮点异常

// 20-31 Intel 保留
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

// 32～255 用户自定义异常
void isr32();        // 0 电脑系统计时器
void isr33();        // 1 键盘
void isr34();        // 2 与 IRQ9 相接，MPU-401 MD 使用
void isr35();        // 3 串口设备
void isr36();        // 4 串口设备
void isr37();        // 5 建议声卡使用
void isr38();        // 6 软驱传输控制使用
void isr39();        // 7 打印机传输控制使用
void isr40();        // 8 即时时钟
void isr41();        // 9 与 IRQ2 相接，可设定给其他硬件
void isr42();        // 10 建议网卡使用
void isr43();        // 11 建议 AGP 显卡使用
void isr44();        // 12 接 PS/2 鼠标，也可设定给其他硬件
void isr45();        // 13 协处理器使用
void isr46();        // 14 IDE0 传输控制使用
void isr47();        // 15 IDE1 传输控制使用

void isr255();       // 系统调用

#endif // INCLUDE_IDT_H
