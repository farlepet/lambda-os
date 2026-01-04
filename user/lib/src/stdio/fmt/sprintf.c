#include "helpers.h"

int snprintf(char *out, size_t size, const char *format, ...) {
	__builtin_va_list varg;
    __builtin_va_start(varg, format);
    
    int ret = print(out, size, format, varg);
    
	__builtin_va_end(varg);
	return ret;
}

int sprintf(char *out, const char *format, ...) {
	__builtin_va_list varg;
    __builtin_va_start(varg, format);
    
    int ret = print(out, (size_t)-1, format, varg);
    
	__builtin_va_end(varg);
	return ret;
}