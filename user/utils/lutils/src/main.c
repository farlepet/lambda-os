#include <stdio.h>
#include <string.h>

#include "commands.h"

typedef struct {
    char *name;
    int (*func)(int, char **);
} command_entry_t;

command_entry_t commands[] = {
    { "cat",  command_cat  },
    { "ls",   command_ls   },
    { "echo", command_echo }
};

static const char *get_basename(const char *str);

int main(int argc, char **argv, char **envp) {
    (void)envp;
    
    if(argc < 1) {
        printf("FATAL: No program basename provided via argc.\n");
        return 1;
    }

    const char *cmd = get_basename(argv[0]);

    for(size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if(!strcmp(cmd, commands[i].name)) {
            if(commands[i].func) {
                return commands[i].func(argc, argv);
            } else {
                printf("FATAL: Command entry for `%s` does not have a valid function pointer!\n", argv[0]);
                return 1;
            }
        }
    }

    printf("FATAL: Basename `%s` invalid!\n", cmd);

    return 1;
}

static const char *get_basename(const char *str) {
    int idx = strlen(str) - 1;
    while(str[idx] != '/' && idx >= 0) {
        idx--;
    }

    return &str[idx + 1];
}
