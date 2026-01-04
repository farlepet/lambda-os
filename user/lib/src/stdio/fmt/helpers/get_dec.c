#include "../helpers.h"

int get_dec(const char *str, arg_t *out)
{
	int n = 0;
	while(*str >= '0' && *str <= '9')
	{
		n *= 10;
		n += (int)(*str - '0');
		str++;
	}
	*out = (arg_t)str;
	
	return n;
}