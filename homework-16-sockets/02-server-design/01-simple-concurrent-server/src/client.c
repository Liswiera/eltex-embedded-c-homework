#include <locale.h>
#include "common.h"

#define MESSAGE "Hello!"

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 3) {
        printf("Usage: client [IPV4_ADDR] [PORT]\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // Считываем адрес назначения из первого аргумента
    if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) != 1) {
        fprintf(stderr, "Некорректно указан IPv4 адрес.\n");
        return 2;
    }

    // Считываем номер порта назначения из второго аргумента
    uint16_t port;
    if (port_str_to_val(argv[2], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт назначения.\n");
        return 3;
    }
    server_addr.sin_port = htons(port);

    // Создаём сокет и устанавливаем соединение с сервером
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 4;
    }

    int connect_status = connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (connect_status == -1) {
        fprintf(stderr, "Не удалось подключиться к серверу.\n");

        close(server_fd);
        return 5;
    }

    // TODO: ADD NCURSES

    send_message(server_fd, MESSAGE);
    receive_message(server_fd);

    // Cleanup
    close(server_fd);
    return 0;
}
