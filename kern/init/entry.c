#include "common.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "clock.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "sched.h"
#include "keyboard.h"
#include "tty.h"

// 内核初始化
void kern_init();

// 开启分页后的 multiboot 结构体指针
multiboot_t *glb_mboot_ptr;

// 开启分页后的 内核栈
char kern_stack[STACK_SIZE];
uint32_t kern_stack_top;

// 临时页表，使用 0 - 640 KB 空闲内存
__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_hign = (pgd_t *)0x3000;

// 内核入口，建立临时页表，调用内核初始化函数
__attribute__((section(".init.text"))) void kern_entry()
{
    pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
    pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_hign | PAGE_PRESENT | PAGE_WRITE;

    // 映射内核虚拟地址 4MB 到物理地址的前 4MB， 保证入口函数正常运行
    for (int i = 0; i < 1024; i++) {
        pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    // 映射 0xC0000000 后面的 4MB 到物理地址前 4MB
    for (int i = 0; i < 1024; i++) {
        pte_hign[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    // 设置临时页表
    asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

    uint32_t cr0;

    // 启用分页，将 cr0 寄存器的 31位 置 1
    asm volatile ("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" : : "r" (cr0));

    // 更新内核栈地址
    kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
    asm volatile ("mov %0, %%esp\n\t"
            "xor %%ebp, %%ebp" : : "r" (kern_stack_top));

    // 更新全局 multiboot_t 指针
    glb_mboot_ptr = (multiboot_t *)((uint32_t)tmp_mboot_ptr + PAGE_OFFSET);

    // 内核初始化
    kern_init();
}

// 内核初始化
void kern_init()
{
    // 初始化字符显示
    init_debug();
    console_clear();
    // 初始化全局描述符表
    init_gdt();
    // 初始化中断描述符表、可编程中断控制器
    init_idt();
    // 初始化时钟中断
    init_timer(100);
    // 初始化物理内存管理
    init_pmm();
    // 初始化页表
    init_vmm();
    // 初始化内核堆
    init_heap();
    // 初始化进程调度
    init_sched();
    // 初始化键盘
    init_keyboard();

    // 显示可用内存
    printk_color(rc_black, rc_red, "\nfree physical memory: %u MB\n", phy_page_count * 4 / 1024);

    // 测试堆内存管理
    printk_color(rc_black, rc_magenta, "\ntest kmalloc() && kfree() now ...\n\n");

    void *addr2 = kmalloc(500);
    printk("kmalloc   500 byte in 0x%X\n", addr2);
    void *addr3 = kmalloc(5000);
    printk("kmalloc  5000 byte in 0x%X\n", addr3);
    void *addr4 = kmalloc(50000);
    printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

    printk("free mem in 0x%X\n", addr2);
    kfree(addr2);
    printk("free mem in 0x%X\n", addr3);
    kfree(addr3);
    printk("free mem in 0x%X\n\n", addr4);
    kfree(addr4);

    // 初始化 TTY 进程
    kernel_thread(task_tty, NULL);

    // 开启中断
    //sti();

    while (1) {
        schedule();
    }
}

