#include "proc.h"
#include "sched.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "debug.h"
#include "string.h"

pid_t now_pid = 0;

int32_t kernel_thread(int (*fn)(void *), void *arg)
{
    proc_struct_t *new_proc = (proc_struct_t *)kmalloc(STACK_SIZE);
    assert(new_proc != NULL, "kernel_thread : kmalloc error\n");

    bzero(new_proc, sizeof(proc_struct_t));

    // 内核线程共享内核页表
    new_proc->state = TASK_RUNNABLE;
    new_proc->pid   = now_pid++;
    set_proc_name(new_proc, "");
    new_proc->kstack = kern_stack;
    new_proc->mm    = NULL;

    // 填写函数调用栈
    uint32_t *stack_top = (uint32_t *)((uint32_t)new_proc + STACK_SIZE);
    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)kthread_exit;
    *(--stack_top) = (uint32_t)fn;

    new_proc->context.esp = (uint32_t)new_proc + STACK_SIZE - 3 * sizeof(uint32_t);
    // 开中断
    new_proc->context.eflags = 0x200;

    // 插入调度链表
    new_proc->next = running_proc_head;
    new_proc->prev = running_proc_head->prev;
    running_proc_head->prev->next = new_proc;
    running_proc_head->prev = new_proc;

    return new_proc->pid;
}

void kthread_exit()
{
    register uint32_t val asm("eax");
    printk("Thread exited with value %d", val);
}

char *
set_proc_name(struct proc_struct *proc, const char *name) {
    bzero(proc->name, sizeof(proc->name));
    return strcpy(proc->name, name);
}

