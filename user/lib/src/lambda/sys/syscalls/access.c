#include <lambda/sys/syscalls/access.h>
#include <lambda/sys/syscalls.h>

int syscall_access(int dirfd, const char *pathname, uint32_t mode, uint32_t flags) {
    syscallarg_t args[4] = { (syscallarg_t)dirfd, (syscallarg_t)pathname, (syscallarg_t)mode, (syscallarg_t)flags }; 
    call_syscall(SYSCALL_FS_ACCESS, args);
    return (int)args[0];
}

