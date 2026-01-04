#include <unistd.h>
#include <stdio.h>

int puts(const char *s) {

    size_t len = 0;
    while(s[len] != '\0') len++;

    if(write(STDOUT_FILENO, s, len) != len) {
        return EOF;
    }

    return len;
}