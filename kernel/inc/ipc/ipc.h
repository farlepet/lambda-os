#ifndef IPC_IPC_H
#define IPC_IPC_H

#ifdef CONFIG_IPC

#include "ipc/types/ipc.h"

int sys_ipc_create(const char *path);

int sys_ipc_send(int fd, lambda_ipc_message_t *msg);

int sys_ipc_recv(int fd, lambda_ipc_message_t *msg);

#endif /* CONFIG_IPC */

#endif

