#pragma once
#pragma pack(1)

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_LEN 32
#define PAYLOAD_BUF_LEN 64

#define ETH_HDR_LEN 14
#define INET_HDR_LEN 20
#define UDP_HDR_LEN 8
#define PACKET_HEADER_LEN (ETH_HDR_LEN + INET_HDR_LEN + UDP_HDR_LEN)

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

struct eth_header {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t type;
};

struct inet_udp_datagram {
    struct inet_header inet_hdr;
    struct udp_packet udp_segment;
};

// Для данной структуры нужно убрать выравнивание,
// т.к. paload начинается с 14-го байта, не кратному выравниванию в 4 байта из-за поля src_addr/dst_addr
struct inet_udp_frame {
    struct eth_header e_hdr;
    struct inet_udp_datagram payload;
};

void inet_header_set_checksum(struct inet_header *hdr);

int is_number(const char *str);
int port_str_to_val(const char *port_str, uint16_t *port_out);

int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message);
int send_packet_to(int fd, const struct sockaddr_ll *dest_addr, socklen_t addrlen, const struct inet_udp_frame *packet);
int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
ssize_t receive_packet_from(int fd, uint16_t expected_port, struct sockaddr_ll* restrict src_addr, socklen_t* restrict addrlen);
