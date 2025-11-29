#include "lib.h"

uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte) {
    uint8_t *byte_ptr = (uint8_t*)&value;
    *(byte_ptr + 2) = new_byte;

    return value;
}
