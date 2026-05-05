#include <locale.h>
#include "common.h"
#include "message.h"
#include "worker.h"

#define LISTEN_BACKLOG 4
#define BUF_SIZE 64
#define WORKER_COUNT 3

#define SERVER_QUEUE_NAME "/homework-16-02-03-server-queue"

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

static void set_queue_creat_attributes(struct mq_attr *attributes) {
    attributes->mq_maxmsg = MQ_MAXMSG;
    attributes->mq_msgsize = MQ_MSGSIZE;
}

void* worker_thread(void *arg) {
    struct worker *wrk = (struct worker*)arg;

    int worker_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (worker_sock_fd == -1) {
        fprintf(stderr, "[THREAD #%d] Не удалось создать новый сокет.\n", wrk->id);
        exit(1);
    }

    mqd_t req_queue = mq_open(SERVER_QUEUE_NAME, O_RDONLY);
    if (req_queue == (mqd_t)-1) {
        fprintf(stderr, "[THREAD #%d] Не удалось открыть очередь '%s' на чтение.\n", wrk->id, SERVER_QUEUE_NAME);
        exit(2);
    }

    while (1) {
        // Принимаем новую заявку
        struct message msg;
        mq_receive(req_queue, (char*)&msg, MQ_MSGSIZE, NULL);

        // и обрабатываем её
        printf("[THREAD #%d] Получено сообщение: %s\n", wrk->id, msg.contents);

        char time_buf[BUF_SIZE];
        get_local_time(time_buf);
        ssize_t bytes_written = sendto(worker_sock_fd, time_buf, strlen(time_buf), 0, (struct sockaddr*)&msg.client, sizeof(msg.client));
        if (bytes_written == -1) {
            fprintf(stderr, "[THREAD #%d] Не удалось отправить клиенту сообщение.\n", wrk->id);
            continue;
        }
        else {
            printf("[THREAD #%d] Отправлено сообщение: %s\n", wrk->id, time_buf);
        }
        
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

    int listener_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    // Создаём очередь, через которую будем передавать заявки от клиентов к потокам-обработчикам
    struct mq_attr attributes;
    set_queue_creat_attributes(&attributes);

    mqd_t req_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0600, &attributes);
    if (req_queue == (mqd_t)-1) {
        int errno_saved = errno;

        fprintf(stderr, "Не удалось создать очередь '%s' на запись.\n", SERVER_QUEUE_NAME);
        if (errno_saved == EINVAL) {
            fprintf(stderr, "EINVAL\n");
        }
        
        close(listener_fd);
        return 5; 
    }
    
    // Инициализация структур каждого отдельного обслуживающего потока
    struct worker workers[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; i++) {
        struct worker *wrk = &workers[i];

        wrk->id = i;

        int th_status = pthread_create(&wrk->thread, NULL, worker_thread, wrk);
        if (th_status != 0) {
            fprintf(stderr, "Не удалось создать поток.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Ожидаю получения заявок от клиентов...\n");
    while (1) {
        struct message msg;
        socklen_t client_addr_len = sizeof(msg.client);

        ssize_t bytes_read = recvfrom(listener_fd, msg.contents, CONTENTS_BUF_LEN - 1, 0, (struct sockaddr*)&msg.client, &client_addr_len);
        if (bytes_read == -1) {
            fprintf(stderr, "Не удалось получить сообщение от клиента.\n");
            continue;
        }
        msg.contents[bytes_read] = '\0';

        mq_send(req_queue, (char*)&msg, MQ_MSGSIZE, 0);
    }

    close(listener_fd);
    mq_close(req_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}
