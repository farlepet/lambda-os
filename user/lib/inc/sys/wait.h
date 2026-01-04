#ifndef LAMBDA_LIB_SYS_WAIT_H
#define LAMBDA_LIB_SYS_WAIT_H

#include <sys/types.h>

/**
 * Wait for child process to exit.
 * 
 * @param stat_loc Pointer to variable in which to store child exit information, or NULL.
 * @returns PID of child process
 */
pid_t wait(int *stat_loc);

/** Whether child process exited normally */
#define WIFEXITED(stat_val)    (0) // TODO
/** Lower 8 bits of exit status if non-zero */
#define WEXITSTATUS(stat_val)  (0) // TODO
/** Whether or not child processes exited due to unhandled signal */
#define WIFSIGNALED(stat_val)  (0) // TODO
/** Signal number that terminated child process, if WIFSIGNALED evaluates to true */
#define WTERMSIG(stat_val)     (0) // TODO
/** Whether child process is currently stopped */
#define WIFSTOPPED(stat_val)   (0) // TODO
/** Signal number that caused child process to stop, if WTERMSIG evaluates to true */
#define WSTOPSIG(stat_val)     (0) // TODO
/** Whether child process has continued after being stopped */
#define WIFCONTINUED(stat_val) (0) // TODO

#endif