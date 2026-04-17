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

int is_number(const char *str);
int port_str_to_val(const char *port_str, uint16_t *port_out);
int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message);
int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
