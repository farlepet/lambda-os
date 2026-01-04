#include <string.h>
#include <stdint.h>

void *memset(void * s, int c, size_t n) {
    /* @todo: Optimize */
    uint8_t *db = s;

    while(n--) {
        *db++ = c;
    }

    return s;
}
