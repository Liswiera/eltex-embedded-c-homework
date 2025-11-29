#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

// 2. В приведенном ниже коде измените только одну строку (помеченную), чтобы напечатать “Результат: 12.0”.
void task_2(void) {
    float x = 5.0;
    printf("x = %f, ", x);
    float y = 6.0;
    printf("y = %f\n", y);

    // float *xp = &x; // TODO: отредактируйте эту строку, и только правую часть уравнения
    float *xp = &y; // Новая строка
    float *yp = &y;
    printf("Результат: %f\n", *xp + *yp);
}

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
        case 2:
            task_2();
            break;
        case 3:
            uint32_t arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            size_t length = sizeof(arr) / sizeof(uint32_t);

            print_array(arr, length);
            break;
        default:
            printf("Неправильный номер задания (число должно быть от 1 до 4)\n");
            return 2;
    }


    return 0;
}
