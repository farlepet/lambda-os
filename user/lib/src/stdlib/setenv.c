#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char **environ;

int setenv(const char *name, const char *value, int overwrite) {
    if(environ == NULL) {
        return -1;
    }

    if((name == NULL) || !strlen(name) || (value == NULL)) {
        return -1;
    }

    size_t len = strlen(name) + strlen(value) + 2;
    char *new = malloc(strlen(name) + strlen(value) + 2);
    if(new == NULL) {
        return -1;
    }
    snprintf(new, len, "%s=%s", name, value);

    size_t i = 0;
    while(environ[i]) {
        char *eq = strchr(environ[i], '=');
        if(eq && !strncmp(environ[i], name, (eq - environ[i]))) {
            if(overwrite) {
                free(environ[i]);
                environ[i] = new;
            } else {
                free(new);
            }
            return 0;
        }
        i++;
    }

    /* Add var */
    char **newenviron = realloc(environ, (i+2) * sizeof(char **));
    if(newenviron == NULL) {
        return -1;
    }
    environ = newenviron;

    environ[i]   = new;
    environ[i+1] = NULL;

    return 0;
}
