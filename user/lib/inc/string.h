#ifndef LAMBDA_LIB_STRING_H
#define LAMBDA_LIB_STRING_H

#include <stddef.h>

/**
 * Calculates the length of a string.
 * 
 * @param str the string to calculate the length of
 * @return the length of the string
 */
size_t strlen(const char *str);

/**
 * Compares two strings.
 * 
 * @param a first string to compare
 * @param b seconds string to compare
 * @return 0 if equal. Less than 0 if a < b, greater than 0 if a > b at the differing character.
 */
int strcmp(const char *a, const char *b);

/**
 * Compares the first n bytes of two strings.
 * 
 * @param a first string to compare
 * @param b seconds string to compare
 * @param n number of characters to compare
 * @return 0 if equal. Less than 0 if a < b, greater than 0 if a > b at the differing character.
 */
int strncmp(const char *a, const char *b, size_t n);

/**
 * @brief Locates a character within a string
 *
 * @param str String to search
 * @param c Character to search for
 * @return char* NULL of character not found, else pointer to character within str
 */
char *strchr(const char *str, int c);

/**
 * @brief Copy string, including null terminator
 *
 * @param dest Destination to copy string to
 * @param src Source to copy string from
 *
 * @return char* Pointer to dest
 */
char *strcpy(char *restrict dest, const char *src);

/**
 * @brief Copy string, with size limit
 *
 * @param dest Destination to copy string to
 * @param src Source to copy string from
 * @param n Nnumber of characters to copy/write, including null terminator
 *
 * @return char* Pointer to dest
 */
char *strncpy(char *restrict dest, const char *src, size_t n);

/**
 * @brief Duplicate string
 *
 * @param s String to duplicate
 * @return char* Pointer to new string, or NULL on failure
 */
char *strdup(const char *s);

/**
 * @brief Fill memory region with byte value
 *
 * @param s Pointer to memory region to fill
 * @param c Byte value to fill region with
 * @param n Number of bytes to write
 *
 * @return void* Pointer to memory region s
 */
void *memset(void * s, int c, size_t n);

/**
 * @brief Copy data from one location to another
 *
 * @note Memory regions must be non-overlapping
 *
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 *
 * @return void* Pointer to dest
 */
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

/**
 * @brief Copy data from one location to another. Locations may overlap
 *
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 *
 * @return void* Pointer to dest
 */
void *memmove(void * dest, const void * src, size_t n);

#endif
