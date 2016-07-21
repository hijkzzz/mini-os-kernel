#ifndef INCLUDE_HEAP_H
#define INCLUDE_HEAP_H

#include "types.h"

// 堆起始地址，内核页表映射区域为 0xC0000000 ~ 0xDFFFFFFF
#define HEAP_START 0xE0000000

// 内存块管理结构
typedef
struct header {
    struct header *prev;
    struct header *next;
    uint32_t allocated : 1;  // 内存是否被申请
    uint32_t length : 31;
} header_t;

// 初始化堆
void init_heap();

void *kmalloc(uint32_t len);

void kfree(void *p);

#endif // INCLUDE_HEAP_H
