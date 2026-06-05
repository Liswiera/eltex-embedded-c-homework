#pragma once
#ifndef _PROJ_02_DRIVER_H_
#define _PROJ_02_DRIVER_H_

#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "message.h"
#include "utility.h"

typedef enum {
    S_DRIVER_AVAILABLE = 0,
    S_DRIVER_BUSY,
} driver_status;

struct driver {
    size_t id;
    pid_t pid;
    int read_fd;
    int write_fd;
    driver_status status;
};

int driver_init_fork(struct driver *drv, size_t id);
int driver_terminate(struct driver *drv);
noreturn void driver_entry(struct driver *self);

#endif
