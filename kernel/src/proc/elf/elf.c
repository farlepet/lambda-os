#include <errno.h>
#include <string.h>

#include <proc/mtask.h>
#include <proc/exec.h>
#include <err/error.h>
#include <mm/alloc.h>
#include <proc/elf.h>
#include <io/output.h>

int elf_find_section_idx(const Elf32_Ehdr *elf, const char *section_name, uint32_t *idx) {
    const Elf32_Shdr *sections     = elf_get_sections(elf);
    const Elf32_Shdr *strTabSec    = &sections[elf->e_shstrndx];
    const char       *strTabSecStr = (const char *)((uintptr_t)elf + strTabSec->sh_offset);

    for(uint32_t i = 0; i < elf->e_shnum; i++) {
        if(!strcmp(&strTabSecStr[sections[i].sh_name], section_name)) {
            if(idx) {
                *idx = i;
            }
            return 0;
        }
    }

    kdebug(DEBUGSRC_MODULE, ERR_DEBUG, "elf_find_section_idx: Could not find section %s", section_name);

    /* Section not found */
    return -ENOEXEC;
}

int elf_find_section(const Elf32_Ehdr *elf, const char *section_name, const Elf32_Shdr **section) {
    uint32_t idx;
    if (elf_find_section_idx(elf, section_name, &idx)) {
        return -ENOEXEC;
    }

    if (section) {
        *section = &elf_get_sections(elf)[idx];
    }

    return 0;
}

int elf_get_section(const Elf32_Ehdr *elf, uint32_t index, const Elf32_Shdr **section) {
    if (index > elf->e_shnum) {
        return -EINVAL;
    }

    const Elf32_Shdr *sections = elf_get_sections(elf);
    *section = &sections[index];

    return 0;
}

uintptr_t elf_find_data(const Elf32_Ehdr *elf, uintptr_t addr) {
    const Elf32_Shdr *sections = elf_get_sections(elf);

    for(uint32_t i = 0; i < elf->e_shnum; i++) {
        if((addr >= sections[i].sh_addr) &&
           (addr <  (sections[i].sh_addr + sections[i].sh_size))) {
            return (uintptr_t)elf + sections[i].sh_offset +
                   (addr - sections[i].sh_addr);
        }
    }

    return 0;
}

int elf_check_header(void *data) {
    const Elf32_Ehdr *head = data;

    if( (head->e_ident[0] != ELF_IDENT0) ||
        (head->e_ident[1] != ELF_IDENT1) ||
        (head->e_ident[2] != ELF_IDENT2) ||
        (head->e_ident[3] != ELF_IDENT3)) {
        kerror(ERR_ERROR, "Tried to load an ELF with incorrect header: %x %c %c %c",
            (head->e_ident[0]),
            (head->e_ident[1]),
            (head->e_ident[2]),
            (head->e_ident[3]));
        return -ENOEXEC;
    }

    if(head->e_ident[4] != HOST_CLASS) {
        kerror(ERR_ERROR, "Tried to load ELF not compatible with current bittiness: %d", head->e_ident[4]);
        return -ENOEXEC;
    }

    /*if(head->e_type != ET_EXEC) {
        kerror(ERR_ERROR, "Tried to load non-executable ELF with type %d", head->e_type);
        return 1;
    }*/

    if(head->e_machine != HOST_MACHINE) {
        kerror(ERR_ERROR, "Tried to load ELF not compatible with current architecture: %d", head->e_machine);
        return -ENOEXEC;
    }

    return 0;
}

int elf_shndx_from_symidx(const Elf32_Ehdr *elf, uint32_t symidx, uint32_t *shndx) {
    const Elf32_Shdr *symtab;
    if (elf_find_section(elf, ".symtab", &symtab)) {
        return -EUNSPEC;
    }

    if ((symidx * symtab->sh_entsize) > symtab->sh_size) {
        return -EUNSPEC;
    }
    *shndx = ((Elf32_Sym *)((uintptr_t)elf + symtab->sh_offset))[symidx].st_shndx;

    if (*shndx == SHN_UNDEF) {
        return -ENOENT;
    }

    return 0;
}

#define _SYM_CRITERIA(sym) (sym.st_shndx != SHN_UNDEF)

int elf_load_symbols(const Elf32_Ehdr *elf, symbol_t **symbols) {
    const Elf32_Shdr *elf_strtab = NULL;
    const Elf32_Shdr *elf_symtab = NULL;
    if(elf_find_section(elf, ".strtab", &elf_strtab) ||
       elf_find_section(elf, ".symtab", &elf_symtab)) {
        return -1;
    }

    const Elf32_Sym *syms   = (Elf32_Sym *)((uintptr_t)elf + elf_symtab->sh_offset);
    uint32_t         n_syms = elf_symtab->sh_size / elf_symtab->sh_entsize;
    
    /* We only want to keep track of symbols within this ELF */
    uint32_t used_syms = 0;
    for(uint32_t i = 0; i < n_syms; i++) {
        if(_SYM_CRITERIA(syms[i])) {
            used_syms++;
        }
    }

    *symbols = (symbol_t *)kmalloc((used_syms + 1) * sizeof(symbol_t) + elf_strtab->sh_size);
    if(*symbols == NULL) {
        return -ENOMEM;
    }
    
    char *sym_strtab = (char *)((uintptr_t)*symbols + (used_syms + 1) * sizeof(symbol_t));
    memcpy(sym_strtab, (void *)((uintptr_t)elf + elf_strtab->sh_offset), elf_strtab->sh_size);

    uint32_t idx = 0;
    for(uint32_t i = 0; i < n_syms; i++) {
        if(_SYM_CRITERIA(syms[i])) {
            (*symbols)[idx].section = syms[i].st_shndx;
            (*symbols)[idx].name = &sym_strtab[syms[i].st_name];
            (*symbols)[idx].addr = syms[i].st_value;
            (*symbols)[idx].size = syms[i].st_size;
            idx++;
        }
    }

    (*symbols)[used_syms].name = NULL;
    (*symbols)[used_syms].addr = 0xFFFFFFFF;
    (*symbols)[used_syms].size = 0x00000000;

    return 0;
}

char *sht_strings[] = {
    [SHT_NONE]          = "NONE",
    [SHT_PROGBITS]      = "PROG",
    [SHT_SYMTAB]        = "SYMTAB",
    [SHT_STRTAB]        = "STRTAB",
    [SHT_RELA]          = "RELA",
    [SHT_HASH]          = "HASH",
    [SHT_DYNAMIC]       = "DYNAMIC",
    [SHT_NOTE]          = "NOTE",
    [SHT_NOBITS]        = "NOBITS",
    [SHT_REL]           = "REL",
    [SHT_SHLIB]         = "SHLIB",
    [SHT_DYNSYM]        = "DYNSYM",
    [SHT_INIT_ARRAY]    = "INIT_ARRAY",
    [SHT_FINI_ARRAY]    = "FINI_ARRAY",
    [SHT_PREINIT_ARRAY] = "PREINIT_ARRAY",

    [(SHT_DYNSYM+1)...(SHT_INIT_ARRAY-1)] = "RESERVED"
};
