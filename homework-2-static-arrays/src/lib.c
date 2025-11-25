#include "lib.h"

void fill_array(uint32_t *arr, size_t length) {
    uint32_t value = 0;

    while (length > 0) {
        *arr = ++value;

        length--;
        arr++;
    }
}