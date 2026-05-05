#include <locale.h>
#include "common.h"

#define MESSAGE "time"

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
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 4;
    }

    socklen_t server_addr_len = sizeof(server_addr);

    send_message_to(server_fd, MESSAGE, &server_addr, server_addr_len);
    receive_message_from(server_fd, NULL, NULL);


    // Cleanup
    close(server_fd);
    return 0;
}
