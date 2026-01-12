#ifndef DATA_COBUFF_H
#define DATA_COBUFF_H

#include <stddef.h>

#include "data/types/cobuff.h"

int cobuff_allocate(cobuff_t *buff, uint16_t obj_sz, uint16_t obj_cnt);

void cobuff_free(cobuff_t *buff);

/**
 * @brief Place object into circular buffer
 *
 * @param data Byte to place in buffer
 * @param buff Circular buffer in which to place byte
 * @return int 0 on success,
 *             -E* on failure
 */
int cobuff_put(cobuff_t *buff, const void *obj);

/**
 * @brief Read object from circular buffer
 *
 * @todo Modify to take timeout, and do same for cond API
 *
 * @param buff Circular buffer from which to read
 * @param obj Where to place object data
 * @param block Whether to block if there are no objects available
 *
 * @return int Read byte on success,
 *             -E* on failure
 */
int cobuff_get(cobuff_t *buff, void *obj, uint8_t block);


#endif // CBUFF_H
