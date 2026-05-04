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

    //int socket_fd = socket(AF_INET, SOCK_RAW, IP_HDRINCL);
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

    int opt = 1;
    int status = setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));
    if (status == -1) {
        fprintf(stderr, "Не удалось добавить допцию IP_HDRINCL для raw-сокета.\n");

        close(socket_fd);
        return 2;
    }

    printf("Inet header size: %zu\n", sizeof(struct inet_header));

    struct inet_udp_packet datagram;
    struct inet_header *header = &datagram.inet_hdr;
    header->ver_ihl = (4 << 4) | 5; // IPv4 | 5 32-bit words in the header (20 bytes)
    header->ds = 0; // TODO
    header->len = htons(sizeof(struct inet_header) + 8 + MESSAGE_LEN);
    header->identification = htons(0); // TODO
    header->flags_offset = htons(0b010 << 13); // Don't fragment
    header->ttl = 255;
    header->protocol = 17; // UDP
    header->src_addr.s_addr = htonl(0); // Filled in automatically
    memcpy(&header->dst_addr, &server_addr.sin_addr, sizeof(struct in_addr));
    inet_header_set_checksum(header);
    
    struct udp_packet *segment = &datagram.udp_segment;
    segment->src_port = htons(SRC_PORT);
    segment->dst_port = htons(port);
    segment->len = htons(8 + MESSAGE_LEN);
    segment->checksum = htons(0);
    strncpy(segment->payload, MESSAGE, PAYLOAD_BUF_LEN);

    send_message_to_inet_udp_raw(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr), &datagram);
    receive_message_from_udp_raw(socket_fd, SRC_PORT, NULL, NULL);

    // Cleanup
    close(socket_fd);
    return 0;
}
