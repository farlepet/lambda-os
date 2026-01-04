#include <unistd.h>
#include <lambda/sys/syscalls.h>

size_t read(int desc, void *buff, size_t len) {
    uint32_t args[4] = { (uint32_t)desc, 0, (uint32_t)len, (uint32_t)buff };
	call_syscall(SYSCALL_FS_READ_BLK, args);
	return (int)args[0];
}
