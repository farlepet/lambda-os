#include <string.h>

char *strncpy(char *restrict dest, const char *src, size_t n) {
    char *save = dest;

    while(n-- && *src) {
        *dest++ = *src++;
    }
    while(n--) { *dest++ = '\0'; }

    return save;
}
