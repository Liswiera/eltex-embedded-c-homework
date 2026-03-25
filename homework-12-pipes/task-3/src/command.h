#pragma once
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "args_tools.h"

struct command {
    const char *cmd_start;
    const char *cmd_end;
    char **args;
};

void cmd_free(struct command *cmd);
void cmds_free(struct command **cmds);
struct command** cmd_from_str(const char *start, const char *end);
int cmd_exec_count(struct command **cmds);
int cmd_execute(struct command **cmds);
