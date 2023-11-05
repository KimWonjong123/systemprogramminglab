#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "linkedList.h"

char *REDIRECTIONS[] = {" < ", " > ", " >> "};
char *EXECUTABLES[] = {"ls",
                       "man",
                       "grep",
                       "sort",
                       "awk",
                       "bc"
                       };
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

    return cnt;
}

int main() {
    size_t size;
    char *cmd;
    bool b_daemon;
    LinkedList pipelines = {0, NULL, NULL};
    Command *pCommands;

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

        for (int i = 0; i < pipelines.num; i++) {
            char **arg = pCommands[i].args;
            for (int j = 0; j < pCommands[i].arg_num; j++) {
                printf("%s ", arg[j]);
            }
            if (redirect_in != NULL) printf("< %s ", redirect_in);
            if (redirect_out != NULL) printf("> %s ", redirect_out);
            if (redirect_out_append != NULL) printf(">> %s ", redirect_out_append);
            printf("\n");
        }

        delete_all_node(&pipelines);
        free(cmd);
        free(pCommands);
    }

    free(cmd);

    return 0;
}