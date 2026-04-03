#pragma once
#include "protocol.h"

struct queue_pair {
    mqd_t read_end;
    mqd_t write_end;
    const char* q_name;
};

int create_queue_pair(const char *q_name, struct queue_pair *q_pair);
void destroy_queue_pair(struct queue_pair *q_pair);
