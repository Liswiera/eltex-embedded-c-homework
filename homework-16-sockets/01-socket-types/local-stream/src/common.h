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
#define SOCKET_PATH "/tmp/homework-16-local-stream"

int send_message(int fd, const char *message);
int receive_message(int fd);
