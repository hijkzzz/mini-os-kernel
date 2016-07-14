#include "debug.h"
#include "gdt.h"
#include "idt.h"

int kern_entry()
{
    init_debug();
    init_gdt();
    init_idt();

    //测试字符驱动
    console_clear();
    console_write_color("Hello World!\n", rc_black, rc_white);

    //测试中断处理
    asm volatile ("int $0x3");
	return 0;
}

