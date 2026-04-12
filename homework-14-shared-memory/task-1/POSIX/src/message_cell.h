#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define MSG_CELL_SIZE sizeof(struct message_cell)
#define MSGSIZE (1024 - sizeof(sem_t) - sizeof(sem_t))

struct message_cell {
    sem_t read_sem;
    sem_t write_sem;
    char msg[MSGSIZE];
};

void msg_cell_init(struct message_cell *cell);
void msg_cell_destroy(struct message_cell *cell);
void msg_cell_send_string(struct message_cell *cell, const char *str_ptr, size_t str_len);
void msg_cell_recv_string(struct message_cell *cell, char *buf_ptr, size_t buf_len);
