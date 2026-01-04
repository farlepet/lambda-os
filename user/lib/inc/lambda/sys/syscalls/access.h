#ifndef LAMBDA_LIB_LAMBDA_SYS_SYSCALLS_ACCESS_H
#define LAMBDA_LIB_LAMBDA_SYS_SYSCALLS_ACCESS_H

#include <stdint.h>

#define SYSCALL_ACCESS_FLAG_CWDOVER (1UL << 0) /** Override CWD with given fd */
int syscall_access(int dirfd, const char *pathname, uint32_t mode, uint32_t flags);

#endif
