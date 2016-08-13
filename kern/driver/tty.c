#include "types.h"
#include "keyboard.h"

void task_tty()
{
    while (1) {
        keyboard_read();
    }
}

