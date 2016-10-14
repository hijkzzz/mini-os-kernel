#include "string.h"

inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
    for (; len != 0; --len) {
        *dest++ = *src++;
    }
}

inline void memset(void *dest, uint8_t val, uint32_t len)
{
    uint8_t *dst = (uint8_t *)dest;

    for ( ; len != 0; --len) {
        *dst++ = val;
    }
}

inline void bzero(void *dest, uint32_t len)
{
    memset(dest, 0, len);
}

inline int strcmp(const char *str1, const char *str2)
{
    //找到第一个不相同的字符
    while (*str1 != '\0' && *str1 == *str2) {
        ++str1;
        ++str2;
    }
    return (uint8_t)*str1 - (uint8_t)*str2;
}

inline char *strcpy(char *dest, const char *src)
{
    char *p = dest;
    while ((*p++ = *src++) != '\0');
    return dest;
}

inline char *strcat(char *dest, const char *src)
{
    return strcpy(dest + strlen(dest), src);
}

inline int strlen(const char *s) {
    int cnt = 0;
    while (*s++ != '\0') {
        ++cnt;
    }
    return cnt;
}
