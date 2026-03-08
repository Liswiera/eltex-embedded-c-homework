#include "lib.h"

size_t uint32_to_binary(uint32_t value, char* output_buf) {
    if (value == 0) {
        *output_buf = '0';
        return 1;
    }

    // Задать старший бит равный 1
    uint32_t mask_bit = UINT32_MAX ^ (UINT32_MAX >> 1);

    // Пропустить старшие нули
    while ((mask_bit != 0) && ((value & mask_bit) == 0)) {
        mask_bit >>= 1;
    }

    size_t bytes_written = 0;
    while (mask_bit != 0) {
        *(output_buf + bytes_written) = ((value & mask_bit) == 0) ? '0' : '1';

        bytes_written++;
        mask_bit >>= 1;
    }

    return bytes_written;
}

size_t int32_to_binary(int32_t value, char* output_buf) {
    if (value < 0) {
        *output_buf = '-';
        return uint32_to_binary(-value, output_buf + 1) + 1;
    }
    else {
        return uint32_to_binary(value, output_buf);
    }
}

uint32_t uint32_bit_count(uint32_t value) {
    value = (value & 0x55555555u) + ((value >> 1u) & 0x55555555u);
    value = (value & 0x33333333u) + ((value >> 2u) & 0x33333333u);
    value = (value & 0x0F0F0F0Fu) + ((value >> 4u) & 0x0F0F0F0Fu);
    value = (value & 0x00FF00FFu) + ((value >> 8u) & 0x00FF00FFu);
    value = (value & 0x0000FFFFu) + ((value >> 16u) & 0x0000FFFFu);
    return value;
}

uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte) {
    return (value & ~0x00FF0000u) | ((uint32_t)new_byte << 16);
}