#include "args_tools.h"

int args_get_next_command(const char *start, const char *end, const char **cmd_start_ret, const char **cmd_end_ret) {
    const char *cmd_start = start;
    for (cmd_start = start; cmd_start < end; cmd_start++) {
        if (!isspace(*cmd_start)) break;
    }
    
    if (cmd_start == end) {
        // Следующая команда отсутствует, возвращаем 0
        return 0;
    }

    const char *cmd_end;
    for (cmd_end = cmd_start + 1; cmd_end < end; cmd_end++) {
        if (*cmd_end == '|') break;
    }

    // Команда найдена в диапазоне адресов [cmd_start; cmd_end)
    if (cmd_start_ret != NULL) *cmd_start_ret = cmd_start;
    if (cmd_end_ret != NULL) *cmd_end_ret = cmd_end;
    return 1;
}

size_t args_get_command_count(const char *start, const char *end) {
    size_t cmd_count;
    for (cmd_count = 0; args_get_next_command(start, end, NULL, &start); cmd_count++) {
        if (*start == '|') {
            start++;
        }
    }
    return cmd_count;
}

int args_get_next_arg(const char *start, const char *end, const char **arg_start_ret, const char **arg_end_ret) {
    const char *arg_start;
    for (arg_start = start; arg_start < end; arg_start++) {
        if (!isspace(*arg_start)) break;
    }

    if (arg_start == end) {
        // Следующий аргумент отсутствует, возвращаем 0
        return 0;
    }

    const char *arg_end;
    for (arg_end = arg_start + 1; arg_end < end; arg_end++) {
        if (isspace(*arg_end)) break;
    }

    // Аргумент найден в диапазоне адресов [arg_start; arg_end)
    if (arg_start_ret != NULL) *arg_start_ret = arg_start;
    if (arg_end_ret != NULL) *arg_end_ret = arg_end;
    return 1;
}

size_t args_get_arg_count(const char *start, const char *end) {
    size_t arg_count;
    for (arg_count = 0; args_get_next_arg(start, end, NULL, &start); arg_count++);
    return arg_count;
}


void args_free(char **args) {
    if (args != NULL) {
        for (size_t i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    }
}
