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

char *REDIRECTIONS[] = {" < ", " > ", " << ", " >> "};
char *EXECUTABLES[] = {"ls",
                       "man",
                       "grep",
                       "sort",
                       "awk",
                       "bc"
                       "pwd",
                       "head",
                       "tail",
                       "cat",
                       "cp",
                       "mv",
                       "rm",
                       "pwd",
                       "cd",
                       "exit"};
char *IMPLEMENTED[] = {"head", "tail", "cat", "cp", "mv", "rm"};

bool parse_commands(char *cmd, LinkedList *commands) {
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

int parse_command(char *cmd, char *args[]) {
    int cnt = 0;
    char *pos = cmd;
    char *filename1 = NULL, *filename2 = NULL;
    char *command_end = NULL;

    // parse filename
    for (int i = 0; i < 4; i++) {
        int len = strlen(REDIRECTIONS[i]);
        char *pos_save = pos;
        if ((pos = strstr(pos, REDIRECTIONS[i]))) {
            if (filename1 == NULL) {
                filename1 = pos + len;
            } else if (filename1 != NULL) {
                filename2 = pos + len;
            }
            *pos++ = '\0';
            command_end = command_end == NULL ? pos : command_end;
        }

        if (pos == NULL) {
            pos = pos_save;
        }
    }

    // parse arguments
    *command_end = '\0';
    pos = cmd;
    char *next;
    char *ptr = strtok_r(pos, " ", &next);
    while (ptr) {
        args[cnt++] = ptr;
        ptr = strtok_r(NULL, " ", &next);
    }
    args[cnt++] = filename1;
    args[cnt++] = filename2;

    return cnt;
}

int main() {
    size_t size;
    char *cmd;
    bool bDaemon;
    LinkedList tokens = {0, NULL, NULL};
    LinkedList commands = {0, NULL, NULL};

    while (1) {
        cmd = NULL;

        if (getline(&cmd, &size, stdin) == -1) {
            perror("getline error");
            exit(1);
        }
        cmd[strlen(cmd) - 1] = '\0';

        if (!strcmp(cmd, "exit")) {
            printf("exiting minishell...\n");
            exit(0);
        }

        // bDaemon = parse_commands(cmd, &commands);

        char *args[201];
        int cnt = parse_command(cmd, args);

        for (int i = 0; i < cnt; i++) {
            printf("%s\n", args[i] != NULL ? args[i] : "NULL");
        }

        delete_all_node(&tokens);
        delete_all_node(&commands);
        free(cmd);
    }

    free(cmd);

    return 0;
}