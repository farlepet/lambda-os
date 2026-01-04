#include <string.h>
#include <stdint.h>

void *memmove(void * dest, const void * src, size_t n) {
    uint8_t       *db = dest;
    const uint8_t *sb = src;

    if(src > dest) {
        for(unsigned i = 0; i < n; i++) {
            db[i] = sb[i];
        }
    } else {
        for(int i = (n-1); i >= 0; i--) {
            db[i] = sb[i];
        }
    }

    return dest;
}
