#ifndef LAMBDA_LIB_SYS_STAT_H
#define LAMBDA_LIB_SYS_STAT_H

#include <sys/types.h>

#include <time.h>

struct stat {
    dev_t           st_dev;
    ino_t           st_ino;
    mode_t          st_mode;
    nlink_t         st_nlink;
    uid_t           st_uid;
    gid_t           st_gid;
    dev_t           st_rdev;
    off_t           st_size;
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
    blksize_t       st_blksize;
    blkcnt_t        st_blocks;
};

int stat(const char *__restrict path, struct stat *__restrict buf);

#endif