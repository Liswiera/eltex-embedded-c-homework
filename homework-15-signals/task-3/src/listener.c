#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Блокируем обработку сигнала SIGUSR1
    sigset_t set;
    sigemptyset(&set); // Return value is ignored
    sigaddset(&set, SIGUSR1); // Return value is ignored

    int status = sigprocmask(SIG_BLOCK, &set, NULL);
    if (status == -1) {
        fprintf(stderr, "Не удалось заблокировать сигнал SIGINT.\n");
        return 1;
    }

    // Задаём полный набор сигналов, которые будут ожидаться в sigwait
    sigfillset(&set); // Return value is ignored

    printf("[PID=%d] Обработка сигнала SIGUSR1 заблокирована.\n", getpid());
    printf("[PID=%d] Перехожу в бесконечный цикл...\n", getpid());

    while (1) {
        int signal_received;
        sigwait(&set, &signal_received);

        printf("[PID=%d] Был получен сигнал с номером %d.\n", getpid(), signal_received);
    }

    return 0;
}
