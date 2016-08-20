#include "sched.h"
#include "common.h"
#include "proc.h"
#include "pmm.h"
#include "heap.h"
#include "debug.h"
#include "gdt.h"

proc_struct_t *running_proc_head = NULL;
proc_struct_t *wait_proc_head = NULL;
proc_struct_t *current = NULL;

// 初始化进程调度
void init_sched()
{
    // 初始化 idleproc 进程
    current = (proc_struct_t *)(kern_stack_top - STACK_SIZE);
    current->state = TASK_RUNNABLE;
    current->pid = now_pid++;
    set_proc_name(current, "idleproc");
    current->cr3 = (uint32_t)pgd_kern - PAGE_OFFSET;
    current->kstack = (uint32_t)kern_stack + STACK_SIZE;

    // 双向循环链表
    current->next = current;
    current->prev = current;
    running_proc_head = current;
}

// 进程调度
void schedule()
{
    if (current) {
         change_task_to(current->next);
    }
}

// 切换 PCB
void change_task_to(proc_struct_t *next)
{
    if (current != next) {
        proc_struct_t *prev = current;
        current = next;
        // 切换内核堆和页表
        set_kernel_stack(next->kstack);
        switch_pgd(next->cr3);
        switch_to(&(prev->context), &(current->context));
    }
}
