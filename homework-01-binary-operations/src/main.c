#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

#define BUF_SIZE 40

int main(int argc, char** argv) {
    char buf[BUF_SIZE];

    if (argc < 2) {
        printf("USAGE: main [1-4]\n");
        return 1;
    }

    int task_id = atoi(argv[1]);

    switch (task_id) {
        case 1:
            printf("Введите целое положительное число, чтобы получить его двоичное представление:\n\n");
            while (1) {
                printf("> ");
                fflush(stdout);

                uint32_t value;
                scanf("%" SCNu32, &value);

                size_t bytes_written = uint32_to_binary(value, buf);
                buf[bytes_written] = '\0';
                printf("%" PRIu32 " => %s\n\n", value, buf);
            }
        case 2:
            printf("Введите целое число, чтобы получить его двоичное представление:\n\n");
            while (1) {
                printf("> ");
                fflush(stdout);

                int32_t value;
                scanf("%" SCNi32, &value);

                size_t bytes_written = int32_to_binary(value, buf);
                buf[bytes_written] = '\0';
                printf("%" PRIi32 " => %s\n\n", value, buf);
            }
        case 3:
            printf("Введите целое положительное число, чтобы найти количество единиц в его двоичном представлении:\n\n");
            while (1) {
                printf("> ");
                fflush(stdout);

                uint32_t value;
                scanf("%" SCNu32, &value);

                uint32_t bit_count = uint32_bit_count(value);
                printf("%" PRIu32 " => %" PRIu32 " бит\n\n", value, bit_count);
            }
        case 4:
            printf("Введите два целых положительных числа, чтобы заменить третий байт первого числа на значение второго числа:\n\n");
            while (1) {
                printf("> ");
                fflush(stdout);

                uint32_t value;
                uint8_t new_byte;
                scanf("%" SCNu32 "%" SCNu8, &value, &new_byte);

                uint32_t new_value = uint32_set_byte_2(value, new_byte);
                printf("%" PRIu32 " => %" PRIu32 "\n\n", value, new_value);
            }
        default:
            printf("Неправильный номер задания (число должно быть от 1 до 4)\n");
            return 2;
    }

    return 0;
}