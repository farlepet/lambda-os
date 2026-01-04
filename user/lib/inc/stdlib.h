#ifndef LAMBDA_LIB_STDLIB_H
#define LAMBDA_LIB_STDLIB_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Terminate currently-running process
 * 
 * @param status Exit status to give to parent process.
 */
void exit(int status);

/**
 * @brief Get value of environment variable
 *
 * @param name Environment variable name
 *
 * @return char* Null if not found, else pointer to value in environ.
 */
char *getenv(const char *name);

/**
 * @brief Set value of environment variable
 *
 * @param name Name of environment variable
 * @param value Desired value of environment variable
 * @param overwrite Whether to overwrite variable if it already exists
 * @return int 0 on success, -1 on error
 */
int setenv(const char *name, const char *value, int overwrite);

/**
 * @brief Allocate memory region of given size
 *
 * @param size Size of desired memory region
 *
 * @return void* Pointer to allocated memory region, or NULL on failure
 */
void *malloc(size_t size);

/**
 * @brief Free a previously allocated memory region
 *
 * @param ptr Pointer to allocated memory region
 */
void free(void *ptr);

/**
 * @brief Change size of allocated memory region
 *
 * If ptr == NULL, it is equivalent to malloc
 * If size == 0, it is equivalent to free
 *
 * @param ptr Pointer to origional memory region
 * @param size Desired size of memory region
 * @return void* Pointer to new memory region
 */
void *realloc(void *ptr, size_t size);

#endif