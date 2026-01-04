#include <unistd.h>

#include <lambda/sys/syscalls/access.h>

int access(const char *path, int amode) {
    (void)amode; /* @todo Currently unsupported by the kernel */

    return syscall_access(0, path, 0, 0);
}
