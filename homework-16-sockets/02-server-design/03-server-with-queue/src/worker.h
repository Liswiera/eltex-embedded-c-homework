#pragma once
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

struct worker {
    int id;
    pthread_t thread;
};
