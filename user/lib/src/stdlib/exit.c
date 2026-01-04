#include <stdlib.h>
#include <lambda/sys/syscalls.h>

void exit(int status) {
    uint32_t arg = status; // <- Just in case int != uint32_t on some architecture
    call_syscall(SYSCALL_EXIT, &arg);
}
