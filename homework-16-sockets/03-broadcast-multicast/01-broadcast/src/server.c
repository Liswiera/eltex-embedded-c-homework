#include <locale.h>
#include "common.h"

#define MESSAGE "Hi!"
#define ITER_COUNT 5

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: server [PORT]\n");
        return 8;
    }


    // Считываем номер порта из первого аргумента
    uint16_t port;
    if (port_str_to_val(argv[1], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт.\n");
        return 6;
    }

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }

    int optval = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));


    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_BROADCAST;
    client_addr.sin_port = htons(port);

    for (int i = 0; i < ITER_COUNT; i++) {
        send_message_to(socket_fd, MESSAGE, &client_addr, sizeof(client_addr));
        sleep(1);
    }
    

    // Cleanup
    close(socket_fd);
    return 0;
}
