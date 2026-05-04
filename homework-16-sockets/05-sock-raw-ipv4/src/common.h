#pragma once
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_LEN 32
#define PAYLOAD_BUF_LEN 64

struct udp_packet {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t checksum;
    char payload[PAYLOAD_BUF_LEN];
};

struct inet_header {
    uint8_t ver_ihl;
    uint8_t ds;
    uint16_t len;
    uint16_t identification;
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    struct in_addr src_addr;
    struct in_addr dst_addr;
};

struct inet_udp_packet {
    struct inet_header inet_hdr;
    struct udp_packet udp_segment;
};

void inet_header_set_checksum(struct inet_header *hdr);

int is_number(const char *str);
int port_str_to_val(const char *port_str, uint16_t *port_out);
int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message);
int send_message_to_udp_raw(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const struct udp_packet *packet);
int send_message_to_inet_udp_raw(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const struct inet_udp_packet *packet);
int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
ssize_t receive_message_from_udp_raw(int fd, uint16_t port, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
