#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <lambda/export.h>
#include <mod/symbols.h>
#include <mod/module.h>
#include <err/error.h>
#include <proc/thread.h>
#include <proc/proc.h>
#include <proc/elf.h>
#include <mm/alloc.h>
#include <fs/fs.h>
#include <io/output.h>

static llist_t loaded_modules;

/* TODO: Better method for choosing where to place module */
static uintptr_t _current_base = KERNEL_OFFSET + 0x10000000;

static lambda_mod_head_t *_module_place(const Elf32_Ehdr *elf, uintptr_t baseaddr, module_entry_t *mod_ent, symbol_t *symbols);

int module_read(kfile_hand_t *file, uintptr_t *base, Elf32_Ehdr **elf) {
    Elf32_Ehdr        *elf_data;
    const Elf32_Shdr  *mod_section;
    lambda_mod_head_t *mod_head;
    kstat_t            file_stat;

    if(!file) {
        return -EINVAL;
    }

    TRY_OR_RET(kfstat(file->file, &file_stat));

    elf_data = (Elf32_Ehdr *)kmalloc((size_t)file_stat.size);
    if(!elf_data) {
        return -ENOMEM;
    }
    if((size_t)fs_read(file, 0, sizeof(Elf32_Ehdr), (void *)elf_data) != sizeof(Elf32_Ehdr)) {
        kfree(elf_data);
        return -EUNSPEC;
    }
    if(elf_check_header(elf_data)) {
        kfree(elf_data);
        return -EUNSPEC;
    }

    if((size_t)fs_read(file, 0, (size_t)file_stat.size, (void *)elf_data) != file_stat.size) {
        kfree(elf_data);
        return -EUNSPEC;
    }
    if(elf_find_section(elf_data, LAMBDA_MODULE_SECTION_NAME, &mod_section)) {
        kfree(elf_data);
        return -EUNSPEC;
    }

    mod_head = (lambda_mod_head_t *)((uintptr_t)elf_data + mod_section->sh_offset);
    if(mod_head->head_magic != LAMBDA_MODULE_HEAD_MAGIC) {
        kfree(elf_data);
        return -EUNSPEC;
    }

    *base = mod_section->sh_addr;

    if(elf) {
        *elf = elf_data;
    } else {
        /* TODO: Copy data, and free memory. */
    }

    return 0;
}

static int _check_requirements(lambda_mod_head_t *mod_head) {
    if(!mod_head->metadata.requirements) {
        /* No requirements, auto-pass */
        return 0;
    }
    
    llist_iterator_t iter;
    module_entry_t  *mod;

    const char *const *reqs = (const char *const *)mod_head->metadata.requirements;
    /* TODO: Potentially enforce maximum number of requirements */
    for(size_t i = 0; reqs[i]; i++) {
        int found = 0;
        llist_iterator_init(&loaded_modules, &iter);
        while(!found && llist_iterate(&iter, (void **)&mod)) {
            if(!strcmp(reqs[i], mod->ident)) {
                found = 1;
            }
        }
        if(!found) {
            /* Requirement not satisfied */
            kdebug(DEBUGSRC_MODULE, ERR_WARN, "%s: Requirement not satisfied: %s", mod_head->metadata.name, reqs[i]);
            return -EUNSPEC;
        }
    }

    return 0;
}

