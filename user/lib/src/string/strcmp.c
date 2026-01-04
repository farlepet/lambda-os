#include <string.h>

int strcmp(const char *a, const char *b) {
    while(*a != '\0' && *b != '\0') {
        if(*a != *b) {
            return *a - *b;
        }
        a++;
        b++;
    }

    return *a - *b;
}
