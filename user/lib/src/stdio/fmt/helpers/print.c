#include "../helpers.h"

#include <string.h>
#include <wchar.h>

int print(char *out, size_t maxlen, const char *format, __builtin_va_list varg)
{
	uint8_t is_in_spec = 0;
	int8_t  size = 0;      // Size of the integer
	uint8_t width = 0;     // Width of the number at minimum
	uint8_t precision = 0; // Precision
	uint8_t showsign = 0;  // Show the sign on positive numbers
	uint8_t signspace = 0; // Place a space before positive numbers
	uint8_t leftalign = 0; // Align to the left
	uint8_t padzeros = 0;  // Use zeros instead of spaces for padding
	
	size_t nchars = 0;    // Number of chars printed so far
	
	arg_t temp;
	
	for(; *format != 0; format++)
	{
		if(nchars >= maxlen) {
			/* @todo Make sure we don't exceed maxlen anywhere */
			out[maxlen-1] = '\0';
			return (int)maxlen;
		}

		if(!is_in_spec)
		{
			if(*format == FMT_SPEC)
			{
				is_in_spec = 1;
				continue;
			}
			*out++ = *format;
			nchars++;
			continue;
		}
		
		switch(*format)
		{
			case FMT_SPEC: is_in_spec = 0;
						   *out++ = FMT_SPEC;
						   nchars++;
						   break;
			
			case 'l': if(size < 2) size++;
					  break;
		
			case 'L': size = 1;
					  break;
			
			case 'h': if(size > -2) size--;
					  break;
			
			case 'z':
			case 'j':
			case 't': size = 0;
					  break;
		
			case '+': showsign = 1;
					  break;
			
			case ' ': signspace = 1;
					  break;
			
			case '-': leftalign = 1;
					  break;
		
			case '0': padzeros = 1;
					  break;
			
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': width = (uint8_t)get_dec(format, &temp);
					  format = (char *)(temp - 1);
					  break;
			
			case '.': format++;
					  precision = (uint8_t)get_dec(format, &temp);
					  format = (char *)(temp - 1);
					  break;
			
			
		// Numbers, strings, etc...
			
			case 'd':
			case 'i': temp = (arg_t)va_arg(varg, int);
					  if(size == -1) temp &= 0xFFFF;
					  if(size == -2) temp &= 0xFF;
					  if(size == 2){} // TODO: Handle this!
					  temp = (arg_t)print_int(temp, 10, 0, width, padzeros, showsign, signspace, 0, out);
					  nchars += temp;
					  out += temp;
					  ZERO_ALL_VID();
					  break;
					  
			case 'u': temp = (arg_t)va_arg(varg, uint32_t);
					  if(size == -1) temp &= 0xFFFF;
					  if(size == -2) temp &= 0xFF;
					  if(size == 2){} // TODO: Handle this!
					  temp = (arg_t)print_int(temp, 10, 1, width, padzeros, showsign, signspace, 0, out);
					  nchars += temp;
					  out += temp;
					  ZERO_ALL_VID();
					  break;
			
			case 'f':
			case 'F':
			case 'e':
			case 'E':
			case 'g':
			case 'G': (void)va_arg(varg, double);
					  ZERO_ALL_VID();
					  break;
					  
			case 'x':
			case 'X': temp = (arg_t)va_arg(varg, uint32_t);
					  if(size == -1) temp &= 0xFFFF;
					  if(size == -2) temp &= 0xFF;
					  if(size == 2){} // TODO: Handle this!
					  temp = (arg_t)print_int(temp, 16, 1, width, padzeros, showsign, signspace, (*format == 'X'), out);
					  nchars += temp;
					  out += temp;
					  ZERO_ALL_VID();
					  break;
			
			case 'o': temp = (arg_t)va_arg(varg, uint32_t);
					  if(size == -1) temp &= 0xFFFF;
					  if(size == -2) temp &= 0xFF;
					  if(size == 2){} // TODO: Handle this!
					  temp = (arg_t)print_int(temp, 8, 1, width, padzeros, showsign, signspace, 0, out);
					  nchars += temp;
					  out += temp;
					  ZERO_ALL_VID();
					  break;
					 
			case 's': if(size > 0)
					  {
						  temp = (arg_t)va_arg(varg, wchar_t *);
						  nchars += wcslen((wchar_t *)temp);
						  while(*(wchar_t *)temp) *out++ = (char)*(wchar_t *)temp++;
					  }
					  else
					  {
						  temp = (arg_t)va_arg(varg, char *);
						  nchars += strlen((char *)temp);
						  while(*(char *)temp) *out++ = *(char *)temp++;
					  }
					  ZERO_ALL_VID();
					  break;
					
			case 'c': temp = (arg_t)va_arg(varg, int);
					  if(size > 0) *out++ = (char)temp;
					  else         *out++ = (char)temp;
					  nchars++;
					  ZERO_ALL_VID();
					  break;
					
			case 'p': temp = (arg_t)va_arg(varg, arg_t);
					  temp = (arg_t)print_int(temp, 16, 1, width, padzeros, showsign, signspace, 1 /* Should this be upper or lower case? */, out);
					  nchars += temp;
					  out += temp;
					  break;
					
			case 'a':
			case 'A': (void)va_arg(varg, double);
					  ZERO_ALL_VID();
					  break;
					
			case 'n': (void)va_arg(varg, int);
					  ZERO_ALL_VID();
					  break;
		}
		
	}
	
	*(out) = 0;
	
	// These aren't used yet:
	(void)precision;
	(void)leftalign;
	
	return (int)nchars;
}