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
