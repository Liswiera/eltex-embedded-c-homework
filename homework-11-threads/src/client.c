#include "client.h"

void client_init(struct client *cl, size_t id, struct store *stores, size_t store_count, uint64_t demand, unsigned int time_to_wait) {
    cl->id = id;
    cl->stores = stores;
    cl->store_count = store_count;
    cl->demand = demand;
    cl->time_to_wait = time_to_wait;
}
