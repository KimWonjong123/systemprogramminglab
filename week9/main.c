#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void make_tokens(char *cmd, char *arg[], char *arg2[], char *target)
{
    // insert your code
    // make tokens which will be used in pipe or redirections
    // you can change the parameters if you want

    int i = 0;
    char *pos = cmd;
    char *next;
    char *ptr = strtok_r(pos, " ", &next);
    while (ptr && ptr[0] != target[0])
    {
        arg[i++] = ptr;
        ptr = strtok_r(NULL, " ", &next);
    }
    arg[i] = NULL;

    i = 0;
    while ((ptr = strtok_r(NULL, " ", &next)) != NULL)
    {
        arg2[i++] = ptr;
    }
    arg2[i] = NULL;
}

int main()
{
    size_t size;
    char *cmd;
    char *ptr;
    char *arg[128];
    char *arg2[128];
    char path[128];
    char path2[128];
    int child_status;
    int fd[2];
    int fdr;

    while (1)
    {
        int num = 0;
        cmd = NULL;

        if (getline(&cmd, &size, stdin) == -1)
        {
            perror("getline error");
            exit(1);
        }

        cmd[strlen(cmd) - 1] = '\0';

        if (strcmp(cmd, "quit") == 0)
        {
            break;
        }

        if (strchr(cmd, '|') != NULL)
        {

            make_tokens(cmd, arg, arg2, "|");

            sprintf(path, "/bin/%s", arg[0]);
            sprintf(path2, "/bin/%s", arg2[0]);

            // insert your code
            // implement pipe using pipe and dup2 functions
            pipe(fd);
            if (fork() == 0)
            {
                close(fd[0]);
                dup2(fd[1], 1);
                execv(path, arg);
                exit(1);
            }
            else
            {
                if(fork() == 0)
                {
                    close(fd[1]);
                    dup2(fd[0], 0);
                    execv(path2, arg2);
                    exit(1);
                }
                close(fd[0]);
                close(fd[1]);
                wait(NULL);
            }
        }
        else if (strchr(cmd, '>') != NULL)
        {

            make_tokens(cmd, arg, arg2, ">");
            sprintf(path, "/bin/%s", arg[0]);

            // insert your code
            // implement > redirection using dup2
            if (fork() == 0)
            {
                fdr = open(arg2[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                dup2(fdr, 1);
                close(fdr);
                execv(path, arg);
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
        else if (strchr(cmd, '<') != NULL)
        {

            make_tokens(cmd, arg, arg2, "<");
            sprintf(path, "/bin/%s", arg[0]);

            // insert your code
            // implement < redirection using dup2
            if (fork() == 0)
            {

                fdr = open(arg2[0], O_RDONLY);
                dup2(fdr, 0);
                close(fdr);
                execv(path, arg);
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
        else
        {

            // mini shell from week5 - do not modify
            char *pos = cmd;
            while ((ptr = strtok_r(pos, " ", &pos)))
                arg[num++] = ptr;
            arg[num] = NULL;

            sprintf(path, "/bin/%s", arg[0]);

            if (fork() == 0)
            {
                execv(path, arg);
                perror("exec error");
                exit(3);
            }
            else
                wait(&child_status);
        }
        free(cmd);
    }

    free(cmd);
    return 0;
}