#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static int is_number(const char *str) {
    for (; *str != '\0'; str++) {
        if (!isdigit(*str)) {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: signaler [PID]\n");
        return 1;
    }

    const char *pid_str = argv[1];
    if (!is_number(pid_str)) {
        printf("Введён некорректный PID процесса.\n");
        return 2;
    }

    errno = 0;
    pid_t pid = (pid_t)strtol(pid_str, NULL, 10);
    if (errno == ERANGE) {
        printf("Введён некорректный PID процесса.\n");
        return 2;
    }

    kill(pid, SIGUSR1);
    printf("Был послан сигнал SIGUSR1 процессу с PID=%d.\n", pid);

    return 0;
}
