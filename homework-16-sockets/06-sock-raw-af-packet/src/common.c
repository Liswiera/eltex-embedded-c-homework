#include "common.h"

void inet_header_set_checksum(struct inet_header *hdr) {
    hdr->header_checksum = 0;

    size_t word_count = sizeof(struct inet_header) / 2;

    uint32_t sum = 0;
    uint16_t *word_ptr = (uint16_t*)hdr;

    for (size_t i = 0; i < word_count; i++) {
        sum += *(word_ptr++);
    }

    while (sum > __UINT16_MAX__) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    hdr->header_checksum = ~sum;
}

int is_number(const char *str) {
    for (; *str != '\0'; str++) {
        if (!isdigit(*str)) {
            return 0;
        }
    }

    return 1;
}

int port_str_to_val(const char *port_str, uint16_t *port_out) {
    if (!is_number(port_str)) {
        return 1;
    }

    errno = 0;
    long port = strtol(port_str, NULL, 10);
    if ((port < 0) || (port > __UINT16_MAX__) || (errno == ERANGE)) {
        return 2;
    }

    *port_out = (uint16_t)port;
    return 0;
}


int send_packet_to(int fd, const struct sockaddr_ll *dest_addr, socklen_t addrlen, const struct inet_udp_frame *packet) {
    const size_t packet_length = sizeof(struct eth_header) + ntohs(packet->payload.inet_hdr.len);
    const char* msg = (char*)((uint8_t*)packet + PACKET_HEADER_LEN);

    int bytes_sent = sendto(fd, packet, packet_length, 0, (struct sockaddr*)dest_addr, addrlen);
      
    if (bytes_sent != -1) {
        printf("Было отправлено сообщение '%s'.\n", msg);
    }
    else {
        int saved_errno = errno;
        fprintf(stderr, "Не удалось отправить сообщение '%s' (ERRNO = %d).\n", msg, saved_errno);
    }

    return bytes_sent;
}

int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message) {
    int bytes_sent = sendto(fd, message, strlen(message), 0, dest_addr, addrlen);
    if (bytes_sent != -1) {
        printf("Было отправлено сообщение '%s'.\n", message);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение '%s'.\n", message);
    }

    return bytes_sent;
}

int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen) {
    char buf[BUF_LEN];

    int bytes_read = recvfrom(fd, buf, BUF_LEN - 1, 0, src_addr, addrlen);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение: '%s'.\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение.\n");
    }

    return bytes_read;
}

ssize_t receive_packet_from(int fd, uint16_t expected_port, struct sockaddr_ll* restrict src_addr, socklen_t* restrict addrlen) {
    struct inet_udp_frame packet;

    while (1) {
        ssize_t bytes_read = recvfrom(fd, &packet, sizeof(packet), 0, (struct sockaddr*)src_addr, addrlen);
        
        if (bytes_read != -1) {
            struct udp_packet *udp = &packet.payload.udp_segment;

            if (ntohs(udp->dst_port) == expected_port) {
                udp->payload[ntohs(udp->len) - 8] = '\0';

                printf("Было получено сообщение: '%s'.\n", udp->payload);
                return bytes_read;
            }
        }
        else {
            fprintf(stderr, "Не удалось принять сообщение.\n");
            return bytes_read;
        }
    }
}
