#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#include "types.h"

#define KB_IN_BYTES 32    /* size of keyboard input buffer */

// 输入缓冲
typedef struct kb_input {
    char*   p_head;            /* 队列首部 */
    char*   p_tail;            /* 队列尾部，指向下一个位置 */
    int     count;             /* 缓冲区中共有多少字节 */
    char    buff[KB_IN_BYTES];  /* 缓冲区 */
} kb_input_t;

// 键盘初始化
void init_keyboard();

// 从缓冲区读取
void keyboard_read();

#endif // INCLUDE_KEYBOARD_H
