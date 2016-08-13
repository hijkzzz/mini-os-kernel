#include "keyboard.h"
#include "keymap.h"
#include "common.h"
#include "idt.h"
#include "debug.h"
#include "string.h"

// 输入缓冲区
kb_input_t kb_in;

// 键盘事件处理
void keyboard_handler(pt_regs_t *regs)
{
    // 从8042读取数据
    uint8_t scan_code = inb(0x60);
    if (kb_in.count < KB_IN_BYTES) {
        *(kb_in.p_tail) = scan_code;
        ++kb_in.p_tail;
        if (kb_in.p_tail == kb_in.buff + KB_IN_BYTES) {
            kb_in.p_tail = kb_in.buff;
        }
        ++kb_in.count;
    }
    // 缓冲区满了直接丢弃
}

// 初始化键盘
void init_keyboard()
{
    //初始化缓冲区
    kb_in.count = 0;
    kb_in.p_head = kb_in.p_tail = kb_in.buff;

    //注册中断处理函数
    register_interrupt_handler(33, keyboard_handler);
}

// 从缓冲区读取
void keyboard_read()
{
    uint8_t scan_code;
    char output[2];
    int make;    // TRUE, make / FALSE, break

    bzero(output, sizeof(output));

    if (kb_in.count > 0) {
        // 进入临界区
        cli();
        scan_code = *(kb_in.p_head);
        kb_in.p_head++;
        if (kb_in.p_head == kb_in.buff + KB_IN_BYTES) {
            kb_in.p_head = kb_in.buff;
        }
        kb_in.count--;
        sti();

        /* 下面开始解析扫描码 */
        if (scan_code == 0xE1) {

        }
        else if (scan_code == 0xE0) {

        }
        else {    /* 下面处理可打印字符 */

            /* 首先判断 Make Code 还是 Break Code */
            make = (scan_code & FLAG_BREAK ? FALSE : TRUE);

            /* 如果是 Make Code 就打印，是 Break Code 则不做处理 */
            if(make) {
                output[0] = keymap[(scan_code & 0x7F) * MAP_COLS];
                printk("%s", output);
            }
        }
    }
}

