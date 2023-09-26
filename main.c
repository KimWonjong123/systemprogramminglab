#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "linkedList.h"
#include "mystring.h"

#define FILE_READ_ERROR "File Read Error!\n"
#define FILE_OPEN_ERROR "File Open Error!\n"
#define BUFSIZE 25565

enum MODE
{
    SWORD,
    MWORD,
    CWORD,
    REGEXP
};

char *int_to_string(int num)
{
    int digit = 0;
    char buffer[100];
    if (num == 0)
    {
        buffer[0] = '0';
        digit++;
    }
    while (num > 0)
    {
        buffer[digit] = (num % 10) + '0';
        num /= 10;
        digit++;
    }

    char *ret = (char *)malloc(digit + 1);
    for (int i = digit-1; i >= 0; i--)
    {
        ret[digit - i - 1] = buffer[i];
    }
    ret[digit] = '\0';
    return ret;
}

int read_line(int fd, char result[BUFSIZE])
{
    int cnt = 0;
    char buf[2];
    int nbytes;
    while ((nbytes = read(fd, buf, 1)) > 0)
    {
        result[cnt++] = buf[0];
        if (buf[0] == '\n')
        {
            break;
        }
    }
    result[cnt - 1] = '\0';
    if (nbytes < 0)
    {
        write(2, FILE_READ_ERROR, stringlen(FILE_READ_ERROR));
        exit(-1);
    }
    return nbytes;
}

void resolve_input(char *input, enum MODE *mode, LinkedList *inputList)
{
    read(0, input, BUFSIZE);
    char buffer[BUFSIZE];
    int cnt = 0;
    input[stringlen(input) - 1] = '\0';
    if (input[0] != '"')
    {
        *mode = !issubstring(" ", input) ? SWORD : MWORD;
        if (*mode == SWORD)
        {
            char *content = (char *)malloc(stringlen(input) + 1);
            stringcpy(input, content);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);
        }
        else
        {
            char *inputp1 = input; // end point of word
            char *inputp2 = input; // starting point of word
            while (*inputp1 != '\0')
            {
                while (*inputp1 != ' ')
                {
                    inputp1++;
                }
                char *content = (char *)malloc(inputp1 - inputp2 + 1);
                stringncpy(inputp2, content, inputp1 - inputp2);
                insert_at_tail(inputList, create_node(++cnt, content));
                inputp2 = ++inputp1;
                free(content);
            }
        }
    }
    else
    {
        *mode = !issubstring("*", input) ? CWORD : REGEXP;
        if (*mode == CWORD)
        {
            char *content = (char *)malloc(stringlen(input) - 1);
            stringncpy(input + 1, content, stringlen(input) - 2);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);
        }
        else
        {
            char *asterisk = issubstring("*", input);
            char *content = (char *)malloc(asterisk - input);
            stringncpy(input + 1, content, asterisk - input - 1);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);

            content = (char *)malloc(stringlen(input) - stringlen(asterisk));
            stringncpy(asterisk + 1, content, stringlen(input) - stringlen(asterisk) - 1);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);
        }
    }
}

void handle_sword(LinkedList *textFile, LinkedList *inputList)
{
    Node *text = textFile->head;
    char *toFind = inputList->head->content;
    for (int i = 0; i < textFile->num; i++)
    {
        char *pos = issubstring(toFind, text->content);
        while (pos != NULL) {
            char *lineNum = int_to_string(text->lineNum);
            char *idx = int_to_string(pos - text->content);
            write(1, lineNum, stringlen(lineNum));
            write(1, ":", 1);
            write(1, idx, stringlen(idx));
            write(1, " ", 1);
            free(lineNum);
            free(idx);
            pos = issubstring(toFind, pos + 1);
        }
        text = text->next;
    }
    write(1, "\n", 1);
}

void handle_mword(LinkedList *textFile, LinkedList *inputList)
{
    
}

void handle_cword(LinkedList *textFile, LinkedList *inputList)
{
    
}

void handle_regexp(LinkedList *textFile, LinkedList *inputList)
{
    
}

int main(int argc, char *argv[])
{
    char *fileName = argv[1];
    int fd;
    char buffer[BUFSIZE];
    LinkedList textFile = {0, NULL, NULL};
    LinkedList inputList = {0, NULL, NULL};
    int lineCnt = 0;

    if ((fd = open(fileName, O_RDONLY | O_CREAT, 0755)) < 0)
    {
        write(2, FILE_OPEN_ERROR, stringlen(FILE_OPEN_ERROR));
        exit(-1);
    }

    char input[BUFSIZE];
    enum MODE mode;
    resolve_input(input, &mode, &inputList);

    while (read_line(fd, buffer))
    {
        insert_at_tail(&textFile, create_node(++lineCnt, buffer));
    }
    insert_at_tail(&textFile, create_node(++lineCnt, buffer));

    switch(mode)
    {
    case SWORD:
        handle_sword(&textFile, &inputList);
        break;
    case MWORD:
        handle_mword(&textFile, &inputList);
        break;
    case CWORD:
        handle_cword(&textFile, &inputList);
        break;
    case REGEXP:
        handle_regexp(&textFile, &inputList);
        break;
    default:
    }
    
    // write(1, "\nfile content:\n\n", 16);
    // print_list(&textFile);

    // write(1, "\ninput:\n\n", 9);
    // print_list(&inputList);

    delete_all_node(&textFile);
    delete_all_node(&inputList);

    close(fd);

    // insert_at_tail(&list, create_node(++lineCnt, "first line."));
    // insert_at_tail(&list, create_node(++lineCnt, "second line."));

    // print_list(&list);

    // delete_all_node(&list);

    // char *str = "this is a string.\n";
    // char *substr = "string";

    // char *result = issubstring(substr, str);

    // if (result != NULL) {
    //     write(2, result, stringlen(result));
    // }

    // char *testA = "stringC\n";
    // char *testB = "stringC\n";

    // int result2 = stringcmp(testA, testB);
    // if (result2) {
    //     write(2, "Two strings are equal.\n", 23);
    // }
    // else {
    //     write(2, "Two strings are not equal.\n", 27);
    // }

    close(fd);

    return 0;
}
