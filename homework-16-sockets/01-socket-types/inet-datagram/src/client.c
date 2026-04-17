#include <locale.h>
#include "common.h"

#define MESSAGE "Hello!"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 3) {
        printf("Usage: client [IPV4_ADDR] [PORT]\n");
        return 8;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // Считываем адрес назначения из первого аргумента
    if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) != 1) {
        fprintf(stderr, "Некорректно указан IPv4 адрес.\n");
        return 7;
    }

    // Считываем номер порта назначения из второго аргумента
    uint16_t port;
    if (port_str_to_val(argv[2], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт назначения.\n");
        return 6;
    }
    server_addr.sin_port = htons(port);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }

    send_message_to(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr), MESSAGE);
    receive_message_from(socket_fd, NULL, NULL);

    // Cleanup
    close(socket_fd);
    return 0;
}
