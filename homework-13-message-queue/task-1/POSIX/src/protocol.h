#pragma once
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQUEUE_PATH "/homework-13-1-queue"
#define MQ_MSGSIZE 512

int send_string(mqd_t mq, const char *str_ptr, size_t str_len, unsigned int prio);
ssize_t receive_string(mqd_t mq, char *buf_ptr, size_t buf_len, unsigned int *prio);
