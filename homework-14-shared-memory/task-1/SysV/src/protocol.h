#pragma once
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "message_duplex.h"

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

#define SERVER_WRITE_SEM_NUM 0
#define SERVER_READ_SEM_NUM 1
#define CLIENT_WRITE_SEM_NUM 2
#define CLIENT_READ_SEM_NUM 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};
