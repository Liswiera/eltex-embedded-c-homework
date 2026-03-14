#pragma once
#include <inttypes.h>
#include <stdlib.h>

struct supplier {
    size_t id;
    struct store *stores;
    size_t store_count;
    uint64_t supply;
    unsigned int time_to_wait;
};

void supplier_init(struct supplier *spl, size_t id, struct store *stores, size_t store_count, uint64_t supply, unsigned int time_to_wait);
