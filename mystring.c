#include "mystring.h"
#include <stdlib.h>

int stringlen(char *string)
{
	int length = 0;
	while (*string++ != '\0')
	{
		length++;
	}
	return length;
}

int stringcmp(char *a, char *b)
{
	while (*a != '\0' && *b != '\0')
	{
		if (*a != *b)
		{
			return 0;
		}
		a++;
		b++;
	}
	return *a == '\0' && *b == '\0' ? 1 : 0;
}

char *issubstring(char *substr, char *str)
{
	int sublen = stringlen(substr);
	while (*str != '\0')
	{
		char *substrp = substr;
		if (*substrp == *str)
		{
			int i;
			for (i = 0; i < sublen; i++)
			{
				if (*(substrp + i) != *(str + i))
				{
					break;
				}
			}
			if (i == sublen)
			{
				return str;
			}
		}
		str++;
	}
	return NULL;
}

void stringcpy(char *source, char *dest) {
	int len = stringlen(source);
	for (int i = 0; i < len; i++) {
		dest[i] = source[i];
	}
	dest[len] = '\0';
}