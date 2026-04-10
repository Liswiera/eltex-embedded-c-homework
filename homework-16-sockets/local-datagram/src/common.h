#pragma once
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUF_LEN 32
#define SERVER_SOCKET_PATH "/tmp/homework-16-local-datagram-server"
#define CLIENT_SOCKET_PATH "/tmp/homework-16-local-datagram-client"

int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message);
int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen);
