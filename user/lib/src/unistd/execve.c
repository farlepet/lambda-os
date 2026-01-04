#include <lambda/sys/syscalls.h>

#include <unistd.h>

int execve(const char *path, char *const argv[], char *const envp[]) {
    uint32_t args[3] = { (uint32_t)path, (uint32_t)argv, (uint32_t)envp };
    call_syscall(SYSCALL_EXECVE, args);
    return (int)args[0];
}