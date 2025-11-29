#pragma once

#include <inttypes.h>
#include <stdio.h>

// 1. Поменять в целом положительном числе (типа int) значение третьего байта
// на введенное пользователем число через указатель (не применяя битовые операции).
uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte);
