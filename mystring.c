#include "mystring.h"

int stringlen(char *string) {
	int length = 0;
	while (*string++ != '\0')
	{
		length++;
	}
	return length;
}
