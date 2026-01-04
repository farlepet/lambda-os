#ifndef LAMBDA_LIB_DIRENT_H
#define LAMBDA_LIB_DIRENT_H

#include <stdint.h>

typedef uint32_t ino_t;

typedef struct {
    uint32_t fd; /** File descriptor of directory. */
/* The following is subject to change */
    uint32_t c_idx; /** Current directory child index */
} DIR;

#define DIRENT_NAME_SIZE 256

struct dirent {
    ino_t d_ino;                    /** File ID */
    char  d_name[DIRENT_NAME_SIZE]; /** Filename */
};

DIR *fdopendir(int fd);

DIR *opendir(const char *name);

struct dirent *readdir(DIR *dirp);

#endif
