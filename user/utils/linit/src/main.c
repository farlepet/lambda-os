#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINIT_VERSION "v0.0.1"

void show_motd();

static char *envp[] = {
    "PATH=/:/bin",
    NULL
};

static void _run_login(char *);

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    puts("linit "LINIT_VERSION", built "__DATE__"\n\n");

    /* TODO: Actually use a login system, and determine login binary from config. */
    _run_login("/bin/lshell");

    return 0;
}

static void _run_login(char *login) {
    pid_t pid = fork();

    char *const args[2] = {
        login,
        NULL
    };

    if(pid == -1) {
        printf("linit: Error while forking!\n");
        while(1);
    } else if(pid == 0) { // Child process
        execve(login, args, envp);

        printf("linit: `execve` returned, something went wrong!\n");
        while(1);
    } else { // Parent process
        pid_t child = wait(NULL);
        if(child == -1) {
            printf("Something went wrong while waiting.\n");
        } else {
            /* @todo Get child's return status */
            printf("linit: login exited!\n");
            while(1);
        }
    }
}