#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "err.h"

int main(int argc, char **argv) {
    struct stat src_statbuf, dest_statbuf;
    char *src, *dest;
    if (argc < 2) {
        printf("mv: missing file operand\n");
        return 0;
    }
    else {
        src = argv[1];
    }

    if (argc < 3) {
        printf("mv: missing destination file operand after '%s'\n", src);
        return 0;
    }
    else {
        dest = argv[2];
    }

    stat(src, &src_statbuf);
    stat(dest, &dest_statbuf);
    if (S_ISREG(src_statbuf.st_mode) && S_ISDIR(dest_statbuf.st_mode)) {
        // parse file name
        char *ptr = src + strlen(src) - 1;
        while (*ptr != '/' && ptr >= src) {
            --ptr;
        }
        ++ptr;
        if (dest[strlen(dest) - 1] != '/')
            strcat(dest, "/");
        strcat(dest, ptr);
    }

    if (rename(src, dest) == -1) {
        print_err("mv", errno);
    }

    return 0;
}