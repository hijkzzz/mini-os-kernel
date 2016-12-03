#include "proc.h"
#include "sched.h"
#include "pmm.h"
#include "vmm.h"
#include "gdt.h"
#include "heap.h"
#include "debug.h"
#include "string.h"
#include "common.h"

pid_t now_pid = 0;

int32_t kernel_thread(int (*fn)(void *), void *arg)
{
    proc_struct_t *new_proc = (proc_struct_t *)kmalloc(STACK_SIZE);
    assert(new_proc != NULL, "kernel_thread : kmalloc error\n");

    bzero(new_proc, sizeof(proc_struct_t));

    // 内核线程共享内核页表
    new_proc->cr3   = (uint32_t)pgd_kern - PAGE_OFFSET;
    new_proc->state = TASK_RUNNABLE;
    new_proc->pid   = now_pid++;
    set_proc_name(new_proc, "");
    new_proc->kstack = (uint32_t)new_proc + STACK_SIZE;

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
    // 暂时没有释放内存
    register uint32_t val asm("eax");
    printk("Thread exited with value %d", val);
}

char * set_proc_name(struct proc_struct *proc, const char *name) {
    bzero(proc->name, sizeof(proc->name));
    return strcpy(proc->name, name);
}

int32_t getpid()
{
    return current->pid;
}

int32_t fork()
{
    // We are modifying kernel structures, and so cannot be interrupted.
    cli();

    // 父进程控制块
    proc_struct_t *parent   = current;

    // 创建新进程
    proc_struct_t *new_proc = (proc_struct_t *)kmalloc(STACK_SIZE);
    assert(new_proc != NULL, "kernel_thread : kmalloc error\n");
    new_proc->pid           = now_pid++;
    new_proc->state         = TASK_RUNNABLE;
    set_proc_name(new_proc, "");
    new_proc->parent        = parent;

    // 复制堆栈
    memcpy((uint8_t *)((uint32_t)new_proc + sizeof(proc_struct_t)), (uint8_t *)((uint32_t)parent + sizeof(proc_struct_t)),
            STACK_SIZE - sizeof(proc_struct_t));
    new_proc->kstack  = (uint32_t)new_proc + STACK_SIZE;

    // 复制页帧
    uint32_t dir_phys = clone_pgd((pgd_t *)current->cr3);
    new_proc->cr3     = ROUNDDOWN(dir_phys, PAGE_SIZE);

    // 插入调度链表
    new_proc->next = running_proc_head;
    new_proc->prev = running_proc_head->prev;
    running_proc_head->prev->next = new_proc;
    running_proc_head->prev = new_proc;

    // This will be the entry point for the new process.
    uint32_t eip = read_eip();

    if (current == parent) {
        uint32_t esp; asm volatile("mov %%esp, %0" : "=r"(esp));
        uint32_t ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
        new_proc->context.esp = esp - (uint32_t)current + (uint32_t)new_proc;
        new_proc->context.ebp = ebp - (uint32_t)current + (uint32_t)new_proc;
        // 开中断
        new_proc->context.eflags = 0x200;

        // 压入 eip
        uint32_t *stack_top = (uint32_t *)(new_proc->context.ebp);
        *(--stack_top) = eip;
        new_proc->context.ebp = (uint32_t)stack_top;

        // All finished: Reenable interrupts.
        sti();
        // And by convention return the PID of the child.
        return new_proc->pid;

    } else {
        // fork 系统调用的返回值
        return 0;
    }
}
