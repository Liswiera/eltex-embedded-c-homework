#include "lib.h"

void print_array(uint32_t *arr, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%" PRIu32 " ", arr[i]);
    }
    printf("\n");
}
void print_matrix(uint32_t *arr, size_t n, size_t m) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            printf("%" PRIu32 " ", arr[i * n + j]);
        }
        printf("\n");
    }
    printf("\n");
}


void fill_array(uint32_t *arr, size_t length) {
    uint32_t value = 0;

    while (length > 0) {
        *arr = ++value;

        length--;
        arr++;
    }
}

static void swap_u32(uint32_t *left, uint32_t *right) {
    *left ^= *right;
    *right ^= *left;
    *left ^= *right;
}

void reverse_array(uint32_t *arr, size_t length) {
    for (size_t i = 0, j = length - 1; i < j; i++, j--) {
        swap_u32(arr + i, arr + j);
    }
}

void fill_triangle_matrix(uint32_t *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            arr[i * n + j] = (i + j < n - 1) ? 0 : 1;
        }
    }
}

void fill_spiral_matrix(uint32_t *arr, size_t n) {
    size_t side_length = n - 1;
    size_t iteration_count = n / 2;
    size_t value = 0;
    size_t i = 0, j = 0;
    for (size_t iter_id = 0; iter_id < iteration_count; iter_id++) {
        for (size_t k = 0; k < side_length; k++) {
            arr[i * n + j] = ++value;
            j++;
        }
        for (size_t k = 0; k < side_length; k++) {
            arr[i * n + j] = ++value;
            i++;
        }
        for (size_t k = 0; k < side_length; k++) {
            arr[i * n + j] = ++value;
            j--;
        }
        for (size_t k = 0; k < side_length; k++) {
            arr[i * n + j] = ++value;
            i--;
        }

        i++;
        j++;
        side_length -= 2;
    }

    if (n % 2 == 1) {
        arr[i * n + j] = ++value;
    }
}
