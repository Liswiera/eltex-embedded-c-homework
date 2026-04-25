#include <locale.h>
#include <sys/epoll.h>
#include "common.h"

#define LISTEN_BACKLOG 4
#define MESSAGE "Hi!"
#define BUF_SIZE 64
#define WORKER_COUNT 3

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
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

    
    // Создаём epoll и привязываем к нему дескрипторы TCP и UDP сокетов
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        fprintf(stderr, "Не удалось создать epoll.\n");
        return 6;
    }

    struct epoll_event udp_event;
    udp_event.data.fd = udp_listener_fd;
    udp_event.events = EPOLLIN;

    struct epoll_event tcp_event;
    tcp_event.data.fd = tcp_listener_fd;
    tcp_event.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_listener_fd, &udp_event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_listener_fd, &tcp_event);
    

    // Привязываем сокеты к портам
    my_addr.sin_port = htons(udp_port);
    int udp_bind_status = bind(udp_listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (udp_bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к UDP сокету.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 7;
    }

    my_addr.sin_port = htons(tcp_port);
    int tcp_bind_status = bind(tcp_listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (tcp_bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к TCP сокету.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 8;
    }

    int tcp_listen_status = listen(tcp_listener_fd, LISTEN_BACKLOG);
    if (tcp_listen_status == -1) {
        fprintf(stderr, "Не удалось сделать сокет прослушиваемым.\n");

        close(tcp_listener_fd);
        close(udp_listener_fd);
        return 9;
    }


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

        ssize_t bytes_read;
        if (event.events & EPOLLIN) {
            int ready_fd = event.data.fd;

            if (ready_fd == tcp_listener_fd) {
                // Было получено событие от дескриптора по TCP
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);

                int client_fd = accept(tcp_listener_fd, (struct sockaddr*)&client_addr, &client_addr_len);
                if (client_fd == -1) {
                    fprintf(stderr, "Не удалось принять клиента (TCP).\n");
                    continue;
                }

                bytes_read = receive_message(client_fd);
                if (bytes_read == -1) {
                    fprintf(stderr, "Не удалось принять сообщение от клиента (TCP).\n");
                }

                close(client_fd);
            }
            else if (ready_fd == udp_listener_fd) {
                // Было получено событие от дескриптора по UDP
                bytes_read = receive_message_from(udp_listener_fd, NULL, NULL);
                if (bytes_read == -1) {
                    fprintf(stderr, "Не удалось принять сообщение от клиента (UDP).\n");
                }
            }
        }
    }

    close(tcp_listener_fd);
    close(udp_listener_fd);
    close(epoll_fd);
    return 0;
}
