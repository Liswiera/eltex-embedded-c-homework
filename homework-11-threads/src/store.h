#pragma once
#include <inttypes.h>
#include <pthread.h>
#include <stdlib.h>

struct store {
    size_t id;
    uint64_t amount;
    pthread_mutex_t lock;
};

void store_init(struct store *st, size_t id, uint64_t amount);
void store_destroy(struct store *st);
void store_free(struct store* st);

void store_lock(struct store *st);
void store_unlock(struct store *st);

uint64_t store_retrieve_amount(struct store *st, uint64_t value);
uint64_t store_add_amount(struct store *st, uint64_t value);
