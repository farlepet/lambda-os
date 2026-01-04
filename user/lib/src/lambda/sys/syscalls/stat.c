#include <lambda/sys/syscalls/stat.h>
#include <lambda/sys/syscalls.h>

int syscall_stat(const char *path, kstat_t *buf, uint32_t flags) {
    syscallarg_t args[3] = { (syscallarg_t)path, (syscallarg_t)buf, (syscallarg_t)flags }; 
    call_syscall(SYSCALL_FS_STAT, args);
    return (int)args[0];
}
