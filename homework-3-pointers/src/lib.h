#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

void print_array(uint32_t *arr, size_t length);

// 1. Поменять в целом положительном числе (типа int) значение третьего байта
// на введенное пользователем число через указатель (не применяя битовые операции).
uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte);

// 4. Напишите программу, которая ищет во введенной строке
// введенную подстроку и возвращает указатель на начало
// подстроки, если подстрока не найдена в указатель записывается NULL.
const char* find_substr_naive(const char *text, const char *pattern);
