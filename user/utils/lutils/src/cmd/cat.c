#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define CAT_BUFF_LEN 512

static int _cat_file(char *file);

int command_cat(int argc, char **argv) {
    if(argc == 1) {
        return _cat_file("--");
    } else {
        /* @todo Handle flags */
        for(int i = 1; i < argc; i++) {
            if(_cat_file(argv[i])) {
                return 1;
            }
        }
    }

    return 0;
}

static int _cat_file(char *file) {
    int fd;
    if(!strcmp(file, "--")) {
        /* Read from STDIN */
        fd = STDIN_FILENO;
    } else {
        fd = open(file, O_RDONLY);
        if(fd < 0) return -1;
    }

    char *buff = malloc(CAT_BUFF_LEN);
    if(buff == NULL) {
        return -1;
    }

    size_t len;
    while((len = read(fd, buff, CAT_BUFF_LEN)) > 0) {
        write(STDOUT_FILENO, buff, len);
    }

    free(buff);
    close(fd);

    return 0;
}