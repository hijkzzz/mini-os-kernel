#include "vmm.h"
#include "debug.h"

// 内核页目录区域，对齐到 4KB
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

// 内核页表区域，对齐到 4KB
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

// 初始化页表
void init_vmm()
{
    uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);

    // 填写页目录，映射 VM 0xC0000000 ~ 0xC0000000 +  512 MB => PM 0 ~ 512MB
    for (uint32_t i = kern_pte_first_idx, j = 0; j < PTE_COUNT; i++, j++) {
        // MMU 需要的是物理地址，所以减去内核偏移地址
        pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
    }

    pte_t* pte = (pte_t *)pte_kern;
    // 填写页表，页 0 是无效页
    for (uint32_t i = 1; i < PTE_COUNT * PTE_SIZE; i++) {
        pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    // 设置缺页处理函数
    register_interrupt_handler(14, page_fault);

    // 加载页表需要物理地址
    uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;
    switch_pgd(pgd_kern_phy_addr);
}

// 更换当前页目录
void switch_pgd(uint32_t pd)
{
    // 页目录地址写入 cr3
    asm volatile ("mov %0, %%cr3" : : "r"(pd));
}

// 缺页异常处理
void page_fault(pt_regs_t *regs)
{
    // cr2 保存当前访问地址
    uint32_t cr2;
    asm volatile ("mov %%cr2, %0" : "=r"(cr2));
    printk("Page fault at 0x%x, virtual address 0x%x\n", regs->eip, cr2);
    printk("Error code : %x\n", regs->err_code);

    // bit 0 为 0 表示页不存在
    if (!(regs->err_code & 0x1)) {
        printk_color(rc_black, rc_red, "The page wasn't present.\n");
    }

    // bit 1 为 0 表示读错误，为 1 为写错误
    if (regs->err_code & 0x2) {
        printk_color(rc_black, rc_red, "Write error.\n");
    } else {
        printk_color(rc_black, rc_red, "Read error.\n");
    }

    // bit 2 为 1 表示在用户模式打断的，为 0 是在内核模式打断的
    if (regs->err_code & 0x4) {
        printk_color(rc_black, rc_red, "User mode.\n");
    } else {
        printk_color(rc_black, rc_red, "Kernel mode.\n");
    }

    // bit 3 为 1 表示错误是由保留位覆盖造成的
    if (regs->err_code & 0x8) {
        printk_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
    }

    // bit 4 为 1 表示错误发生在取指令的时候
    if (regs->err_code & 0x10) {
        printk_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
    }

    while (1);
}

// 映射虚拟地址到物理地址，flags 为页权限
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// 取消映射
void unmap(pgd_t *pgd_now, uint32_t va);

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

