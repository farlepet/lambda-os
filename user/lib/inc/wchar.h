#ifndef LAMBDA_LIB_WCHAR_H
#define LAMBDA_LIB_WCHAR_H

#include <stddef.h>

/**
 * Calculates the length of a wide string.
 * 
 * @param str the wide string to calculate the length of
 * @return the length of the wide string
 */
size_t  wcslen(const wchar_t *str);

#endif