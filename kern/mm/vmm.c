#include "vmm.h"
#include "debug.h"
#include "pmm.h"
#include "string.h"
#include "heap.h"

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

// 映射虚拟地址到物理地址，flags 为页帧权限
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    // 从页目录获取页表
    pte_t *pte = (pte_t *)(ROUNDDOWN(pgd_now[pgd_idx], PAGE_SIZE));
    if (!pte) {
        // 分配新页表
        pte = (pte_t *)pmm_alloc_page();
        pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;

        // 页表清空
        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
        bzero(pte, PAGE_SIZE);
    } else {
        // 加上内核偏移地址
        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
    }

    // 填写页表项
    pte[pte_idx] = (ROUNDDOWN(pa, PAGE_SIZE) | flags);
    // 更新快表
    asm volatile ("invlpg (%0)" : : "a"(va));
}

// 取消映射
void unmap(pgd_t *pgd_now, uint32_t va)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    // 从页目录获取页表
    pte_t *pte = (pte_t *)(ROUNDDOWN(pgd_now[pgd_idx], PAGE_SIZE));
    if (!pte) {
        return;
    }

    // 加上内核偏移地址
    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

    // 清空页表项
    pte[pte_idx] = 0;
    // 更新快表
    asm volatile ("invlpg (%0)" : : "a"(va));
}

// 获取虚拟地址对应的物理地址，失败返回 0
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    // 从页目录获取页表
    pte_t *pte = (pte_t *)(ROUNDDOWN(pgd_now[pgd_idx], PAGE_SIZE));
    if (!pte) {
        return 0;
    }

    // 加上内核偏移地址
    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

    if (pte[pte_idx] != 0 && pa) {
        *pa = ROUNDDOWN(pte[pte_idx], PAGE_SIZE);
        return 1;
    }

    return 0;
}

// 克隆页目录，返回页目录地址
uint32_t clone_pgd(pgd_t *src)
{
    // 加上内核偏移地址
    src = (pgd_t *)((uint32_t)src + PAGE_OFFSET);

    // 分配空间
    uint32_t phys = pmm_alloc_page();
    pgd_t *dir = (pte_t *)(phys + PAGE_OFFSET);
    bzero(dir, PAGE_SIZE);

    for (int i = 0; i < PGD_SIZE; ++i) {
        if (!src[i]) continue;

        // 如果是内核区域
        if (src[i] == pgd_kern[i])
            dir[i] = pgd_kern[i];
        else {
            uint32_t phys = clone_pte(
                    (page_helper_t *)ROUNDDOWN(src[i], PAGE_SIZE));
            dir[i] = (phys | PAGE_PRESENT | PAGE_USER | PAGE_WRITE);
        }
    }

    return phys;
}

// 克隆页表，返回页表地址
uint32_t clone_pte(page_helper_t *src)
{
    src = (page_helper_t *)((uint32_t)src + PAGE_OFFSET);

    uint32_t phys = pmm_alloc_page();
    page_helper_t *table = (page_helper_t *)(phys + PAGE_OFFSET);
    bzero(table, PAGE_SIZE);

    int i;
    for (i = 0; i < PTE_SIZE; ++i) {
        if (!src[i].present) continue;
        // 页帧内存
        uint32_t phys2 = pmm_alloc_page();
        // 填入物理地址
        table[i].frame = (phys2 >> 12);

        if (src[i].present) table[i].present = 1;
        if (src[i].rw)      table[i].rw = 1;
        if (src[i].user)    table[i].user = 1;
        if (src[i].accessed)table[i].accessed = 1;
        if (src[i].dirty)   table[i].dirty = 1;

        copy_page_physical((src[i].frame << 12), (table[i].frame << 12));
    }
    return phys;
}

void copy_page_physical(uint32_t src, uint32_t dst)
{
    memcpy((uint8_t *)(dst + PAGE_OFFSET), (uint8_t *)(src + PAGE_OFFSET), PAGE_SIZE);
}
