#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "abonent_list.h"

#define STR_LIMIT 127
#define SCN_STR_LIMIT "127"


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

static void add_contact(struct abonent_list *list) {
    char name_buf[STR_LIMIT + 1];
    printf("Введите имя:\n");
    read_non_empty_string(name_buf);

    char second_name_buf[STR_LIMIT + 1];
    printf("Введите фамилию:\n");
    read_non_empty_string(second_name_buf);   

    char tel_buf[STR_LIMIT + 1];
    printf("Введите номер телефона:\n");
    read_non_empty_string(tel_buf);

    const struct abonent_list_node *new_node = abonent_list_add(list, name_buf, second_name_buf, tel_buf);
    printf("Добавлена новая запись:\n");
    abonent_print(&new_node->data);
    printf("\n");
}

static void remove_contact(struct abonent_list *list) {
    if (list->size > 0) {
        char num_buf[STR_LIMIT + 1];
        printf("Введите номер удаляемого элемента массива (число от 1 до %zu включительно):\n", list->size);
        read_non_empty_string(num_buf);

        long id = strtol(num_buf, NULL, 10);
        if ((errno == ERANGE) || (id < 1) || (id > list->size)) {
            printf("Введено некорректное число.\n\n");
        }
        else {
            struct abonent_list_node *node = abonent_list_extract_node_at(list, id); // Not NULL

            printf("Удалена запись под номером %zu:\n", id);
            abonent_print(&node->data);
            printf("\n");

            abonent_list_node_free(node);
        }
    }
    else {
        printf("Массив абонентов пуст.\n\n");
    }
}

static void search_contacts(const struct abonent_list *list) {
    char name_buf[STR_LIMIT + 1];
    printf("Введите имя для поиска абонентов:\n");
    read_non_empty_string(name_buf);

    abonent_list_search_by_name_and_print(list, name_buf);
}

static void print_all_contacts(const struct abonent_list *list) {
    printf("Вывод полной информации:\n");
    abonent_list_print_all(list);
}

int main() {
    char input_buf[STR_LIMIT + 1];
    struct abonent_list *list = abonent_list_create_empty();

    print_info();
    do {
        read_non_empty_string(input_buf);
        int option = atoi(input_buf);

        switch (option) {
            case 1:
                add_contact(list);
                break;
            case 2:
                remove_contact(list);
                break;
            case 3:
                search_contacts(list);
                break;
            case 4:
                print_all_contacts(list);
                break;
            case 5:
                printf("Завершение программы...\n");
                return 0;
            default:
                printf("Введён некорректный номер.\n\n");
                print_info();
        }
    } while (1);

    abonent_list_free(list);
    return 0;
}
