#include <locale.h>
#include "common.h"

#define LISTEN_BACKLOG 4
#define MESSAGE "Hi!"
#define BUF_LEN 32

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: server [PORT]\n");
        return 8;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Считываем номер порта из первого аргумента
    uint16_t port;
    if (port_str_to_val(argv[1], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт.\n");
        return 6;
    }
    my_addr.sin_port = htons(port);

    int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }
    
    int bind_status = bind(listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать адрес к сокету.\n");

        close(listener_fd);
        return 2;
    }

    int listen_status = listen(listener_fd, LISTEN_BACKLOG);
    if (listen_status == -1) {
        fprintf(stderr, "Не удалось сделать сокет прослушиваемым.\n");

        close(listener_fd);
        return 3;
    }
    
    printf("Ожидаю соединение от клиента...\n");

    int client_fd = accept(listener_fd, NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "Не удалось соединиться с клиентом.\n");

        close(listener_fd);
        return 4;
    }

    receive_message(client_fd);
    send_message(client_fd, MESSAGE);

    // Cleanup
    close(client_fd);
    close(listener_fd);
    return 0;
}
