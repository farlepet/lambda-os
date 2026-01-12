#ifndef PROC_COND_H
#define PROC_COND_H

#include <mm/alloc.h>
#include <proc/types/cond.h>
#include <proc/atomic/types/tlock.h>

/**
 * @brief Initialize condition variable
 *
 * @param cond Condition veriable
 * @return int 0 on success, else non-zero
 */
int cond_init(cond_t *cond);

/**
 * @brief Free resources used by the condition variable
 *
 * @note This currently is not implemented, but should be
 *
 * @param cond Condition variable to destroy
 */
void cond_destroy(cond_t *cond);

/**
 * @brief Allocate and initialize a condition variable
 *
 * @return Pointer to condition variable on success, else NULL
 */
static inline cond_t *cond_create(void) {
    cond_t *cond = kmalloc(sizeof(cond_t));
    if(!cond) {
        return NULL;
    }
    /* Currently, cond_init never returns error */
    cond_init(cond);
    return cond;
}

/**
 * @brief Wait for condition variable to be signaled
 *
 * @param cond Condition variable
 * @param lock Lock protecting data condition variable is waiting on
 * @return int 0 on success, else non-zero
 */
int cond_wait(cond_t *cond, tlock_t *lock);

/**
 * @brief Wakeup threads waiting on condition varialbe
 *
 * @param cond Condition variable
 * @return int 0 on success, else non-zero
 */
int cond_signal(cond_t *cond);

#endif
