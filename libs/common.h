#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#include "types.h"

// 输出字节
void outb(uint16_t port, uint8_t value);

// 输入字节
uint8_t inb(uint16_t port);

// 输入字
uint16_t inw(uint16_t port);

#endif // INCLUDE_COMMON_H
