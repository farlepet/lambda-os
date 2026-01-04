#include <string.h>

char *strchr(const char *str, int c) {
    do {
        if(*str == c) {
            return (char *)str;
        }
    } while(*(str++));

    return NULL;
}
