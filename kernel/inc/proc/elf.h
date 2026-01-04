#ifndef PROC_ELF_H
#define PROC_ELF_H

#include <stdint.h>

#include <mm/symbols.h>
#include <proc/types/exec.h>
#include <proc/types/elf.h>

int elf_find_section_idx(const Elf32_Ehdr *elf, const char *section_name, uint32_t *idx);

int elf_find_section(const Elf32_Ehdr *elf, const char *section_name, const Elf32_Shdr **section);

uintptr_t elf_find_data(const Elf32_Ehdr *elf, uintptr_t addr);

static inline const Elf32_Shdr *elf_get_sections(const Elf32_Ehdr *elf) {
    return (Elf32_Shdr *)((uintptr_t)elf + elf->e_shoff);
}

int elf_get_section(const Elf32_Ehdr *elf, uint32_t index, const Elf32_Shdr **section);

int elf_check_header(void *data);

int elf_shndx_from_symidx(const Elf32_Ehdr *elf, uint32_t shndx, uint32_t *symidx);

int elf_load_symbols(const Elf32_Ehdr *elf, symbol_t **symbols);

int exec_elf(exec_data_t *exec_data);

#endif
