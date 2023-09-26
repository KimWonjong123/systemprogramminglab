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

int read_line(int fd, char result[BUFSIZE]) {
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
    if (nbytes < 0) {
        write(2, FILE_READ_ERROR, stringlen(FILE_READ_ERROR));
        exit(-1);
    }
    return nbytes;
}

enum MODE resolve_input() {
    char buf[BUFSIZE];
    read(0, buf, BUFSIZE);
    if (buf[0] != '"') {
        return !issubstring(" ", buf) ? SWORD : MWORD;
    }
    else {
        return !issubstring("*", buf) ? CWORD : REGEXP;
    }
}

int main(int argc, char *argv[])
{
    char *fileName = argv[1];
    int fd;
    char buffer[BUFSIZE];
    LinkedList textfile = {0, NULL, NULL};
    int lineCnt = 0;

    if ((fd = open(fileName, O_RDONLY | O_CREAT, 0755)) < 0)
    {
        write(2, FILE_OPEN_ERROR, stringlen(FILE_OPEN_ERROR));
        exit(-1);
    }

    enum MODE mode = resolve_input();

    while (read_line(fd, buffer)) {
        insert_at_tail(&textfile, create_node(++lineCnt, buffer));
    }
    insert_at_tail(&textfile, create_node(++lineCnt, buffer));

    print_list(&textfile);

    delete_all_node(&textfile);

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
