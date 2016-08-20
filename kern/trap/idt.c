#include "idt.h"
#include "string.h"
#include "debug.h"
#include "common.h"

idt_entry_t idt_entries[256];

// IDTR
idt_ptr_t idt_ptr;

// 中断处理函数数组
interrupt_handler_t interrupt_handlers[256];

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

static void init_8259a();

extern void idt_flush(uint32_t);

void init_idt()
{
    // 初始化 8259A
    init_8259a();

    // 中断处理函数表、中断描述符表初始化
    bzero((uint8_t *)&interrupt_handlers, sizeof(interrupt_handlers));
    bzero((uint8_t *)&idt_entries, sizeof(idt_entries));

    // 填写中断描述符表
    // 0-32:  用于 CPU 的中断
    // 0x08 内核代码段选择子， 0x8E P = 1， DPL = 00
    idt_set_gate( 0, (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate( 1, (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate( 2, (uint32_t)isr2,  0x08, 0x8E);
    idt_set_gate( 3, (uint32_t)isr3,  0x08, 0x8E);
    idt_set_gate( 4, (uint32_t)isr4,  0x08, 0x8E);
    idt_set_gate( 5, (uint32_t)isr5,  0x08, 0x8E);
    idt_set_gate( 6, (uint32_t)isr6,  0x08, 0x8E);
    idt_set_gate( 7, (uint32_t)isr7,  0x08, 0x8E);
    idt_set_gate( 8, (uint32_t)isr8,  0x08, 0x8E);
    idt_set_gate( 9, (uint32_t)isr9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    // 32-47，自定义硬件中断
    idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)isr34, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)isr35, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)isr36, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)isr37, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)isr38, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)isr39, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)isr40, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)isr41, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)isr42, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)isr43, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)isr44, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)isr45, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)isr46, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)isr47, 0x08, 0x8E);

    // 80 用于实现系统调用
    idt_set_gate(80, (uint32_t)isr80, 0x08, 0xEE);

    idt_ptr.base = (uint32_t)&idt_entries;
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_flush((uint32_t)&idt_ptr);
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;

    idt_entries[num].flags = flags;
}

void isr_handler(pt_regs_t *regs)
{
    // 如果是硬件中断
    if (regs->int_no >= 32 && regs->int_no <= 47) {
        // 发送中断结束信号给 8259A
        // 单片 8259A 芯片只能处理 8 级中断
        // 大于等于 40 的中断号是由从片处理的
        if (regs->int_no >= 40) {
            // 从片 EOI
            outb(0xA0, 0x20);
        }
        // 主片 EOI
        outb(0x20, 0x20);
    }

    if (interrupt_handlers[regs->int_no]) {
         interrupt_handlers[regs->int_no](regs);
    } else {
        printk_color (rc_black, rc_blue, "Unhandled interrupt : %d\n", regs->int_no);
    }
}


void register_interrupt_handler(uint8_t n, interrupt_handler_t h)
{
    interrupt_handlers[n] = h;
}

static void init_8259a()
{
    // 两片级联的 8259A 芯片
    // 主片端口 0x20 0x21
    // 从片端口 0xA0 0xA1

    // 初始化主片、从片
    // 0001 0001，ICW1，电平触发，级联，要 ICW4
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2，设置主片 IRQ 从 0x20(32) 号中断开始
    outb(0x21, 0x20);

    // ICW2，设置从片 IRQ 从 0x28(40) 号中断开始
    outb(0xA1, 0x28);

    // ICW3，设置主片 IR2 引脚连接从片
    outb(0x21, 0x04);

    // ICW3，设置从片输出引脚和主片 IR2 号相连
    outb(0xA1, 0x02);

    // ICW4，设置主片和从片按照 8086 的方式工作
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // OCW1，设置允许中断
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}