int module_install(kfile_hand_t *file) {
    /* @todo A lot of work is needed here in order to prevent memory leaks */
    lambda_mod_head_t *mod_head_reloc;
    uintptr_t          mod_base;
    Elf32_Ehdr        *mod_elf;
    symbol_t          *symbols;
    
    kdebug(DEBUGSRC_MODULE, ERR_DEBUG, "module_install: Reading module");
    if(module_read(file, &mod_base, &mod_elf)) {
        return -EUNSPEC;
    }

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "module_install: Generating module entry");
    module_entry_t *modent = (module_entry_t *)kmalloc(sizeof(module_entry_t) /*+ strlen(ident) + 1*/);
    memset(modent, 0, sizeof(module_entry_t));

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "module_install: Reading symbol table");
    if(elf_load_symbols(mod_elf, &symbols)) {
        kfree(mod_elf);
        return -EUNSPEC;
    }

    /* This is potentially wasteful if the module ends up getting rejected, but
     * it makes it much easier to perform the requisite checks, and shouldn't be
     * big deal since the calling process (should) be priviledged. Alternatively,
     * we can do a partial placement first. */
    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "module_install: Placing and relocating module");
    mod_head_reloc = _module_place(mod_elf, _current_base, modent, symbols);
    if(mod_head_reloc == NULL) {
        /* @todo Remove module */
        kfree(mod_elf);
        kfree(symbols);
        kfree(modent);

        return -EUNSPEC;
    }
    _current_base += 0x10000; /* TODO: Actually determine module space */

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "module_install: Checking requirements");
    if(_check_requirements(mod_head_reloc)) {
        kfree(mod_elf);
        kfree(symbols);
        kfree(modent);

        return -EUNSPEC;
    }

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "module_install: Calling function @ %p", modent->func);
    if(modent->func(LAMBDA_MODFUNC_START, modent)) {
        kdebug(DEBUGSRC_MODULE, ERR_WARN, "module_install: Module function return non-zero!");
        /* @todo: remove module */
    }

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "module_install: Adding module to list");
    modent->list_item.data = (void *)modent;
    llist_append(&loaded_modules, &modent->list_item);

    return 0;
}

int module_uninstall(module_entry_t *mod) {
    mod->func(LAMBDA_MODFUNC_STOP, mod);
    /* @todo Actually remove module */
    llist_remove(&loaded_modules, &mod->list_item);
    
    kfree(mod);

    return 0;
}


typedef struct {
    /** Section index */
    uint32_t  section;
    /** New virtual address */
    uintptr_t addr_virt;
    /** Physical address */
    uintptr_t addr_phys;
    /** Size in bytes */
    size_t    size;
} elf_reloc_t;

static inline const elf_reloc_t *_get_reloc(const elf_reloc_t *relocs, uint32_t section) {
    uint32_t i = 0;
    while (relocs[i].section != 0xFFFFFFFF) {
        if (relocs[i].section == section) {
            return &relocs[i];
        }
        i++;
    }

    return NULL;
}

static int _translate_symbol(const elf_reloc_t *relocs, symbol_t *sym) {
    const elf_reloc_t *reloc = _get_reloc(relocs, sym->section);
    if (!reloc) {
        return -ENOENT;
    }

    sym->addr = reloc->addr_virt;

    return 0;
}

static uintptr_t _translate_addr(const elf_reloc_t *relocs, uint32_t section, uintptr_t addr) {
    const elf_reloc_t *reloc = _get_reloc(relocs, section);
    if (!reloc) {
        return 0;
    }

    return reloc->addr_virt + addr;
}

static int _do_reloc(const elf_reloc_t *relocs, uintptr_t symaddr, const Elf32_Rel *rel, uint32_t section) {
    uintptr_t dataaddr = _translate_addr(relocs, section, rel->r_offset);

    if (dataaddr < 0x1000) {
        kdebug(DEBUGSRC_MODULE, ERR_WARN, "_do_reloc: Bad address translation (_, %08x, %x, %u) %u", symaddr, rel->r_info, section);
        return -EUNSPEC;
    }

    switch(ELF32_R_TYPE(rel->r_info)) {
        case R_386_NONE:
            break;
        case R_386_32:
            *((uint32_t *)dataaddr) += symaddr;
            break;
        case R_386_PC32:
            *((uint32_t *)dataaddr) += (symaddr - dataaddr);
            break;
        case R_386_RELATIVE:
            *((uint32_t *)dataaddr) = _translate_addr(relocs, section, *((uint32_t *)dataaddr));
            break;
        case R_386_GLOB_DAT:
        case R_386_JMP_SLOT:
            *((uint32_t *)dataaddr) = symaddr;
            break;
        default:
            kdebug(DEBUGSRC_MODULE, ERR_WARN, "_do_reloc: Unhandled relocation type: %d", ELF32_R_TYPE(rel->r_info));
            return -EUNSPEC;
    }

    /*kdebug(DEBUGSRC_MODULE, ERR_TRACE, "_do_reloc: Wrote %08X to %08X [%d]", *(uint32_t *)dataaddr, dataaddr, ELF32_R_TYPE(rel->r_info));*/

    return 0;
}

