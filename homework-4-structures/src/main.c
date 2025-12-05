#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

#define STR_LIMIT 127
#define SCN_STR_LIMIT "127"
#define ABONENT_ARRAY_CAPACITY 100

struct abonent abonent_array[ABONENT_ARRAY_CAPACITY];


static void print_info() {
    printf(
        "Выберите один из следующих пунктов:\n"
        "1) Добавить абонента\n"
        "2) Удалить абонента\n"
        "3) Поиск абонентов по имени\n"
        "4) Вывод всех записей\n"
        "5) Выход\n"
        "\n"
    );
}

static void read_string(char *str) {
    printf("> ");
    fflush(stdout);

    int arg_read_count = scanf("%" SCN_STR_LIMIT "[^\n]", str);
    if (arg_read_count == 0) {
        *str = '\0';
    }
    getchar();
}

static void read_non_empty_string(char *str) {
    do {
        read_string(str);
    } while (str[0] == '\0');
}

static void add_contact(struct abonent_vec *vec) {
    if (vec->len < vec->capacity) {
        char name_buf[STR_LIMIT + 1];
        printf("Введите имя:\n");
        read_non_empty_string(name_buf);

        char second_name_buf[STR_LIMIT + 1];
        printf("Введите фамилию:\n");
        read_non_empty_string(second_name_buf);   

        char tel_buf[STR_LIMIT + 1];
        printf("Введите номер телефона:\n");
        read_non_empty_string(tel_buf);

        const struct abonent *new_contact = abonent_vec_push(vec, name_buf, second_name_buf, tel_buf);
        printf("Добавлена новая запись:\n");
        abonent_print(new_contact);
        printf("\n");
    }
    else {
        printf("Массив абонентов полностью заполнен, добавление невозможно.\n\n");
    }
}

static void remove_contact(struct abonent_vec *vec) {
    if (vec->len > 0) {
        char num_buf[STR_LIMIT + 1];
        printf("Введите номер удаляемого элемента массива (число от 1 до %zu включительно):\n", vec->len);
        read_non_empty_string(num_buf);

        long id = strtol(num_buf, NULL, 10);
        if ((errno == ERANGE) || (id < 1) || (id > vec->len)) {
            printf("Введено некорректное число.\n\n");
        }
        else {
            printf("Удалена запись под номером %zu:\n", id);
            abonent_print(&vec->abonents[id - 1]);
            printf("\n");

            abonent_vec_remove_at(vec, id - 1);
        }
    }
    else {
        printf("Массив абонентов пуст.\n\n");
    }
}

static void search_contacts(const struct abonent_vec *vec) {
    char name_buf[STR_LIMIT + 1];
    printf("Введите имя для поиска абонентов:\n");
    read_non_empty_string(name_buf);

    abonent_vec_search_by_name_and_print(vec, name_buf);
}

static void print_all_contacts(const struct abonent_vec *vec) {
    printf("Вывод полной информации:\n");
    abonent_vec_print_all(vec);
}

int main() {
    char input_buf[STR_LIMIT + 1];
    struct abonent_vec vec = abonent_vec_create(abonent_array, ABONENT_ARRAY_CAPACITY);

    print_info();
    do {
        read_non_empty_string(input_buf);
        int option = atoi(input_buf);

        switch (option) {
            case 1:
                add_contact(&vec);
                break;
            case 2:
                remove_contact(&vec);
                break;
            case 3:
                search_contacts(&vec);
                break;
            case 4:
                print_all_contacts(&vec);
                break;
            case 5:
                printf("Завершение программы...\n");
                return 0;
            default:
                printf("Введён некорректный номер.\n\n");
                print_info();
        }
    } while (1);

    return 0;
}
