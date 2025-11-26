#pragma once

#include <inttypes.h>
#include <stdio.h>

void print_array(uint32_t *arr, size_t length);
void print_matrix(uint32_t *arr, size_t n, size_t m);

void fill_array(uint32_t *arr, size_t length);
void reverse_array(uint32_t *arr, size_t length);
void fill_triangle_matrix(uint32_t *arr, size_t n);