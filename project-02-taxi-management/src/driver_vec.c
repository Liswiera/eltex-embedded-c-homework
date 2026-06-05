#include "driver_vec.h"

int driver_vec_init(struct driver_vec *vec, size_t capacity) {
    vec->capacity = capacity;
    vec->len = 0;
    vec->drivers = NULL;
    
    if (capacity > 0) {
        vec->drivers = malloc(sizeof(struct driver) * capacity);
        return (vec->drivers != NULL) ? 0 : 1;
    }
    else {
        return 0;
    }
}

int driver_vec_push_back(struct driver_vec *vec, const struct driver *elem) {
    if (vec->len < vec->capacity) {
        memcpy(&vec->drivers[vec->len], elem, sizeof(struct driver));
        vec->len++;
        return 1;
    }
    else {
        return 0;
    }
}

struct driver* driver_vec_find_with_pid(struct driver_vec *vec, pid_t pid) {
    // Linear search
    // NOTE: We should never assume that the sequence of PID values is always increasing
    // even if we append new fork'd drivers to the vector sequentially

    for (size_t i = 0; i < vec->len; i++) {
        struct driver *drv = &vec->drivers[i];
        
        if (drv->pid == pid) {
            return drv;
        }
    }

    return NULL;
}

void driver_vec_destroy(struct driver_vec *vec) {
    for (size_t i = 0; i < vec->len; i++) {
        driver_terminate(&vec->drivers[i]);
    }
    free(vec->drivers);
}
