#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "libcalc/libcalc.h"

#define STR_LIMIT 127
#define SCN_STR_LIMIT "127"


static void print_info() {
    printf(
        "Выберите один из следующих пунктов:\n"
        "1) Сложение\n"
        "2) Вычитание\n"
        "3) Умножение\n"
        "4) Деление\n"
        "5) Выход\n"
        "\n"
    );
}

static void read_string(char *output_buf) {
    printf("> ");
    fflush(stdout);

    int arg_read_count = scanf("%" SCN_STR_LIMIT "[^\n]", output_buf);
    if (arg_read_count == 0) {
        *output_buf = '\0';
    }
    getchar(); // Пропустить '\n' из буфера ввода
}

static void read_non_empty_string(char *output_buf) {
    do {
        read_string(output_buf);
    } while (output_buf[0] == '\0');
}

static long read_number() {
    char num_buf[STR_LIMIT + 1];

    while (1) {
        read_non_empty_string(num_buf);

        errno = 0;
        long res = strtol(num_buf, NULL, 10);

        if (errno == ERANGE) {
            printf("Введено некорректное число.\n\n");
            continue;
        }

        return res;
    }
}

static void read_args_and_perform_op(int64_t (*op_func)(int64_t, int64_t), int rhs_can_be_zero) {
    printf("Введите первое число:\n");
    long a = read_number();

    printf("Введите второе число:\n");
    long b;
    while (1) {
        b = read_number();
        if (b == 0 && !rhs_can_be_zero) {
            printf("Второе число не может быть равно нулю.\n\n");
            continue;
        }

        break;
    }
    
    int64_t res = op_func(a, b);
    printf("Результат операции: %" PRIu64 "\n\n", res);
}

int main() {
    do {
        print_info();
        long option = read_number();
        printf("\n");

        switch (option) {
            case 1:
                read_args_and_perform_op(calc_add, 1);
                break;
            case 2:
                read_args_and_perform_op(calc_sub, 1);
                break;
            case 3:
                read_args_and_perform_op(calc_mul, 1);
                break;
            case 4:
                read_args_and_perform_op(calc_div, 0);
                break;
            case 5:
                printf("Завершение программы...\n");
                return 0;
            default:
                printf("Введён некорректный номер.\n\n");
        }
    } while (1);

    return 0;
}
