#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <sys/wait.h>

#include <shell.h>

const char *_default_prompt = SHELL_PROMPT_STR;
const char *_prompt_var     = "PS1";

static void _handle_command(const char *cmd);

static void _display_prompt(void) {
    const char *prompt = getenv(_prompt_var);
    if(prompt == NULL) {
        prompt = "> ";
    }
    puts(prompt);
}

void shell_loop(void) {
    char cmd[256];

    setenv(_prompt_var, _default_prompt, 0);

    _display_prompt();
    while(1) {
        // TODO: Replace gets, as it is unsafe
        gets(cmd);
        putchar('\n'); /* Workaround for '\n' character not being echoed right away via serial. */
        if(strlen(cmd) > 0) {
            _handle_command(cmd);
        }
        _display_prompt();
    }
}

#define QUOTE_REPLACE  '\x1F' /* Quotes are replaced with this value, so they can be used next to an environment variable */
#define DOLLAR_REPLACE '\x01' /* Escaped '$' are replaced with this, so they do not trigger a variable repolacement */

/**
 * Splits a string into a list of the command and arguments. Modifies `str`
 * in-place.
 *
 * TODO: Allow use of special characters (including spaces) using a `\`
 *
 * @param str String to split
 *
 * @return List of pointers to string, terminated by a NULL pointer.
 */
static char **_args_split(const char *str) {
#define MAX_PARAMS 32
    char   **params    = NULL;

#define RUN_COUNT 0
#define RUN_ALLOC 1
#define RUN_REAL  2
    for(unsigned run = 0; run < 3; run++) {
        char     strch  = 0;
        unsigned param  = 0;
        size_t   len    = 0;

        for(unsigned i = 0; i < strlen(str); i++) {
            if(strch && (str[i] == strch)) {
                strch = 0;
                if(run == RUN_REAL) {
                    params[param][len] = QUOTE_REPLACE;
                }
                len++;
            } else if(isspace(str[i]) && !strch) {
                if(run == RUN_ALLOC) {
                    params[param] = malloc(len+1);
                    if(params[param] == NULL) {
                        printf("ERROR: Could not allocate memory for parameter!\n");
                        goto FAILURE;
                    }
                } else if(run == RUN_REAL) {
                    params[param][len] = 0;
                }

                param++;
                len = 0;

                if(param >= MAX_PARAMS) {
                    printf("ERROR: Too many parameters!\n");
                    goto FAILURE;
                }

                while(isspace(str[i+1])) {
                    i++;
                }
            } else if((str[i] == '\\') && str[i+1]) {
                i++;
                if(run == RUN_REAL) {
                    if(str[i] == '$') {
                        params[param][len] = DOLLAR_REPLACE;
                    } else {
                        params[param][len] = str[i];
                    }
                }
                len++;
            } else if(!strch && ((str[i] == '\'') || (str[i] == '\"'))) {
                strch = str[i];
                if(run == RUN_REAL) {
                    params[param][len] = QUOTE_REPLACE;
                }
                len++;
            } else {
                if(run == RUN_REAL) {
                    params[param][len] = str[i];
                }
                len++;
            }
        }

        if(run == RUN_COUNT) {
            params = malloc((param + 2) * sizeof(char *));
            if(params == NULL) {
                printf("ERROR: Could not allocate memory for parameter list!\n");
                return NULL;
            }
            memset(params, 0, (param + 2) * sizeof(char *));
        } else if(run == RUN_ALLOC) {
            params[param] = malloc(len+1);
            if(params[param] == NULL) {
                printf("ERROR: Could not allocate memory for parameter!\n");
                goto FAILURE;
            }
        } else if(run == RUN_REAL) {
            params[param][len] = 0;
        }

        params[param+1] = NULL;
    }

    return params;

FAILURE:
    for(unsigned i = 0; params[i]; i++) {
        free(params[i]);
    }
    free(params);

    return NULL;
}

/**
 * @brief Replaces variables with their values, and cleans up arguments
 *
 * @param args Pointer to agument array
 *
 * @return int 0 on success, else non-zero
 */
static int _args_replace(char **args) {
#define MAX_ENVLEN 64
    for(unsigned i = 0; args[i] != NULL; i++) {
        size_t arg_len = strlen(args[i]);
        for(size_t j = 0; j < arg_len; j++) {
            if(args[i][j] == '$') {
                /* Variable */
                for(size_t k = j+1; k < (arg_len+1); k++) {
                    if(args[i][k] != '_' && !isalnum(args[i][k])) {
                        size_t var_sz = k - (j+1);
                        if(var_sz > MAX_ENVLEN) {
                            printf("Environment variable too long!\n");
                            return -1;
                        }

                        char *env_name = malloc(var_sz+1);
                        if(env_name == NULL) {
                            printf("Could not allocate memory for environment variable!\n");
                            return -1;
                        }
                        memcpy(env_name, &args[i][j+1], var_sz);
                        env_name[var_sz] = 0;

                        char *env_val = getenv(env_name);
                        if(env_val == NULL) {
                            env_val = "";
                        }
                        free(env_name);

                        arg_len = (arg_len - (var_sz + 1)) + strlen(env_val);
                        char *newarg = realloc(args[i], arg_len);
                        if(newarg == NULL) {
                            printf("Could not realloc argument!\n");
                            return -1;
                        }
                        args[i] = newarg;

                        memmove(&args[i][j+strlen(env_val)], &args[i][k], strlen(&args[i][k]));
                        memcpy(&args[i][j], env_val, strlen(env_val));
                        args[i][arg_len] = '\0';

                        j = j + strlen(env_val);
                        break;
                    }
                }
            } else if(args[i][j] == DOLLAR_REPLACE) {
                /* Just the dollar symbol */
                args[i][j] = '$';
            } else if(args[i][j] == QUOTE_REPLACE) {
                /* Delete character. strlen+1 to inlcude null terminator. */
                memcpy(&args[i][j], &args[i][j+1], strlen(&args[i][j+1])+1);
                arg_len--;
                j--;
            }
        }
    }

    return 0;
}

static int _execute_command(char **args) {
    char *cmd = args[0];

    if(cmd == NULL) {
        printf("ERR: No command found.\n");
        return 1;
    }

    int i = 0;
    while(args[++i]) {
        if(*args[i] == 0) break;
    }

    pid_t pid = fork();

    if(pid == -1) {
        printf("Error while forking!\n");
        return 1;
    } else if(pid == 0) { // Child process
        execvp(cmd, args);

        printf("`execve` returned, something went wrong!\n");
        exit(1);
    } else { // Parent process
        pid_t child = wait(NULL);
        if(child == -1) {
            printf("Something went wrong while waiting.\n");
        } else {
            /* @todo Get child's return status */
        }
    }

    return 0;
}


static void _handle_command(const char *str) {
    char **args = _args_split(str);

    if(args) {
        if(_args_replace(args)) {
            goto CMD_DONE;
        }
        _execute_command(args);

CMD_DONE:
        for(unsigned i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    }
}
