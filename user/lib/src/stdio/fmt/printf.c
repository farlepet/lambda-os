#include "helpers.h"

#include <unistd.h>

int printf(const char *format, ...)
{
	__builtin_va_list varg;
	__builtin_va_start(varg, format);
    
    char temp[1024];
	int i = 0;
    while(i < 1024) temp[i++] = ' ';
    
    int ret = print(temp, (size_t)-1, format, varg);
    
    write(STDOUT_FILENO, temp, ret);
    
    __builtin_va_end(varg);
    
	return ret;
}