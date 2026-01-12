#ifndef IPC_TYPES_IPC_H
#define IPC_TYPES_IPC_H

#include "data/types/cobuff.h"
#include "lambda/ipc.h"


typedef struct {
    /** PID of process waiting on response */
    int pid;
    /** Conditional for waiter waiting on response */
    cond_t cond;
    /** Lock for response channel */
    tlock_t lock;
    /** Response */
    lambda_ipc_message_t response;
    /** Whether a response has been send since last checking */
    uint8_t updated;
    /** Linked list item */
    llist_item_t list_item;
} ipc_resp_chan_t;

typedef struct {
    /** PID of process that owns channel */
    int pid;
    /** Message queue */
    cobuff_t queue;
    /** Response channels */
    llist_t resp_chans;
} ipc_chan_t;

#endif
