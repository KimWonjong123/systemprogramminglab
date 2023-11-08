#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "err.h"

int main(int argc, char **argv) {
    char *file;
    char *buf[4096];
    int n;

    if (argc == 1) {
        file = NULL;
    }
    else {
        file = argv[1];
    }

    if (file == NULL) {
        while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
            write(STDOUT_FILENO, buf, n);
        }
    } else {
        int fd = open(file, O_RDONLY);
        if (fd == -1) {
            print_err("cat", errno);
            return 0;
        }
        while ((n = read(fd, buf, sizeof(buf))) > 0) {
            write(STDOUT_FILENO, buf, n);
        }
        close(fd);
    }
    return 0;
}