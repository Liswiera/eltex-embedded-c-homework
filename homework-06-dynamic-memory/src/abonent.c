#include "abonent.h"

static void strncpy_safe(char *restrict dest, const char *restrict src, size_t dest_buf_size) {
    strncpy(dest, src, dest_buf_size);

    // strncpy не гарантирует добавление символа '\0',
    // если в dest записано ровно dest_buf_size символов,
    // поэтому символ '\0' необходимо добавить вручную
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
