#ifndef __UTIL_H__
#define __UTIL_H__

typedef enum __type
{
    COMMANDLINE,
    PIPELINE,
    COMMANDS,
    COMMAND,
    EXECUTABLE,
    IMPLEMENT,
    BUILTIN,
    PATH,
    UNKNOWN
} TYPE;

typedef struct _command
{
    TYPE type;
    char *args[200];
    int arg_num;
} Command;

#endif