#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: main [1-4]\n");
        return 1;
    }

    int task_id = atoi(argv[1]);

    switch (task_id) {
        case 1:
            printf("Введите целое неотрицательное число, чтобы задать размер квадратной матрицы:\n\n");
            while (1) {
                printf("> ");
                fflush(stdout);

                size_t n;
                scanf("%zu", &n);

                size_t length = n * n;
                uint32_t *arr = (uint32_t*)malloc(length * sizeof(uint32_t));
                fill_array(arr, length);

                uint32_t *element_ptr = arr;
                for (size_t i = 0; i < n; i++) {
                    for (size_t j = 0; j < n; j++) {
                        printf("%" PRIu32 " ", *element_ptr);
                        element_ptr++;
                    }
                    printf("\n");
                }
                printf("\n");

                free(arr);
            }
        default:
            printf("Неправильный номер задания (число должно быть от 1 до 4)\n");
            return 2;
    }

    return 0;
}