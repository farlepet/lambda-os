#include <unistd.h>
#include <lambda/sys/syscalls.h>

pid_t fork(void) {
    uint32_t ret;
    call_syscall(SYSCALL_FORK, &ret);
    return (pid_t)ret;
}