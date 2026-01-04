#include <sys/mman.h>

#include <lambda/sys/syscalls.h>

void *mmap(void *addr, size_t len, int prot, int flags, int filedes, off_t off) {
    syscallarg_t args[6] = { (syscallarg_t)addr,  (syscallarg_t)len,     (syscallarg_t)prot,
                             (syscallarg_t)flags, (syscallarg_t)filedes, (syscallarg_t)off };
    call_syscall(SYSCALL_MMAP, args);
    return (void *)args[0];
}
