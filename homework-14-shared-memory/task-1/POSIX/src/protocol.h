#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_PATH "/homework-14-1-shm"
#define MQ_MAXMSG 8
#define SHM_MSGSIZE (4096 - sizeof(sem_t) - sizeof(sem_t))

#define SERVER_ID 0
#define CLIENT_ID 1

struct message_cell {
    sem_t read_sem;
    sem_t write_sem;
    char msg[SHM_MSGSIZE];
};

#define MSG_CELL_SIZE sizeof(struct message_cell)

void message_cell_init(struct message_cell *cell);
void send_string(struct message_cell *cell, const char *str_ptr, size_t str_len);
void receive_string(struct message_cell *cell, char *buf_ptr, size_t buf_len);
