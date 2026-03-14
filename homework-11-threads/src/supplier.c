#include "supplier.h"

void supplier_init(struct supplier *spl, size_t id, struct store *stores, size_t store_count, uint64_t supply, unsigned int time_to_wait) {
    spl->id = id;
    spl->stores = stores;
    spl->store_count = store_count;
    spl->supply = supply;
    spl->time_to_wait = time_to_wait;
}
