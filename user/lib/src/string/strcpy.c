#include <string.h>

char *strcpy(char *restrict dest, const char *src) {
    char *save = dest;

    do {
        *dest++ = *src;
    } while(*src++);

    return save;
}
