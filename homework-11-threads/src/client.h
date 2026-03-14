#pragma once
#include <inttypes.h>
#include <stdlib.h>

struct client {
    size_t id;
    struct store *stores;
    size_t store_count;
    uint64_t demand;
    unsigned int time_to_wait;
};

void client_init(struct client *cl, size_t id, struct store *stores, size_t store_count, uint64_t demand, unsigned int time_to_wait);
