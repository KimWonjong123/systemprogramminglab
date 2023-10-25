#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include "linkedList.h"

bool parse_commands(char *cmd, LinkedList *pipelines)
{
    bool daemon = false;

    // check daemon and remove '&' character
    if (cmd[strlen(cmd) - 1] == '&' && cmd[strlen(cmd) - 2] == ' ')
    {
        daemon = true;
        cmd[strlen(cmd) - 2] = '\0';
    }

    char *next;
    char *pos = strtok_r(cmd, "|", &next);
    while (pos)
    {
        if (*pos == ' ')
            pos++;
        Node *node = create_node(COMMANDS, pos);
        pos = strtok_r(NULL, "|", &next);
        insert_at_tail(pipelines, node);
    }

    return daemon;
}

int main()
{
    size_t size;
    char *cmd;
    bool bDaemon;
    LinkedList tokens = {0, NULL, NULL};
    LinkedList commands = {0, NULL, NULL};

    while (1)
    {
        cmd = NULL;

        if (getline(&cmd, &size, stdin) == -1)
        {
            perror("getline error");
            exit(1);
        }
        cmd[strlen(cmd) - 1] = '\0';

        if (!strcmp(cmd, "exit")) {
            printf("exiting minishell...\n");
            exit(0);
        }

        bDaemon = parse_commands(cmd, &commands);
        print_list(&commands);
        printf("daemon: %s\n", bDaemon ? "true" : "false");

        delete_all_node(&tokens);
        delete_all_node(&commands);
        free(cmd);
    }

    free(cmd);

    return 0;
}