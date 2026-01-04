#include <wchar.h>

size_t  wcslen(const wchar_t *str)
{
	register size_t i = 0;
	while(str[i++]);
	return i;
}