static int _find_undefined_symbol(const char *ident, uintptr_t *symaddr) {
    if (!module_symbol_find_kernel(ident, symaddr)) {
        return 0;
    }

    /* Check other loaded modules */
    llist_iterator_t iter;
    module_entry_t  *ent;
    llist_iterator_init(&loaded_modules, &iter);
    while (llist_iterate(&iter, (void **)&ent)) {
        if (ent->symbols && !module_symbol_find_module(ident, symaddr, ent->symbols)) {
            return 0;
        }
    }

    return -ENOENT;
}

/* TODO: Move to elf.c */
static int _find_symbol(const Elf32_Ehdr *elf, const symbol_t *symbols, uint32_t table, uint32_t index, uintptr_t *value) {
    const Elf32_Shdr *sections = (Elf32_Shdr *)((uintptr_t)elf + elf->e_shoff);

    const Elf32_Shdr *symtab = &sections[table];
    if ((index * symtab->sh_entsize) > symtab->sh_size) {
        kdebug(DEBUGSRC_MODULE, ERR_WARN, "_handle_reloc: Symbol index %u out of bounds", index);
        return -EUNSPEC;
    }

    const Elf32_Sym *sym = &((Elf32_Sym *)((uintptr_t)elf + symtab->sh_offset))[index];

    if (sym->st_shndx == SHN_ABS) {
        *value = sym->st_value;
    } else if (sym->st_shndx == SHN_UNDEF) {
        const Elf32_Shdr *strtab = &sections[symtab->sh_link];
        const char *ident = (const char *)((uintptr_t)elf + strtab->sh_offset + sym->st_name);

        /* TODO: Support weak symbols? */
        if (_find_undefined_symbol(ident, value)) {
            kdebug(DEBUGSRC_MODULE, ERR_WARN, "_find_symbol: Could not find external symbol `%s`", ident);
            return -ENOENT;
        }
    } else {
        for (uint32_t i = 0; symbols[i].addr != 0xFFFFFFFF; i++) {
            if (symbols[i].section == sym->st_shndx) {
                *value = symbols[i].addr + sym->st_value;

                return 0;
            }
        }

        kdebug(DEBUGSRC_MODULE, ERR_WARN, "_find_symbol: Could not find internal symbol [%u, %u, %u]",
               table, index, sym->st_shndx);


        return -ENOENT;
    }

    return 0;
}

#define _RELOC_NEED_SYM(R) ((ELF32_R_TYPE(R->r_info) != R_386_NONE) && \
                            (ELF32_R_TYPE(R->r_info) != R_386_RELATIVE))

static int _handle_reloc(const Elf32_Ehdr *elf, const elf_reloc_t *relocs, const symbol_t *symbols, const Elf32_Rel *rel, uint32_t symtab, uint32_t shndx) {
    uintptr_t symaddr = 0;

    if(_RELOC_NEED_SYM(rel)) {
        uint32_t index = ELF32_R_SYM(rel->r_info);

        if (_find_symbol(elf, symbols, symtab, index, &symaddr)) {
            kdebug(DEBUGSRC_MODULE, ERR_WARN, "_handle_reloc: Could not find symbol [%u, %u]", symtab, index);
            return -ENOENT;
        }
    }

    if(_do_reloc(relocs, symaddr, rel, shndx)) {
        return -EUNSPEC;
    }

    return 0;
}

