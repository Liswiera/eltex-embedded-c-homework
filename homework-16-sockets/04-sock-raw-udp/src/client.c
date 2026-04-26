#include <locale.h>
#include "common.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)
#define SRC_PORT 49123


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

    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (socket_fd == -1) {
        if (errno == EPERM) {
            fprintf(stderr, "Не удалось создать сокет (недостаточно прав).\n");
        }
        else {
            fprintf(stderr, "Не удалось создать сокет.\n");
        }
        
        return 1;
    }


    struct udp_packet packet;
    packet.src_port = htons(SRC_PORT);
    packet.dst_port = htons(port);
    packet.len = htons(8 + MESSAGE_LEN);
    packet.checksum = htons(0);
    strncpy(packet.payload, MESSAGE, PAYLOAD_BUF_LEN);

    send_message_to_udp_raw(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr), &packet);
    receive_message_from_udp_raw(socket_fd, SRC_PORT, NULL, NULL);

    // Cleanup
    close(socket_fd);
    return 0;
}
