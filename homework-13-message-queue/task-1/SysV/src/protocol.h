#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

#define FTOK_PATH "./server"
#define FTOK_PROJ_ID 83
#define MQ_MSGSIZE 512
#define BUF_LEN (MQ_MSGSIZE + 1)

#define MSG_TYPE_SERVER 1
#define MSG_TYPE_CLIENT 2

struct message {
    long mtype;
    char mtext[BUF_LEN];
};

int send_string(int queue_id, const void *msgp, size_t str_len, int msgflg);
ssize_t receive_string(int queue_id, void *msgp, size_t buf_len, long msgtyp, int msgflg);
