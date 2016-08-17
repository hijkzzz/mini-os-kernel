#include "types.h"
#include "keyboard.h"
#include "debug.h"

int task_tty(void *args)
{
    while (1) {
        keyboard_read();
    }
    return 0;
}

void in_process(uint32_t key)
{
        char output[2] = {'\0', '\0'};

        if (!(key & FLAG_EXT)) {
                output[0] = key & 0xFF;
                printk("%s", output);
        }
}