static int _module_apply_relocs(const Elf32_Ehdr *elf, const elf_reloc_t *relocs, const symbol_t *symbols) {
    const Elf32_Shdr *sects = elf_get_sections(elf);

    for(uint32_t i = 0; i < elf->e_shnum; i++) {
        if(sects[i].sh_type != SHT_REL) { continue; }

        uint32_t symtab      = sects[i].sh_link;
        uint32_t target_sect = sects[i].sh_info;

        const Elf32_Rel *rel    = (Elf32_Rel *)((uintptr_t)elf + sects[i].sh_offset);
        uint32_t         n_rels = sects[i].sh_size / sects[i].sh_entsize;
        for(uint32_t j = 0; j < n_rels; j++) {
            if (_handle_reloc(elf, relocs, symbols, &rel[j], symtab, target_sect)) {
                return -EUNSPEC;
            }
        }
    }

    return 0;
}

static void *_alloc_map(uintptr_t virt, size_t len) {
    void *paddr = kmamalloc(len, 0x1000);
    /* TODO: Possibly do not make all pages writable and executable */
    mmu_map(virt, (uintptr_t)paddr, len, MMU_FLAG_READ | MMU_FLAG_WRITE | MMU_FLAG_EXEC | MMU_FLAG_KERNEL);

    return paddr;
}

#define FORCE_ALIGN(V, AM) \
    if ((V) & (AM)) { \
        (V) = (((V) + (AM)) & ~(AM)); \
    }

