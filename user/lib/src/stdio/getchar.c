#include <unistd.h>
#include <stdio.h>

int getchar(void) {
    char ch;
    if(read(STDIN_FILENO, &ch, sizeof(char)) != sizeof(char)) {
        return EOF;
    }

    return ch;
}