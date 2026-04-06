#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void user_signal_handler(int signo, siginfo_t *info, void *context) {
    printf("Был получен сигнал SIGUSR1.\n");
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    sigset_t set;
    sigemptyset(&set); // Return value is ignored
    sigaddset(&set, SIGINT); // Return value is ignored

    int status = sigprocmask(SIG_BLOCK, &set, NULL);
    if (status == -1) {
        fprintf(stderr, "Не удалось заблокировать сигнал SIGINT.\n");
        return 1;
    }

    printf("[PID=%d] Обработка сигнала SIGINT заблокирована.\n", getpid());
    printf("[PID=%d] Перехожу в бесконечный цикл...\n", getpid());

    while (1) {
        pause();
    }

    return 0;
}
