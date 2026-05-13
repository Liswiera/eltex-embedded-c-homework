#include <locale.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "common.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)

const uint8_t dest_mac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Hardcoded

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 6) {
        printf("Usage: client [INTERFACE_NAME] [IPV4_SRC_ADDR] [IPV4_DEST_ADDR] [SRC_PORT] [DEST_PORT]\n");
        printf("Destination MAC-address is hardcoded in the program\n");
        return 8;
    }

    // Создаём RAW-сокет
    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (socket_fd == -1) {
        if (errno == EPERM) {
            fprintf(stderr, "Не удалось создать сокет (недостаточно прав).\n");
        }
        else {
            fprintf(stderr, "Не удалось создать сокет.\n");
        }
        
        return 1;
    }

    // Считываем имя сетевого интерфейса из 1-го аргумента
    const char* if_name = argv[1];

    // Получаем индекс сетевого интерфейса, на который будем отправлять пакет
    struct ifreq if_index;
    memset(&if_index, 0, sizeof(if_index));
    strncpy(if_index.ifr_name, if_name, sizeof(if_index.ifr_name));
    if (ioctl(socket_fd, SIOCGIFINDEX, &if_index) < 0) {
        fprintf(stderr, "Неправильно указано имя сетевого интерфейса.\n");
        return 5;
    }

    // Получаем MAC-адрес этого же сетевого интерфейса
    struct ifreq if_mac;
    memset(&if_mac, 0, sizeof(if_mac));
    strncpy(if_mac.ifr_name, if_name, sizeof(if_mac.ifr_name));
    if (ioctl(socket_fd, SIOCGIFHWADDR, &if_mac) < 0) {
        fprintf(stderr, "Неправильно указано имя сетевого интерфейса.\n");
        return 5;
    }

    struct sockaddr_ll server_ll;
    memset(&server_ll, 0, sizeof(server_ll));
    server_ll.sll_family = AF_PACKET;
    server_ll.sll_protocol = htons(ETH_P_ALL);
    server_ll.sll_ifindex = if_index.ifr_ifindex;
    server_ll.sll_halen = ETH_ALEN;
    for (int i = 0; i < 6; i++) {
        server_ll.sll_addr[i] = dest_mac[i];
    }

    // Считываем адрес источника из 2-го аргумента
    struct in_addr client_addr;
    if (inet_pton(AF_INET, argv[2], &client_addr) != 1) {
        fprintf(stderr, "Некорректно указан IPv4 адрес источника.\n");
        return 7;
    }

    // Считываем адрес назначения из 3-го аргумента
    struct in_addr server_addr;
    if (inet_pton(AF_INET, argv[3], &server_addr) != 1) {
        fprintf(stderr, "Некорректно указан IPv4 адрес назначения.\n");
        return 7;
    }

    // Считываем номер порта назначения из 5-го аргумента
    uint16_t src_port;
    if (port_str_to_val(argv[4], &src_port) != 0) {
        fprintf(stderr, "Некорректно указан порт источника.\n");
        return 6;
    }

    // Считываем номер порта назначения из 5-го аргумента
    uint16_t dst_port;
    if (port_str_to_val(argv[5], &dst_port) != 0) {
        fprintf(stderr, "Некорректно указан порт назначения.\n");
        return 6;
    }

    // Начинаем заполнять данные в сетевом пакете
    struct inet_udp_frame frame;


    // Заполнение заголовка канального уровня
    struct eth_header *eth_hdr = &frame.e_hdr;
    for (int i = 0; i < 6; i++) {
        eth_hdr->src_mac[i] = ((uint8_t*)&if_mac.ifr_hwaddr.sa_data)[i];
        eth_hdr->dst_mac[i] = dest_mac[i];
    }
    eth_hdr->type = htons(ETH_P_IP);


    // Заполнение заголовка IPv4
    struct inet_udp_datagram *datagram = &frame.payload;
    struct inet_header *in_hdr = &datagram->inet_hdr;
    in_hdr->ver_ihl = (4 << 4) | 5; // IPv4 | 5 32-bit words in the header (20 bytes)
    in_hdr->ds = 0;
    in_hdr->len = htons(sizeof(struct inet_header) + 8 + MESSAGE_LEN);
    in_hdr->identification = htons(0);
    in_hdr->flags_offset = htons(0b010 << 13); // Don't fragment
    in_hdr->ttl = UINT8_MAX;
    in_hdr->protocol = 17; // UDP
    memcpy(&in_hdr->src_addr, &client_addr, sizeof(struct in_addr));
    memcpy(&in_hdr->dst_addr, &server_addr, sizeof(struct in_addr));
    inet_header_set_checksum(in_hdr);
    

    // Заполнение заголовка UDP
    struct udp_packet *segment = &datagram->udp_segment;
    segment->src_port = htons(src_port);
    segment->dst_port = htons(dst_port);
    segment->len = htons(8 + MESSAGE_LEN);
    segment->checksum = htons(0);
    strncpy(segment->payload, MESSAGE, PAYLOAD_BUF_LEN);

    
    // Взаимодействие с сервером
    send_packet_to(socket_fd, &server_ll, sizeof(server_ll), &frame);
    receive_packet_from(socket_fd, src_port, NULL, NULL);

    // Cleanup
    close(socket_fd);
    return 0;
}