static lambda_mod_head_t *_module_place(const Elf32_Ehdr *elf, uintptr_t baseaddr, module_entry_t *mod_ent, symbol_t *symbols) {
    const Elf32_Shdr *sects = elf_get_sections(elf);

    /* TODO: Differentiate R/W/X sections */
    uintptr_t prog_base   = baseaddr;
    uintptr_t prog_size   = 0;
    uintptr_t nobits_base = 0;
    uintptr_t nobits_size = 0;

    uint32_t n_relocs = 0;

    /* Get section count and required memory */
    for (uint32_t i = 0; i < elf->e_shnum; i++) {
        const Elf32_Shdr *sect = &sects[i];

        if (sect->sh_addr) {
            kdebug(DEBUGSRC_MODULE, ERR_WARN, "_module_place: Section %u has address set, which is not supported", i);
            return NULL;
        }

        /* Arbitrary max alignment */
        if (sect->sh_addralign > 0x80) {
            kdebug(DEBUGSRC_MODULE, ERR_WARN, "_module_place: Section %u address alignment too large: %u",
                   i, sect->sh_addralign);
            return NULL;
        }

        /* TODO: Validate that alignment is following spec */
        uint32_t align_mask = sect->sh_addralign ? (sect->sh_addralign - 1) : 0;

        switch (sect->sh_type) {
            case SHT_PROGBITS:
                /* Ensure base is at max required alignment */
                FORCE_ALIGN(prog_base, align_mask);
                FORCE_ALIGN(prog_size, align_mask);
                prog_size += sect->sh_size;
                n_relocs++;
                break;
            case SHT_NOBITS:
                /* Ensure base is at max required alignment */
                FORCE_ALIGN(nobits_base, align_mask);
                FORCE_ALIGN(nobits_size, align_mask);
                nobits_size += sect->sh_size;
                n_relocs++;
                break;
            default:
                break;
        }
    }

    /* Place nobits one page after prog (TODO use page size def) */
    nobits_base = (prog_base + 0x1000) & ~0xFFF;

    elf_reloc_t *relocs = kmalloc((n_relocs + 1) * sizeof(elf_reloc_t));

    uintptr_t prog_phys   = (uintptr_t)_alloc_map(prog_base, prog_size);
    /* TODO: Mark pages read-only */
    uintptr_t nobits_phys = (uintptr_t)_alloc_map(nobits_base, nobits_size);
    memset((void *)nobits_phys, 0, nobits_size);

    uintptr_t prog_offset   = 0;
    uintptr_t nobits_offset = 0;

    /* Create actual mappings, copy in data */
    uint32_t j = 0;
    for (uint32_t i = 0; i < elf->e_shnum; i++) {
        const Elf32_Shdr *sect = &sects[i];

        uint32_t align_mask = sect->sh_addralign ? (sect->sh_addralign - 1) : 0;

        switch (sect->sh_type) {
            case SHT_PROGBITS:
                FORCE_ALIGN(prog_offset, align_mask);

                relocs[j].section = i;
                relocs[j].addr_virt = prog_base + prog_offset;
                relocs[j].addr_phys = prog_phys + prog_offset;
                relocs[j].size = sect->sh_size;

                /*kdebug(DEBUGSRC_MODULE, ERR_TRACE, "_module_place: Section %u @ %08x, %d bytes",
                       i, relocs[j].addr_virt, relocs[j].size);*/

                memcpy((void *)relocs[j].addr_phys, (void *)((uintptr_t)elf + sect->sh_offset), relocs[j].size);

                j++;
                prog_offset += sect->sh_size;
                break;
            case SHT_NOBITS:
                /* Ensure base is at max required alignment */
                FORCE_ALIGN(nobits_offset, align_mask);

                relocs[j].section = i;
                relocs[j].addr_virt = nobits_base + nobits_offset;
                relocs[j].addr_phys = nobits_phys + nobits_offset;
                relocs[j].size = sect->sh_size;

                /*kdebug(DEBUGSRC_MODULE, ERR_TRACE, "_module_place: Section %u @ %08x, %d bytes",
                       i, relocs[j].addr_virt, relocs[j].size);*/

                j++;
                nobits_offset += sect->sh_size;
                break;
            default:
                break;
        }
    }

    relocs[j].section   = 0xFFFFFFFF;
    relocs[j].addr_virt = 0xFFFFFFFF;
    relocs[j].addr_phys = 0xFFFFFFFF;
    relocs[j].size      = 0;

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "Translating module symbol table");
    for(uint32_t i = 0; symbols[i].addr != 0xFFFFFFFF; i++) {
        _translate_symbol(relocs, &symbols[i]);
        kdebug(DEBUGSRC_MODULE, ERR_TRACE, "  %3d => %08x [%3u, %s]", i, symbols[i].addr, symbols[i].section, symbols[i].name);
    }

    if(_module_apply_relocs(elf, relocs, symbols)) {
        /* TODO: Free and unmap prog and nobits */
        kfree(relocs);
        return NULL;
    }

    uint32_t mod_section;
    if(elf_find_section_idx(elf, LAMBDA_MODULE_SECTION_NAME, &mod_section)) {
        /* TODO: Free and unmap prog and nobits */
        kfree(relocs);
        return NULL;
    }
    lambda_mod_head_t *new_mod_head = (lambda_mod_head_t *)_translate_addr(relocs, mod_section, 0);
    mod_ent->func    = new_mod_head->function;
    mod_ent->symbols = symbols;
    mod_ent->ident   = new_mod_head->metadata.ident;

    kdebug(DEBUGSRC_MODULE, ERR_TRACE, "Mod func: %p", mod_ent->func);

    kfree(relocs);

    return new_mod_head;
}


int module_start_thread(module_entry_t *mod, void (*entry)(void *), void *data, const char *name) {
    if((mod == NULL) ||
       (entry == NULL)) {
        return -EINVAL;
    }
    
    static char _name[256];
    if(name == NULL) {
        strncpy(_name, mod->ident, 255);
        _name[255] = '\0';
        name = _name;
    }

    uint32_t tidx = 0;
    for(; tidx < MOD_THREAD_MAX; tidx++) {
        if(mod->threads[tidx] == 0) {
            break;
        }
    }
    if(tidx == MOD_THREAD_MAX) {
        kdebug(DEBUGSRC_MODULE, ERR_ERROR, "module_start_thread: Ran out of thread slots!");
        return -EUNSPEC;
    }

    /* TODO: Allow configuration of stack size and priority? */
    int tid = thread_spawn((uintptr_t)entry, data, name, CONFIG_PROC_KERN_STACK_SIZE, PRIO_DRIVER);
    if(tid < 0) {
        return tid;
    }

    mod->threads[tidx] = tid;

    return tid;
}
EXPORT_FUNC(module_start_thread);
