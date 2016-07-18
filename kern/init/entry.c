#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"

// 内核初始化
void kern_init();

// 开启分页后的 multiboot 结构体指针
multiboot_t *glb_mboot_ptr;

// 开启分页后的 内核栈
char kern_stack[STACK_SIZE];

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
    uint32_t kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
    asm volatile ("mov %0, %%esp\n\t"
            "xor %%ebp, %%ebp" : : "r" (kern_stack_top));

    // 更新全局 multiboot_t 指针
    glb_mboot_ptr = tmp_mboot_ptr + PAGE_OFFSET;

    // 内核初始化
    kern_init();
}

// 内核初始化
void kern_init()
{
    // 初始化字符显示
    init_debug();

    // 测试字符显示
    console_clear();
    console_write_color("Hello World!\n", rc_black, rc_white);

    // 初始化全局描述符表，中断描述符表，可编程中断控制器
    init_gdt();
    init_idt();

    // 初始化时钟
    init_timer(3);

    // 显示内核加载地址
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

    // 初始化页表
    init_vmm();

    while (1) {
        asm volatile ("hlt");
    }
}

