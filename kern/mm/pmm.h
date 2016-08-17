#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "multiboot.h"

// 内核在内存中的起始和结束地址
// 由链接脚本定义
// 只是个单纯的符号，用下面的形式声明
extern uint8_t kern_start[];
extern uint8_t kern_end[];

#define STACK_SIZE    8192
#define KERN_STACK_SIZE 2048      // 内核栈大小
#define PMM_MAX_SIZE  0x20000000  // 可用内存 512MB
#define PMM_PAGE_SIZE 0x1000      // 页面大小 4KB
#define PAGE_MAX_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)

// 动态分配物理页面数
extern uint32_t phy_page_count;

// 内核栈
extern char kern_stack[];
extern uint32_t kern_stack_top;

// 初始化内存管理
void init_pmm();

// 分配页面
uint32_t pmm_alloc_page();

// 释放页面
void pmm_free_page(uint32_t p);

// 内存分布状态
void show_memory_map();

#endif // INCLUDE_PMM_H
