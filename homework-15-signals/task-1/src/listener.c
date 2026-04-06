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

    struct sigaction act = { 0 };
    act.sa_sigaction = &user_signal_handler;

    int status = sigaction(SIGUSR1, &act, NULL);
    if (status == -1) {
        fprintf(stderr, "Не удалось переопределить диспозицию для сигнала SIGUSR1.\n");
        return 1;
    }

    printf("[PID=%d] Жду поступления сигнала SIGUSR1...\n", getpid());
    while (1) {
        pause();
    }

    return 0;
}
