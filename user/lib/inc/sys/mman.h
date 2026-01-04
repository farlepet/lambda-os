#ifndef LAMBDA_LIB_SYS_MMAN_H
#define LAMBDA_LIB_SYS_MMAN_H

#include <stdint.h>
#include <sys/types.h>

#define MAP_PRIVATE   (1)
#define MAP_SHARED    (2)

#define MAP_FIXED     (1 << 4)
#define MAP_ANONYMOUS (1 << 5)

#define PROT_NONE  (0)
#define PROT_READ  (1 << 0)
#define PROT_WRITE (1 << 1)
#define PROT_EXEC  (1 << 2)

void *mmap(void *addr, size_t len, int prot, int flags, int filedes, off_t off);

#endif
