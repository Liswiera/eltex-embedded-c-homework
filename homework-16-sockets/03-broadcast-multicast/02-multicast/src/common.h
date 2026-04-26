#pragma once
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MULTI_ADDR_STR "226.1.2.3"
#define BUF_LEN 32

int is_number(const char *str);
int port_str_to_val(const char *port_str, uint16_t *port_out);
ssize_t send_message(int fd, const char *message);
ssize_t receive_message(int fd);
ssize_t send_message_to(int fd, const char *message, const struct sockaddr_in *addr, socklen_t addr_len);
ssize_t receive_message_from(int fd, struct sockaddr_in *addr, socklen_t *addr_len);
