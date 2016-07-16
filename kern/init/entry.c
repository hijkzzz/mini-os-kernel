#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "timer.h"

int kern_entry()
{
    // 初始化字符驱动，全局描述符表，中断描述符表，可编程中断控制器
    init_debug();
    init_gdt();
    init_idt();

    // 测试字符驱动
    console_clear();
    console_write_color("Hello World!\n", rc_black, rc_white);

    // 初始化时钟
    init_timer(3);

    // 开中断
    //asm volatile ("sti");

    // 内存布局
    printk("kernel in memory start: 0x%08X\n", kern_start);
    printk("kernel in memory end:   0x%08X\n", kern_end);
    printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
    show_memory_map();

    // 初始化内存管理
    init_pmm();

    // 测试内存管理
    uint32_t allc_addr = NULL;
    printk_color(rc_black, rc_light_brown, "Test Physical Memory Alloc :\n");
    allc_addr = pmm_alloc_page();
    printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
    allc_addr = pmm_alloc_page();
    printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
    allc_addr = pmm_alloc_page();
    printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
    allc_addr = pmm_alloc_page();
    printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n\n", allc_addr);

    return 0;
}

