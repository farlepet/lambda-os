#include <string.h>
#include <stdint.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    /* @todo: Optimize */
    uint8_t       *db = dest;
    const uint8_t *sb = src;

    while(n--) {
        *db++ = *sb++;
    }

    return dest;
}