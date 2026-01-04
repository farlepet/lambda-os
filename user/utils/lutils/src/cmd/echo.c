#include <stdio.h>

int command_echo(int argc, char **argv) {
    /* @todo Handle flags */
    for(int i = 1; i < argc; i++) {
        puts(argv[i]);
        putchar(' ');
    }

    putchar('\n');

    return 0;
}
