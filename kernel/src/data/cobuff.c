#include <errno.h>
#include <string.h>

#include "lambda/export.h"
#include "data/cobuff.h"
#include "mm/alloc.h"
#include "proc/cond.h"
#include "proc/atomic/tlock.h"

#define WRAP(N, M) (((N) >= (M)) ? ((N) - (M)) : (N))

int cobuff_allocate(cobuff_t *buff, uint16_t obj_sz, uint16_t obj_cnt) {
    if(!obj_sz || !obj_cnt) {
        return -EINVAL;
    }

    memset(buff, 0, sizeof(&buff));

    if (cond_init(&buff->cond)) {
        return -EUNSPEC;
    }

    if (tlock_init(&buff->lock)) {
        return -ENOMEM;
    }

    buff->obj_sz = obj_sz;
    buff->size = obj_cnt;

    buff->buff = kmalloc(obj_sz * obj_cnt);
    if(!buff->buff) {
        return -ENOMEM;
    }

    return 0;
}
EXPORT_FUNC(cobuff_allocate);

void cobuff_free(cobuff_t *buff) {
    cond_destroy(&buff->cond);
    tlock_destroy(&buff->lock);

    kfree(buff->buff);
    memset(buff, 0, sizeof(*buff));
}
EXPORT_FUNC(cobuff_free);

int cobuff_put(cobuff_t *buff, const void *obj) {
    if(!buff || !buff->buff) {
        /* Invalid buffer */
        return -EINVAL;
    }
    if(buff->count >= buff->size) {
        /* Not enough room in cobuff */
        return -ENOMEM;
    }

    tlock_acquire(&buff->lock);

    uint16_t idx = WRAP((uint32_t)buff->begin + buff->count, buff->size);
    memcpy(buff->buff + (idx * buff->obj_sz), obj, buff->obj_sz);
    buff->count++;

    cond_signal(&buff->cond);

    tlock_release(&buff->lock);

    return 0;
}
EXPORT_FUNC(cobuff_put);

int cobuff_get(cobuff_t *buff, void *obj, uint8_t block) {
    if(!buff || !buff->buff) {
        /* Invalid buffer */
        return -EINVAL;
    }

    tlock_acquire(&buff->lock);

    if(buff->count == 0) {
        if (!block) {
            tlock_release(&buff->lock);
            return -ENOMEM;
        }
        /* TODO: If there are ever two readers of a cbuff, this is unsafe */
        if (cond_wait(&buff->cond, &buff->lock)) {
            tlock_release(&buff->lock);
            return -EUNSPEC;
        }
    }

    memcpy(obj, buff->buff + (buff->begin * buff->obj_sz), buff->obj_sz);

    buff->begin = WRAP(buff->begin + 1, buff->size);
    buff->count--;

    tlock_release(&buff->lock);

    return 0;
}
EXPORT_FUNC(cobuff_get);

