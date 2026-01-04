#include <stddef.h>
#include <dirent.h>
#include <fcntl.h>

DIR *fdopendir(int fd) {
    /* @todo Test file to ensure it is a directory */
    /* @todo Allocate memory at time of call */
    static DIR dir;
    
    dir.fd = fd;
	
    return &dir;
}
