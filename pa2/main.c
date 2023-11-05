#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "linkedList.h"

char *REDIRECTIONS[] = {" < ", " > ", " >> "};
char *EXECUTABLES[] = {"ls", "man", "grep", "sort", "awk", "bc"};
char *IMPLEMENTED[] = {"head", "tail", "cat", "cp", "mv", "rm", "pwd", "cd", "exit"};

bool parse_pipelines(char *cmd, LinkedList *commands) {
    bool daemon = false;

    // check daemon and remove '&' character
    if (cmd[strlen(cmd) - 1] == '&' && cmd[strlen(cmd) - 2] == ' ') {
        daemon = true;
        cmd[strlen(cmd) - 2] = '\0';
    }

    // parse commands
    char *next;
    char *pos = strtok_r(cmd, "|", &next);
    while (pos) {
        if (*pos == ' ') pos++;
        Node *node = create_node(COMMANDS, pos);
        pos = strtok_r(NULL, "|", &next);
        insert_at_tail(commands, node);
    }

    return daemon;
}

int parse_command(char *cmd, Command *command, char **redirect_in, char **redirect_out, char **redirect_out_append) {
    int cnt = 0;
    char *pos = cmd;
    char *command_end = NULL;
    char **args = command->args;

    // parse filename
    for (int i = 0; i < 3; i++) {
        int len = strlen(REDIRECTIONS[i]);
        char *pos_save = pos;
        if ((pos = strstr(pos, REDIRECTIONS[i]))) {
            switch (i) {
                case 0:
                    *redirect_in = *redirect_in == NULL ? pos + len : NULL;
                    break;
                case 1:
                    *redirect_out = *redirect_out == NULL && *redirect_out_append == NULL ? pos + len : NULL;
                    break;
                case 2:
                    *redirect_out_append  = *redirect_out == NULL && *redirect_out_append == NULL ? pos + len : NULL;
                    break;
                default:
                    break;
            }
            *pos++ = '\0';
            command_end = command_end == NULL ? pos : command_end;
        }

        if (pos == NULL) {
            pos = pos_save;
        }
    }

    // parse arguments
    if (command_end != NULL) *command_end = '\0';
    pos = cmd;
    char *next;
    char *ptr = strtok_r(pos, " ", &next);
    while (ptr) {
        args[cnt++] = ptr;
        ptr = strtok_r(NULL, " ", &next);
    }
    args[cnt] = NULL;

    for(int i = 0; i < 6; i++) {
        if (!strcmp(args[0], EXECUTABLES[i])) {
            command->type = EXECUTABLE;
            break;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (!strcmp(args[0], IMPLEMENTED[i])) {
            command->type = IMPLEMENT;
            break;
        }
    }

    return cnt;
}

int main() {
    size_t size;
    char *cmd;
    bool b_daemon;
    LinkedList pipelines = {0, NULL, NULL};
    Command *pCommands;
    int status;
    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);

    while (1) {
        cmd = NULL;
        char *redirect_in = NULL, *redirect_out = NULL, *redirect_out_append = NULL;

        if (getline(&cmd, &size, stdin) == -1) {
            perror("getline error");
            exit(1);
        }
        cmd[strlen(cmd) - 1] = '\0';

        if (!strcmp(cmd, "exit")) {
            printf("exiting minishell...\n");
            exit(0);
        }

        b_daemon = parse_pipelines(cmd, &pipelines);

        pCommands = (Command *)malloc(sizeof(Command) * pipelines.num);

        Node *node = pipelines.head;
        for (int i = 0; i < pipelines.num; i++, node = node->next) {
            pCommands[i].arg_num = parse_command(node->content, &pCommands[i], &redirect_in, &redirect_out, &redirect_out_append);
        }

        pid_t pid;
        pid_t pgid = -1;
        int fd1[2] = {0, 0};
        int fd2[2] = {0, 0};

        // TODO: fix pipelining
        for (int i = 0; i < pipelines.num; i++) {
            // set pipe
            if (fd1[0] == 0) {
                pipe(fd1);
                printf("pipe: %d %d\n", fd1[0], fd1[1]);
            }
            else if (fd2[0] == 0) {
                pipe(fd2);
                printf("pipe: %d %d\n", fd2[0], fd2[1]);
            }

            if((pid = fork()) == 0)
            {
                // set pgid to pid of first child
                if (i == 0) pgid = getpgid(getpid());
                setpgid(getpid(), pgid);

                // set pipe
                if (i != 0) {
                    if (fd1[0] != 0) {
                        dup2(fd1[0], STDIN_FILENO);
                        close(fd1[0]);
                    }
                    if (fd2[0] != 0) {
                        dup2(fd2[0], STDIN_FILENO);
                        close(fd2[0]);
                    }
                }
                if (pipelines.num != 1 && i != pipelines.num - 1) {
                    if (fd1[1] != 0) {
                        dup2(fd1[1], STDOUT_FILENO);
                        close(fd1[1]);
                    }
                    if (fd2[1] != 0) {
                        dup2(fd2[1], STDOUT_FILENO);
                        close(fd2[1]);
                    }
                }

                // set redirection
                int fdr;
                if (i == 0) {
                    if (redirect_in != NULL) {
                        printf("redirect_in: %s\n", redirect_in);
                        fdr = open(redirect_in, O_RDONLY);
                        if (fdr == -1) {
                            perror("open error");
                            exit(1);
                        }
                        dup2(fdr, STDIN_FILENO);
                        close(fdr);
                    }
                }
                if (i == pipelines.num - 1) {
                    if (redirect_out != NULL) {
                        printf("redirect_out: %s\n", redirect_out);
                        fdr = open(redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fdr == -1) {
                            perror("open error");
                            exit(1);
                        }
                        dup2(fdr, STDOUT_FILENO);
                        close(fdr);
                    }
                    else if (redirect_out_append != NULL) {
                        printf("redirect_out_append: %s\n", redirect_out_append);
                        fdr = open(redirect_out_append, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (fdr == -1) {
                            perror("open error");
                            exit(1);
                        }
                        dup2(fdr, STDOUT_FILENO);
                        close(fdr);
                    }
                }


                // execute command
                char *path = (char *)malloc(sizeof(char) * 200);
                sprintf(path, "/bin/%s", pCommands[i].args[0]);
                printf("executing %s...\n", path);
                execv(path, pCommands[i].args);
                free(path);
                exit(1);
            }
            else
            {
                // set pipe
                if (i != 0) {
                    if (fd1[0] != 0) {
                        close(fd1[0]);
                    }
                    if (fd2[0] != 0) {
                        close(fd2[0]);
                    }
                }
                if (pipelines.num != 1 && i != pipelines.num - 1) {
                    if (fd1[1] != 0) {
                        close(fd1[1]);
                    }
                    if (fd2[1] != 0) {
                        close(fd2[1]);
                    }
                }

                waitpid(-1, &status, WNOHANG | WUNTRACED);
                dup2(stdin_copy, STDIN_FILENO);
                dup2(stdout_copy, STDOUT_FILENO);
            }
        }

        // close pipe and reset stdin, stdout
        // close(fd1[0]);
        // close(fd1[1]);
        // close(fd2[0]);
        // close(fd2[1]);
        // dup2(stdin_copy, STDIN_FILENO);
        // dup2(stdout_copy, STDOUT_FILENO);

        // for (int i = 0; i < pipelines.num; i++) {
        //     char **arg = pCommands[i].args;
        //     for (int j = 0; j < pCommands[i].arg_num; j++) {
        //         printf("%s ", arg[j]);
        //     }
        //     if (redirect_in != NULL) printf("< %s ", redirect_in);
        //     if (redirect_out != NULL) printf("> %s ", redirect_out);
        //     if (redirect_out_append != NULL) printf(">> %s ", redirect_out_append);
        //     printf("\n");
        // }

        delete_all_node(&pipelines);
        free(cmd);
        free(pCommands);
    }

    if (cmd != NULL) free(cmd);
    if (pCommands != NULL) free(pCommands);

    return 0;
}