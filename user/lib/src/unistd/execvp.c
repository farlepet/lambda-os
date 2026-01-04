#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define EXECVP_PATH_MAX 255
/* @todo Use allocation */
static char filepath[EXECVP_PATH_MAX+1];

int execvp(const char *path, char *const argv[]) {
    if((path == NULL) || (argv == NULL)) {
        return -1;
    }
    
    if(strchr(path, '/')) {
        /* Either absolute (/...) or relative (.../...) path. */
        return execve(path, argv, environ);
    } else {
        char *env_path = getenv("PATH");
        if(env_path == NULL) {
            /* @todo Define this centrally somewhere */
            env_path = "/bin:/usr/bin";
        }

        char *end;
        size_t len;
        size_t pathlen = strlen(path);
        while(env_path) {
            end = strchr(env_path, ':');
            if(end) {
                len = end - env_path;
            } else {
                len = strlen(env_path);
            }

            if((pathlen + len + 1) < EXECVP_PATH_MAX) {
                memcpy(filepath, env_path, len);
                filepath[len] = '/';
                strcpy(filepath + (len+1), path);

                /* @todo Test if file exists via access() */
                if(!access(filepath, X_OK)) {
                    return execve(filepath, argv, environ);
                }

            }

            if(end) {
                env_path = end+1;
            } else {
                break;
            }
        }
    }

    /* @todo: errno */
    return -1;
}
