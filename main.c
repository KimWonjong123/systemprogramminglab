#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "linkedList.h"
#include "linkedListStr.h"
#include "mystring.h"

#define FILE_READ_ERROR "File Read Error!\n"
#define FILE_OPEN_ERROR "File Open Error!\n"
#define BUFSIZE 4096

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

void read_file(int fd, LinkedList *indexList)
{
    long long offset = 0;
    char buffer[BUFSIZE];
    int nbytes;

    // read 4KB of file at once
    while ((nbytes = read(fd, buffer, BUFSIZE)) > 0)
    {
        int lineCnt = 0;
        char *start = buffer;
        char *end = buffer;
        long long size = 0;

        while (start - buffer + 1 <= nbytes)
        {
            while(end - buffer + 1 <= nbytes && *end != '\n')
            {
                end++;
            }
            size = end - start + 1;

            if (end - buffer + 1 > nbytes)
            {
                char test[1];
                if (read(fd, test, 1) != 0)
                {
                    lseek(fd, -size, SEEK_CUR);
                }
                break;
            }
            
            insert_at_tail_idx(indexList, create_node_idx(offset, size, ++lineCnt));
            offset += size;
            start = ++end;
        }
    }
    if (nbytes < 0)
    {
        write(2, FILE_READ_ERROR, stringlen(FILE_READ_ERROR));
        exit(-1);
    }
    printf("reading file done...\n");
}

enum MODE resolve_input(LinkedListStr *inputList)
{
    enum MODE mode;
    char input[BUFSIZE];
    int nbytes = read(0, input, BUFSIZE);
    int cnt = 0;
    input[nbytes - 1] = '\0';
    delete_all_node(inputList);
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
        char *content = (char *)malloc(stringlen(input) - 1);
        stringncpy(input + 1, content, stringlen(input) - 2);
        insert_at_tail(inputList, create_node(++cnt, content));
        free(content);
    }
    return mode;
}

void handle_sword(LinkedList *indexList, LinkedListStr *inputList, int fd)
{
    Node *idxNode = indexList->head;
    char *toFind = inputList->head->content;
    lseek(fd, idxNode->offset, SEEK_SET);
    for (int i = 0; i < indexList->num; i++, idxNode = idxNode->next)
    {
        int size = idxNode->size;
        // 오프셋, 사이즈 이용해서 파일에서 문장 추출
        char *content = (char *)malloc(size);
        read(fd, content, size);
        content[size - 1] = '\0';
        
        
        char *pos = isincluded(toFind, content);
        while (pos != NULL)
        {
            char *lineNum = int_to_string(idxNode->lineNum);
            char *idx = int_to_string(pos - content);
            write(1, lineNum, stringlen(lineNum));
            write(1, ":", 1);
            write(1, idx, stringlen(idx));
            write(1, " ", 1);
            free(lineNum);
            free(idx);
            pos = isincluded(toFind, pos + 1);
        }
        free(content);
    }
    write(1, "\n", 1);
    // NodeStr *text = textFile->head;
    // NodeStr *toFind = inputList->head;
    // for (int i = 0; i < textFile->num; i++)
    // {
    //     char *pos = isincluded(toFind->content, text->content);
    //     while (pos != NULL)
    //     {
    //         char *lineNum = int_to_string(text->lineNum);
    //         char *idx = int_to_string(pos - text->content);
    //         write(1, lineNum, stringlen(lineNum));
    //         write(1, ":", 1);
    //         write(1, idx, stringlen(idx));
    //         write(1, " ", 1);
    //         free(lineNum);
    //         free(idx);
    //         pos = isincluded(toFind->content, pos + 1);
    //     }
    //     text = text->next;
    // }
    // write(1, "\n", 1);
}

