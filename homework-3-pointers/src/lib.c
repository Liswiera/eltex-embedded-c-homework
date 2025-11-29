#include "lib.h"

void print_array(uint32_t *arr, size_t length) {
    uint32_t *end = arr + length;
    for (uint32_t *ptr = arr; ptr < end; ptr++) {
        printf("%" PRIu32 " ", *ptr);
    }
    printf("\n");
}


uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte) {
    uint8_t *byte_ptr = (uint8_t*)&value;
    *(byte_ptr + 2) = new_byte;

    return value;
}
