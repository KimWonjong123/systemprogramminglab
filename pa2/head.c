#include <unistd.h>
#include <fcntl.h>
#include "err.h"
#include <stdlib.h>

int main(int argc, char **argv) {
    int cnt = 0, line = 10;
    char *file;
    int fd;
    switch (argc) {
        case 1:
            file = NULL;
            break;
        case 2:
            file = argv[1];
            break;
        case 3:
            file = NULL;
        case 4:
            line = (int)strtol(argv[2], NULL, 10);
            file = argv[3];
            break;
        default:
            print_err("head", 0);
            return 0;
    }


    if (file == NULL) {
        fd = STDIN_FILENO;
    } else {
        fd = open(file, O_RDONLY);
        if (fd == -1) {
            print_err("head", errno);
            return 0;
        }
    }

    char buf[1];
    while (cnt < line && read(fd, buf, sizeof(buf)) > 0) {
        if (buf[0] == '\n') {
            cnt++;
        }
        write(STDOUT_FILENO, buf, sizeof(buf));
    }
    close(fd);

    return 0;
}