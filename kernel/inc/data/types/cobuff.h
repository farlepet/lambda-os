#ifndef DATA_TYPES_COBUFF_H
#define DATA_TYPES_COBUFF_H

#include <stdint.h>

#include "proc/types/cond.h"
#include "proc/atomic/types/tlock.h"

/**
 * FIFO cicular buffer for larger objects, with thread-awareness.
 *
 * @note Currently objects are limited to 64 KiB, and buffer is limited to 64 Ki
 * objects.
 */
typedef struct {
    /** Size of a single object in the buffer */
    uint16_t obj_sz;
    /** Size of the buffer, in whole objects */
    uint16_t size;
    /** Index of oldest written object */
    uint16_t begin;
    /** Number of objects in the buffer */
    uint16_t count;
    /** The buffer */
    void *buff;
    /** Conditional to notify of new data */
    cond_t cond;
    /** Lock for buffer data */
    tlock_t lock;
} cobuff_t;

#endif /* DATA_TYPES_COBUFF_H */
