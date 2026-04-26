#include <locale.h>
#include "common.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: client [PORT]\n");
        return 8;
    }


    uint16_t port;
    if (port_str_to_val(argv[1], &port) != 0) {
        fprintf(stderr, "Некорректно указан порт назначения.\n");
        return 6;
    }
    
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }

    // Привязываем сокет к порту
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(port);

    int bind_status = bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать имя к сокету.\n");

        close(socket_fd);
        return 2;
    }
    
    // Добавляем сокет в группу для получения сообщений через мультикаст
    struct ip_mreqn optval;
    optval.imr_multiaddr.s_addr = inet_addr(MULTI_ADDR_STR);
    optval.imr_address.s_addr = INADDR_ANY;
    optval.imr_ifindex = 0;
    int status = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &optval, sizeof(optval));
    if (status == -1) {
        fprintf(stderr, "Не удалось подписать клиента к группе '%s'\n", MULTI_ADDR_STR);
        return 5;
    }

    printf("Жду широковещательное сообщение...\n");
    receive_message_from(socket_fd, NULL, NULL);

    // Cleanup
    close(socket_fd);
    return 0;
}
