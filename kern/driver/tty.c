#include "types.h"
#include "keyboard.h"

void task_tty()
{
    while (1) {
        keyboard_read();
    }
}

void in_process(uint32_t key)
{
        char output[2] = {'\0', '\0'};

        if (!(key & FLAG_EXT)) {
                output[0] = key & 0xFF;
                printk("%s", output);
        }
}

