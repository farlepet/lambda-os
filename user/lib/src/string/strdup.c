#include <string.h>
#include <stdlib.h>

char *strdup(const char *s) {
    char *new = malloc(strlen(s) + 1);
    if(new == NULL) {
        return NULL;
    }

    strcpy(new, s);
    return new;
}
