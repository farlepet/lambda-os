#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#define N_FILES 6
char *files[] = {
    "open",
    "./open",
    "etc/../open",
    "etc/motd",
    "/open",
    "/"
};

int main(void) {

    for(int i = 0; i < N_FILES; i++) {
        int fd = open(files[i], O_RDONLY);
        printf("Opened [%s]: %d\n", files[i], fd);
        if(fd > 0) {
            int ret = close(fd);
            printf("  -> Closed: %d\n", ret);
        }
    }
    
    return 0;
}