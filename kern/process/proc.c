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
    asm volatile("cli");

    // Take a pointer to this process' task struct for later reference.
    task_t *parent_task = (task_t*)current_task;

    // Clone the address space.
    page_directory_t *directory = clone_directory(current_directory);

    // Create a new process.
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->id = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    current_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
    new_task->next = 0;

    // Add it to the end of the ready queue.
    // Find the end of the ready queue...
    task_t *tmp_task = (task_t*)ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;
    // ...And extend it.
    tmp_task->next = new_task;

    // This will be the entry point for the new process.
    u32int eip = read_eip();

    // We could be the parent or the child here - check.
    if (current_task == parent_task)
    {
        // We are the parent, so set up the esp/ebp/eip for our child.
        u32int esp; asm volatile("mov %%esp, %0" : "=r"(esp));
        u32int ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        // All finished: Reenable interrupts.
        asm volatile("sti");

        // And by convention return the PID of the child.
        return new_task->id;
    }
    else
    {
        // We are the child - by convention return 0.
        return 0;
    }

}

