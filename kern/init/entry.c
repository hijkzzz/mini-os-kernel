#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"

int kern_entry()
{
    // 初始化各模块
    init_debug();
    init_gdt();
    init_idt();

    // 测试字符驱动
    console_clear();
    console_write_color("Hello World!\n", rc_black, rc_white);

    //启动时钟中断，频率 5
    init_timer(5);

    //开中断
    asm volatile ("sti");

	return 0;
}

