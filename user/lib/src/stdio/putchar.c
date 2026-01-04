#include <unistd.h>
#include <stdio.h>

int putchar(int c) {
    char ch = (char)c;

    if(write(STDOUT_FILENO, &ch, sizeof(char)) != sizeof(char)) {
        return EOF;
    }

    return c;
}