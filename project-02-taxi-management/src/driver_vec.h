#pragma once
#ifndef _PROJ_02_DRIVER_VEC_H_
#define _PROJ_02_DRIVER_VEC_H_

#include <inttypes.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include "driver.h"

struct driver_vec {
    size_t capacity;
    size_t len;
    struct driver *drivers;
};

int driver_vec_init(struct driver_vec *vec, size_t capacity);
int driver_vec_push_back(struct driver_vec *vec, const struct driver *elem);
struct driver* driver_vec_find_with_pid(struct driver_vec *vec, pid_t pid);
void driver_vec_destroy(struct driver_vec *vec);

#endif
