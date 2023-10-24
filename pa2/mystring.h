#ifndef __MYSTRING_H__
#define __MYSTRING_H__

// returns string lenth excluding '\0'
int stringlen(char *string);

/*
    compares if two strings are equal
    if so, returns 1.
    else, returns 0.
*/
int stringcmp(char *a, char *b);

/*
    checks whether 1st arg is substring of 2nd arg.
    if so, returns starting position.
    else, returns NULL.
*/
char *issubstring(char *substr, char *str);

/*
    checks whether given word is included in given string.
    if so, returns starting point of word.
    else, returns NULL.
*/
char *isincluded(char *word, char *str);

/*
    copies string from 1st arg.
*/
void stringcpy(char *source, char *dest);


/*
    copies n bytes from 1st arg.
*/
void stringncpy(char *source, char *dest, int nbytes);


/*
    returns a pointer to the starting point of next word
    or the end of the given string.
    if (*str == '\0'), return NULL
*/
char *nextWord(char *str);

void toLowercase(char *str);

#endif
