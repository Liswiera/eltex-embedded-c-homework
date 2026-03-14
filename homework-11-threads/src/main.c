#include <inttypes.h>
#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "client.h"
#include "store.h"
#include "supplier.h"

#define STORE_COUNT 5
#define CLIENT_COUNT 3
#define SUPPLIER_COUNT 1

#define SUPPLY_START_AVERAGE 1000
#define SUPPLY_START_VARIANCE 100
#define SUPPLY_START_MIN (SUPPLY_START_AVERAGE - SUPPLY_START_VARIANCE)
#define SUPPLY_START_MAX (SUPPLY_START_AVERAGE + SUPPLY_START_VARIANCE)
#define NEW_SUPPLY_AMOUNT 200

#define DEMAND_START_AVERAGE 10000
#define DEMAND_START_VARIANCE 100
#define DEMAND_START_MIN (DEMAND_START_AVERAGE - DEMAND_START_VARIANCE)
#define DEMAND_START_MAX (DEMAND_START_AVERAGE + DEMAND_START_VARIANCE)

#define CLIENT_WAIT_TIME 2
#define SUPPLIER_WAIT_TIME 1


void* thread_client(void *arg) {
    struct client *cl = arg;
    struct store *stores = cl->stores;

    while (cl->demand > 0) {
        sleep(cl->time_to_wait);
        size_t store_id = rand() % cl->store_count;
        struct store *selected_store = &stores[store_id];

        store_lock(selected_store);

        uint64_t old_demand = cl->demand;
        uint64_t old_amount = selected_store->amount;

        uint64_t retrieved_amount = store_retrieve_amount(selected_store, cl->demand);
        cl->demand -= retrieved_amount;

        printf("Client#%zu (%" PRIu64 " -> %" PRIu64 "), Store#%zu (%" PRIu64 " -> %" PRIu64 "), retrieved amount is %" PRIu64 "\n",
            cl->id, old_demand, cl->demand,
            selected_store->id, old_amount, selected_store->amount,
            retrieved_amount
        );

        store_unlock(selected_store);
    }

    printf("Client#%zu: Done.\n", cl->id);

    return NULL;
}

void* thread_supplier(void *arg) {
    struct supplier *spl = arg;
    struct store *stores = spl->stores;

    while (1) {
        sleep(spl->time_to_wait);
        size_t store_id = rand() % spl->store_count;
        struct store *selected_store = &stores[store_id];

        store_lock(selected_store);

        uint64_t old_amount = selected_store->amount;
        uint64_t added_amount = store_add_amount(selected_store, spl->supply);

        printf("Supplier#%zu, Store#%zu (%" PRIu64 " -> %" PRIu64 "), added amount is %" PRIu64 "\n",
            spl->id,
            selected_store->id, old_amount, selected_store->amount,
            added_amount
        );

        store_unlock(selected_store);
    }

    return NULL;
}

static uint64_t rand_in_range(uint64_t min_val, uint64_t max_val) {
    uint64_t len = (max_val - min_val) + 1;
    return min_val + (rand() % len);
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));
    

    // Инициализация магазинов, покупателей и поставщиков
    struct store stores[STORE_COUNT];
    for (size_t i = 0; i < STORE_COUNT; i++) {
        uint64_t start_amount = rand_in_range(SUPPLY_START_MIN, SUPPLY_START_MAX);
        store_init(&stores[i], i + 1, start_amount);
    }

    struct client clients[CLIENT_COUNT];
    for (size_t i = 0; i < CLIENT_COUNT; i++) {
        uint64_t demand = rand_in_range(DEMAND_START_MIN, DEMAND_START_MAX);
        client_init(&clients[i], i + 1, stores, STORE_COUNT, demand, CLIENT_WAIT_TIME);
    }
    
    struct supplier suppliers[SUPPLIER_COUNT];
    for (size_t i = 0; i < SUPPLIER_COUNT; i++) {
        supplier_init(&suppliers[i], i + 1, stores, STORE_COUNT, NEW_SUPPLY_AMOUNT, SUPPLIER_WAIT_TIME);
    }


    // Создание потоков покупателей и поставщиков
    pthread_t client_threads[CLIENT_COUNT];
    for (size_t i = 0; i < CLIENT_COUNT; i++) {
        pthread_create(&client_threads[i], NULL, thread_client, &clients[i]);
    }

    pthread_t supplier_threads[SUPPLIER_COUNT];
    for (size_t i = 0; i < CLIENT_COUNT; i++) {
        pthread_create(&supplier_threads[i], NULL, thread_supplier, &suppliers[i]);
    }


    // Ожидание завершения потоков покупателей
    for (size_t i = 0; i < CLIENT_COUNT; i++) {
        pthread_join(client_threads[i], NULL);
    }

    // Принудительное завершение потоков поставщиков
    for (size_t i = 0; i < SUPPLIER_COUNT; i++) {
        pthread_cancel(supplier_threads[i]); // TODO: it doesn't cancel the threads
    }

    // Освобождение ресурсов
    for (size_t i = 0; i < STORE_COUNT; i++) {
        store_destroy(&stores[i]);
    }

    printf("Main thread: Done.\n");

    return 0;
}
