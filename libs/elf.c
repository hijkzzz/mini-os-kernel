#include "common.h"
#include "string.h"
#include "elf.h"

// 从 multiboot_t 结构获取ELF信息
elf_t elf_from_multiboot(multiboot_t *mb)
{
    elf_t elf;
    elf_section_header_t *sh = (elf_section_header_t*)mb->addr;

    uint32_t shstrtab = sh[mb->shndx].addr;//段字符串表地址
    // 搜索字符串表，符号表
    for (int i = 0; i < mb->num; i++) {
        //sh[i].name 是一个偏移值
        const char *name = (const char *)(shstrtab + sh[i].name);
        if (strcmp(name, ".strtab") == 0) {
            elf.strtab = (const char *)sh[i].addr;
            elf.strtabsz = sh[i].size;
        }
        if (strcmp(name, ".symtab") == 0) {
            elf.symtab = (elf_symbol_t*)sh[i].addr;
            elf.symtabsz = sh[i].size;
        }
    }

    return elf;
}

// 查看ELF的符号信息
const char *elf_lookup_symbol(uint32_t addr, elf_t *elf)
{
    for (int i = 0; i < (elf->symtabsz / sizeof(elf_symbol_t)); i++) {
        //判断是否为函数符号
        if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2) {
              continue;
        }
        // 通过函数调用地址查到函数的名字
        if ( (addr >= elf->symtab[i].value) && (addr < (elf->symtab[i].value + elf->symtab[i].size)) ) {
            return (const char *)((uint32_t)elf->strtab + elf->symtab[i].name);
        }
    }

    return NULL;
}

