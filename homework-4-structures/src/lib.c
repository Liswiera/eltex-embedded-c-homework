#include "lib.h"

static void strncpy_safe(char *restrict dest, const char *restrict src, size_t dest_buf_size) {
    strncpy(dest, src, dest_buf_size);

    // strncpy не обеспечивает наличие символа '\0',
    // если в dest записано ровно dest_buf_size символов,
    // символ '\0' необходимо добавить вручную
    dest[dest_buf_size - 1] = '\0';
}

void abonent_create_from(struct abonent *restrict dest, const char *name, const char *second_name, const char *tel) {
    strncpy_safe(dest->name, name, CHAR_ARR_SIZE);
    strncpy_safe(dest->second_name, second_name, CHAR_ARR_SIZE);
    strncpy_safe(dest->tel, tel, CHAR_ARR_SIZE);
}

void abonent_print(const struct abonent *obj) {
    printf("Имя: %s; Фамилия: %s; Телефон: %s\n",
        obj->name,
        obj->second_name,
        obj->tel);
}


struct abonent_vec abonent_vec_create(struct abonent *abonents, size_t capacity) {
    struct abonent_vec vec;
    vec.abonents = abonents;
    vec.capacity = capacity;
    vec.len = 0;

    return vec;
}

struct abonent* abonent_vec_push(struct abonent_vec *restrict vec, const char *name, const char *second_name, const char *tel) {
    if (vec->len < vec->capacity) {
        struct abonent *new_contact = &vec->abonents[vec->len];
        abonent_create_from(new_contact, name, second_name, tel);

        vec->len++;
        return new_contact;
    }
    else {
        return NULL;
    }
}

uint32_t abonent_vec_remove_at(struct abonent_vec *vec, size_t id) {
    if (id < vec->len) {
        // По аналогии с std::vector происходит сдвиг всех
        // элементов справа налево на место удалённого элемента
        for (size_t pos = id + 1; pos < vec->len; pos++) {
            struct abonent *dest = &vec->abonents[pos - 1];
            struct abonent *src = &vec->abonents[pos];
            memcpy(dest, src, sizeof(struct abonent));
        }
        vec->len--;
        return 1;
    }
    else {
        return 0;
    }
}

void abonent_vec_search_by_name_and_print(const struct abonent_vec *vec, const char *name) {
    printf("Поиск абонентов с именем \"%s\":\n", name);
    for (size_t i = 0; i < vec->len; i++) {
        if (strcmp(vec->abonents[i].name, name) == 0) {
            printf("[ID#%zu] ", i + 1);
            abonent_print(&vec->abonents[i]);
        }
    }
    printf("\n");
}

void abonent_vec_print_all(const struct abonent_vec *vec) {
    printf("Вектор абонентов [len = %zu] [capacity = %zu]\n", vec->len, vec->capacity);
    for (size_t i = 0; i < vec->len; i++) {
        printf("[ID#%zu] ", i + 1);
        abonent_print(&vec->abonents[i]);
    }
    printf("\n");
}
