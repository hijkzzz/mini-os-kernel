#ifndef INCLUDE_PROC_H
#define INCLUDE_PROC_H

#include "types.h"
#include "vmm.h"

#define PROC_NAME_LEN               15
#define MAX_PROCESS                 4096
#define MAX_PID                     (MAX_PROCESS * 2)

// 进程状态
typedef
enum task_state {
    TASK_UNINIT = 0,
    TASK_SLEEPING = 1,
    TASK_RUNNABLE = 2,
    TASK_ZOMBIE = 3,
} task_state_t;

typedef
struct context {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
} context_t;

// 进程控制块
typedef
struct proc_struct {
    volatile task_state_t state;  // 运行状态
    volatile bool need_resched;   // 进程需要被调度
    context_t context;            // 上下文
    uint32_t kstack;              // 内核栈
    uint32_t cr3;                 // 页目录
    pid_t pid;                    // 进程号
    char name[PROC_NAME_LEN + 1]; // 进程名
    uint32_t flags;               // 进程标志
    struct proc_struct *parent;   // 父进程

    struct proc_struct *prev;     // 双向循环链表
    struct proc_struct *next;
} proc_struct_t;


extern pid_t now_pid;

// 内核线程创建
int32_t kernel_thread(int (*fn)(void *), void *arg);

// 内核线程退出
void kthread_exit();

// 设置进程名
char *set_proc_name(struct proc_struct *proc, const char *name);

// 复制进程
int32_t fork();

// 获取进程 id
int32_t getpid();

#endif // INCLUDE_PROC_H