void handle_mword(LinkedList *indexList, LinkedListStr *inputList)
{
    // int numOfLine = textFile->num;
    // int i;
    // NodeStr *text = textFile->head;
    // NodeStr *toFind;
    // for (i = 0; i < numOfLine; i++, text = text->next)
    // {
    //     int j;
    //     int inputNum = inputList->num;
    //     toFind = inputList->head;
    //     for (j = 0; j < inputNum; j++, toFind = toFind->next)
    //     {
    //         if (!isincluded(toFind->content, text->content))
    //         {
    //             break;
    //         }
    //     }
    //     if (j == inputNum)
    //     {
    //         char *lineNum = int_to_string(text->lineNum);
    //         write(1, lineNum, stringlen(lineNum));
    //         free(lineNum);
    //         write(1, " ", 1);
    //     }
    // }
    // write(1, "\n", 1);
}

void handle_cword(LinkedListStr *textFile, LinkedListStr *inputList)
{
    // NodeStr *text = textFile->head;
    // NodeStr *toFind = inputList->head;
    // for (int i = 0; i < textFile->num; i++)
    // {
    //     char *pos = issubstring(toFind->content, text->content);
    //     while (pos != NULL && *(pos + 1) != ' ' && *(pos + 1) != '\t')
    //     {
    //         char *lineNum = int_to_string(text->lineNum);
    //         char *idx = int_to_string(pos - text->content);
    //         write(1, lineNum, stringlen(lineNum));
    //         write(1, ":", 1);
    //         write(1, idx, stringlen(idx));
    //         write(1, " ", 1);
    //         free(lineNum);
    //         free(idx);
    //         pos = isincluded(toFind->content, pos + 1);
    //     }
    //     text = text->next;
    // }
    // write(1, "\n", 1);
}

void handle_regexp(LinkedListStr *textFile, LinkedListStr *inputList)
{
    // char *word1 = inputList->head->content;
    // char *word2 = inputList->tail->content;
    // int numOfLine = textFile->num;
    // NodeStr *text = textFile->head;
    // for (int i = 0; i < numOfLine; i++, text = text->next)
    // {
    //     char *content = text->content;
    //     if ((content = isincluded(word1, content)) != NULL)
    //     {
    //         if ((content = isincluded(word2, nextWord(content))) != NULL)
    //         {
    //             char *lineNum = int_to_string(text->lineNum);
    //             write(1, lineNum, stringlen(lineNum));
    //             free(lineNum);
    //             write(1, " ", 1);
    //         }
    //     }
    // }
    // write(1, "\n", 1);
}

int main(int argc, char *argv[])
{
    char *fileName = argv[1];
    int fd;
    // char buffer[BUFSIZE];
    // LinkedListStr textFile = {0, NULL, NULL};
    LinkedListStr inputList = {0, NULL, NULL};
    LinkedList indexList = {0, NULL, NULL};
    // int lineCnt = 0;
    // char input[BUFSIZE];
    enum MODE mode;

    if ((fd = open(fileName, O_RDONLY | O_CREAT, 0755)) < 0)
    {
        write(2, FILE_OPEN_ERROR, stringlen(FILE_OPEN_ERROR));
        exit(-1);
    }

    read_file(fd, &indexList);

    // while (read_line(fd, buffer))
    // {
    //     insert_at_tail(&textFile, create_node(++lineCnt, buffer));
    // }
    // insert_at_tail(&textFile, create_node(++lineCnt, buffer));

    do
    {
        mode = resolve_input(&inputList);

        switch (mode)
        {
        case SWORD:
            handle_sword(&indexList, &inputList, fd);
            break;
        case MWORD:
            // handle_mword(&textFile, &inputList);
            break;
        case CWORD:
            // handle_cword(&textFile, &inputList);
            break;
        case REGEXP:
            // handle_regexp(&textFile, &inputList);
            break;
        case EXIT:
            break;
        default:
            mode = EXIT;
        }

    } while (mode != EXIT);

    // delete_all_node(&textFile);
    delete_all_node(&inputList);

    close(fd);

    return 0;
}
