#include <lambda/sys/syscalls.h>
#include <stddef.h>
#include <dirent.h>
#include <fcntl.h>

struct dirent *readdir(DIR *dirp) {
    if(dirp == NULL) { return NULL; }

    /* @todo Allocate */
    static struct dirent d;

    uint32_t args[4] = {
        dirp->fd,
        dirp->c_idx,
        (uint32_t)&d,
        sizeof(struct dirent)
    };
    call_syscall(SYSCALL_FS_READDIR, args);

    if((int32_t)args[0] < 0) {
        return NULL;
    }

    dirp->c_idx++;

    return &d;
}
