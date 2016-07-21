/* [ Allocation ]
 * Search the index table to find the smallest hole that will fit the requested size. As the table is ordered, this just entails iterating through until we find a hole which will fit.
 *     If we didn't find a hole large enough, then:
 *     Expand the heap.
 *     If the index table is empty (no holes have been recorded) then add a new entry to it.
 *     Else, adjust the last header's size member and rewrite the footer.
 *     To ease the number of control-flow statements, we can just recurse and call the allocation function again, trusting that this time there will be a hole large enough.
 * Decide if the hole should be split into two parts. This will normally be the case - we usually will want much less space than is available in the hole. The only time this will not happen is if there is less free space after allocating the block than the header/footer takes up. In this case we can just increase the block size and reclaim it all afterwards.
 * If the block should be page-aligned, we must alter the block starting address so that it is and create a new hole in the new unused area.
 *     If it is not, we can just delete the hole from the index.
 * Write the new block's header and footer.
 * If the hole was to be split into two parts, do it now and write a new hole into the index.
 * Return the address of the block + sizeof(header_t) to the user.
 */

#include "heap.h"
#include "vmm.h"
#include "pmm.h"

// 申请内存块
static void alloc_chunk(uint32_t start, uint32_t len);

// 释放内存块
static void free_chunk(header_t *chunk);

// 切分内存块
static void split_chunk(header_t *chunk, uint32_t len);

// 合并内存块
static void glue_chunk(header_t *chunk);

static uint32_t heap_max = HEAP_START;

// 内存块管理头指针
static header_t *heap_first;

void init_heap()
{
    heap_first = 0;
}

void *kmalloc(uint32_t len)
{
    // 加上头大小
    len += sizeof(header_t);

    header_t *cur_header = heap_first;
    header_t *prev_header = 0;

    // 遍历 heap 链表
    while (cur_header) {
        // 如果大小合适
        if (cur_header->allocated == 0 && cur_header->length >= len) {
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            return (void *)((uint32_t)cur_header + sizeof(header_t));
        }

        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    // 没找到合适的内存块
    uint32_t chunk_start;
    if (prev_header) {
         chunk_start = (uint32_t)prev_header + prev_header->length;
    // 第一次分配
    } else {
         chunk_start = HEAP_START;
         heap_first = (header_t *) chunk_start;
    }

    // 扩展堆内存区域
    alloc_chunk(chunk_start, len);

    cur_header = (header_t *)chunk_start;
    cur_header->allocated = 1;
    cur_header->length = len;

    if (prev_header) prev_header->next = cur_header;
    cur_header->prev = prev_header;
    cur_header->next = 0;

    return (void *)(chunk_start + sizeof(header_t));
}

void kfree(void *p)
{
    header_t *header = (header_t *)((uint32_t)p - sizeof(header_t));
    header->allocated = 0;

    // 粘合内存
    glue_chunk(header);
}

// 扩展堆内存
void alloc_chunk(uint32_t start, uint32_t len)
{
    // 循环申请页面
    while (start + len > heap_max) {
        uint32_t page = pmm_alloc_page();
        // 修改页表
        map(pgd_kern, heap_max, page, PAGE_PRESENT | PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
}

void free_chunk(header_t *chunk)
{
    // 修改指针
    if (chunk->prev == 0) {
         heap_first = 0;
    } else {
        chunk->prev->next = 0;
    }

    // 空闲的内存超过 1 页就释放掉
    while ((heap_max - PAGE_SIZE) > (uint32_t)chunk) {
        heap_max -= PAGE_SIZE;

        uint32_t page;
        get_mapping(pgd_kern, heap_max, &page);
        unmap(pgd_kern, heap_max);
        pmm_free_page(page);
    }
}

void split_chunk(header_t *chunk, uint32_t len)
{
    // 剩余空间要能存下一个 header
    if (chunk->length - len > sizeof(header_t)) {
        header_t *new_chunk = (header_t *)((uint32_t)chunk + len);

        // 新 chunk 插入 heap 链表
        new_chunk->prev = chunk;
        new_chunk->next = chunk->next;
        if (chunk->next) chunk->next->prev = new_chunk;
        chunk->next = new_chunk;

        // 长度修改
        new_chunk->allocated = 0;
        new_chunk->length = chunk->length - len;
        chunk->length = len;
    }
}

void glue_chunk(header_t *chunk)
{
    // 合并后面的
    if (chunk->next && chunk->next->allocated == 0) {
        chunk->length += chunk->next->length;

        chunk->next = chunk->next->next;
        if(chunk->next) chunk->next->prev = chunk;
    }

    // 合并前面的
    if (chunk->prev && chunk->prev->allocated == 0) {
        chunk->prev->length += chunk->length;

        chunk->prev->next = chunk->next;
        if(chunk->next) chunk->next->prev = chunk->prev;
        chunk = chunk->prev;
    }

    // 如果后面没有内存块了直接释放
    if (chunk->next == 0) {
         free_chunk(chunk);
    }
}
