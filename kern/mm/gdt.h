#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#include "types.h"

typedef
struct gdt_entry_t {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;        //1 P, 2 DPL, 1, 4 Type
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef
struct gdt_ptr_t {
    uint16_t limit;
    uint32_t base;

} __attribute__((packed)) gdt_ptr_t;

// A struct describing a Task State Segment.
typedef
struct tss_entry
{
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // Unused...
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         // The value to load into ES when we change to kernel mode.
   uint32_t cs;         // The value to load into CS when we change to kernel mode.
   uint32_t ss;         // The value to load into SS when we change to kernel mode.
   uint32_t ds;         // The value to load into DS when we change to kernel mode.
   uint32_t fs;         // The value to load into FS when we change to kernel mode.
   uint32_t gs;         // The value to load into GS when we change to kernel mode.
   uint32_t ldt;        // Unused...
   uint16_t trap;
   uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

// 初始化
void init_gdt();

extern void gdt_flush(uint32_t);
extern void tss_flush();

// 设置内核栈
void set_kernel_stack(uint32_t stack);

#endif // INCLUDE_GDT_H
