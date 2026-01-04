#include <sys/stat.h>

#include <lambda/sys/syscalls/stat.h>

int stat(const char *__restrict path, struct stat *__restrict buf) {
    kstat_t kstat;
    if(syscall_stat(path, &kstat, 0)) {
        return -1;
    }

    buf->st_dev     = kstat.dev_id;
    buf->st_ino     = kstat.inode;
    buf->st_mode    = kstat.mode;
    buf->st_nlink   = kstat.n_link;
    buf->st_uid     = kstat.uid;
    buf->st_gid     = kstat.gid;
    buf->st_rdev    = kstat.rdev_id;
    buf->st_size    = kstat.size;
    buf->st_blksize = kstat.blksize;
    buf->st_blocks  = kstat.alloc_size / 512;

    /*buf->st_atim.tv_sec  = kstat.atime / 1000000000;
    buf->st_atim.tv_nsec = kstat.atime % 1000000000;

    buf->st_ctim.tv_sec  = kstat.ctime / 1000000000;
    buf->st_ctim.tv_nsec = kstat.ctime % 1000000000;

    buf->st_mtim.tv_sec  = kstat.mtime / 1000000000;
    buf->st_mtim.tv_nsec = kstat.mtime % 1000000000;*/

    return 0;
}
