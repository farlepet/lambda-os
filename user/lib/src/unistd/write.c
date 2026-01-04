#include <unistd.h>
#include <lambda/sys/syscalls.h>

size_t write(int desc, const void *buff, size_t len) {
    uint32_t args[4] = { (uint32_t)desc, 0, (uint32_t)len, (uint32_t)buff };
	call_syscall(SYSCALL_FS_WRITE, args);
	return (int)args[0];
}