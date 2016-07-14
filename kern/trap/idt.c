#include "idt.h"
#include "string.h"
#include "debug.h"

idt_entry_t idt_entries[256];

//IDTR
idt_ptr_t idt_ptr;

//中断处理函数数组
interrupt_handler_t interrupt_handlers[256];

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

//定义在 idt_s.s 中的 idt 加载函数
extern void idt_flush(uint32_t);

void init_idt()
{
    //清零
    bzero((uint8_t *)&interrupt_handlers, sizeof(interrupt_handlers));
    bzero((uint8_t *)&idt_entries, sizeof(idt_entries));

    //填中断描述符表
    // 0-32:  用于 CPU 的中断处理
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

    // 255 用于实现系统调用
    idt_set_gate(255, (uint32_t)isr255, 0x08, 0x8E);

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

