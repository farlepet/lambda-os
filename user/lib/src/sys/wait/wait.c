#include <sys/types.h>
#include <sys/wait.h>

#include <lambda/sys/syscalls.h>

pid_t wait(int *stat_loc) {
    uint32_t args[1] = { (uint32_t)stat_loc };
    call_syscall(SYSCALL_WAIT, args);
    return (pid_t)args[0];
}