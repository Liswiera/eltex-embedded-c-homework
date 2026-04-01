#pragma once
#include <inttypes.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQ_MAXMSG 8
#define MQ_MSGSIZE 512

#define SERVER_LISTENER_QUEUE "/homework-13-2-server-listener-queue"
#define USER_QUEUE_PREFIX "/homework-13-2-user-queue"

#define USER_NAME_BUF_LEN 64
#define TEXT_BUF_LEN (MQ_MSGSIZE - sizeof(enum msg_type) - USER_NAME_BUF_LEN)


void get_user_queue_from_name(char* restrict queue_name_buf, const char* restrict user_name);
void set_queue_creat_attributes(struct mq_attr *attributes);
