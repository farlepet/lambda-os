#include <string.h>

int strncmp(const char *a, const char *b, size_t n) {
    if(n == 0) return 0;

    while(*a != '\0' && *b != '\0' && --n) {
        if(*a != *b) {
            return *a - *b;
        }
        a++;
        b++;
    }
    
    return *a - *b;
}
