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

char *isincluded(char *word, char *str) {
	char *start = str;
	char *end = str;
	char *substr;
	while (*start != '\0')
	{
		while (*end != ' ' && *end != '\t' && *end != '\0')
		{
			end++;
		}
		substr = (char *)malloc(end - start + 1);
		stringncpy(start, substr, end - start);
		if (stringcmp(word, substr))
		{
			free(substr);
			return start;
		}
		end = start = nextWord(start);
		free(substr);
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

void stringncpy(char *source, char *dest, int nbytes) {
	for (int i = 0; i < nbytes; i++) {
		dest[i] = source[i];
	}
	dest[nbytes] = '\0';
}

char *nextWord(char *str) {
	while (*str != ' ' && *str != '\t' && *str != '\0') {
		str++;
	}
	while (*str == ' ' || *str == '\t') {
		str++;
	}
	return str;
}