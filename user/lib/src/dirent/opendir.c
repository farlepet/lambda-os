#include <dirent.h>
#include <stddef.h>
#include <fcntl.h>

#define OPENDIR_FLAGS (O_RDONLY | O_DIRECTORY | O_CLOEXEC)

DIR *opendir(const char *name) {
    int fd = open(name, OPENDIR_FLAGS);
    if(fd < 0) {
        return NULL;
    }

    /* @todo Allocate memory at time of call */
    static DIR dir;
    dir.fd = fd;

	return &dir;
}
