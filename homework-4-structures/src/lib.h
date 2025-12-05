#pragma once

#include <inttypes.h>

#define BUF_SIZE 10

struct abonent {
    char name[BUF_SIZE];
    char second_name[BUF_SIZE];
    char tel[BUF_SIZE];
};

void abonent_print(const struct abonent *obj);


struct abonent_vec {
    struct abonent *abonents;
    size_t len;
    size_t capacity;
};

struct abonent_vec abonent_vec_create(struct abonent *abonents, size_t capacity);
uint32_t abonent_vec_push(struct abonent_vec *vec, const char *name, const char *second_name, const char *tel);
uint32_t abonent_vec_remove_at(struct abonent_vec *vec, size_t id);
void abonent_vec_search_by_name_and_print(const struct abonent_vec *vec, const char *name);
void abonent_vec_print_all(const struct abonent_vec *vec);
