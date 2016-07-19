#include "pmm.h"
#include "debug.h"
#include "vmm.h"

// 物理内存页面管理栈
static uint32_t pmm_stack[PAGE_MAX_SIZE + 1];
static uint32_t pmm_stack_top;

uint32_t phy_page_count;

void init_pmm()
{
    // GRUB 提供的内存分布信息
    uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
    uint32_t mmap_length = glb_mboot_ptr->mmap_length;

    uint32_t phy_kern_end = kern_end - PAGE_OFFSET;
    for (mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
            (uint32_t)mmap < mmap_addr + mmap_length; ++mmap) {
        // 如果是有效物理内存
        if (mmap->type == 1) {
            uint32_t begin = mmap->base_addr_low < (uint32_t)phy_kern_end ? (uint32_t)phy_kern_end : mmap->base_addr_low;
            uint32_t end = mmap->base_addr_low + mmap->length_low;

            begin = ROUNDUP(begin, PMM_PAGE_SIZE);
            end   = ROUNDDOWN(end, PMM_PAGE_SIZE);

            // 检查是否和内核空间重叠
            if (end < (uint32_t)phy_kern_end) {
                continue;
            }

            while (begin < end) {
                pmm_free_page(begin);
                begin += PMM_PAGE_SIZE;
                phy_page_count++;
            }
        }
    }
}

uint32_t pmm_alloc_page()
{
    assert(pmm_stack_top != 0, "out of memory");
    return pmm_stack[pmm_stack_top--];
}

void pmm_free_page(uint32_t p)
{
    assert(pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack");
    pmm_stack[++pmm_stack_top] = p;
}

void show_memory_map()
{
    // GRUB 提供的内存信息
    uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
    uint32_t mmap_length = glb_mboot_ptr->mmap_length;

    printk("\nMemory map:\n");

    for (mmap_entry_t *mmap = (mmap_entry_t *)(mmap_addr + PAGE_OFFSET);
            (uint32_t)mmap < (mmap_addr + mmap_length + PAGE_OFFSET); ++mmap) {
        printk("base_addr = 0x%08X, length = 0x%08X, type = 0x%X\n",
            (uint32_t)mmap->base_addr_low, (uint32_t)mmap->length_low,
            (uint32_t)mmap->type);
    }
}
