#include <stdint.h>
#include <string.h>

int print_int(uint32_t num, uint8_t base, uint8_t u, uint8_t pad, uint8_t padzero, uint8_t possign, uint8_t posspace, uint8_t _case, char *out)
{
	int onum = (int)num;
	if(onum < 0) if(!u) num = (~num) + 1;
	char *nums;
	if(_case) nums = "0123456789ABCDEF";
	else      nums = "0123456789abcdef";
	
	char ans[16] = { '0', };
	int i = 0;
	while(num)
	{
		ans[i++] = nums[num % base];
		num /= base;
	}
	if(i == 0) i++;
	
	if(!u)
	{
		if(onum >= 0)
		{
			if(possign)
			{
				*out++ = '+';
			}
			else if(posspace)
				*out++ = ' ';
		}
		else if(onum < 0)
			*out++ = '-';
	}
	
	int p = pad - i;
	if(p > 0)
	{
		while(p--) *out++ = (padzero ? '0' : ' ');
		while(--i >= 0) *out++ = ans[i];
		return (int)((pad > strlen(ans)) ? pad : strlen(ans)) + ((((possign || posspace) && !u) && onum >= 0) || ((onum < 0) && !u)) + 1;
	}
	else
	{
		while(--i >= 0) *out++ = ans[i];
		return (int)(strlen(ans) + ((((possign || posspace) && !u) && onum >= 0) || ((onum < 0) && !u)));
	}
}