#include <lambda/sys/syscalls.h>

void call_syscall(uint32_t scn, uint32_t *args)
{
	asm volatile("int $0xFF" :: "a"(scn), "b"((uint32_t)args));
}
