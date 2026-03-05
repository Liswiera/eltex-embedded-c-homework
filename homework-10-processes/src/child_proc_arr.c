#include "child_proc_arr.h"

struct child_proc_arr* child_proc_arr_create(size_t count, ...) {
    struct child_proc_arr *res = malloc(sizeof(struct child_proc_arr));
    if (res == NULL) {
        return NULL;
    }
    res->id_arr = NULL;

    res->count = count;
    if (count > 0) {
        res->id_arr = malloc(sizeof(uint32_t) * count);
        if (res->id_arr == NULL) {
            child_proc_arr_free(res);
            return NULL;
        }

        va_list args;
        va_start(args, count);

        for (size_t i = 0; i < count; i++) {
            res->id_arr[i] = va_arg(args, uint32_t);
        }
    }

    return res;
}

void child_proc_arr_free(struct child_proc_arr *arr) {
    if (arr != NULL) {
        free(arr->id_arr);
        free(arr);
    }
}
