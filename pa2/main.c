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
char *BUILTINS[] = {"cd", "exit"};

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
                    if (*redirect_in == NULL) {
                        *redirect_in = pos + len;
                        if ((*redirect_in)[strlen(*redirect_in) - 1] == ' ')
                            (*redirect_in)[strlen(*redirect_in) - 1] = '\0';
                    }
                    break;
                case 1:
                    if (*redirect_out == NULL) {
                        *redirect_out = pos + len;
                        if ((*redirect_out)[strlen(*redirect_out) - 1] == ' ')
                            (*redirect_out)[strlen(*redirect_out) - 1] = '\0';
                    }
                    break;
                case 2:
                    if (*redirect_out_append == NULL) {
                        *redirect_out_append = pos + len;
                        if ((*redirect_out_append)[strlen(*redirect_out_append) - 1] == ' ')
                            (*redirect_out_append)[strlen(*redirect_out_append) - 1] = '\0';
                    }
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
            return cnt;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (!strcmp(args[0], IMPLEMENTED[i])) {
            command->type = IMPLEMENT;
            return cnt;
        }
    }
    for (int i = 0; i < 2; i++) {
        if (!strcmp(args[0], BUILTINS[i])) {
            command->type = BUILTIN;
            return cnt;
        }
    }
    if (args[0][0] == '.')
        command->type = PATH;
    else 
        command->type = UNKNOWN;
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

        Command __cmd = pCommands[0];
        bool b_notfound = false;
        for (int i = 0; i < pipelines.num; i++, __cmd = pCommands[i]) {
            if (__cmd.type == UNKNOWN) {
                b_notfound = true;
                break;
            }
        }
        if (b_notfound) {
            printf("mini: command not found\n");
            continue;
        }

        pid_t pid;
        pid_t pgid = -1;
        int fd1[2] = {-1, -1};
        int fd2[2] = {-1, -1};

        // TODO: fix pipelining
        for (int i = 0; i < pipelines.num; i++) {
            stdin_copy = dup(STDIN_FILENO);
            stdout_copy = dup(STDOUT_FILENO);

            // set pipe
            if (i % 2) {
                pipe(fd2);
            }
            else {
                pipe(fd1);
            }

            if((pid = fork()) == 0)
            {
                // set pgid to pid of first child
                if (i == 0) pgid = getpgid(getpid());
                setpgid(getpid(), pgid);

                // set pipe
                if (i != 0) {
                    if (i % 2) {
                        dup2(fd1[0], STDIN_FILENO);
                        close(fd1[0]);
                    }
                    else {
                        dup2(fd2[0], STDIN_FILENO);
                        close(fd2[0]);
                    }
                }
                if (pipelines.num != 1 && i != pipelines.num - 1) {
                    if (i % 2) {
                        dup2(fd2[1], STDOUT_FILENO);
                        close(fd2[1]);
                    }
                    else {
                        dup2(fd1[1], STDOUT_FILENO);
                        close(fd1[1]);
                    }
                }

                // set redirection
                int fdr;
                if (i == 0) {
                    if (redirect_in != NULL) {
                        fdr = open(redirect_in, O_RDONLY);
                        if (fdr == -1) {
                            perror("redirect in error");
                            exit(1);
                        }
                        dup2(fdr, STDIN_FILENO);
                        close(fdr);
                    }
                }
                if (i == pipelines.num - 1) {
                    if (redirect_out != NULL) {
                        fdr = open(redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fdr == -1) {
                            perror("redirect out error");
                            exit(1);
                        }
                        dup2(fdr, STDOUT_FILENO);
                        close(fdr);
                    }
                    else if (redirect_out_append != NULL) {
                        fdr = open(redirect_out_append, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (fdr == -1) {
                            perror("redirect out append error");
                            exit(1);
                        }
                        dup2(fdr, STDOUT_FILENO);
                        close(fdr);
                    }
                }


                // execute command
                char *path = (char *)malloc(sizeof(char) * 200);
                sprintf(path, "/bin/%s", pCommands[i].args[0]);
                execv(path, pCommands[i].args);
                free(path);
                exit(1);
            }
            else
            {
                while (waitpid(-1, &status, WNOHANG | WUNTRACED) > 0) {
                    ;
                }
                dup2(stdin_copy, STDIN_FILENO);
                dup2(stdout_copy, STDOUT_FILENO);

                // set pipe
                if (i != 0) {
                    if (i % 2) {
                        close(fd1[0]);
                    }
                    else {
                        close(fd2[0]);
                    }
                }
                if (pipelines.num != 1 && i != pipelines.num - 1) {
                    if (i % 2) {
                        close(fd2[1]);
                    } else {
                        close(fd1[1]);
                    }
                }
            }
        }

        // close pipe and reset stdin, stdout
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        dup2(stdin_copy, STDIN_FILENO);
        dup2(stdout_copy, STDOUT_FILENO);

        delete_all_node(&pipelines);
        free(cmd);
        free(pCommands);
    }

    if (cmd != NULL) free(cmd);
    if (pCommands != NULL) free(pCommands);

    return 0;
}