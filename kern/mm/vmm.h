#ifndef INCLUDE_VMM_H
#define INCLUDE_VMM_H

#include "types.h"
#include "idt.h"

// 偏移地址
#define PAGE_OFFSET  0xC0000000

// 页表项 [31~12 页帧地址, 3 AVL, 00,  D, A, 00, U/S, R/W, P]
// A 访问位，D 修改位，P 存在位
#define PAGE_PRESENT 0x1
#define PAGE_WRITE   0x2
#define PAGE_USER    0x4

// 页帧大小
#define PAGE_SIZE    4096

// 地址转换为目录项索引，3FF == 1111111111
#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)
// 地址转换为页表项索引
#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)

// 获取页内偏移
#define OFFSET_INDEX(x) ((x) & 0xFFF)

// 页目录项、页表项 类型
typedef uint32_t pgd_t;
typedef uint32_t pte_t;

// 页表成员数
#define PGD_SIZE (PAGE_SIZE / sizeof(pgd_t))
#define PTE_SIZE (PAGE_SIZE / sizeof(pte_t))

// 页表数，512 MB / 4MB
#define PTE_COUNT (512 / 4)

// 页目录区域
extern pgd_t pgd_kern[PGD_SIZE];

// 初始化页表
void init_vmm();

// 更换当前页目录
void switch_pgd(uint32_t pd);

// 映射虚拟地址到物理地址，flags 为页权限
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// 取消映射
void unmap(pgd_t *pgd_now, uint32_t va);

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

// 缺页异常处理
void page_fault(pt_regs_t *regs);

#endif  // INCLUDE_VMM_H
