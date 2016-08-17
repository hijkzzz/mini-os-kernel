#ifndef INCLUDE_SCHEDULE_H
#define INCLUDE_SCHEDULE_H

#include "proc.h"

// 运行队列， 等待队列
extern proc_struct_t *running_proc_head;
extern proc_struct_t *wait_proc_head;
extern proc_struct_t *current;

// 初始化进程调度
void init_sched();

// 进程调度
void schedule();

// 切换 PCB
void change_task_to(proc_struct_t *next);

// 切换上下文
void switch_to(context_t *prev, context_t *next);

// read eip
uint32_t read_eip();

#endif // INCLUDE_SCHEDULE_H
