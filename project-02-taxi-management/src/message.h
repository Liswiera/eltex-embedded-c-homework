#pragma once
#ifndef _PROJ_02_MESSAGE_H_
#define _PROJ_02_MESSAGE_H_

#include <inttypes.h>
#include <unistd.h>

typedef enum {
    M_SEND_TASK = 0,
    M_TASK_ACCEPTED,
    M_TASK_REFUSED_BAD_ARG,
    M_TIMEOUT,
    M_GET_STATUS,
    M_DRIVER_AVAILABLE,
    M_DRIVER_BUSY,
} message_type;

struct message {
    message_type type;
    uint32_t time;
};

#endif
