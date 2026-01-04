#include <stdio.h>
#include <stddef.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

static int _display_file(const char *dir, const struct dirent *d) {
    /* @todo: Use dynamic buffer, and snprintf */
    size_t sz = strlen(dir) + strlen(d->d_name) + 2;
    char *buff = malloc(sz);
    if(buff == NULL) {
        return -1;
    }

    snprintf(buff, sz, "%s/%s", dir, d->d_name);

    struct stat s;
    if(stat(buff, &s)) {
        free(buff);
        return -1;
    }

    /* @todo Check if directory */
    printf("-%c%c%c%c%c%c%c%c%c %u %d %s\n",
           ((s.st_mode&0400)?'r':'-'), ((s.st_mode&0200)?'w':'-'), ((s.st_mode&0100)?'x':'-'),
           ((s.st_mode&0040)?'r':'-'), ((s.st_mode&0020)?'w':'-'), ((s.st_mode&0010)?'x':'-'),
           ((s.st_mode&0004)?'r':'-'), ((s.st_mode&0002)?'w':'-'), ((s.st_mode&0001)?'x':'-'),
           /* @todo Suport long long in lambda-lib */
           s.st_ino, (int)s.st_size, d->d_name);

    free(buff);
    return 0;
}

int command_ls(int argc, char **argv) {
    char *dirname = ".";
    if(argc > 1) {
        /* @todo Allow flags */
        dirname = argv[1];
    }

    DIR *dir = opendir(dirname);

    if(dir == NULL) {
        printf("ERR: Could not open '%s' as a directory.\n", dirname);
    }
    
    struct dirent *d;
    while((d = readdir(dir)) != NULL) {
        /* TODO: More advanced operations */
        _display_file(dirname, d);
    }


    return 0;
}