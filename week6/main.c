#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    char *input;
    size_t nread = 0;
    while (getline(&input, &nread, stdin) >= 0)
    {
        input[strlen(input) - 1] = '\0';
        char *args[100];
        char *ptr = strtok(input, " ");
        int i = 0;
        while (ptr != NULL) {
            args[i++] = ptr;
            ptr = strtok(NULL, " ");
        }
        args[i] = NULL;
        if (strncmp("quit", args[0], 4) == 0)
        {
            break;
        }
        char *path = (char *)malloc(strlen(args[0]) + strlen("/bin/") + 1);
        snprintf(path, strlen(args[0]) + strlen("/bin/") + 1, "/bin/%s", args[0]);
        pid_t pid = fork();
        if (pid == 0)
        {
            execv(path, args);
        }
        if (wait(NULL) == -1)
        {
            exit(0);
        }
        if (pid != 0)
        {
            free(path);
        }
    }
    free(input);
    return 0;
}