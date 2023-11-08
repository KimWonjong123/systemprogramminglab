#include <unistd.h>
#include <errno.h>
#include "err.h"

int main(int argc, char **argv) {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) == NULL) {
        print_err("pwd", errno);
    }
    printf("%s\n", buf);

    return 0;
}
