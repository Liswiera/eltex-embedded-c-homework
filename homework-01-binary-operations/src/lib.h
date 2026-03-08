#pragma once

#include <inttypes.h>
#include <stdio.h>

// 1. Вывести двоичное представление целого положительного числа, используя битовые операции.
size_t uint32_to_binary(uint32_t value, char* output_buf);

// 2. Вывести двоичное представление целого отрицательного числа, используя битовые операции.
size_t int32_to_binary(int32_t value, char* output_buf);

// 3. Найти количество единиц в двоичном представлении целого положительного числа.
uint32_t uint32_bit_count(uint32_t value);

// 4. Поменять в целом положительном числе (типа int) значение третьего байта на введенное пользователем число
uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte);