#pragma once
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>

int args_get_next_command(const char *start, const char *end, const char **cmd_start_ret, const char **cmd_end_ret);
size_t args_get_command_count(const char *start, const char *end);
int args_get_next_arg(const char *start, const char *end, const char **arg_start_ret, const char **arg_end_ret);
size_t args_get_arg_count(const char *start, const char *end);

void args_free(char **args);
