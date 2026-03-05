#pragma once
#include <inttypes.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>

struct child_proc_arr {
    uint32_t *id_arr;
    size_t count;
};

struct child_proc_arr* child_proc_arr_create(size_t count, ...);
void child_proc_arr_free(struct child_proc_arr *arr);
