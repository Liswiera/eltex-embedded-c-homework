#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define MSG_DUPLEX_SIZE sizeof(struct message_duplex)
#define MSGSIZE 2048

struct message_duplex {
    char server[MSGSIZE];
    char client[MSGSIZE];
};

void msg_duplex_server_send(struct message_duplex *duplex, const char *str_ptr, size_t str_len, int sem_id, int sem_write_num, int sem_read_num);
void msg_duplex_client_send(struct message_duplex *duplex, const char *str_ptr, size_t str_len, int sem_id, int sem_write_num, int sem_read_num);
void msg_duplex_server_recv(struct message_duplex *duplex, char *buf_ptr, size_t buf_len, int sem_id, int sem_write_num, int sem_read_num);
void msg_duplex_client_recv(struct message_duplex *duplex, char *buf_ptr, size_t buf_len, int sem_id, int sem_write_num, int sem_read_num);
