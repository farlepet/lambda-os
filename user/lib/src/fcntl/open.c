#include <lambda/sys/syscalls.h>
#include <fcntl.h>

int open(const char *name, int flags, ...) {
    // TODO: Handle extra arguments if necessary!

    uint32_t args[4] = { (uint32_t)name, (uint32_t)flags };
	call_syscall(SYSCALL_FS_OPEN, args);
	return (int)args[0];
}