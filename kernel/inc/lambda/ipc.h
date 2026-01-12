#ifndef LAMBDA_IPC_H
#define LAMBDA_IPC_H

#define LAMBDA_IPC_MAX_LENGTH 48

typedef struct {
    /** PID of process that sent initial request, used in responses to find
     *  correct process */
    int pid;
    /** Size of message in bytes */
    uint8_t length;
    /** Contents of message */
    uint8_t data[LAMBDA_IPC_MAX_LENGTH];
} lambda_ipc_message_t;

#endif
