#pragma once
#ifndef _PROJ_01_MESSAGE_H_
#define _PROJ_01_MESSAGE_H_

#include <stdio.h>
#include "packet.h"

#define TEXT_BUF_LEN (APP_DATA_BUF_LEN - sizeof(uint8_t) - sizeof(uint16_t))

typedef enum {
    ECHO_REQUEST = 0,
    ECHO_REPLY,
    CONN_TERM,
} message_type;

// This struct is OS-agnostic and can be safely sent over the network
struct message {
    uint16_t len;               // Big-endian
    uint8_t type;
    char text[TEXT_BUF_LEN];    // The string is NOT null-terminated
};

void message_init(struct message *msg, message_type type, const char *text);
size_t message_struct_len(const struct message *msg);
size_t message_text_len(const struct message *msg);
void message_print(const struct message *msg);
void message_null_terminate(struct message *msg);
ssize_t send_raw_udp_message(int socket_fd, const struct sockaddr_in *dest_addr, uint16_t src_port, const char* text, message_type type);
ssize_t recv_raw_inet_message(int socket_fd, struct sockaddr_in *src_addr, uint16_t listen_port, struct message *msg_output);

#endif
