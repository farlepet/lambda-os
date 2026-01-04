#include <lambda/sys/syscalls.h>

#include <unistd.h>

int close(int fd) {
    uint32_t args[4] = { (uint32_t)fd };
	call_syscall(SYSCALL_FS_CLOSE, args);
	return (int)args[0];
}