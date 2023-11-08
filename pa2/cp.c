#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "err.h"

int main(int argc, char **argv) {
    char *file1;
    if (argc < 2) {
        printf("cp: missing file operand\n");
        return 0;
    }
    else {
        file1 = argv[1];
    }
    char *file2;
    if (argc < 3) {
        printf("cp: missing destination file operand after '%s'\n", file1);
        return 0;
    }
    else {
        file2 = argv[2];
    }

    int fd1 = open(file1, O_RDONLY);
    if (fd1 == -1) {
        print_err("cp", errno);
    }

    int fd2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        print_err("cp", errno);
    }

    char buf[4096];
    int n;
    while ((n = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, n);
    }

    close(fd1);
    close(fd2);
}