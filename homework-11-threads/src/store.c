#include "store.h"

void store_init(struct store *st, size_t id, uint64_t amount) {
    st->id = id;
    st->amount = amount;
    pthread_mutex_init(&st->lock, NULL);
}

void store_destroy(struct store *st) {
    pthread_mutex_destroy(&st->lock);
}

void store_free(struct store* st) {
    if (st != NULL) {
        store_destroy(st);
        free(st);
    }
}

void store_lock(struct store *st) {
    pthread_mutex_lock(&st->lock);
}

void store_unlock(struct store *st) {
    pthread_mutex_unlock(&st->lock);
}

uint64_t store_retrieve_amount(struct store *st, uint64_t value) {
    uint64_t retrieved_amount;

    if (st->amount > value) {
        retrieved_amount = value;
        st->amount -= value;
    }
    else {
        retrieved_amount = st->amount;
        st->amount = 0;
    }

    return retrieved_amount;
}

uint64_t store_add_amount(struct store *st, uint64_t value) {
    st->amount += value;

    return value;
}
