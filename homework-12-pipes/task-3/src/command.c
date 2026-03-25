#include "command.h"

void cmd_free(struct command *cmd) {
    if (cmd != NULL) {
        args_free(cmd->args);
        free(cmd);
    }
}

void cmds_free(struct command **cmds) {
    if (cmds != NULL) {
        for (size_t i = 0; cmds[i] != NULL; i++) {
            cmd_free(cmds[i]);
        }
        free(cmds);
    }
}

static char** args_from_str(const char *start, const char *end) {
    size_t arg_count = args_get_arg_count(start, end);

    char **args = malloc(sizeof(char*) * (arg_count + 1));
    if (args == NULL) {
        return NULL;
    }

    int malloc_status = 1;

    const char *arg_start = start;
    for (size_t i = 0; i < arg_count; i++) {
        const char *arg_end;
        args_get_next_arg(arg_start, end, &arg_start, &arg_end);

        size_t arg_str_len = arg_end - arg_start;
        args[i] = malloc(sizeof(char) * (arg_str_len + 1));
        if (args[i] == NULL) {
            malloc_status = 0;
            break;
        }

        strncpy(args[i], arg_start, arg_str_len);
        args[i][arg_str_len] = '\0';

        arg_start = arg_end;
    }
    args[arg_count] = NULL;

    if (malloc_status != 1) {
        args_free(args);
        args = NULL;
    }

    return args;
}

struct command** cmd_from_str(const char *start, const char *end) {
    size_t cmd_count = args_get_command_count(start, end);

    struct command **cmds = malloc(sizeof(struct command*) * (cmd_count + 1));
    if (cmds == NULL) {
        return NULL;
    }

    int malloc_status = 1;

    const char *cmd_start = start;
    for (size_t i = 0; i < cmd_count; i++) {
        const char *cmd_end;
        args_get_next_command(cmd_start, end, &cmd_start, &cmd_end);

        cmds[i] = malloc(sizeof(struct command));
        if (cmds[i] == NULL) {
            malloc_status = 0;
            break;
        }

        cmds[i]->cmd_start = cmd_start;
        cmds[i]->cmd_end = cmd_end;
        cmds[i]->args = args_from_str(cmd_start, cmd_end);

        if (cmds[i]->args == NULL) {
            cmds[i + 1] = NULL;
            malloc_status = 0;
            break;
        }

        cmd_start = cmd_end;
        if (*cmd_start == '|') {
            cmd_start++;
        }
    }
    cmds[cmd_count] = NULL;

    if (malloc_status != 1) {
        cmds_free(cmds);
        cmds = NULL;
    }

    return cmds;
}

int cmd_exec_count(struct command **cmds) {
    if (cmds == NULL) {
        return -1;
    }

    size_t i;
    for (i = 0; cmds[i] != NULL; i++) {
        const struct command *cmd = cmds[i];

        if (cmd->args == NULL) {
            return -1;
        }

        if (cmd->args[0] == NULL) {
            return -1;
        }
    }

    return i;
}

int cmd_execute(struct command **cmds) {
    int exec_count = cmd_exec_count(cmds);
    if (exec_count <= 0) {
        return exec_count;
    }

    int input_fd = STDIN_FILENO;
    int child_count = 0;

    for (size_t i = 0; cmds[i] != NULL; i++) {
        const struct command *cmd = cmds[i];

        int is_first = (i == 0);
        int is_last = (cmds[i + 1] == NULL);

        int new_fds[2];
        if (!is_last) {
            pipe(new_fds);
        }

        pid_t child_pid = fork();
        switch(child_pid) {
            case -1:
                fprintf(stderr, "Не удалось создать дочерний процесс.\n");
                break;
            case 0:
                // Дочерний процесс
                if (!is_first) {
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }
                if (!is_last) {
                    dup2(new_fds[1], STDOUT_FILENO);
                    close(new_fds[1]);
                }

                execvp(cmd->args[0], cmd->args);
                fprintf(stderr, "Не удалось выполнить команду '%s'\n", cmd->args[0]);
                exit(EXIT_FAILURE);
            default:
                // Родительский процесс
                child_count++;
        }

        if (!is_first) {
            close(input_fd);
        }
        if (!is_last) {
            input_fd = new_fds[0];
            close(new_fds[1]);
        }
    }

    int success = 1;
    for (int i = 0; i < child_count; i++) {
        int status;
        wait(&status);

        if (WEXITSTATUS(status) == EXIT_FAILURE) {
            success = 0;
        }
    }

    return (success && (child_count == exec_count)) ? 0 : -1;
}
