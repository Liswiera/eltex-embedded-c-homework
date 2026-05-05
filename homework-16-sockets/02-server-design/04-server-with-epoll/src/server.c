#include <locale.h>
#include <sys/epoll.h>
#include <time.h>
#include "common.h"

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

void udp_query(int client_fd) {
    struct sockaddr_in client_udp_addr;
    socklen_t client_udp_addr_len = sizeof(client_udp_addr);

    ssize_t bytes_read = receive_message_from(client_fd, &client_udp_addr, &client_udp_addr_len);
    if (bytes_read > 0) {
        char time_buf[BUF_SIZE];
        get_local_time(time_buf);

        sendto(client_fd, time_buf, strlen(time_buf), 0, (struct sockaddr*)&client_udp_addr, sizeof(client_udp_addr));
    }
}

void tcp_query(int listener_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(listener_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        fprintf(stderr, "Не удалось принять клиента (TCP).\n");
        return;
    }

    ssize_t bytes_read = receive_message(client_fd);
    if (bytes_read > 0) {
        char time_buf[BUF_SIZE];
        get_local_time(time_buf);

        send(client_fd, time_buf, strlen(time_buf), 0);
    }

    close(client_fd);
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 3) {
        printf("Usage: server [TCP_PORT] [UDP_PORT]\n");
        return 1;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Считываем номер порта из первого аргумента
    uint16_t tcp_port;
    if (port_str_to_val(argv[1], &tcp_port) != 0) {
        fprintf(stderr, "Некорректно указан TCP порт.\n");
        return 2;
    }

    uint16_t udp_port;
    if (port_str_to_val(argv[2], &udp_port) != 0) {
        fprintf(stderr, "Некорректно указан UDP порт.\n");
        return 3;
    }


    // Создаём TCP и UDP сокеты
    int tcp_listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_listener_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 4;
    }

    int udp_listener_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_listener_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");

        close(tcp_listener_fd);
        return 5;
    }


    // Привязываем сокеты к портам
    my_addr.sin_port = htons(udp_port);
    int udp_bind_status = bind(udp_listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (udp_bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к UDP сокету.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 6;
    }

    my_addr.sin_port = htons(tcp_port);
    int tcp_bind_status = bind(tcp_listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (tcp_bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к TCP сокету.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 7;
    }

    int tcp_listen_status = listen(tcp_listener_fd, LISTEN_BACKLOG);
    if (tcp_listen_status == -1) {
        fprintf(stderr, "Не удалось сделать сокет прослушиваемым.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 8;
    }


    // Создаём epoll и привязываем к нему дескрипторы TCP и UDP сокетов
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        fprintf(stderr, "Не удалось создать epoll.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 9;
    }

    struct epoll_event udp_event;
    udp_event.data.fd = udp_listener_fd;
    udp_event.events = EPOLLIN;

    struct epoll_event tcp_event;
    tcp_event.data.fd = tcp_listener_fd;
    tcp_event.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_listener_fd, &udp_event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_listener_fd, &tcp_event);


    printf("Ожидаю сообщения от клиентов...\n");
    while (1) {
        struct epoll_event event;
        int ready_count = epoll_wait(epoll_fd, &event, 1, -1);

        if (ready_count == -1) {
            fprintf(stderr, "epoll_wait returned -1\n");
            break;
        }
        else if (ready_count == 0) {
            continue;
        }

        if (event.events & EPOLLIN) {
            int ready_fd = event.data.fd;

            if (ready_fd == tcp_listener_fd) {
                // Было получено событие от дескриптора по TCP
                tcp_query(tcp_listener_fd);
            }
            else if (ready_fd == udp_listener_fd) {
                // Было получено событие от дескриптора по UDP
                udp_query(udp_listener_fd);
            }
        }
    }

    close(tcp_listener_fd);
    close(udp_listener_fd);
    close(epoll_fd);
    return 0;
}
