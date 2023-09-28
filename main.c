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
    REGEXP,
    EXIT
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
    for (int i = digit - 1; i >= 0; i--)
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
    if (buf[0] == '\n')
    {
        result[cnt - 1] = '\0';
    }
    if (nbytes == 0)
    {
        result[cnt] = '\0';
    }
    if (nbytes < 0)
    {
        write(2, FILE_READ_ERROR, stringlen(FILE_READ_ERROR));
        exit(-1);
    }
    return nbytes;
}

enum MODE resolve_input(char *input, LinkedList *inputList)
{
    enum MODE mode;
    read(0, input, BUFSIZE);
    int cnt = 0;
    input[stringlen(input) - 1] = '\0';
    if (stringcmp("PA1EXIT", input))
    {
        return EXIT;
    }
    if (input[0] != '"')
    {
        if (issubstring("*", input))
        {
            mode = REGEXP;
        }
        else
        {
            mode = !issubstring(" ", input) ? SWORD : MWORD;
        }
        if (mode == SWORD)
        {
            char *content = (char *)malloc(stringlen(input) + 1);
            stringcpy(input, content);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);
        }
        else if (mode == MWORD)
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
        else
        {
            char *asterisk = issubstring("*", input);
            char *content = (char *)malloc(asterisk - input + 1);
            stringncpy(input, content, asterisk - input);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);

            content = (char *)malloc(stringlen(asterisk));
            stringncpy(asterisk + 1, content, stringlen(asterisk) - 1);
            insert_at_tail(inputList, create_node(++cnt, content));
            free(content);
        }
    }
    else
    {
        mode = CWORD;
        // if (mode == CWORD)
        // {
        char *content = (char *)malloc(stringlen(input) - 1);
        stringncpy(input + 1, content, stringlen(input) - 2);
        insert_at_tail(inputList, create_node(++cnt, content));
        free(content);
        // }
    }
    return mode;
}

void handle_sword(LinkedList *textFile, LinkedList *inputList)
{
    Node *text = textFile->head;
    Node *toFind = inputList->head;
    for (int i = 0; i < textFile->num; i++)
    {
        char *pos = isincluded(toFind->content, text->content);
        while (pos != NULL)
        {
            char *lineNum = int_to_string(text->lineNum);
            char *idx = int_to_string(pos - text->content);
            write(1, lineNum, stringlen(lineNum));
            write(1, ":", 1);
            write(1, idx, stringlen(idx));
            write(1, " ", 1);
            free(lineNum);
            free(idx);
            pos = isincluded(toFind->content, pos + 1);
        }
        text = text->next;
    }
    write(1, "\n", 1);
}

void handle_mword(LinkedList *textFile, LinkedList *inputList)
{
    int numOfLine = textFile->num;
    int i;
    Node *text = textFile->head;
    Node *toFind;
    for (i = 0; i < numOfLine; i++, text = text->next)
    {
        int j;
        int inputNum = inputList->num;
        toFind = inputList->head;
        for (j = 0; j < inputNum; j++, toFind = toFind->next)
        {
            if (!isincluded(toFind->content, text->content))
            {
                break;
            }
        }
        if (j == inputNum)
        {
            char *lineNum = int_to_string(text->lineNum);
            write(1, lineNum, stringlen(lineNum));
            free(lineNum);
            write(1, " ", 1);
        }
    }
    write(1, "\n", 1);
}

void handle_cword(LinkedList *textFile, LinkedList *inputList)
{
    Node *text = textFile->head;
    Node *toFind = inputList->head;
    for (int i = 0; i < textFile->num; i++)
    {
        char *pos = issubstring(toFind->content, text->content);
        while (pos != NULL && *(pos + 1) != ' ' && *(pos + 1) != '\t')
        {
            char *lineNum = int_to_string(text->lineNum);
            char *idx = int_to_string(pos - text->content);
            write(1, lineNum, stringlen(lineNum));
            write(1, ":", 1);
            write(1, idx, stringlen(idx));
            write(1, " ", 1);
            free(lineNum);
            free(idx);
            pos = isincluded(toFind->content, pos + 1);
        }
        text = text->next;
    }
    write(1, "\n", 1);
}

void handle_regexp(LinkedList *textFile, LinkedList *inputList)
{
    char *word1 = inputList->head->content;
    char *word2 = inputList->tail->content;
    int numOfLine = textFile->num;
    Node *text = textFile->head;
    for (int i = 0; i < numOfLine; i++, text = text->next)
    {
        char *content = text->content;
        if ((content = isincluded(word1, content)) != NULL)
        {
            if ((content = isincluded(word2, nextWord(content))) != NULL)
            {
                char *lineNum = int_to_string(text->lineNum);
                write(1, lineNum, stringlen(lineNum));
                free(lineNum);
                write(1, " ", 1);
            }
        }
    }
    write(1, "\n", 1);
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
    enum MODE mode = resolve_input(input, &inputList);

    while (read_line(fd, buffer))
    {
        insert_at_tail(&textFile, create_node(++lineCnt, buffer));
    }
    insert_at_tail(&textFile, create_node(++lineCnt, buffer));

    switch (mode)
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
    case EXIT:
        break;
    default:
        break;
    }

    // write(1, "\nfile content:\n", 16);
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

    // close(fd);

    return 0;
}
