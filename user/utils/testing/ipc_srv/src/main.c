#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include <lambda/sys/syscalls.h>

/* TODO: Move to liblambda */
typedef struct {
    /** PID from which the message was sent. */
    int pid;
    /** Size of message in bytes */
    uint8_t length;
    /** Contents of message */
    uint8_t data[48];
} lambda_ipc_message_t;

int lambda_ipc_create(char *path) {
    syscallarg_t arg = (syscallarg_t)path;

    call_syscall(SYSCALL_IPC_CREATE, &arg);

    return (int)arg;
}

int lambda_ipc_send(int fd, lambda_ipc_message_t *msg) {
    syscallarg_t args[] = { (syscallarg_t)fd, (syscallarg_t)msg };

    call_syscall(SYSCALL_IPC_SEND, args);

    return (int)args[0];
}

int lambda_ipc_recv(int fd, lambda_ipc_message_t *msg) {
    syscallarg_t args[] = { (syscallarg_t)fd, (syscallarg_t)msg };

    call_syscall(SYSCALL_IPC_RECV, args);

    return (int)args[0];
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int ret;

    puts("ipc_srv\n");

    /* As it is currently written, create will modify this */
    ret = lambda_ipc_create("/ipc");
    if (ret) {
        printf("Failed to create IPC channel: %d\n", ret);
        return 1;
    }

    /* TODO: Have create return this */
    int fd = open("ipc", O_RDONLY);
    if (fd < 0) {
        printf("Failed to open IPC channel: %d\n", fd);
        return 1;
    }

    lambda_ipc_message_t msg = { 0, };
    puts("Waiting on message...\n");
    ret = lambda_ipc_recv(fd, &msg);
    if (ret) {
        printf("Failed waiting on IPC channel: %d\n", ret);
        return 1;
    }

    printf("Received IPC message of %u bytes from %d\n", msg.length, msg.pid);

    puts("ipc_srv done\n");

    return 0;
}


