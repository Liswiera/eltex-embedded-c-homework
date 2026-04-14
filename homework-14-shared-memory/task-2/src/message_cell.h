#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define MSGSIZE (4096 - sizeof(sem_t) - sizeof(sem_t))
#define MSG_CELL_SIZE sizeof(struct message_cell)

#define USER_NAME_BUF_LEN 64
#define TEXT_BUF_LEN (MSGSIZE - sizeof(enum msg_type) - USER_NAME_BUF_LEN)

enum msg_type {
    connection_request,
    disconnection_request,
    user_connected,
    user_disconnected,
    message_from_user,
    message_from_server,
    session_ended,
};

struct message {
    enum msg_type type;
    char user_name[USER_NAME_BUF_LEN];
    char text[TEXT_BUF_LEN];
};

void message_set(struct message *msg, enum msg_type type, const char *user_name, const char* text);

struct message_cell {
    sem_t read_sem;
    sem_t write_sem;
    struct message msg;
};

void msg_cell_init(struct message_cell *cell);
void msg_cell_destroy(struct message_cell *cell);
void msg_cell_send_message(struct message_cell *cell, const struct message *msg);
void msg_cell_recv_message(struct message_cell *cell, struct message *msg);
