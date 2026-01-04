#include <stdlib.h>
#include <string.h>

/* @todo Perhaps not the best place to store this. */
char **environ;

/* @todo Place this somewhere else */
extern int __lib_alloc_init();

/* Arguments are passed in via crt0. */
void _lib_init(int argc, char **argv, char **envp) {
    (void)argc;
    (void)argv;

    if(__lib_alloc_init()) {
        exit(-1);
    }

    unsigned envc = 0;
    while(envp[envc] != NULL) {
        envc++;
    }
    if(envc) {
        environ = malloc((envc + 1) * sizeof(char **));
        for(unsigned i = 0; i < envc; i++) {
            environ[i] = strdup(envp[i]);
            if(environ[i] == NULL) {
                exit(-1);
            }
        }
        environ[envc] = NULL;
    }
}