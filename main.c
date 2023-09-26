#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "linkedList.h"
#include "mystring.h"

#define FILE_READ_ERROR "File Read Error!\n"

int main(int argc, char *argv[])
{
    char *fileName = argv[1];
    int fd;
    LinkedList list = {0, NULL, NULL};
    int lineCnt = 0;
    // if ((fd = open(fileName, O_RDONLY | O_CREAT, 0755)) < 0)
    // {
    //     write(2, FILE_READ_ERROR, stringlen(FILE_READ_ERROR));
    //     exit(-1);
    // }

    insert_at_tail(&list, create_node(++lineCnt, "first line."));
    insert_at_tail(&list, create_node(++lineCnt, "second line."));

    print_list(&list);

    delete_all_node(&list);

    char *str = "this is a string.\n";
    char *substr = "string";

    char *result = issubstring(substr, str);

    if (result != NULL) {
        write(2, result, stringlen(result));
    }

    char *testA = "stringC\n";
    char *testB = "stringC\n";

    int result2 = stringcmp(testA, testB);
    if (result2) {
        write(2, "Two strings are equal.\n", 23);
    }
    else {
        write(2, "Two strings are not equal.\n", 27);
    }

    // close(fd);

    return 0;
}
