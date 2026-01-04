#include <stdlib.h>
#include <string.h>

extern char **environ;

char *getenv(const char *name) {
    if(environ == NULL) {
        return NULL;
    }

    size_t i = 0;
    while(environ[i]) {
        char *eq = strchr(environ[i], '=');
        if(eq) {
            if(!strncmp(environ[i], name, (eq - environ[i]))) {
                return (eq + 1);
            }
        }
        i++;
    }

    return NULL;
}
