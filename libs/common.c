#include "common.h"

inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

inline void sti(void)
{
    asm volatile ("sti");
}

inline void cli(void)
{
    asm volatile ("cli" ::: "memory");
}

inline uint32_t read_eflags(void)
{
    uint32_t eflags;
    asm volatile ("pushfl; popl %0" : "=r" (eflags));
    return eflags;
}

inline void write_eflags(uint32_t eflags)
{
    asm volatile ("pushl %0; popfl" :: "r" (eflags));
}
