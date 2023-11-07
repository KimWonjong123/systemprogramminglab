#ifndef __ERR_H__
#define __ERR_H__

#include <stdio.h>
#include <errno.h>

void print_err(char *cmd, int err_no) {
    switch (err_no) {
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

#endif