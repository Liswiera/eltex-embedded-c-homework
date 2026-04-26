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

struct net_udp_packet {
    char inet_header[20];
    struct udp_packet udp_segment;
};

int is_number(const char *str);
int port_str_to_val(const char *port_str, uint16_t *port_out);
int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message);
int send_message_to_udp_raw(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const struct udp_packet *packet);
int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
ssize_t receive_message_from_udp_raw(int fd, uint16_t port, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
