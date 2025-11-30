#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

#define STR_LIMIT 128
#define SCN_STR_LIMIT "128"


static void read_string(char *str) {
    printf("> ");
    fflush(stdout);

    int arg_read_count = scanf("%" SCN_STR_LIMIT "[^\n]", str);
    if (arg_read_count == 0) {
        *str = '\0';
    }
    getchar();
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
        case 4:
            printf("Введите две строки, чтобы найти первое вхождение второй подстроки в первой строке.\n\n");

            char text[STR_LIMIT + 1];
            char pattern[STR_LIMIT + 1];
            while (1) {
                read_string(text);
                read_string(pattern);

                const char* substr = find_substr_naive(text, pattern);
                if (substr) {
                    size_t pos = (size_t)(substr - text);
                    printf("Найдена подстрока по индексу %zu.\n", pos);
                }
                else {
                    printf("Подстрока не найдена.\n");
                }
                printf("\n");
            }
        default:
            printf("Неправильный номер задания (число должно быть от 1 до 4)\n");
            return 2;
    }


    return 0;
}
