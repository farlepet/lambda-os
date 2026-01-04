#ifndef LAMBDA_LIB_UNISTD_H
#define LAMBDA_LIB_UNISTD_H

#include <stddef.h>
#include <sys/types.h>

extern char **environ;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

size_t read(int desc, void *buff, size_t len);

size_t write(int desc, const void *buff, size_t len);

/**
 * \brief Close an open file descriptor
 * 
 * @param fd File descriptor to close
 * 
 * @return 0 on success, else -1
 */
int close(int fd);


/**
 * @brief Replace the current process image with a new one created from the
 * specified executable
 * 
 * @param path Path to executable file
 * @param argv List of program arguments, the first of which should represent the file being executed
 * @param envp List of environment variables
 * 
 * @return int -1 on error, else no return
 */
int execve(const char *path, char *const argv[], char *const envp[]);

/**
 * @brief Replace the current process image with a new one created from the
 * specified executable, with PATH searching.
 *
 * @param path Path to executable file
 * @param argv List of program arguments, the first of which should represent the file being executed
 *
 * @return int -1 on error, else no return
 */
int execvp(const char *path, char *const argv[]);

/**
 * @brief Forks the current running process into two identical processes.
 *
 * @return pid_t -1 on error, 0 for the for the child process, otherwise the PID
 * of the child process.
 */
pid_t fork(void);

#define F_OK (0)      /** File existence */
#define X_OK (1 << 0) /** Execute access */
#define W_OK (1 << 1) /** Write access */
#define R_OK (1 << 2) /** Read access */

/**
 * @brief Test accessibility or existence of file
 *
 * @param path Path of file to check
 * @param amode Access mode(s) to check
 *
 * @return int -1 on error, else 0
 */
int access(const char *path, int amode);

#endif