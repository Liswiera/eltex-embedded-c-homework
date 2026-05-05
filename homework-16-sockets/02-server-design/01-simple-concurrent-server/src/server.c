#include <locale.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include "common.h"

#define LISTEN_BACKLOG 4
#define BUF_SIZE 64

static void sigchld_handler(int sig) {
    int saved_errno = errno;

    pid_t child_pid;
    while ((child_pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        printf("Освобождены ресурсы дочернего зомби-процесса [PID=%d].\n", child_pid);
        continue;
    }

    errno = saved_errno;
}

static void get_local_time(char* buf) {
    time_t t;
    struct tm timeinfo;

    t = time(NULL);
    localtime_r(&t, &timeinfo);
    asctime_r(&timeinfo, buf);

    size_t len = strlen(buf);
    if (len > 0) {
        buf[len - 1] = '\0';
    }
}

static void handle_request(int client_fd) {
    char buf[BUF_SIZE + 1];
    ssize_t bytes_read;
    pid_t worker_pid = getpid();

    while ((bytes_read = recv(client_fd, buf, BUF_SIZE, 0)) > 0) {
        buf[bytes_read] = '\0';
        printf("[PID=%d] Получено сообщение: %s\n", worker_pid, buf);
        
        char time_buf[BUF_SIZE];
        get_local_time(time_buf);
        ssize_t bytes_written = send(client_fd, time_buf, strlen(time_buf), 0);
        if (bytes_written == -1) {
            fprintf(stderr, "[PID=%d] Не удалось отправить клиенту сообщение.\n", worker_pid);
            break;
        }
        else {
            printf("[PID=%d] Отправлено сообщение: %s\n", worker_pid, time_buf);
        }
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: server [PORT]\n");
        return 1;
    }

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigchld_handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "Ошибка при переопределении диспозиции по SIGCHLD.\n");
        return 2;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Считываем номер порта из первого аргумента
    uint16_t port;
    if (port_str_to_val(argv[1], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт.\n");
        return 3;
    }
    my_addr.sin_port = htons(port);

    int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 4;
    }
    
    int bind_status = bind(listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к сокету.\n");

        close(listener_fd);
        return 5;
    }

    int listen_status = listen(listener_fd, LISTEN_BACKLOG);
    if (listen_status == -1) {
        fprintf(stderr, "Не удалось сделать сокет прослушиваемым.\n");

        close(listener_fd);
        return 6;
    }
    
    printf("Ожидаю соединение от клиентов...\n");
    while (1) {
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = accept(listener_fd, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_fd == -1) {
            fprintf(stderr, "Не удалось соединиться с клиентом.\n");
            continue;
        }

        pid_t worker_pid = fork();
        switch (worker_pid) {
            case -1:
                fprintf(stderr, "Не удалось создать дочерний процесс. Закрываю соединение.\n");
                continue;
            case 0:
                // Дочерний процесс
                close(listener_fd);
                handle_request(client_fd);
                exit(EXIT_SUCCESS);
                break;
            default:
                // Родительский процесс
                char ip_buf[INET_ADDRSTRLEN];
                const char *status = inet_ntop(AF_INET, &client_addr.sin_addr, ip_buf, client_addr_len);
                if (status == NULL) {
                    ip_buf[0] = '\0';
                }

                uint16_t port = ntohs(client_addr.sin_port);

                printf("Создан дочерний процесс [PID=%d], обрабатывающий запросы от %s:%d\n", worker_pid, ip_buf, port);
                break;
        }

        close(client_fd);
    }

    close(listener_fd);
    return 0;
}
