#pragma once
#include <pthread.h>
#include <unistd.h>

struct worker {
    int id;
    int client_fd;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};
