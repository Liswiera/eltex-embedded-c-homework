#include <locale.h>
#include <time.h>
#include "common.h"
#include "worker.h"

#define LISTEN_BACKLOG 4
#define BUF_SIZE 64
#define WORKER_COUNT 3

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

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
int free_worker_count = 0;

void* worker_thread(void *arg) {
    struct worker *wrk = (struct worker*)arg;

    while (1) {
        // Ожидаем, пока главный поток не назначит дескриптор клиента
        pthread_mutex_lock(&wrk->mutex);
        while(wrk->client_fd == -1) {
            pthread_cond_wait(&wrk->condition, &wrk->mutex);
        }
        pthread_mutex_unlock(&wrk->mutex);

        char buf[BUF_SIZE + 1];
        ssize_t bytes_read;
        while ((bytes_read = recv(wrk->client_fd, buf, BUF_SIZE, 0)) > 0) {
            buf[bytes_read] = '\0';
            printf("[THREAD #%d] Получено сообщение: %s\n", wrk->id, buf);
            
            char time_buf[BUF_SIZE];
            get_local_time(time_buf);
            ssize_t bytes_written = send(wrk->client_fd, time_buf, strlen(time_buf), 0);
            if (bytes_written == -1) {
                fprintf(stderr, "[THREAD #%d] Не удалось отправить клиенту сообщение.\n", wrk->id);
                break;
            }
            else {
                printf("[THREAD #%d] Отправлено сообщение: %s\n", wrk->id, time_buf);
            }
        }

        // Закрываем декскриптор клиента и сообщаем главному потоку о доступности обслуживать нового клиента
        close(wrk->client_fd);

        pthread_mutex_lock(&wrk->mutex);
        wrk->client_fd = -1;
        pthread_mutex_unlock(&wrk->mutex);

        pthread_mutex_lock(&counter_mutex);
        free_worker_count++;
        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: server [PORT]\n");
        return 1;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Считываем номер порта из первого аргумента
    uint16_t port;
    if (port_str_to_val(argv[1], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт.\n");
        return 2;
    }
    my_addr.sin_port = htons(port);

    int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 3;
    }
    
    int bind_status = bind(listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к сокету.\n");

        close(listener_fd);
        return 4;
    }

    int listen_status = listen(listener_fd, LISTEN_BACKLOG);
    if (listen_status == -1) {
        fprintf(stderr, "Не удалось сделать сокет прослушиваемым.\n");

        close(listener_fd);
        return 5;
    }
    
    // Инициализация структур каждого отдельного обслуживающего потока
    struct worker workers[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; i++) {
        struct worker *wrk = &workers[i];

        pthread_mutex_init(&wrk->mutex, NULL);
        pthread_cond_init(&wrk->condition, NULL);
        wrk->id = i;
        wrk->client_fd = -1;

        int th_status = pthread_create(&wrk->thread, NULL, worker_thread, wrk);
        if (th_status != 0) {
            fprintf(stderr, "Не удалось создать поток.\n");
            exit(EXIT_FAILURE);
        }
    }

    free_worker_count = WORKER_COUNT;
    int next_worker_id = 0;

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

        // Проверяем наличие свободного потока
        pthread_mutex_lock(&counter_mutex);
        if (free_worker_count > 0) {
            free_worker_count--;
            pthread_mutex_unlock(&counter_mutex);

            // Находим следующий свободный поток по принципу round-robin
            while (workers[next_worker_id].client_fd != -1) {
                next_worker_id++;
                if (next_worker_id >= WORKER_COUNT) {
                    next_worker_id = 0;
                }
            }
            
            // Передаём дескриптор сокета и уведомляем поток, чтобы он начал обслуживать клиента
            struct worker *next_worker = &workers[next_worker_id++];

            pthread_mutex_lock(&next_worker->mutex);
            next_worker->client_fd = client_fd;
            pthread_cond_signal(&next_worker->condition);
            pthread_mutex_unlock(&next_worker->mutex);
        }
        else {
            pthread_mutex_unlock(&counter_mutex);

            printf("Не удалось принять нового клиента, т.к. все потоки заняты.\n");
            close(client_fd);
        }
    }

    close(listener_fd);
    return 0;
}
