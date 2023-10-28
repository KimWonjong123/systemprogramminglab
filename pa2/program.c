#include "program.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void handle_err(char *cmd) {
    switch (errno) {
        case EACCES:
            printf("%s: Permission denied\n", cmd);
            break;
        case EISDIR:
            printf("%s: Is a directory\n", cmd);
            break;
        case ENOENT:
            printf("%s: No such file or directory\n", cmd);
            break;
        case ENOTDIR:
            printf("%s: Not a directory\n", cmd);
            break;
        case EPERM:
            printf("%s: Operation not permitted\n", cmd);
            break;
        default:
            printf("%s: Error Occured: %d\n", cmd, errno);
            break;
    }
}

void head(char *args[]) {
    int cnt = 0, line = 10;
    char *file;
    if (args[2] == NULL) {
        file = args[1];
    } else {
        line = atoi(args[2]);
        file = args[3];
    }

    int fd;

    if (file == NULL) {
        fd = stdin;
    }
    else {
        fd = open(file, O_RDONLY);
        if (fd == -1) {
            handle_err(args[0]);
            return;
        }
    }

    char buf[1];
    while (cnt < line && read(fd, buf, sizeof(buf)) > 0) {
        if (buf[0] == '\n') {
            cnt++;
        }
        write(stdout, buf, sizeof(buf));
    }
    close(fd);
}

void tail(char *args[]) {
    int cnt = 0, line = 10;
    char *file;
    if (args[2] == NULL) {
        file = args[1];
    } else {
        line = atoi(args[2]);
        file = args[3];
    }

    int fd;

    if (file == NULL) {
        fd = stdin;
    } else {
        fd = open(file, O_RDONLY);
        if (fd == -1) {
            handle_err(args[0]);
            return;
        }
    }

    char buf[1];
    off_t offset = lseek(fd, -1, SEEK_END);
    while (cnt < line && offset > 0) {
        read(fd, buf, sizeof(buf));
        if (buf[0] == '\n') {
            cnt++;
        }
        offset = lseek(fd, -2, SEEK_CUR);
    }

    if (offset <= 0)
        lseek(fd, 0, SEEK_SET);
    else
        lseek(fd, 2, SEEK_CUR);

    while (read(fd, buf, sizeof(buf)) > 0) {
        write(stdout, buf, sizeof(buf));
    }
}

void cat(char *args[]) {
    char *file = args[1];
    char *buf[4096];
    int n;

    // if file is null, read from stdin
    if (file == NULL) {
        while ((n = read(stdin, buf, sizeof(buf))) > 0) {
            write(stdout, buf, n);
        }
        return;
    } else {
        int fd = open(file, O_RDONLY);
        if (fd == -1) {
            handle_err(args[0]);
            return;
        }
        while ((n = read(fd, buf, sizeof(buf))) > 0) {
            write(STDOUT_FILENO, buf, n);
        }
        close(fd);
    }
}

void cp(char *args[]) {
    char *file1 = args[1];
    if (file1 == NULL) {
        printf("cp: missing file operand\n");
        return;
    }
    char *file2 = args[2];
    if (file2 == NULL) {
        printf("cp: missing destination file operand after '%s'\n", file1);
        return;
    }

    int fd1 = open(file1, O_RDONLY);
    if (fd1 == -1) {
        handle_err(args[0]);
    }

    int fd2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        handle_err(args[0]);
    }

    char buf[4096];
    int n;
    while ((n = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, n);
    }

    close(fd1);
    close(fd2);
}

void mv(char *args[]) {
    char src = args[1];
    if (src == NULL) {
        printf("mv: missing file operand\n");
        return;
    }
    char dest = args[2];
    if (dest == NULL) {
        printf("mv: missing destination file operand after '%s'\n", src);
        return;
    }

    if (rename(src, dest) == -1) {
        handle_err(args[0]);
    }
}

void rm(char *args[]) {
    char *file = args[1];

    if (unlink(file) == -1) {
        handle_err(args[0]);
    }
}

void cd(char *args[]) {
    if (chdir(args[1]) == -1) {
        handle_err(args[0]);
    }
}

void pwd(char *args[]) {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) == NULL) {
        handle_err(args[0]);
    }
    printf("%s\n", buf);
}

void exitshell(char *args[]) {
    if (args[1] == NULL) {
        exit(0);
    } else {
        exit(atoi(args[1]));
    }
}