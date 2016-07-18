#ifndef INCLUDE_MULTIBOOT_H_
#define INCLUDE_MULTIBOOT_H_

#include "types.h"

typedef
struct multiboot_t {
    uint32_t flags;            // Multiboot 的版本信息
    uint32_t mem_lower;        // 从 BIOS 获知的可用内存
    uint32_t mem_upper;

    uint32_t boot_device;      // 指出引导程序从哪个BIOS磁盘设备载入的OS映像
    uint32_t cmdline;          // 内核命令行
    uint32_t mods_count;       // boot 模块列表
    uint32_t mods_addr;

    uint32_t num;              // ELF 格式内核映像的 section 头表
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;            // 段字符串表索引

    // 内存分布的缓冲区的地址和长度
    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;    // 指出第一个驱动器结构的物理地址
    uint32_t drives_addr;      // 指出第一个驱动器这个结构的大小
    uint32_t config_table;     // ROM 配置表
    uint32_t boot_loader_name; // boot loader 的名字
    uint32_t apm_table;        // APM 表
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} __attribute__((packed)) multiboot_t;

typedef
struct mmap_entry_t {
    uint32_t size;             // size 是不含 size 自身变量的大小
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;

// 临时 multiboot 结构体指针
extern multiboot_t *tmp_mboot_ptr;
// 建立页表后的 multiboot 结构体指针
extern multiboot_t *glb_mboot_ptr;

#endif
