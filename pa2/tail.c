#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "err.h"

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
    struct stat stbuf;
    stat(file, &stbuf);
    off_t file_size = stbuf.st_size;
    off_t offset = lseek(fd, -1, SEEK_END);
    while (cnt < line && offset > 0) {
        read(fd, buf, sizeof(buf));
        if (buf[0] == '\n' && offset != file_size - 1) {
            cnt++;
        }
        offset = lseek(fd, -2, SEEK_CUR);
    }

    if (offset <= 0)
        lseek(fd, 0, SEEK_SET);
    else
        lseek(fd, 2, SEEK_CUR);

    while (read(fd, buf, sizeof(buf)) > 0) {
        write(STDOUT_FILENO, buf, sizeof(buf));
    }

    return 0;
}