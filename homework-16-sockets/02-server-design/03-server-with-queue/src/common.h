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

#define BUF_LEN 2048
#define MQ_MAXMSG 8
#define MQ_MSGSIZE 512

int is_number(const char *str);
int port_str_to_val(const char *port_str, uint16_t *port_out);
int send_message_to(int fd, const char *message, const struct sockaddr_in *addr, socklen_t addr_len);
int receive_message_from(int fd, struct sockaddr_in *addr, socklen_t *addr_len);
