#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "command.h"

#define STR_LIMIT 511
#define SCN_STR_LIMIT "511"


static void read_string(char *output_buf) {
    printf("> ");
    fflush(stdout);

    int arg_read_count = scanf("%" SCN_STR_LIMIT "[^\n]", output_buf);
    if (arg_read_count == 0) {
        *output_buf = '\0';
    }
    getchar(); // Пропустить '\n' из буфера ввода
}


static int shell() {
    char buf[STR_LIMIT + 1];

    while (1) {
        read_string(buf);
        
        struct command **cmds = cmd_from_str(buf, buf + strlen(buf));
        int exec_count = cmd_exec_count(cmds);
        if (exec_count == 1) {
            char* cmd_str = cmds[0]->args[0];
            if ((cmd_str != NULL) && (strcmp(cmd_str, "exit")) == 0) {
                break;
            }
        }

        int status = cmd_execute(cmds);

        if (status == -1) {
            fprintf(stderr, "Произошла ошибка при выполнении команды.\n");
        }

        cmds_free(cmds);
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int result = shell();

    return result;
}
