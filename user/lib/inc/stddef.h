#ifndef LAMBDA_LIB_STDDEF_H
#define LAMBDA_LIB_STDDEF_H

// Valid for 32-bit machines:
typedef unsigned int       ptrdiff_t;
typedef unsigned long long    size_t;
typedef short                wchar_t;

#define NULL (void *)0

#endif