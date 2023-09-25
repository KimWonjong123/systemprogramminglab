#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define FILE_READ_ERROR "File Read Error!\n"

int stringlen(char *string) {
    int length = 1;
    while(*string++ != '\0') {
        length++;
    }
    return length;
}

int main(int argc, char *argv[]) {
    char *fileName = argv[1];
    int fd;
    if((fd = open(fileName, O_RDONLY)) < 0) {
        write(2, FILE_READ_ERROR, stringlen(FILE_READ_ERROR));
        exit(-1);
    }

    close(fd);

    return 0;
}