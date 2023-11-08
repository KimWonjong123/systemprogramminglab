#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "err.h"

int main(int argc, char **argvc) {
    char *file;
    if (argc < 2) {
        printf("rm: missing operand\n");
        return 0;
    }
    else {
        file = argvc[1];
    }

    if (unlink(file) == -1) {
        print_err("rm", errno);
    }

    return 0;
}