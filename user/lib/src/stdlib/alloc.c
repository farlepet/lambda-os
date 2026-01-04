#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>


/** Describes a block of memory */
typedef struct {
#define ALCENTFLAG_VALID (1U << 0)
#define ALCENTFLAG_USED  (1U << 1)
    uint8_t   flags; /** Block flags */
    uintptr_t addr;  /** The address of the block of memory */
    size_t    size;  /** Size of the block of memory */
} alcent_t;

/* @todo Make platform-dependant and/or dynamic */
#define PAGE_SZ      4096
#define ALLOC_BLOCK  (PAGE_SZ / sizeof(alcent_t))
#define ALLOC_BLOCKS (PAGE_SZ / sizeof(void *))

#define INITIAL_SZ   (PAGE_SZ * 8)

#define MIN_ALIGN    8

static alcent_t **_allocs = NULL;

/* @todo munmap support */
static void *_mmap_alloc(size_t sz) {
    return mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

int __lib_alloc_init() {
    _allocs = _mmap_alloc(PAGE_SZ);
    if(_allocs == NULL) {
        return -1;
    }
    memset(_allocs, 0, PAGE_SZ);

    _allocs[0] = _mmap_alloc(PAGE_SZ);
    if(_allocs[0] == NULL) {
        return -1;
    }
    memset(_allocs[0], 0, PAGE_SZ);

    _allocs[0][0].addr = (uintptr_t)_mmap_alloc(INITIAL_SZ);
    if(_allocs[0][0].addr == 0) {
        return -1;
    }
    _allocs[0][0].size = INITIAL_SZ;
    _allocs[0][0].flags |= ALCENTFLAG_VALID;

    return 0;
}

static int _add_alloc(alcent_t *al) {
    // Search for a free allocation slot
    for(unsigned j = 0; j < ALLOC_BLOCKS; j++) {
        if(!_allocs[j]) { continue; }
        for(unsigned i = 0; i < ALLOC_BLOCK; i++) {
            if(!(_allocs[j][i].flags & ALCENTFLAG_VALID)) {
                // Copy `al` to this allocation slot
                memcpy(&_allocs[j][i], al, sizeof(alcent_t));
                return 0;
            }
        }
    }

    return -1;
}

static void _rm_alloc(int block, int idx) {
    uintptr_t addr = _allocs[block][idx].addr;
    uintptr_t size = _allocs[block][idx].size;

    // See if this block immediately preceeds or proceeds any other block
    for(unsigned j = 0; j < ALLOC_BLOCKS; j++) {
        if(_allocs[j] == NULL) {
            continue;
        }
        for(unsigned i = 0; i < ALLOC_BLOCK; i++) {
            if( (_allocs[j][i].flags & ALCENTFLAG_VALID) &&
               !(_allocs[j][i].flags & ALCENTFLAG_USED)) {
                if(((_allocs[j][i].addr + size) == addr)  ||
                    (_allocs[j][i].addr == (addr + size))) {
                    _allocs[j][i].size += size;
                    if(_allocs[j][i].addr > addr) {
                        _allocs[j][i].addr = addr;
                    }
                    _allocs[block][idx].flags &= ~ALCENTFLAG_VALID;
                    return;
                }
            }
        }
    }

    // No preceeding or proceeding block found
    _allocs[block][idx].flags &= ~ALCENTFLAG_USED;
}

/**
 * @brief Find smallest available allocation block that fits the criteria
 *
 * @param sz Required size of memory region
 * @param align Required alignment. Must be a power of 2.
 *
 * @return uint32_t Two-part index of allocation block, or 0xFFFFFFFF on failure
 */
static uint32_t _find_hole(size_t sz, size_t align) {
    uint32_t idx  = 0xFFFFFFFF;
    uint32_t size = 0xFFFFFFFF;

    size_t alignmask = align - 1;

    for(unsigned j = 0; j < ALLOC_BLOCKS; j++) {
        if(!_allocs[j]) { continue; }
        for(unsigned i = 0; i < ALLOC_BLOCK; i++) {
            if( (_allocs[j][i].flags & ALCENTFLAG_VALID) &&
               !(_allocs[j][i].flags & ALCENTFLAG_USED)) {
                size_t off = align - (_allocs[j][i].addr & alignmask);
                if(off == align) {
                    off = 0;
                }
                if(_allocs[j][i].size >= (sz + off)) {
                    if(_allocs[j][i].size < size) {
                        idx = (uint16_t)i | j << 16;
                        size = _allocs[j][i].size;
                    }
                }
            }
        }
    }

    return idx;
}

static unsigned _empty_slots(int block) {
    unsigned fill = ALLOC_BLOCK;

    for(unsigned i = 0; i < ALLOC_BLOCK; i++) {
        if(_allocs[block][i].flags & ALCENTFLAG_VALID) {
            fill--;
        }
    }

    return fill;
}

static unsigned _get_free_block() {
    unsigned i = 0;
    for(; i < ALLOC_BLOCKS; i++) {
        if(_allocs[i] == 0) {
            return i;
        }
    }

    return ALLOC_BLOCKS;
}

static int _alloc_new_block(void) {
    const size_t asz = PAGE_SZ;

    /* @todo Request more memory when we run out */
    uint32_t idx = _find_hole(asz, MIN_ALIGN);
    if(idx == 0xFFFFFFFF) {
        /* Couold not create a new allocation block */
        return -1;
    }

    uint16_t block = idx >> 16;
    uint16_t index = idx & 0xFFFF;

    unsigned blk = _get_free_block();
    if(blk == ALLOC_BLOCKS) {
        return -1;
    }

    if(_allocs[block][index].size == asz) {
        _allocs[block][index].flags |= ALCENTFLAG_USED;
        _allocs[blk] = (alcent_t *)_allocs[block][index].addr;
    } else {
        _allocs[block][index].size -= asz;
        alcent_t ae = { .flags = (ALCENTFLAG_VALID | ALCENTFLAG_USED),
                        .addr = _allocs[block][index].addr, .size = asz };
        _allocs[block][index].addr += asz;
        _add_alloc(&ae);
        _allocs[blk] = (alcent_t *)ae.addr;
    }

    return 0;
}

static void *_alloc(size_t sz, size_t align) {
    if(!align || !sz) {
        return NULL;
    }

    if(align & (align - 1)) {
        /* Align must be a power of 2 */
        return NULL;
    }

    size_t alignmask = (align - 1);

    // We don't want two processes using the same memory block!
    //lock(&alloc_lock);

    // Find the smallest memory block that we can use
    uint32_t idx = _find_hole(sz, align);

    // Couldn't find one...
    if(idx == 0xFFFFFFFF) {
        //unlock(&alloc_lock);
        return NULL;
    }
    uint16_t block = idx >> 16;
    uint16_t index;

    /* TODO: This should be checking all free slots, not just in the returned block */
    if(_empty_slots(block) == 4) {
        _alloc_new_block();
    }

    // If the previous block of code was used, we may have to reinitialize these
    idx = _find_hole(sz, align);
    if(idx == 0xFFFFFFFF) {
        //unlock(&alloc_lock);
        return NULL;
    }
    block = idx >> 16;
    index = idx & 0xFFFF;


    if(_allocs[block][index].size == sz) {
        _allocs[block][index].flags |= ALCENTFLAG_USED;
        //unlock(&alloc_lock);
        return (void *)_allocs[block][index].addr;
    }
    
    alcent_t ae = { .flags = (ALCENTFLAG_VALID | ALCENTFLAG_USED),
                    .addr = _allocs[block][index].addr, .size = sz };

    if(_allocs[block][index].addr & alignmask) {
        /* Block isn't perfectly aligned */
        size_t correction = align - (_allocs[block][index].addr & alignmask);
        ae.addr = _allocs[block][index].addr + correction;
        if(_allocs[block][index].size > (sz + correction)) {
            /* Block also extends beyond needed space */
            alcent_t fe = { .flags = ALCENTFLAG_VALID,
                            .addr  = _allocs[block][index].addr + (sz + correction),
                            .size  = _allocs[block][index].size - (sz + correction) };
            _add_alloc(&fe);
        }
        _allocs[block][index].size = correction;
    } else {
        /* Block is aligned, but has extra space at the end */
        _allocs[block][index].size -= sz;
        _allocs[block][index].addr += sz;
    }

    _add_alloc(&ae);

    // Let other processes allocate memory
    //unlock(&alloc_lock);

    return (void *)ae.addr;
}

void *malloc(size_t size) {
    return _alloc(size, MIN_ALIGN);
}

static int _find_addr(uintptr_t addr, uint16_t *block, uint16_t *idx) {
    for(unsigned j = 0; j < ALLOC_BLOCKS; j++) {
        if(_allocs[j] == NULL) { continue; }
        for(unsigned i = 0; i < ALLOC_BLOCK; i++) {
            if(_allocs[j][i].flags & ALCENTFLAG_VALID) {
                if(_allocs[j][i].addr == addr) {
                    if(block) { *block = j; }
                    if(idx)   { *idx   = i; }
                    return 0;
                }
            }
        }
    }

    return -1;
}

void free(void *ptr) {
    //lock(&alloc_lock);

    uint16_t block = 0;
    uint16_t idx   = 0;

    if(!_find_addr((uintptr_t)ptr, &block, &idx)) {
        _rm_alloc(block, idx);
    }

    //unlock(&alloc_lock);

    /* @todo Error if not found */
}

void *realloc(void *ptr, size_t size) {
    /* @todo Attempt to enlarge region first */
    uint16_t block = 0;
    uint16_t idx   = 0;

    void *new = NULL;

    if(ptr && _find_addr((uintptr_t)ptr, &block, &idx)) {
        return NULL;
    }
    if(size) {
        new = malloc(size);
        if(ptr) {
            size_t sz = _allocs[block][idx].size;
            if(sz > size) { sz = size; }
            memcpy(new, ptr, sz);
        }
    }
    if(ptr) {
        free(ptr);
    }

    return new;
}
